#include "TMEGeneratorFactory.h"
#include "FixedWindowMEGenerator.h"
#include "TDetectorPulse.h"
#include "TMuonEvent.h"
//#define ALCAP_NO_DEBUG
#include "debug_tools.h"

#include <iostream>
using std::cout;
using std::endl;

FixedWindowMEGenerator::FixedWindowMEGenerator(TMEGeneratorOptions* opts):
	TVMuonEventGenerator("FixedWindow",opts){
        fEventWindow=opts->GetDouble("event_window",10);
}

int FixedWindowMEGenerator::Init(const SourceDetPulseMap& detectorPulsesIn){
    for(SourceDetPulseMap::const_iterator i_source=detectorPulsesIn.begin();
            i_source!=detectorPulsesIn.end();++i_source){
        // if i_source->source is muSc then
        if(i_source->first.matches(IDs::channel(IDs::kMuSc))){
            // if source is wildcard then select fMuSc 
            if(!fMuSc.source || fMuSc.source->isWildCardChannel()){
                fMuSc=Detector_t(i_source);
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
    fMuSc.Reset();
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

    // Loop over all muons
    for(DetectorPulseList::const_iterator i_muSc=fMuSc.pulses->begin();
            i_muSc!=fMuSc.pulses->end(); ++i_muSc){
        // Make a TME centred on this muon
        TMuonEvent* tme=new TMuonEvent(*i_muSc);

        // Add all muon pulses in the event window to this TME
        AddPulsesInWindow(tme,fEventWindow,fMuSc);

        for(SourceList::iterator i_det=fDetectors.begin();
                i_det!=fDetectors.end(); ++i_det){
            // Add all pulses in the event window to this TME
            AddPulsesInWindow(tme,fEventWindow,*i_det);
        }
        //if(Debug()) cout<<"FixedWindowMEGenerator::ProcessPulses:"
        //    " Created TME with "<<tme->TotalNumPulses()<<" pulses" <<endl;

        // Add TME to list of event
        muonEventsOut.push_back(tme);
    }
    return 0;
}
void FixedWindowMEGenerator::AddPulsesInWindow(
        TMuonEvent* tme, double window, Detector_t& detector){
    // Get the central time for this event;
    double central_time=tme->GetTime();
    double early_edge=central_time-window;
    double late_edge=central_time+window;

    // advance the start and stop iterators that define the range of pulses
    // contained in the window
    DetectorPulseList::const_iterator& start=detector.start_window;
    DetectorPulseList::const_iterator& stop=detector.end_window;
    const DetectorPulseList::const_iterator& end=detector.pulses->end();
    //const DetectorPulseList::const_iterator& begin=detector.pulses->begin();

    // skip empty lists
    if(start==end) return;


    // Move the iteator for the start of the window
    int look_ahead=1;
    while((start+look_ahead!=end) && (start!=end) ){
        double time = (*(start+look_ahead))->GetTime();
        if(time==definitions::DefaultValue){
            // If time is DefaultValue then we're looking at an unpaired
            // detector which we should ignore
            ++look_ahead;
            continue;
        }
        look_ahead=1;
        if(time>early_edge) break;
        start+=look_ahead;
    }
    // Move the iteator for the end of the window
    stop=start;
    while( stop!=end ){  
        double time = (*stop)->GetTime();
        if(time>late_edge) break;
        ++stop;
    }

    // Add all pulses in the current window to the tme
    tme->AddPulses(*detector.source,start,stop);

    // Flag if we've used all of a channels pulses
    if(stop==detector.pulses->end()) tme->AllPulsesUsed(*detector.source);
}

ALCAP_TME_GENERATOR(FixedWindow, event_window);
