#ifndef FIRSTCOMPLETE_H__
#define FIRSTCOMPLETE_H__

#include "TSetupData.h"
#include "TVAnalysedPulseGenerator.h"
#include "definitions.h"

#include "utils/TAPAlgorithms.h"

class FirstCompleteAPGenerator:public TVAnalysedPulseGenerator {

 public:
  FirstCompleteAPGenerator(TAPGeneratorOptions* opts);
  virtual ~FirstCompleteAPGenerator(){};

 public:
   virtual int ProcessPulses( const PulseIslandList&,AnalysedPulseList&);

   // This function should return true if this generator could break up a TPI
   // into more than one TAP
   virtual bool MayDivideTPIs(){return true;};

 private:
   // The algorithms that this generator will use
   Algorithm::MaxBinAmplitude fMaxBinAmplitude;
   Algorithm::ConstantFractionTime fConstantFractionTime;
   Algorithm::SimpleIntegral fSimpleIntegral;
};

#endif //FIRSTCOMPLETE_H__
