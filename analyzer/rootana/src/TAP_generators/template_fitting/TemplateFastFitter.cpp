#include "TemplateFastFitter.h"
#include "MultiHistogramFastFitFCN.h"
#include "SetupNavigator.h"
#include "EventNavigator.h"
#include "debug_tools.h"
#include <iostream>

#include "TMath.h"
#include "TDirectory.h"

using std::cout;
using std::endl;

TemplateFastFitter::TemplateFastFitter(const IDs::channel& ch): 
   fMinuitFitter(NULL), fFitFCN(NULL), fChannel(ch),fPedestal(0), fRefineFactor(0) ,fTemplates(2){

  // Calculate the max ADC value
  fMinADC = 0;
  fMaxADC = EventNavigator::Instance().GetSetupRecord().GetMaxADC(fChannel);
}

void TemplateFastFitter::SetTemplate(TTemplate* tpl){
  fTemplate=tpl;
}

void TemplateFastFitter::Init(){
  if(fFitFCN || fMinuitFitter) {
     cout<< "TemplateFastFitter: Warning: trying to initialize fitter which was already initialised!"<<endl;
     return;
  }

  fRefineFactor=1;
  fTemplate->RebinToOriginalSampling();
  fTemplate->NormaliseToAmplitude();

  fFitFCN=new MultiHistogramFastFitFCN(fRefineFactor);
  fFitFCN->SetTemplateHist(fTemplate->GetHisto());
  fFitFCN->Initialise();
 
  fMinuitFitter = new TFitterMinuit(2);
  fMinuitFitter->SetMinuitFCN(fFitFCN);
  fMinuitFitter->SetPrintLevel(-1); // set the debug level to quiet (-1=quiet, 0=normal, 1=verbose)
}

TemplateFastFitter::~TemplateFastFitter() {
}

int TemplateFastFitter::FitWithOne( const TH1D* hPulse){
  // Prepare for minimizations
  fMinuitFitter->Clear();
  fFitFCN->SetPulseHist(hPulse);
  fMinuitFitter->CreateMinimizer(TFitterMinuit::kMigrad);

  fFitFCN->SetFitNTemplates(1);
  int guess = hPulse->GetMaximumBin() - fTemplate->GetTime();
  fMinuitFitter->SetParameter(0,"time_template_",guess,-1,0,hPulse->GetNbinsX());
  fMinuitFitter->SetParameter(1,"time_template_2",0,0,0,1);
  fMinuitFitter->FixParameter(1);
  fMinuitFitter->SetParameter(0,"time_template_",guess,-1,0,hPulse->GetNbinsX());
  return Fit(hPulse);
}

int TemplateFastFitter::FitWithTwo( const TH1D* hPulse){
  // Prepare for minimizations
  fMinuitFitter->Clear();
  fFitFCN->SetPulseHist(hPulse);
  //fMinuitFitter->CreateMinimizer(TFitterMinuit::kMigrad);

  fFitFCN->SetFitNTemplates(2);
  fMinuitFitter->SetParameter(0,"time_template_",0,10,0,hPulse->GetNbinsX());
  fMinuitFitter->SetParameter(1,"time_template_2",10,10,0,hPulse->GetNbinsX());
  fMinuitFitter->ReleaseParameter(1);
  return Fit(hPulse);
}

int TemplateFastFitter::Fit(const TH1* hPulse){

  // Minimize and notify if there was a problem
  int status = fMinuitFitter->Minimize(1000); // set limit of 1000 calls to FCN

  // Refit for the best value we found
  // Ensures we get the right values of amplitude etc
  std::vector<double> parameters(2);
  for(int i=0; i<2; ++i){
    parameters[i]=fMinuitFitter->GetParameter(i); 
  }
  (*fFitFCN)(parameters);

  // Store the Chi2 and degrees of freedom
  // get the chi-2
  fChi2 = fFitFCN->GetChi2();
  fNDoF = fFitFCN->GetNDoF();
  fPedestal = fFitFCN->GetPedestal();

  for(int i=0; i<2; ++i){
    SetAmplitudeSF(i, fFitFCN->GetAmplitudeScaleFactor(i));
    SetTimeOffset(i,fMinuitFitter->GetParameter(i)); 
  }


  return status; // return status for the calling module to look at
}

void TemplateFastFitter::SaveSumHistograms(TDirectory* dir){
   fFitFCN->GetHistTpl()->SetDirectory(dir);
   fFitFCN->GetHistSum()->SetDirectory(dir);
   fFitFCN->GetHistSumSq()->SetDirectory(dir);
   fFitFCN->GetHistSumCross()->SetDirectory(dir);
}
