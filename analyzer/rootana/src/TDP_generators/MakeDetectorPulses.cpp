#include "RegisterModule.inc"
#include "MakeDetectorPulses.h"
#include "TVDetectorPulseGenerator.h"
#include "MaxTimeDiffDPGenerator.h"
#include "TDPGeneratorFactory.h"
using std::endl;
using std::cout;

extern SourceAnalPulseMap gAnalysedPulseMap;
extern StringDetPulseMap gDetectorPulseMap;

MakeDetectorPulses::MakeDetectorPulses(modules::options* opts):
    BaseModule("MakeDetectorPulses",opts,false),fOptions(opts){
        // Get the algorithm option from the modules file
        // If nothing was set, use MaxTimeDiff by default
        fAlgorithm=opts->GetString("algorithm","MaxTimeDiff");
    }

MakeDetectorPulses::~MakeDetectorPulses(){
}

int MakeDetectorPulses::ProcessEntry(TGlobalData *gData, const TSetupData* gSetup){
    IDs::channel detector(IDs::kAnyDetector,IDs::kNotApplicable);
    const AnalysedPulseList *slow_pulses;
    const AnalysedPulseList *fast_pulses;
    SourceAnalPulseMap::const_iterator current;
    //DetectorPulseList *detectorPulses;

    // Loop through paired channels
    for (ChannelPairing_t::const_iterator i_detector = fFastSlowPairs.begin();
            i_detector != fFastSlowPairs.end(); i_detector++) {
        detector=i_detector->first.Channel();

        // find the list of analyzed pulses for the fast channel
        current=gAnalysedPulseMap.find(i_detector->first);
        if(current==gAnalysedPulseMap.end()){
            std::cout<<"Unable to find source for fast channel: "<<i_detector->first<<std::endl;
            std::cout<<"gAnalysedPulseMap contains:"<<std::endl;
            DumpgAnalysedPulseMap(gAnalysedPulseMap);
            return 1;
        }
        fast_pulses=&(current->second);

        // check if the fast_pulses is empty
        if(fast_pulses->empty()) {
            cout<< "List of TAPs for "<<i_detector->first<<" is empty."<<endl;
            continue;
        }

        // if the fast and slow sources are the same then pass the slow channel
        // as NULL
        if(i_detector->first==i_detector->second) slow_pulses=NULL;
        else{
            current=gAnalysedPulseMap.find(i_detector->second);
            if(current==gAnalysedPulseMap.end()){
                std::cout<<"Unable to find source for slow channel: "<<i_detector->second<<std::endl;
                std::cout<<"gAnalysedPulseMap contains:"<<std::endl;
                DumpgAnalysedPulseMap(gAnalysedPulseMap);
                return 1;
            }
            slow_pulses=&(current->second);
            // check if the fast_pulses is empty
            if(slow_pulses->empty()) {
                cout<< "List of TAPs for "<<i_detector->second<<" is empty."<<endl;
                continue;
            }
        }

        fGenerator->ProcessPulses(gSetup,detector.str(),fast_pulses,slow_pulses,gDetectorPulseMap[detector.str()]);
    }
    return 0;
}

int MakeDetectorPulses::BeforeFirstEntry(TGlobalData* gData, const TSetupData* setup){
    // Set up the generator
    fGenerator=MakeGenerator(fAlgorithm);
    if(!fGenerator) return 1;

    const IDs::channel* ch;
    const IDs::generator* gen;
    IDs::source partner;
    // Find all fast detectors
    for (SourceAnalPulseMap::const_iterator i_source = gAnalysedPulseMap.begin();
            i_source != gAnalysedPulseMap.end(); i_source++) {
        ch=&i_source->first.Channel();
        gen=&i_source->first.Generator();
        partner=IDs::source(ch->GetCorrespondingFastSlow(),*gen);
        if(gAnalysedPulseMap.count(partner)==0) partner=i_source->first;

        if(ch->isFast()) {
            // fast channels go first
            fFastSlowPairs.insert(std::make_pair(i_source->first,partner));
        }else{ 
            // slow channels go second
            // if both fast and slow are the same then later there will be
            // nothing to do.
            fFastSlowPairs.insert(std::make_pair(partner,i_source->first));
        }

    }

    if(Debug()){
        for( ChannelPairing_t::iterator i=fFastSlowPairs.begin();i!=fFastSlowPairs.end();i++){
            std::cout<<"Paired: "<<i->first<<" with "<<i->second<<std::endl;
        }
    }
    return 0;
}

TVDetectorPulseGenerator* MakeDetectorPulses::MakeGenerator(const std::string& generatorType){
    // Select the generator type
    TVDetectorPulseGenerator* generator=TDPGeneratorFactory::Instance()->createModule(generatorType);
    if (!generator){
        return NULL;
    }
    // setup some options on the generator
    generator->SetDebug(Debug());
    return generator;
}

void MakeDetectorPulses::DumpgAnalysedPulseMap(const SourceAnalPulseMap& aMap){
    for(SourceAnalPulseMap::const_iterator it=aMap.begin();
            it!=aMap.end(); it++){
        cout<<it->first<<" contains "<<it->second.size()<<" TAPs"<<endl;
    }

}
ALCAP_REGISTER_MODULE(MakeDetectorPulses,algorithm);
