#include "TAPGeneratorFactory.h"
#include "TemplateFitAPGenerator.h"
#include "TPulseIsland.h"
#include "TAnalysedPulse.h"
#include "TTemplateFitAnalysedPulse.h"
#include "InterpolatePulse.h"
#include "EventNavigator.h"
#include "debug_tools.h"
#include <iostream>
using std::cout;
using std::endl;

TemplateArchive* TemplateFitAPGenerator::fTemplateArchive=NULL;

TemplateFitAPGenerator::TemplateFitAPGenerator(TAPGeneratorOptions* opts):
   TVAnalysedPulseGenerator("TemplateFitAPGenerator",opts),
    fInitPedestal(EventNavigator::Instance().GetSetupRecord().GetPedestal(GetChannel())),
    fIntegralRatio(NULL),
    fMaxBin(fInitPedestal, EventNavigator::Instance().GetSetupRecord().GetPolarity(GetChannel()))
{
   if(!fTemplateArchive){
     fTemplateArchive=new TemplateArchive(opts->GetString("template_archive").c_str(),"READ");
   }
   fTemplate=fTemplateArchive->GetTemplate(GetChannel());
   fFitter = new TemplateFitter(GetChannel().str(), fTemplate->GetRefineFactor());
   if(opts->GetFlag("use_IR_cut")){
      fIntegralRatio=new Algorithm::IntegralRatio(
                      opts->GetInt("start_integral",10),
                      opts->GetInt("start_tail",60),
                      opts->GetInt("stop_integral",0),
                      EventNavigator::Instance().GetSetupRecord().GetPolarity(GetChannel()));
      fIntegralMax=opts->GetDouble("max_integral");
      fIntegralMin=opts->GetDouble("min_integral");
      fIntegralRatioMax=opts->GetDouble("max_ratio");
      fIntegralRatioMin=opts->GetDouble("min_ratio");
  }

  fTemplateAmp=fTemplate->GetAmplitude();
  fTemplateTime=fTemplate->GetTime();
  fTemplatePed=fTemplate->GetPedestal();
}

int TemplateFitAPGenerator::ProcessPulses( 
      const PulseIslandList& pulseList,
      AnalysedPulseList& analysedList){

  // Loop over all the TPIs given to us
  TTemplateFitAnalysedPulse* tap;
  double integral, ratio;
  for (PulseIslandList::const_iterator tpi=pulseList.begin();
       tpi!=pulseList.end(); tpi++){

    if(fIntegralRatio && !PassesIntegralRatio(*tpi, integral,ratio)){
       continue;
    }
    double init_amp=fMaxBin(*tpi)/fTemplateAmp;
    double init_time= fTemplateTime-fMaxBin.time*fTemplate->GetRefineFactor();
    fFitter->SetInitialParameterEstimates(fInitPedestal, init_amp, init_time);

    // Analyse each TPI
    TH1D* hPulseToFit=InterpolatePulse(*tpi,"hPulseToFit","hPulseToFit",false,fTemplate->GetRefineFactor());
    
    int fit_status = fFitter->FitPulseToTemplate(fTemplate, hPulseToFit, (*tpi)->GetBankName());

    // Now that we've found the information we were looking for make a TAP to
    // hold it.  This method makes a TAP and sets the parent TPI info.  It needs
    // the index of the parent TPI in the container as an argument
    tap = MakeNewTAP<TTemplateFitAnalysedPulse>(tpi-pulseList.begin());
    tap->SetTemplate(fTemplate);
    tap->SetAmplitude(fFitter->GetAmplitudeScaleFactor());
    tap->SetPedestal(fFitter->GetPedestalOffset());
    tap->SetTime(fFitter->GetTimeOffset());
    tap->SetChi2(fFitter->GetChi2());
    tap->SetNDoF(fFitter->GetNDoF());
    tap->SetFitStatus(fit_status);
    tap->SetIntegral(integral);
    tap->SetIntegralRatio(ratio);

    // Histogram the residual
      TH1F* residual=(TH1F*) hPulseToFit->Clone("residual");
      const TH1F* tap_pulse=tap->GetHisto();
      residual->Add(tap_pulse, -1);

    tap->SetResidualIntegral(residual->Integral());
    delete residual;
    //tap->SetResidual(residual);

    // Finally add the new TAP to the output list
    analysedList.push_back(tap);
   }

   // returning 0 tells the caller that we were successful, return non-zero if not
   return 0;
}

bool TemplateFitAPGenerator::PassesIntegralRatio(const TPulseIsland* pulse, double& integral, double& ratio)const{
   if(!fIntegralRatio) return true;
   try{
     (*fIntegralRatio)(pulse);
   }catch(std::out_of_range& e){
     return false;
   }
   integral=fIntegralRatio->GetTotal();
   ratio=fIntegralRatio->GetRatio();
   if( fIntegralMax < integral || fIntegralMin > integral
       || fIntegralRatioMax < ratio || fIntegralRatioMin > ratio) {
       return false;
   }
   return true;
}
// Similar to the modules, this macro registers the generator with
// MakeAnalysedPulses. The first argument is compulsory and gives the name of
// this generator. All subsequent arguments will be used as names for arguments
// given directly within the modules file.  See the github wiki for more.
//
// NOTE: for TAP generators OMIT the APGenerator part of the class' name
ALCAP_TAP_GENERATOR(TemplateFit,template_archive,use_IR_cut, min_integral, max_integral, min_ratio, max_ratio);