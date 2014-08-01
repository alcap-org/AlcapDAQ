#ifndef PASSTHROUGHDPGENERATOR_H_
#define PASSTHROUGHDPGENERATOR_H_

#include "TVDetectorPulseGenerator.h"

class PassThroughDPGenerator:public TVDetectorPulseGenerator{

    public:
        PassThroughDPGenerator(TDPGeneratorOptions* opts);
        virtual ~PassThroughDPGenerator(){};
        virtual bool ChecksForPileUp()const {return false;};

    public:
        virtual int ProcessPulses( DetectorPulseList& output);
};
#endif //PASSTHROUGHDPGENERATOR_H_
