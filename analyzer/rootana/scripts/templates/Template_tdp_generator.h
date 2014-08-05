#ifndef %MODULE%DPGENERATOR_H_
#define %MODULE%DPGENERATOR_H_

#include "TVDetectorPulseGenerator.h"

class %module%DPGenerator:public TVDetectorPulseGenerator{

    public:
        %module%DPGenerator(TDPGeneratorOptions* opts);
        virtual ~%module%DPGenerator(){};
        virtual bool ChecksForPileUp()const {return false;};

    public:
        virtual int ProcessPulses( DetectorPulseList& output);
};
#endif //%MODULE%DPGENERATOR_H_
