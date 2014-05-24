#ifndef MAXTIMEDIFFDPGENERATOR_H_
#define MAXTIMEDIFFDPGENERATOR_H_

#include "TVDetectorPulseGenerator.h"

class MaxTimeDiffDPGenerator:public TVDetectorPulseGenerator{

 public:
  MaxTimeDiffDPGenerator(){};
  virtual ~MaxTimeDiffDPGenerator(){};

 public:
  virtual void ProcessPulses(const TSetupData* setup, const
		  AnalysedPulseList_t* fast, const AnalysedPulseList_t* slow,
		  DetectorPulseList_t& output);

};
#endif //MAXTIMEDIFFDPGENERATOR_H_
