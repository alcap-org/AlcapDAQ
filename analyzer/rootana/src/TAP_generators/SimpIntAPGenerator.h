#ifndef SIMPINT_H__
#define SIMPINT_H__

#include "TSetupData.h"
#include "TVAnalysedPulseGenerator.h"
#include "definitions.h"

#include "TAPAlgorithms.h"

class SimpIntAPGenerator:public TVAnalysedPulseGenerator {

 public:
  SimpIntAPGenerator(TAPGeneratorOptions* opts);
  virtual ~SimpIntAPGenerator(){};


 public:
   virtual int ProcessPulses( const PulseIslandList&,AnalysedPulseList&);
   virtual bool MayDivideTPIs(){return false;};


 private:
   // The algorithms that this generator will use
   Algorithm::SimpleIntegral fSimpleIntegral;
};

#endif //SIMPLEINTEGRATOR_H__
