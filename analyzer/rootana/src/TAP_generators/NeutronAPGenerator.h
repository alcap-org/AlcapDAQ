#ifndef NEUTRON_H__
#define NEUTRON_H__

#include "TSetupData.h"
#include "TVAnalysedPulseGenerator.h"
#include "definitions.h"

#include "TAPAlgorithms.h"
#include "PulseCandidateFinder.h"

namespace Algorithm{class SimpleIntegral ;}

class NeutronAPGenerator:public TVAnalysedPulseGenerator {

 public:
  NeutronAPGenerator(TAPGeneratorOptions* opts);
  virtual ~NeutronAPGenerator();

 public:
  virtual int ProcessPulses(const PulseIslandList&, AnalysedPulseList&);

  virtual bool MayDivideTPIs(){return true;};

  static const char* TapType();

 private:

  bool fStartTailAsFraction;
  double fStartIntegral;
  double fStopIntegral;
  double fStartTail;
  double fPedestal;
  double fPolarity;

  //algorithms that this generator will use
  const Algorithm::ConstantFractionTime fConstantFractionTime;
  const Algorithm::MaxBinAmplitude fMaxBinAmplitude;
  Algorithm::IntegralRatio fIntegralRatioAlgo;

  PulseIslandList fSubPulses;

  void DrawPulse(int original, int pulse_timestamp, int n_pulse_samples);


};

#endif
