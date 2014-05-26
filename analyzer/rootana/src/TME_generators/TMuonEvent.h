#ifndef TMuonEvent_hh_
#define TMuonEvent_hh_

#include "TAnalysedPulse.h"

struct TDetectorEvent{
        TDetectorEvent():fast(NULL),slow(NULL){};

        TAnalysedPulse* fast;
        TAnalysedPulse* slow;
};

class TMuonEvent{
typedef std::map<std::string, TDetectorEvent> TEventList;
        public:
                TMuonEvent();
                ~TMuonEvent();

                void AddSlowPulse(std::string detector,TAnalysedPulse* pulse){
                        fDetectorPulses[detector].slow=pulse;
                };
                void AddFastPulse(std::string detector,TAnalysedPulse* pulse){
                        fDetectorPulses[detector].fast=pulse;
                };

                TAnalysedPulse* GetSlowPulse(std::string detector)const{
                        const TDetectorEvent* det=GetDetectorEvent(detector);
                        if(det) return det->slow;
                        return NULL;
                }
                TAnalysedPulse* GetFastPulse(std::string detector)const{
                        const TDetectorEvent* det=GetDetectorEvent(detector);
                        if(det) return det->fast;
                        return NULL;
                }
                const TDetectorEvent* GetDetectorEvent(std::string detector)const{
                        TEventList::const_iterator det=fDetectorPulses.find(detector);
                        if(det!=fDetectorPulses.end()) return &det->second;
                        return NULL;
                }

                // Check if a pulse was registered in the named detector for
                // this event. Use the second argument to specify whether the
                // pulse should be a fast 'F' or slow pulse 'S' 
                // any channel 'A', or both 'B'.
                bool HasPulseIn(std::string detector,const char& type='A')const;


        private:
                TEventList fDetectorPulses;

};
#endif // TMuonEvent_hh_
