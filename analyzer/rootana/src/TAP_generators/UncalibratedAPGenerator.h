#ifndef UNCALIBRATED_H__
#define UNCALIBRATED_H__

#include "TSetupData.h"
#include "TVAnalysedPulseGenerator.h"
#include "TAPAlgorithms.h"
#include "definitions.h"

class UncalibratedAPGenerator:public TVAnalysedPulseGenerator {

 public:
  UncalibratedAPGenerator(TAPGeneratorOptions* opts);
  virtual ~UncalibratedAPGenerator(){};

 public:
   virtual int ProcessPulses( const PulseIslandList&,AnalysedPulseList&);

   // This function should return true if this generator could break up a TPI
   // into more than one TAP
   virtual bool MayDivideTPIs(){return false;};

   // The algorithms that this generator will use
   Algorithm::NoShiftTime fNoShiftTime;

};

#endif //UNCALIBRATED_H__
