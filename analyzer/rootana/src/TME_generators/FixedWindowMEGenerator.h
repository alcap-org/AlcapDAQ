#ifndef FIXEDWINDOW_H__
#define FIXEDWINDOW_H__

#include "TSetupData.h"
#include "TVMuonEventGenerator.h"
#include "definitions.h"

#include "SetupNavigator.h"

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

        virtual int ProcessPulses(MuonEventList& muonEventsOut,
                const SourceDetPulseMap& detectorPulsesIn);

    private:
        int Init(const SourceDetPulseMap&);
        void Reset();
        void AddPulsesInWindow(TMuonEvent* tme,
			       double window, Detector_t& detector);

    private:
	bool fInit;
        double fEventWindow;
        SourceList fMuonCounters;
        SourceList fOtherDetectors;

	double fCentralMuonEnergyCut;
};

#endif //FIXEDWINDOW_H__
