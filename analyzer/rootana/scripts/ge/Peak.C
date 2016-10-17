#include "Peak.h"
#include "Run.h"

#include "TMath.h"
#include "TF1.h"
#include "TFitResult.h"
#include "TPad.h"
#include "TH1.h"

Peak::Peak(Double_t e, Double_t i, Double_t adc)
  : e(e), i(i) {
  char str[64];
  sprintf(str, "p%d", int(e));
  fnc = new TF1(str, "gaus(0) + pol1(3)");
  fnc->SetParNames("Amp", "Mu", "Sigma", "BG0", "BG1");
  fnc->SetParameter(1, adc);
}

Double_t Peak::E() {
  return e;
}
Double_t Peak::LogE() {
  return TMath::Log(e);
}
Double_t Peak::I() {
  return i;
}
Double_t Peak::N() {
  return TMath::Sqrt(2.*TMath::Pi())*A()*Std();
}
Double_t Peak::NErr() {
  return N() * TMath::Sqrt(TMath::Power(AErr()/A(), 2) +
			   TMath::Power(StdErr()/Std(), 2));
}
Double_t Peak::Eff(Run run) {
  return N()/(i*run.Decays());
}
Double_t Peak::EffErr(Run run) {
  return NErr()/N()*Eff(run);
}
Double_t Peak::Chi2() {
  return fitresult->Chi2();
}
Double_t Peak::Ndf() {
  return fitresult->Ndf();
}
bool Peak::ValidFit(){
  return fitresult->IsValid();
}
Double_t Peak::A(){
  return fnc->GetParameter(0);
}
Double_t Peak::ADC() {
  return fnc->GetParameter(1);
}
Double_t Peak::Std() {
  return fnc->GetParameter(2);
}
Double_t Peak::Bg0() {
  return fnc->GetParameter(3);
}
Double_t Peak::Bg1() {
  return fnc->GetParameter(4);
}
Double_t Peak::AErr()   {
  return fnc->GetParError(0);
}
Double_t Peak::ADCErr() {
  return fnc->GetParError(1);
}
Double_t Peak::StdErr() {
  return fnc->GetParError(2);
}
Double_t Peak::Bg0Err() {
  return fnc->GetParError(3);
}
Double_t Peak::Bg1Err() {
  return fnc->GetParError(4);
}
void Peak::A(Double_t val) {
  fnc->SetParameter(0, val);
}
void Peak::ADC(Double_t val) {
  fnc->SetParameter(1, val);
}
void Peak::Std(Double_t val) {
  fnc->SetParameter(2, val);
}
void Peak::Bg0(Double_t val) {
  fnc->SetParameter(3, val);
}
void Peak::Bg1(Double_t val) {
  fnc->SetParameter(4, val);
}
void Peak::Fit(TH1* h, bool print) {
  Double_t mu  = fnc->GetParameter(1);
  Std(2.5 + 0.00127*mu); // Data snooping
  fnc->SetParLimits(2, 0., 20.);
  h->GetXaxis()->SetRange(mu-6.*Std(), mu+6.*Std());
  fitresult = h->Fit(fnc, "+SMEQ");
  if (print && gPad) {
    char str[32]; sprintf(str, "%dkeV.png", (int)e);
    gPad->Print(str);
  }
  h->GetXaxis()->SetRange();
}
