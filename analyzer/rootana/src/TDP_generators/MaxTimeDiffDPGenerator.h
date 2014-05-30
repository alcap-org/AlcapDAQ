#ifndef MAXTIMEDIFFDPGENERATOR_H_
#define MAXTIMEDIFFDPGENERATOR_H_

#include "TVDetectorPulseGenerator.h"

class MaxTimeDiffDPGenerator:public TVDetectorPulseGenerator{

 public:
  MaxTimeDiffDPGenerator(): TVDetectorPulseGenerator(){};
  virtual ~MaxTimeDiffDPGenerator(){};
  virtual bool ChecksForPileUp()const {return false;};

 public:
  virtual void ProcessPulses(const TSetupData* setup,const std::string& detector, 
		  const AnalysedPulseList* fast, const AnalysedPulseList* slow,
		  DetectorPulseList& output);

};
#endif //MAXTIMEDIFFDPGENERATOR_H_
