#ifndef FIRSTCOMPLETE_H__
#define FIRSTCOMPLETE_H__

#include "TSetupData.h"
#include "TVAnalysedPulseGenerator.h"
#include "definitions.h"

#include "TAPAlgorithms.h"
#include "PulseCandidateFinder.h"

class FirstCompleteAPGenerator:public TVAnalysedPulseGenerator {

 public:
  FirstCompleteAPGenerator(TAPGeneratorOptions* opts);
  virtual ~FirstCompleteAPGenerator();

 public:
   virtual int ProcessPulses( const PulseIslandList&,AnalysedPulseList&);

   // This function should return true if this generator could break up a TPI
   // into more than one TAP
   virtual bool MayDivideTPIs(){return true;};

 private:
   void DrawPulse(int original, int pulse_timestamp, int n_pulse_samples);

   // The algorithms that this generator will use
   Algorithm::MaxBinAmplitude fMaxBinAmplitude;
   Algorithm::ConstantFractionTime fConstantFractionTime;
   Algorithm::SimpleIntegral fSimpleIntegral;

   // The pulse candidate finder that we will use
   PulseCandidateFinder* fPulseCandidateFinder;

   /// \brief
   /// We need to overload the SetChannel function in TVAnalysedPulseGenerator so that the PCF can be created once, when the channel is known
   virtual void SetChannel(const std::string& det){ TVAnalysedPulseGenerator::SetChannel(det); fPulseCandidateFinder =  new PulseCandidateFinder(GetChannel.str(), fOpts); }

   // The module options
   TAPGeneratorOptions* fOpts;

   // A vector of sub-pulses so we don't need to declare each time we call
   // ProcessPulses
   PulseIslandList fSubPulses;
};

#endif //FIRSTCOMPLETE_H__
