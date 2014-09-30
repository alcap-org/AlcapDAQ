#include "TTemplateConvolveAnalysedPulse.h"
#include "TLine.h"
#include "InterpolatePulse.h"
#include <algorithm>
#include "debug_tools.h"
#include <TF1.h>
#include <TPaveText.h>

TTemplateConvolveAnalysedPulse::TTemplateConvolveAnalysedPulse():TAnalysedPulse(),
      fNPeaks(0), fPeakRank(0), fIntegralRatio(0){}

TTemplateConvolveAnalysedPulse::TTemplateConvolveAnalysedPulse(
       const IDs::source& sourceID, const TPulseIslandID& parentID, const TPulseIsland* parentTPI):
            TAnalysedPulse(sourceID,parentID,parentTPI),
      fNPeaks(0), fPeakRank(0), fIntegralRatio(0)
{}

TTemplateConvolveAnalysedPulse::~TTemplateConvolveAnalysedPulse(){
}

void TTemplateConvolveAnalysedPulse::Draw(const TH1F* tpi_pulse)const{

   // make sure theres a TPI
   if(!tpi_pulse) return;

   std::string name(tpi_pulse->GetName());

   /// make the energy (template convolution) waveform
   TH1F* energy_hist=functions::VectorToHist(fEnergySamples,name+"_energy","Convolution with template");

   /// make the time (second derivative of the energy) waveform
   TH1F* time_hist=functions::VectorToHist(fTimeSamples,name+"_time","First derivative of convolution with template");
    
   //TMarker* marker=new TMarker(GetTime(), GetAmplitude()+10, 23);
   TLine* line=new TLine(GetTime(), GetPedestal(), GetTime(), GetAmplitude()+GetPedestal());
   line->SetLineColor(kRed);
   tpi_pulse->GetListOfFunctions()->Add(line->Clone());

   line->SetX1(fTimeOffset);
   line->SetX2(fTimeOffset);
   line->SetY1(0);
   line->SetY2(fAmplitudeScale);

   energy_hist->GetListOfFunctions()->Add(line);
   time_hist->GetListOfFunctions()->Add(line->Clone());

   TPaveText* text_b=new TPaveText(0.7,0.60,0.9,0.9,"NB NDC");
   text_b->AddText(Form("A = %g",GetAmplitude()));
   text_b->AddText(Form("t = %g",GetTime()));
   text_b->AddText(Form("a = %g",fQuad));
   text_b->AddText(Form("b = %g",fLinear));
   text_b->AddText(Form("c = %g",fConstant));
   text_b->SetFillColor(kWhite);
   text_b->SetBorderSize(1);
   energy_hist->GetListOfFunctions()->Add(text_b);

   TF1* fit=new TF1("Fit","[0]*(x-[3])**2+[1]*(x-[3])+[2]", 0 , energy_hist->GetNbinsX());
   fit->SetParameter(0,fQuad);
   fit->SetParameter(1,fLinear);
   fit->SetParameter(2,fConstant);
   fit->SetParameter(3,fTimeOffset );
   energy_hist->GetListOfFunctions()->Add(fit);
   energy_hist->Write();
   time_hist->Write();

   //tap_pulse->Write();
   
}
