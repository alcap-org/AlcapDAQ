#include "EventNavigator.h"
#include "TTemplateFitAnalysedPulse.h"
#include "TPaveText.h"
#include "TH1F.h"
#include "Functions.h"
#include "debug_tools.h"

TTemplateFitAnalysedPulse::TTemplateFitAnalysedPulse(
       const IDs::source& sourceID, const TPulseIslandID& parentID, const TPulseIsland* parentTPI):
            TAnalysedPulse(sourceID,parentID,parentTPI), fResidual(NULL), fHisto(NULL){}

void TTemplateFitAnalysedPulse::Draw(const TH1F* tpi_pulse)const{
   if(tpi_pulse) {
      std::string name=tpi_pulse->GetName();
      name+="_templateAP";
      TH1F* tap_pulse=(TH1F*)GetHisto()->Clone(name.c_str());

      TPaveText* text_b=new TPaveText(0.5,0.75,0.7,0.9,"NB NDC");
      text_b->AddText(Form("#chi^2 = %g",GetChi2()));
      text_b->AddText(Form("Status = %d",GetFitStatus()));
      text_b->SetFillColor(kWhite);
      text_b->SetBorderSize(1);
      tap_pulse->GetListOfFunctions()->Add(text_b);

      tap_pulse->SetDirectory(TDirectory::CurrentDirectory());
      //tap_pulse->Write();

      TH1F* residual=(TH1F*) tpi_pulse->Clone(Form("%s_residual",tpi_pulse->GetName()));
      TH1F* tap_rebinned=(TH1F*) tap_pulse->Rebin(GetTemplate()->GetRefineFactor(),Form("%s_rebin",tap_pulse->GetName()));
      residual->Add(tap_rebinned, -1./GetTemplate()->GetRefineFactor());
      double integral=residual->Integral(5,residual->GetNbinsX()-5);
      text_b->AddText(Form("Total residue = %g",integral));
       delete tap_rebinned;
      //residual->Write();
   } 
}

const TH1F* TTemplateFitAnalysedPulse::GetHisto()const{
      if(fHisto) return fHisto;

      std::string name=Form("Pulse_%s_%4lld_%3d_templateAP",
                                GetSource().Channel().str().c_str(),
                                EventNavigator::Instance().EntryNo(),
                                GetParentID());
      const TH1* tpl = GetTemplate()->GetHisto();
      int num_samples=GetTPILength()*GetTemplate()->GetRefineFactor();
      TH1F* tap_pulse=new TH1F(name.c_str(), name.c_str(),num_samples,0,GetTPILength());
      for ( int i=0; i<num_samples; i++){
         // deduce right bin to look in
         int tpl_bin=i+ GetTime();
         // get bin sample
         double sample=tpl->GetBinContent(tpl_bin);
         // Set sample in histogram that's to be saved
         tap_pulse->SetBinContent(i,sample*GetAmplitude() + GetPedestal());
      }
      tap_pulse->SetDirectory(0);

      fHisto=tap_pulse;
      return fHisto;
}
