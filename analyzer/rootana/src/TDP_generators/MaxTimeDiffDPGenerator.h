#ifndef MAXTIMEDIFFDPGENERATOR_H_
#define MAXTIMEDIFFDPGENERATOR_H_

#include "TVDetectorPulseGenerator.h"

class MaxTimeDiffDPGenerator:public TVDetectorPulseGenerator{

    public:
        MaxTimeDiffDPGenerator(TDPGeneratorOptions* opts);
        virtual ~MaxTimeDiffDPGenerator(){};
        virtual bool ChecksForPileUp()const {return false;};

    public:
        virtual int ProcessPulses( DetectorPulseList& output);

    private:
        double fTimeDifference;
};
#endif //MAXTIMEDIFFDPGENERATOR_H_
