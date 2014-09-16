#include "TTemplateConvolveAnalysedPulse.h"
#include "TLine.h"
#include "InterpolatePulse.h"
#include <algorithm>
#include "debug_tools.h"

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
   TLine* line=new TLine(GetTime(), 0, GetTime(), GetAmplitude()+10);
   line->SetLineColor(kRed);
   energy_hist->GetListOfFunctions()->Add(line);
   time_hist->GetListOfFunctions()->Add(line->Clone());
   tpi_pulse->GetListOfFunctions()->Add(line->Clone());

   //tap_pulse->Write();
   
}
