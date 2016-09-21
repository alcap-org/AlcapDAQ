#ifndef PEAK_H
#define PEAK_H

class Run;

#include "TFitResultPtr.h"
class TF1;
class TH1;

class Peak {
  Double_t e; // Energy
  Double_t i; // Intensity
  TF1* fnc;
  TFitResultPtr fitresult;
 public:
  Peak(Double_t e = 0., Double_t i = 0., Double_t adc = 0.);
  Double_t E();
  Double_t LogE();
  Double_t I();
  Double_t N();
  Double_t NErr();
  Double_t Eff(Run);
  Double_t EffErr(Run);
  Double_t Chi2();
  Double_t Ndf();
  bool ValidFit();
  Double_t A();
  Double_t ADC();
  Double_t Std();
  Double_t Bg0();
  Double_t Bg1();
  Double_t AErr();
  Double_t ADCErr();
  Double_t StdErr();
  Double_t Bg0Err();
  Double_t Bg1Err();
  void A(Double_t);
  void ADC(Double_t);
  void Std(Double_t);
  void Bg0(Double_t);
  void Bg1(Double_t);
  void Fit(TH1*, bool print = false);
};

#endif /* PEAK_H */
