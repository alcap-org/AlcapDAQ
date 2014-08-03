#ifndef CFTIME_H__
#define CFTIME_H__

#include "TSetupData.h"
#include "TVAnalysedPulseGenerator.h"
#include "definitions.h"
#include "TFile.h"
#include "utils/TAPAlgorithms.h"

class CFTimeAPGenerator:public TVAnalysedPulseGenerator {

 public:
  CFTimeAPGenerator(TAPGeneratorOptions* opts);
  virtual ~CFTimeAPGenerator(){};

 public:
   virtual int ProcessPulses( const PulseIslandList&,AnalysedPulseList&);

   // This function should return true if this generator could break up a TPI
   // into more than one TAP
   virtual bool MayDivideTPIs(){return true;};

 private:
   /// \brief
   /// The algorithms that will be used by this generator
   Algorithm::ConstantFractionTime fConstantFractionTime;
};

#endif //CFTIME_H__
