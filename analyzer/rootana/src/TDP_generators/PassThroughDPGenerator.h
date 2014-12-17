#ifndef PASSTHROUGHDPGENERATOR_H_
#define PASSTHROUGHDPGENERATOR_H_

#include "TVDetectorPulseGenerator.h"

class PassThroughDPGenerator:public TVDetectorPulseGenerator{

    public:
        PassThroughDPGenerator(TDPGeneratorOptions* opts);
        virtual ~PassThroughDPGenerator(){};
        virtual bool ChecksForPileUp()const {return false;};
        static const IDs::generator& GetStaticId(){return fStaticId;};

    public:
        virtual int ProcessPulses( DetectorPulseList& output);

    private:
        bool fUseFast;
        static IDs::generator fStaticId;
};
#endif //PASSTHROUGHDPGENERATOR_H_
