#ifndef FIXEDWINDOW_H__
#define FIXEDWINDOW_H__

#include "TSetupData.h"
#include "TVMuonEventGenerator.h"
#include "definitions.h"

class FixedWindowMEGenerator:public TVMuonEventGenerator {
    struct Detector_t{
        const IDs::source* source;
        const DetectorPulseList* pulses;
        DetectorPulseList::const_iterator start_window;
        DetectorPulseList::const_iterator end_window;
        Detector_t():source(NULL),pulses(NULL){};
        Detector_t(const SourceDetPulseMap::const_iterator& pair):
            source(NULL),pulses(NULL){ Init(pair);}
        void Init(const SourceDetPulseMap::const_iterator& pair){
            source=&pair->first;
            pulses=&pair->second;
            Reset();
        }
        void Reset(){
            if(pulses){
                start_window=pulses->begin();
                end_window=pulses->begin();
            }
        }
    };
    typedef std::vector<Detector_t> SourceList;

    public:
        FixedWindowMEGenerator(TMEGeneratorOptions* opts);
        virtual ~FixedWindowMEGenerator(){};

        virtual int ProcessPulses(MuonEventList& muonEventsOut,const SourceDetPulseMap& detectorPulsesIn);

    private:
        int Init(const SourceDetPulseMap&);
        void Reset();
        void PulsesInWindow(double central_time, double window,
                DetectorPulseList::const_iterator& start,
                DetectorPulseList::const_iterator& stop,
                const DetectorPulseList::const_iterator& end);

    private:
        double fMuonWindow, fEventWindow;
        Detector_t fMuSc;
        SourceList fDetectors;

};

#endif //FIXEDWINDOW_H__
