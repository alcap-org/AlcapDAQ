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
TemplateArchive* TemplateFitAPGenerator::fTemplateArchive2=NULL;

TemplateFitAPGenerator::TemplateFitAPGenerator(TAPGeneratorOptions* opts):
   TVAnalysedPulseGenerator("TemplateFitAPGenerator",opts),
    fInitPedestal(EventNavigator::Instance().GetSetupRecord().GetPedestal(GetChannel())),
    fIntegralRatio(NULL),
    fMaxBin(fInitPedestal, EventNavigator::Instance().GetSetupRecord().GetPolarity(GetChannel()))
{
   // get the templates from the archive
   if(!fTemplateArchive){
     fTemplateArchive=new TemplateArchive(opts->GetString("template_archive").c_str(),"READ");
   }
   if(opts->HasOption("template_archive_2") && opts->GetString("template_archive_2")!=fTemplateArchive->GetName()){
     std::string template_2_src=opts->GetString("template_archive_2");
     if(!fTemplateArchive2)
     fTemplateArchive2=new TemplateArchive(opts->GetString("template_archive_2").c_str(),"READ");
   } else if(!fTemplateArchive2){
     fTemplateArchive2=fTemplateArchive;
   }
   fTemplate=fTemplateArchive->GetTemplate(GetChannel());
   fTemplate2=fTemplateArchive2->GetTemplate(GetChannel());

   // make the fitters
   fFitter = new TemplateFitter(GetChannel().str(), fTemplate->GetRefineFactor());
   fDoubleFitter = new TemplateMultiFitter(GetChannel().str(), fTemplate->GetRefineFactor());

   // prepare the integral ratio cuts
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

  // cache the template parameters so we dont need to look them up
  fTemplateAmp=fTemplate->GetAmplitude();
  fTemplateTime=fTemplate->GetTime();
  fTemplatePed=fTemplate->GetPedestal();

  fTemplate2Amp=fTemplate2->GetAmplitude();
  fTemplate2Time=fTemplate2->GetTime();
  fTemplate2Ped=fTemplate2->GetPedestal();

  // chi2 to determine when we refit with 2 pulses
  fChi2MinToRefit=opts->GetDouble("min_chi2_to_refit",2e8);
}

TemplateFitAPGenerator::~TemplateFitAPGenerator(){
  delete fFitter;
  delete fDoubleFitter;
  if(fIntegralRatio) delete fIntegralRatio;
  if(fTemplateArchive) {
    delete fTemplateArchive;
    fTemplateArchive=NULL;
  }
  if(fTemplateArchive2) {
    delete fTemplateArchive2;
    fTemplateArchive2=NULL;
  }
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
    double init_time= fTemplateTime - fMaxBin.time * fTemplate->GetRefineFactor();
    fFitter->SetInitialParameterEstimates(fInitPedestal, init_amp, init_time);

    // Analyse each TPI
    TH1D* hPulseToFit=InterpolatePulse(*tpi,"hPulseToFit","hPulseToFit",false,fTemplate->GetRefineFactor());
    
    int fit_status = fFitter->FitPulseToTemplate(fTemplate, hPulseToFit, (*tpi)->GetBankName());

    // Now that we've found the information we were looking for make a TAP to
    // hold it.  This method makes a TAP and sets the parent TPI info.  It needs
    // the index of the parent TPI in the container as an argument
    tap = MakeNewTAP<TTemplateFitAnalysedPulse>(tpi-pulseList.begin());
    tap->SetTemplate(fTemplate);
    tap->SetPedestal(fFitter->GetPedestalOffset());
    tap->SetAmplitudeScaleFactor(fFitter->GetAmplitudeScaleFactor());
    tap->SetTimeOffset(fFitter->GetTimeOffset());
    tap->SetChi2(fFitter->GetChi2());
    tap->SetNDoF(fFitter->GetNDoF());
    tap->SetFitStatus(fit_status);
    tap->SetIntegral(integral);
    tap->SetIntegralRatio(ratio);

    if(fFitter->GetChi2() > fChi2MinToRefit){
        TTemplateFitAnalysedPulse* second_tap=NULL;
        bool better_with_2 = RefitWithTwo(hPulseToFit, tap, second_tap);
        if(better_with_2){
           analysedList.push_back(tap);
           analysedList.push_back(second_tap);
        } else{
           analysedList.push_back(tap);
           if(second_tap) delete second_tap;
        }
    } else {
       analysedList.push_back(tap);
    }

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

bool TemplateFitAPGenerator::RefitWithTwo(TH1D* tpi, TTemplateFitAnalysedPulse*& tap_one, TTemplateFitAnalysedPulse*& tap_two)const{

  // find the max bin after the current fitted tap
  tpi->GetXaxis()->SetRange(tap_one->GetTime());
  int second_time=tpi->GetMaximumBin();
  tpi->GetXaxis()->UnZoom();
  double second_scale=tpi->GetBinContent(second_time) - tap_one->GetBinContent(second_time);
  second_scale /=fTemplate2->GetHisto()->GetMaximum();

  // fit the second template around the second peak
  fDoubleFitter->SetPulseEstimates( 0, tap_one->GetAmplitude(), tap_one->GetTime());
  fDoubleFitter->SetPulseEstimates( 1, second_scale, second_time);
  fDoubleFitter->FixTimeForAllBut(1);
  int fit_status = fDoubleFitter->FitToPulse(tpi);

  // fit the first pulse again
  fDoubleFitter->FixTimeForAllBut(0);
  fit_status = fDoubleFitter->FitToPulse(tpi);

  // fit the second pulse again
  fDoubleFitter->FixTimeForAll();
  fit_status = fDoubleFitter->FitToPulse(tpi);

  if(fit_status==0){
    // Has the double fit improved the chi-2 per NDoF
    double old_chi2=tap_one->GetChi2()/tap_one->GetNDoF();
    double new_chi2=fDoubleFitter->GetChi2()/fDoubleFitter->GetNDoF();
    if( (old_chi2 - new_chi2) < 0){
      // refill tap_one's values
      tap_one->SetPedestal(fDoubleFitter->GetPedestalOffset());
      tap_one->SetAmplitude(fDoubleFitter->GetAmplitude(2));
      tap_one->SetTime(fDoubleFitter->GetTime(2));
      tap_one->SetChi2(fDoubleFitter->GetChi2());
      tap_one->SetNDoF(fDoubleFitter->GetNDoF());
      tap_one->SetFitStatus(fit_status);

      // make the second tap
      tap_two = MakeNewTAP<TTemplateFitAnalysedPulse>(tap_one->GetParentID());
      tap_two->SetTemplate(fTemplate2);
      tap_two->SetPedestal(fDoubleFitter->GetPedestalOffset());
      tap_two->SetAmplitudeScaleFactor(fDoubleFitter->GetAmplitudeScaleFactor(1));
      tap_two->SetTimeOffset(fDoubleFitter->GetTimeOffset());
      tap_two->SetChi2(fDoubleFitter->GetChi2());
      tap_two->SetNDoF(fDoubleFitter->GetNDoF());
      tap_two->SetFitStatus(fit_status);

      return true;
    }
  } 

  return false;
}

// Similar to the modules, this macro registers the generator with
// MakeAnalysedPulses. The first argument is compulsory and gives the name of
// this generator. All subsequent arguments will be used as names for arguments
// given directly within the modules file.  See the github wiki for more.
//
// NOTE: for TAP generators OMIT the APGenerator part of the class' name
ALCAP_TAP_GENERATOR(TemplateFit,template_archive,use_IR_cut, min_integral, max_integral, min_ratio, max_ratio);
