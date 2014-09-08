#include "TTemplateFitAnalysedPulse.h"
#include "TF1.h"
#include "TH1F.h"
#include "Functions.h"

void TTemplateFitAnalysedPulse::Draw(const TH1F* tpi_pulse)const{
   if(tpi_pulse) {
      std::string name=tpi_pulse->GetName();
      name+="_templateAP";
      const TH1* tpl = GetTemplate()->GetHisto();
      TH1F* tap_pulse=(TH1F*) tpi_pulse->Clone(name.c_str());
      for ( int i=0; i<tap_pulse->GetNbinsX(); i++){
         // deduce right bin to look in
         int tpl_bin=i+ GetTime();
         // get bin sample
         double sample=tpl->GetBinContent(tpl_bin);
         // Set sample in histogram that's to be saved
         tap_pulse->SetBinContent(i,sample*GetAmplitude() + GetPedestal());
      }
      tap_pulse->Write();
   }
}
