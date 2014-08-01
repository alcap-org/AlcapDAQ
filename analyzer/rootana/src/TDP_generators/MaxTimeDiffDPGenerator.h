#ifndef MAXTIMEDIFFDPGENERATOR_H_
#define MAXTIMEDIFFDPGENERATOR_H_

#include "TVDetectorPulseGenerator.h"

class MaxTimeDiffDPGenerator:public TVDetectorPulseGenerator{

    public:
        MaxTimeDiffDPGenerator(TDPGeneratorOptions* opts);
        virtual ~MaxTimeDiffDPGenerator(){};
        virtual bool ChecksForPileUp()const {return false;};

    public:
        virtual int ProcessPulses(const TSetupData* setup,const std::string& detector, 
                const AnalysedPulseList* fast, const AnalysedPulseList* slow,
                DetectorPulseList& output);
        int CopyAnalToDet(const AnalysedPulseList* fast_pulses,const std::string&, DetectorPulseList& output);

    private:
        double fTimeDifference;
};
#endif //MAXTIMEDIFFDPGENERATOR_H_
