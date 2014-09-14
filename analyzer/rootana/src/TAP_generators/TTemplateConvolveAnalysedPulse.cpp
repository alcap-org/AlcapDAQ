#include "TTemplateConvolveAnalysedPulse.h"
#include "InterpolatePulse.h"
#include <algorithm>
#include "debug_tools.h"

namespace{
  struct unique_n{
    int operator() () { return ++current; }
    int current;
    unique_n():current(-1){}
  };
}

TTemplateConvolveAnalysedPulse::TTemplateConvolveAnalysedPulse(
       const IDs::source& sourceID, const TPulseIslandID& parentID, const TPulseIsland* parentTPI):
            TAnalysedPulse(sourceID,parentID,parentTPI){}

TTemplateConvolveAnalysedPulse::~TTemplateConvolveAnalysedPulse(){
}

void TTemplateConvolveAnalysedPulse::Draw(const TH1F* tpi_pulse)const{
   if(!tpi_pulse) return;

   // prepare labels for histoagrams
   const int n_samples=fEnergySamples.size();
   unique_n uniq;
   double labels[n_samples];
   std::generate_n(labels,n_samples, uniq);

   std::string name(tpi_pulse->GetName());
   /// make the energy (template convolution) waveform
   TH1F* energy_hist=new TH1F((name+"_energy").c_str(),
                               "Convolution with template",
                               fEnergySamples.size(),0,fEnergySamples.size());
   energy_hist->FillN(n_samples,labels,fEnergySamples.data());

   /// make the time (second derivative of the energy) waveform
   TH1F* time_hist=new TH1F((name+"_time").c_str(),
                               "Second derivative of convolution with template",
                               fTimeSamples.size(),0,fTimeSamples.size());
   time_hist->FillN(fTimeSamples.size(),labels,fTimeSamples.data());
    
   //TMarker* marker=new TMarker(GetTime(), GetAmplitude()+GetPedestal()+10, 23);
   //marker->SetMarkerColor(kRed);
   //tap_pulse->GetListOfFunctions()->Add(marker);

   //tap_pulse->Write();
   
}
