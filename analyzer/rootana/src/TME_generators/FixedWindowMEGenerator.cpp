#include "TMEGeneratorFactory.h"
#include "FixedWindowMEGenerator.h"
#include "TDetectorPulse.h"
#include "TMuonEvent.h"
#include <iostream>
using std::cout;
using std::endl;

FixedWindowMEGenerator::FixedWindowMEGenerator(TMEGeneratorOptions* opts):
	TVMuonEventGenerator("FixedWindow",opts){
        fMuonWindow=opts->GetDouble("muon_window",10);
        fEventWindow=opts->GetDouble("event_window",10);
}

int FixedWindowMEGenerator::Init(const SourceDetPulseMap& detectorPulsesIn){
    for(SourceDetPulseMap::const_iterator i_source=detectorPulsesIn.begin();
            i_source!=detectorPulsesIn.end();++i_source){
        // if i_source->source is muSc then
        if(i_source->first.matches(IDs::channel(IDs::kMuSc))){
            // if source is wildcard then select fMuSc 
            if(!fMuSc.source || fMuSc.source->isWildCardChannel()){
                fMuSc.Init(i_source);
            }else{
                cout<<"FixedWindowMEGenerator: Error: Multiple muSc TDP sources found"<<endl;
                return 1;
            }
        }else{
            fDetectors.push_back(Detector_t(i_source));
        }
    }
    return 0;
}

void FixedWindowMEGenerator::Reset(){
    for(SourceList::iterator i_det=fDetectors.begin();
            i_det!=fDetectors.end(); ++i_det){
        i_det->Reset();
    }
}

int FixedWindowMEGenerator::ProcessPulses(MuonEventList& muonEventsOut,
        const SourceDetPulseMap& detectorPulsesIn){
    static bool init=true;
    if(init) {
        int error=Init(detectorPulsesIn);
        if(error) return error;
        init=false;
    }else{
        Reset();
    }

    for(DetectorPulseList::const_iterator i_muSc=fMuSc.pulses->begin();
            i_muSc!=fMuSc.pulses->end(); ++i_muSc){
        // Make a TME centred on this muon
        TMuonEvent* tme=new TMuonEvent(*i_muSc);
        double central_time=(*i_muSc)->GetTime();

        // Add all muon pulses in the event window to this TME
        PulsesInWindow(central_time,fEventWindow,
                fMuSc.start_window,fMuSc.end_window, fMuSc.pulses->end());
        tme->AddPulses(*fMuSc.source,fMuSc.start_window,fMuSc.end_window);

        for(SourceList::iterator i_det=fDetectors.begin();
                i_det!=fDetectors.end(); ++i_det){
            // Add all pulses in the event window to this TME
            PulsesInWindow(central_time,fEventWindow,
                    i_det->start_window,i_det->end_window, i_det->pulses->end());
            tme->AddPulses(*i_det->source,i_det->start_window,i_det->end_window);
        }
    }
    return 0;
}

void FixedWindowMEGenerator::PulsesInWindow(double central_time, double window,
        DetectorPulseList::const_iterator& start,
        DetectorPulseList::const_iterator& stop,
        const DetectorPulseList::const_iterator& end){
    while(start+1!=end && (*(start+1))->GetTime()<central_time-window ) ++start;
    while(   stop!=end &&      (*stop)->GetTime()<central_time+window ) ++stop;
}

ALCAP_TME_GENERATOR(FixedWindow, muon_window, event_window);
