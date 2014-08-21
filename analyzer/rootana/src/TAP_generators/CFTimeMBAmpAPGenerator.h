#ifndef CFTIMEMBAMP_H__
#define CFTIMEMBAMP_H__

#include "TSetupData.h"
#include "TVAnalysedPulseGenerator.h"
#include "definitions.h"
#include "TFile.h"
#include "TAPAlgorithms.h"

class CFTimeMBAmpAPGenerator:public TVAnalysedPulseGenerator {

 public:
  CFTimeMBAmpAPGenerator(TAPGeneratorOptions* opts);
  virtual ~CFTimeMBAmpAPGenerator(){};

 public:
  virtual int ProcessPulses( const PulseIslandList&,AnalysedPulseList&);

  // This function should return true if this generator could break up a TPI
  // into more than one TAP
  virtual bool MayDivideTPIs(){return true;};

 private:
  /// \brief
  /// The algorithms that will be used by this generator
  //@{
  Algorithm::ConstantFractionTime* fConstantFractionTime;
  Algorithm::MaxBinAmplitude* fMaxBinAmplitude;
  //@}

  /// \brief
  /// The value for the constant fraction parameter
  double fConstantFractionParam;
  bool fDontShiftTime;
};

#endif //CFTIMEMBAMP_H__
