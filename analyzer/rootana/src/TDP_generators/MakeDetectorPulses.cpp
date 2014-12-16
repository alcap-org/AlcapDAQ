#include "RegisterModule.inc"
#include "MakeDetectorPulses.h"
#include "TVDetectorPulseGenerator.h"
#include "MaxTimeDiffDPGenerator.h"
#include "TDPGeneratorFactory.h"
#include "TDPGeneratorOptions.h"
#include "debug_tools.h"

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
    const std::string* gen_name;
    IDs::source partner;
    IDs::source tdp_source;

    // Find all fast detectors
    TVDetectorPulseGenerator* generator;
    for (SourceAnalPulseMap::const_iterator i_source = gAnalysedPulseMap.begin();
            i_source != gAnalysedPulseMap.end(); i_source++) {
        ch=&i_source->first.Channel();
        gen=&i_source->first.Generator();
        DEBUG_VALUE(*ch,*gen);
        if( !analyse_all && 
            (std::find_if(fDetectorsToAnalyse.begin(),
                           fDetectorsToAnalyse.end(),
                            is_equal(*ch))==fDetectorsToAnalyse.end())) continue;
        DEBUG_CHECKPOINT;

	// Find the correct source since the generator options could be different
	partner.Reset();
	for (SourceAnalPulseMap::const_iterator j_source = gAnalysedPulseMap.begin();
	     j_source != gAnalysedPulseMap.end(); ++j_source) {
	  
	  if (j_source->first.Generator().Type() == gen->Type()
	      && j_source->first.Channel() == ch->GetCorrespondingFastSlow()) {
	    partner = j_source->first;
	  }
	}
	if (gAnalysedPulseMap.count(partner)==0) partner=i_source->first;

        // if there is no corresponding fast / slow channel then use the pass
        // through generator
        if(partner==i_source->first) {
            gen_name=&fPassThruName;
        } else {
            gen_name=&fDefaultAlgorithm;
        }

        // Set up the generator
        TDPGeneratorOptions gen_opts("gen opts",fOptions);
        generator=MakeGenerator(*gen_name,&gen_opts);
        if(!generator) {
           cout<<"MakeDetectorPulses::BeforeFirstEntry: problem making generator '"<<*gen_name<<"' for detector '"<<ch<<"'."<<endl;
           return 1;
        }

        // Prepare the source ID for the TDP generator
        tdp_source=i_source->first;
        tdp_source.Channel().SlowFast(IDs::kNotApplicable);
        tdp_source.Generator()=generator->GetGeneratorID();

	//if(Debug())
           cout<< "MakeDetectorPulses::BeforeFirstEntry: Making generator '"<<*gen_name<<"' with source ID: '"<<tdp_source<<"'" <<endl;

        if(ch->isFast()) {
            // fast channels go first
            fFastSlowPairs.insert(Detector_t(tdp_source, i_source->first,partner,generator));
        }else{ 
            // slow channels go second
            // if both fast and slow are the same then later there will be
            // nothing to do.
            fFastSlowPairs.insert(Detector_t(tdp_source, partner,i_source->first,generator));
        }
    }

    // setup gDetectorPulseMap to contain all pulse lists for each TDP source
    for( ChannelPairing_t::iterator i=fFastSlowPairs.begin();i!=fFastSlowPairs.end();i++){
        gDetectorPulseMap[i->source];
        if(Debug()){
            std::cout<<"Paired: "<<i->fast<<" with "<<i->slow<<" processed by: "<<i->generator->GetGeneratorID()<<std::endl;
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

TVDetectorPulseGenerator* MakeDetectorPulses::MakeGenerator(const std::string& generatorType, TDPGeneratorOptions* opts ){
    // Select the generator type
    TVDetectorPulseGenerator* generator=TDPGeneratorFactory::Instance()->createModule(generatorType,opts);
    if (!generator){
        return NULL;
    }
    // setup some options on the generator
    generator->SetDebug(Debug());
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
