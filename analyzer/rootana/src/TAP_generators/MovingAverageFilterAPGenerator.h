#ifndef MOVINGAVERAGEFILTERAPGENERATOR_H_FWS6D5E3
#define MOVINGAVERAGEFILTERAPGENERATOR_H_FWS6D5E3

#include "TSetupData.h"
#include "TVAnalysedPulseGenerator.h"
#include "definitions.h"

#include "TAPAlgorithms.h"
#include "PulseCandidateFinder.h"

class MovingAverageFilterAPGenerator:public TVAnalysedPulseGenerator {

 public:
  MovingAverageFilterAPGenerator(TAPGeneratorOptions* opts);
  virtual ~MovingAverageFilterAPGenerator();

 public:
   virtual int ProcessPulses( const PulseIslandList&,AnalysedPulseList&);

   // This function should return true if this generator could break up a TPI
   // into more than one TAP
   virtual bool MayDivideTPIs(){return true;};

 private:
   void DrawPulse(int original, int pulse_timestamp, int n_pulse_samples);
   void MakeTAPsWithPCF(int tpi_ID,const TPulseIsland* tpi, AnalysedPulseList& analysedList);
   void AnalyseOneTpi(int tpi_ID,const TPulseIsland* tpi, AnalysedPulseList& analysedList);

   // The pulse candidate finder that we will use
   PulseCandidateFinder* fPulseCandidateFinder;
   int fWindowWidth;
   double fConstantFraction;

   // A vector of sub-pulses so we don't need to declare each time we call
   // ProcessPulses
   PulseIslandList fSubPulses;
};
#endif /* end of include guard: MOVINGAVERAGEFILTERAPGENERATOR_H_FWS6D5E3 */
