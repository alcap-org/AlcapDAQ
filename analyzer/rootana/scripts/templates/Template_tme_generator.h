#ifndef %MODULE%_H__
#define %MODULE%_H__

#include "TSetupData.h"
#include "TVMuonEventGenerator.h"
#include "definitions.h"

class %module%MEGenerator:public TVMuonEventGenerator {

    public:
        %module%MEGenerator(TMEGeneratorOptions* opts);
        virtual ~%module%MEGenerator(){};

    public:
        virtual int ProcessPulses(MuonEventList& muonEventsOut,const StringDetPulseMap& detectorPulsesIn);

};

#endif //%MODULE%_H__
