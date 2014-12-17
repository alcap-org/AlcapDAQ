#include "RegisterModule.inc"
#include "MakeDetectorPulses.h"
#include "TVDetectorPulseGenerator.h"
#include "MaxTimeDiffDPGenerator.h"
#include "TDPGeneratorFactory.h"
#include "TDPGeneratorOptions.h"
#include "debug_tools.h"

#include <sstream>
#include <algorithm>
using std::endl;
using std::cout;

extern SourceAnalPulseMap gAnalysedPulseMap;
extern SourceDetPulseMap gDetectorPulseMap;

namespace{
struct is_equal {
  is_equal( const IDs::channel& ch):fCh(ch){fCh.SlowFast(IDs::kAnySlowFast);};
  bool operator()(const std::string& str){return fCh.matches(str);}
  private:
     IDs::channel fCh;
};
}

MakeDetectorPulses::MakeDetectorPulses(modules::options* opts):
    BaseModule("MakeDetectorPulses",opts,false),fOptions(opts),fPassThruGenerator(NULL){
        // Get the algorithm option from the modules file
        // If nothing was set, use MaxTimeDiff by default
        opts->GetVectorStringsByWhiteSpace("analyse_detectors",fDetectorsToAnalyse);
        std::sort(fDetectorsToAnalyse.begin(), fDetectorsToAnalyse.end());
        fDefaultAlgorithm=opts->GetString("default_algorithm","MaxTimeDiff");
        fPassThruName=opts->GetString("pass_through_algorithm","PassThrough");
    }

MakeDetectorPulses::~MakeDetectorPulses(){
}

int MakeDetectorPulses::BeforeFirstEntry(TGlobalData* gData, const TSetupData* setup){
    // do we analyse all channels?
    bool analyse_all=fDetectorsToAnalyse.empty();
    if(!analyse_all) analyse_all= (std::find(fDetectorsToAnalyse.begin(), fDetectorsToAnalyse.end(), "all")!=fDetectorsToAnalyse.end());

    const IDs::channel* ch;
    const IDs::generator* gen;

    // Find all fast detectors
    for (SourceAnalPulseMap::const_iterator i_source = gAnalysedPulseMap.begin();
            i_source != gAnalysedPulseMap.end(); i_source++) {
        ch=&i_source->first.Channel();
        gen=&i_source->first.Generator();

	// Do we need to pair this detector?
        if( !analyse_all && 
            (std::find_if(fDetectorsToAnalyse.begin(),
                           fDetectorsToAnalyse.end(),
                            is_equal(*ch))==fDetectorsToAnalyse.end())) continue;

        // Find the correct source since the generator options could be different
        IDs::source partner =i_source->first;
        for (SourceAnalPulseMap::const_iterator j_source = gAnalysedPulseMap.begin();
             j_source != gAnalysedPulseMap.end(); ++j_source) {
          
          if (j_source->first.Generator().Type() == gen->Type()
              && j_source->first.Channel() == ch->GetCorrespondingFastSlow()) {
            partner = j_source->first;
          }
        }

        // Check whether a specific generator is given for this detector
        const std::string detname=IDs::channel::GetDetectorString(ch->Detector());
        if( fOptions->HasOption(detname) ){
            // If this channel is named explicitly, use that generator type
            // Get a vector for the generator(s) that we want to use for this detector
            bool success=ParseGeneratorList(i_source->first, partner,fOptions->GetString(detname));
            if(! success) return 1;
        }else if(partner==i_source->first) {
           // if there is no corresponding fast / slow channel then use the pass
	   // through generator
            bool success=ParseGeneratorList(i_source->first, partner,fPassThruName);
            if(! success) return 1;
        } else {
            bool success=ParseGeneratorList(i_source->first, partner,fDefaultAlgorithm);
            if(! success) return 1;
        }
    }

    // now make an entry in gAnalysedPulseMap for each generator added
    for(ChannelPairing_t::const_iterator i_pair=fFastSlowPairs.begin();
            i_pair!=fFastSlowPairs.end();i_pair++){
        gDetectorPulseMap[i_pair->source];
        if(Debug()){
            std::cout<<"Paired: "<<i_pair->fast<<" with "<<i_pair->slow<<" processed by: "<<i_pair->generator->GetGeneratorID()<<std::endl;
        }
    }
    return 0;
}

int MakeDetectorPulses::ProcessEntry(TGlobalData *gData, const TSetupData* gSetup){
    IDs::channel detector(IDs::kAnyDetector,IDs::kNotApplicable);
    IDs::source source;
    const AnalysedPulseList *slow_pulses;
    const AnalysedPulseList *fast_pulses;
    SourceAnalPulseMap::const_iterator current;
    //DetectorPulseList *detectorPulses;

    // Loop through paired channels
    for (ChannelPairing_t::const_iterator i_detector = fFastSlowPairs.begin();
            i_detector != fFastSlowPairs.end(); i_detector++) {
        detector=i_detector->fast.Channel();

        // find the list of analyzed pulses for the fast channel
        current=gAnalysedPulseMap.find(i_detector->fast);
        if(current==gAnalysedPulseMap.end()){
            std::cout<<"Unable to find source for fast channel: "<<i_detector->fast<<std::endl;
            DumpgAnalysedPulseMap(gAnalysedPulseMap);
            return 1;
        }
        fast_pulses=&(current->second);

        // check if the fast_pulses is empty
        if(fast_pulses->empty()) {
            if(Debug()) cout<< "List of TAPs for "<<i_detector->fast<<" is empty."<<endl;
            continue;
        }

        // if the fast and slow sources are the same then pass the slow channel
        // as NULL
        if(i_detector->fast==i_detector->slow) slow_pulses=NULL;
        else{
            current=gAnalysedPulseMap.find(i_detector->slow);
            if(current==gAnalysedPulseMap.end()){
                std::cout<<"Unable to find source for slow channel: "<<i_detector->slow<<std::endl;
                DumpgAnalysedPulseMap(gAnalysedPulseMap);
                return 1;
            }
            slow_pulses=&(current->second);
            // check if the fast_pulses is empty
            if(slow_pulses->empty()) {
                if(Debug())
                cout<< "List of TAPs for "<<i_detector->slow<<" is empty."<<endl;
                continue;
            }
        }

        // Set the channel
        i_detector->generator->SetChannel(detector);
        // Set the pulse lists
        i_detector->generator->SetPulseLists(fast_pulses,slow_pulses);
        // Process the pulses
        source=i_detector->source;
        i_detector->generator->ProcessPulses(gDetectorPulseMap.at(source));
    }
    return 0;
}

bool MakeDetectorPulses::ParseGeneratorList(const IDs::source& current_source, const IDs::source& partner_source,
    const std::string& generator_specs){
    std::vector<std::string> generatorList;
    modules::parser::TokeniseByDelimiter(generator_specs,generatorList,":");

    //scan to next item in the list
    std::vector<std::string>::const_iterator gen;
    std::string arg;
    TDPGeneratorOptions* opts;
    modules::parser::Constructor_t generator_request;
    bool still_good=true;
    for(gen=generatorList.begin();gen!= generatorList.end();gen++){
      // check if we have options for this generator
      generator_request=modules::parser::ParseConstructor(*gen,'(',')');
      std::stringstream sstream(generator_request.inside);
      opts=new TDPGeneratorOptions(generator_request.before);
      for(int count=0; std::getline(sstream, arg,','); count++){
        opts->AddArgument(count,arg);
      }
      still_good = MakeGenerator(current_source,partner_source, generator_request.before,opts);
      // Is everything ok to continue?
      if (!still_good) {
	return false;
      }
      // Get ready for next iteration
      opts=NULL;
    }
    // Everything went ok, return true
    return true;
}


TVDetectorPulseGenerator* MakeDetectorPulses::MakeGenerator(const IDs::source& current_source, const IDs::source& partner_source, const std::string& generatorType, TDPGeneratorOptions* opts ){
     const IDs::channel& ch=current_source.Channel();

    // Select the generator type
    TVDetectorPulseGenerator* generator=TDPGeneratorFactory::Instance()->createModule(generatorType,opts);
    if (!generator){
        return NULL;
    }
    // setup some options on the generator
    generator->SetDebug(Debug());

    // Prepare the source ID for the TDP generator
    IDs::source tdp_source=current_source;
    tdp_source.Channel().SlowFast(IDs::kNotApplicable);
    tdp_source.Generator()=generator->GetGeneratorID();

    //if(Debug())
       cout<< "MakeDetectorPulses::BeforeFirstEntry: Making generator '"<<generatorType<<"' with source ID: '"<<tdp_source<<"'" <<endl;

    if(ch.isFast()) {
        // fast channels go first
        fFastSlowPairs.insert(Detector_t(tdp_source, current_source,partner_source,generator));
    }else{ 
        // slow channels go second
        // if both fast and slow are the same then later there will be
        // nothing to do.
        fFastSlowPairs.insert(Detector_t(tdp_source, partner_source,current_source,generator));
    }

    return generator;
}

void MakeDetectorPulses::DumpgAnalysedPulseMap(const SourceAnalPulseMap& aMap){
    std::cout<<"gAnalysedPulseMap contains:"<<std::endl;
    for(SourceAnalPulseMap::const_iterator it=aMap.begin();
            it!=aMap.end(); it++){
        cout<<it->first<<" contains "<<it->second.size()<<" TAPs"<<endl;
    }

}
ALCAP_REGISTER_MODULE(MakeDetectorPulses,default_algorithm);
