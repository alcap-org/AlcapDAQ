#include "TAPGeneratorFactory.h"
#include "FastTemplateFitAPGenerator.h"
#include "TPulseIsland.h"
#include "TAnalysedPulse.h"
#include "TTemplateFitAnalysedPulse.h"
#include "InterpolatePulse.h"
#include "EventNavigator.h"
#include "debug_tools.h"
#include <iostream>
using std::cout;
using std::endl;

TemplateArchive* FastTemplateFitAPGenerator::fTemplateArchive=NULL;
TemplateArchive* FastTemplateFitAPGenerator::fTemplateArchive2=NULL;

FastTemplateFitAPGenerator::FastTemplateFitAPGenerator(TAPGeneratorOptions* opts):
   TVAnalysedPulseGenerator("FastTemplateFitAPGenerator",opts),
    fAttemptRefit(opts->GetBool("attempt_refit",false)),
    fInitPedestal(EventNavigator::Instance().GetSetupRecord().GetPedestal(GetChannel())),
    fTemplate2(NULL),
    fIntegralRatio(NULL),
    fMaxBin(fInitPedestal, EventNavigator::Instance().GetSetupRecord().GetPolarity(GetChannel()))
{
   // get the templates from the archive
   if(!fTemplateArchive){
     fTemplateArchive=new TemplateArchive(opts->GetString("template_archive").c_str(),"READ");
   }
   if(opts->HasOption("template_archive_2")){
     std::string template_2_src=opts->GetString("template_archive_2");
     if (template_2_src!=fTemplateArchive->GetName() && !fTemplateArchive2)
        fTemplateArchive2=new TemplateArchive(opts->GetString("template_archive_2").c_str(),"READ");
     else if(!fTemplateArchive2){
        fTemplateArchive2=fTemplateArchive;
     }
   }
   fTemplate=fTemplateArchive->GetTemplate(GetChannel());
   if(fTemplateArchive2) fTemplate2=fTemplateArchive2->GetTemplate(GetChannel());

   // make the fitters
   fFitter = new TemplateFastFitter(GetChannel().str());
   fFitter->AddTemplate(fTemplate);
   fFitter->Init();

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

  if(fTemplate2){
     fTemplate2Amp=fTemplate2->GetAmplitude();
     fTemplate2Time=fTemplate2->GetTime();
     fTemplate2Ped=fTemplate2->GetPedestal();
  }

  // chi2 to determine when we refit with 2 pulses
  fChi2MinToRefit=opts->GetDouble("min_chi2_to_refit",2e5);
}

FastTemplateFitAPGenerator::~FastTemplateFitAPGenerator(){
  delete fFitter;
    
  delete fFitter;
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

int FastTemplateFitAPGenerator::ProcessPulses( 
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
    double init_time=  fMaxBin.GetTime() * fTemplate->GetRefineFactor()- fTemplateTime ;
    fFitter->SetPulseEstimates(0, init_time);

    // Make histo for TPI
    TH1D* hPulseToFit=InterpolatePulse(*tpi,"hPulseToFit","hPulseToFit",false,fTemplate->GetRefineFactor());
    
    int fit_status = fFitter->FitWithOneTimeFree(0, hPulseToFit);

    // Now that we've found the information we were looking for make a TAP to
    // hold it.  This method makes a TAP and sets the parent TPI info.  It needs
    // the index of the parent TPI in the container as an argument
    tap = MakeNewTAP<TTemplateFitAnalysedPulse>(tpi-pulseList.begin());
    tap->SetTemplate(fTemplate);
    tap->SetPedestal(fFitter->GetPedestal());
    tap->SetAmplitude(fFitter->GetAmplitude(0));
    tap->SetTime(-fFitter->GetTime(0));
    tap->SetChi2(fFitter->GetChi2());
    tap->SetNDoF(fFitter->GetNDoF());
    tap->SetFitStatus(fit_status);
    tap->SetIntegral(integral);
    tap->SetIntegralRatio(ratio);

    if(fAttemptRefit && fFitter->GetChi2() > fChi2MinToRefit){
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

bool FastTemplateFitAPGenerator::PassesIntegralRatio(const TPulseIsland* pulse, double& integral, double& ratio)const{
   if(!fIntegralRatio) return true;
   try{
     fIntegralRatio->SetPedestalToMinimum(pulse);
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

bool FastTemplateFitAPGenerator::RefitWithTwo(TH1D* tpi, TTemplateFitAnalysedPulse*& tap_one, TTemplateFitAnalysedPulse*& tap_two)const{

  // find the max bin after the current fitted tap
  int second_time=-9999999;
  InitializeSecondPulse(tpi,tap_one,second_time);

  // fit the second template around the second peak
  fFitter->SetPulseEstimates( 0, tap_one->GetTime());
  fFitter->SetPulseEstimates( 1,  second_time);
  int fit_status = fFitter->FitWithOneTimeFree(1, tpi);

  // fit the first pulse again
  fit_status = fFitter->FitWithOneTimeFree(0, tpi);

  // fit the second pulse again
  fit_status = fFitter->FitWithAllTimesFixed(tpi);

  if(fit_status==0){
    // Has the double fit improved the chi-2 per NDoF
    double old_chi2=tap_one->GetChi2()/tap_one->GetNDoF();
    double new_chi2=fFitter->GetChi2()/fFitter->GetNDoF();
    if( (old_chi2 - new_chi2) < 0){
      // refill tap_one's values
      tap_one->SetPedestal(fFitter->GetPedestal());
      tap_one->SetAmplitude(fFitter->GetAmplitude(0));
      tap_one->SetTime(fFitter->GetTime(0));
      tap_one->SetChi2(fFitter->GetChi2());
      tap_one->SetNDoF(fFitter->GetNDoF());
      tap_one->SetFitStatus(fit_status);

      // make the second tap
      tap_two = MakeNewTAP<TTemplateFitAnalysedPulse>(tap_one->GetParentID());
      tap_two->SetTemplate(fTemplate2);
      tap_two->SetPedestal(fFitter->GetPedestal());
      tap_two->SetAmplitude(fFitter->GetAmplitude(1));
      tap_two->SetTime(fFitter->GetTime(1));
      tap_two->SetChi2(fFitter->GetChi2());
      tap_two->SetNDoF(fFitter->GetNDoF());
      tap_two->SetFitStatus(fit_status);

      // link the two pulses together
      tap_one->SetOtherPulse(tap_two);
      tap_two->SetOtherPulse(tap_one);
      tap_two->IsPileUpPulse();

      return true;
    }
  } 

  return false;
}

void FastTemplateFitAPGenerator::InitializeSecondPulse(
          TH1D* tpi, const TTemplateFitAnalysedPulse* tap_one,
          int& second_time)const{
    // scan from the time of the first tap and find the bin with the largest residual (within some window)
    int limit = tpi->GetNbinsX();
    int template_limit=tap_one->GetTime() - fTemplate->GetTime() + fTemplate->GetHisto()->GetNbinsX()-10;
    if(template_limit <  limit ) limit= template_limit;
    double residual, max_residual=-1e6;
    int max_bin=0;
    for( int bin=tap_one->GetTime(); bin < limit; ++bin){
      residual=tpi->GetBinContent(bin) - tap_one->GetBinContent(bin);
      if(residual>max_residual){
        max_residual=residual;
        max_bin=bin;
      }
    }
    second_time=max_bin - fTemplate2->GetTime();
}

ALCAP_TAP_GENERATOR(FastTemplateFit,template_archive,use_IR_cut, min_integral, max_integral, min_ratio, max_ratio, attempt_refit,min_chi2_to_refit);
