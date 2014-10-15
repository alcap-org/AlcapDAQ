#include "EventNavigator.h"
#include "TTemplateFitAnalysedPulse.h"
#include "TPaveText.h"
#include "TMarker.h"
#include "TH1F.h"
#include "Functions.h"
#include "debug_tools.h"

TTemplateFitAnalysedPulse::TTemplateFitAnalysedPulse(
       const IDs::source& sourceID, const TPulseIslandID& parentID, const TPulseIsland* parentTPI):
            TAnalysedPulse(sourceID,parentID,parentTPI),
             fResidual(NULL), fHisto(NULL),fIsPileUpPulse(false),fOtherPulse(NULL){}

TTemplateFitAnalysedPulse::~TTemplateFitAnalysedPulse(){
   //if(fHisto) delete fHisto;
}

void TTemplateFitAnalysedPulse::Draw(const TH1F* tpi_pulse)const{
   if(tpi_pulse) {
      std::string name=tpi_pulse->GetName();
      name+="_templateAP";
      TH1F* tap_pulse=(TH1F*)GetHisto()->Clone(name.c_str());
      tap_pulse->SetDirectory(TDirectory::CurrentDirectory());

      TPaveText* text_b=new TPaveText(0.7,0.60,0.9,0.9,"NB NDC");
      text_b->AddText(Form("#chi^2 = %3.2g",GetChi2()));
      text_b->AddText(Form("Status = %d",GetFitStatus()));
      text_b->AddText(Form("Time = %g",GetTime()));
      text_b->AddText(Form("Ampl. = %g",GetAmplitude()));
      text_b->AddText(Form("NDoF = %d",GetNDoF()));
      text_b->SetFillColor(kWhite);
      text_b->SetBorderSize(1);
      tap_pulse->GetListOfFunctions()->Add(text_b);

      TH1F* tap_pulse2=NULL;
      if(fOtherPulse && !fIsPileUpPulse){
        name+="2";
        tap_pulse2=(TH1F*)fOtherPulse->GetHisto()->Clone(name.c_str());
        tap_pulse2->SetDirectory(TDirectory::CurrentDirectory());
      }
       
      //TMarker* marker=new TMarker(GetTime(), GetAmplitude()+GetPedestal()+10, 23);
      //marker->SetMarkerColor(kRed);
      //tap_pulse->GetListOfFunctions()->Add(marker);

      //tap_pulse->Write();

      TH1F* residual=(TH1F*) tpi_pulse->Clone(Form("%s_residual",tpi_pulse->GetName()));
      TH1F* tap_rebinned=(TH1F*) tap_pulse->Rebin(GetTemplate()->GetRefineFactor(),Form("%s_rebin",tap_pulse->GetName()));
      residual->Add(tap_rebinned, -1./GetTemplate()->GetRefineFactor());
      if(tap_pulse2) {
        delete tap_rebinned;
        tap_rebinned=(TH1F*) tap_pulse2->Rebin(GetTemplate()->GetRefineFactor(),Form("%s_rebin",tap_pulse->GetName()));
        residual->Add(tap_rebinned, -1./GetTemplate()->GetRefineFactor());
      }
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
      int num_samples=GetTPILength()*GetTemplate()->GetRefineFactor();
      TH1F* tap_pulse=new TH1F(name.c_str(), name.c_str(),num_samples,0,GetTPILength());
      for ( int i=0; i<num_samples; i++){
         double height=GetBinContent(i);
         tap_pulse->SetBinContent(i,height);
      }
      tap_pulse->SetDirectory(0);

      fHisto=tap_pulse;
      return fHisto;
}

double TTemplateFitAnalysedPulse::GetBinContent(int bin)const{
      // deduce right bin to look in
      int tpl_bin=GetTime()-GetTemplate()->GetTime()/GetTemplate()->GetRefineFactor()+bin;
      // get bin sample
      double sample=GetTemplate()->GetHisto()->GetBinContent(tpl_bin);
      // Set sample in histogram that's to be saved
      return sample*GetAmplitude() + GetPedestal();
}

