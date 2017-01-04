#ifndef FIRSTCOMPLETE_RoughSyncPulseCut_H__
#define FIRSTCOMPLETE_RoughSyncPulseCut_H__

#include "TSetupData.h"
#include "TVAnalysedPulseGenerator.h"
#include "definitions.h"

#include "TAPAlgorithms.h"
#include "PulseCandidateFinder.h"

class FirstCompleteAPGenerator_RoughSyncPulseCut:public TVAnalysedPulseGenerator {

 public:
  FirstCompleteAPGenerator_RoughSyncPulseCut(TAPGeneratorOptions* opts);
  virtual ~FirstCompleteAPGenerator_RoughSyncPulseCut();

 public:
   virtual int ProcessPulses( const PulseIslandList&,AnalysedPulseList&);

   // This function should return true if this generator could break up a TPI
   // into more than one TAP
   virtual bool MayDivideTPIs(){return true;};

 private:
   void DrawPulse(int original, int pulse_timestamp, int n_pulse_samples);
   void MakeTAPsWithPCF(int tpi_ID,const TPulseIsland* tpi, AnalysedPulseList& analysedList);
   void AnalyseOneTpi(int tpi_ID,const TPulseIsland* tpi, AnalysedPulseList& analysedList);

   // The algorithms that this generator will use
   const Algorithm::MaxBinAmplitude fMaxBinAmplitude;
   const Algorithm::ConstantFractionTime fConstantFractionTime;
   const Algorithm::SimpleIntegral fSimpleIntegral;

   // The pulse candidate finder that we will use
   PulseCandidateFinder* fPulseCandidateFinder;

   // A vector of sub-pulses so we don't need to declare each time we call
   // ProcessPulses
   PulseIslandList fSubPulses;
};

#endif //FIRSTCOMPLETE_H__
