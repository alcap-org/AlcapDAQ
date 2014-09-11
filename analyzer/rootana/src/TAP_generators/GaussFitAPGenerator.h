#ifndef GAUSSFIT_H__
#define GAUSSFIT_H__

#include "TSetupData.h"
#include "TVAnalysedPulseGenerator.h"
#include "definitions.h"
class TF1;

class GaussFitAPGenerator:public TVAnalysedPulseGenerator {

 /// an enum to help keep the parameters being fitted in order
 enum FitParams{ kAmplitude=0, kTime , kPedestal , kGradient , kWidth ,kNumParams};
 /// a struct to prevent needing to rewrite the signature of FitPulse each time
 /// we add something
 struct FittedVals{
     int status;
     double chi2;
     double value[kNumParams];
     double error[kNumParams];
 };

 public:
  GaussFitAPGenerator(TAPGeneratorOptions* opts);
  virtual ~GaussFitAPGenerator(){};

 public:
   virtual int ProcessPulses( const PulseIslandList&,AnalysedPulseList&);

   // This function should return true if this generator could break up a TPI
   // into more than one TAP
   virtual bool MayDivideTPIs(){return false;};
   void FitPulse(const TPulseIsland* tpi,const int& id,FittedVals& tap);

   static const char* TapType();

 private:
    double fMean, fWidth, fAmplitude, fPedestal, fGradient; 
    TF1* fFitFunc;
    static const char* fFitName;
};

#endif //GAUSSFIT_H__
