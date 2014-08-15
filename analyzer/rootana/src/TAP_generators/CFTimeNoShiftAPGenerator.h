#ifndef CFTIMENOSHIFT_H__
#define CFTIMENOSHIFT_H__

#include "TSetupData.h"
#include "TVAnalysedPulseGenerator.h"
#include "definitions.h"
#include "TFile.h"
#include "TAPAlgorithms.h"

class CFTimeNoShiftAPGenerator:public TVAnalysedPulseGenerator {

 public:
  CFTimeNoShiftAPGenerator(TAPGeneratorOptions* opts);
  virtual ~CFTimeNoShiftAPGenerator(){};

 public:
  virtual int ProcessPulses( const PulseIslandList&,AnalysedPulseList&);

  // This function should return true if this generator could break up a TPI
  // into more than one TAP
  virtual bool MayDivideTPIs(){return true;};

 private:
  /// \brief
  /// The algorithms that will be used by this generator
  Algorithm::ConstantFractionTime fConstantFractionTime;
  Algorithm::MaxBinAmplitude fMaxBinAmplitude;
};

#endif //CFTIMENOSHIFT_H__
