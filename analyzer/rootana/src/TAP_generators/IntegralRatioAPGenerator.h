#ifndef INTEGRALRATIO_H__
#define INTEGRALRATIO_H__

#include "TSetupData.h"
#include "TVAnalysedPulseGenerator.h"
#include "definitions.h"
namespace Algorithm{class SimpleIntegral ;}

class IntegralRatioAPGenerator:public TVAnalysedPulseGenerator {

 public:
  IntegralRatioAPGenerator(TAPGeneratorOptions* opts);
  virtual ~IntegralRatioAPGenerator();

 public:
   virtual int ProcessPulses( const PulseIslandList&,AnalysedPulseList&);

   // This function should return true if this generator could break up a TPI
   // into more than one TAP
   virtual bool MayDivideTPIs(){return false;};
 private:
        double fStartIntegral;
        double fStopIntegral;
        double fStartTail;
        double fPedestal;
        double fPolarity;
        Algorithm::SimpleIntegral fFullIntegrator;
        Algorithm::SimpleIntegral fTailIntegrator;

};

#endif //INTEGRALRATIO_H__
