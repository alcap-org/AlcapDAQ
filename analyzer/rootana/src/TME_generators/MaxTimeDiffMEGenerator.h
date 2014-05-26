#ifndef MAXTIMEDIFFMEGENERATOR_H_
#define MAXTIMEDIFFMEGENERATOR_H_

#include "TVMuonEventGenerator.h"

class MaxTimeDiffMEGenerator:public TVMuonEventGenerator{

 public:
  MaxTimeDiffMEGenerator(TVMuonEventGenerator::options* opts): TVMuonEventGenerator(opts){};
  virtual ~MaxTimeDiffMEGenerator(){};

 public:
  virtual int ProcessPulses(MuonCentredTree_t& muonEventsOut,const BankDetPulseList_t& detectorPulsesIn);

};
#endif //MAXTIMEDIFFMEGENERATOR_H_
