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
   virtual bool MayDivideTPIs(){return false;};

 private:
   void DrawPulse(int original, int pulse_timestamp, int n_pulse_samples);
   void Analyse(int tpi_ID,const TPulseIsland* tpi,
       AnalysedPulseList& analysedList);

   int fWindowWidth;
   double fConstantFraction;
};
#endif /* end of include guard: MOVINGAVERAGEFILTERAPGENERATOR_H_FWS6D5E3 */
