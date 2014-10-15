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

   static const char* TapType();

   // This function should return true if this generator could break up a TPI
   // into more than one TAP
   virtual bool MayDivideTPIs(){return false;};
 private:
        bool fStartTailAsFraction;
        double fStartIntegral;
        double fStopIntegral;
        double fStartTail;
        double fPedestal;
        double fPolarity;
        Algorithm::IntegralRatio fIntegralRatioAlgo;
        const Algorithm::MaxBinAmplitude fMaxBinAmplitude;

};

#endif //INTEGRALRATIO_H__
