#include "TemplateConvolveAPGenerator.h"

#include "EventNavigator.h"
#include "TAPGeneratorFactory.h"
#include "TemplateConvolver.h"
#include "TPulseIsland.h"
#include "TAnalysedPulse.h"
#include "debug_tools.h"

#include <iostream>
using std::cout;
using std::endl;

TemplateArchive* TemplateConvolveAPGenerator::fTemplateArchive=NULL;

TemplateConvolveAPGenerator::TemplateConvolveAPGenerator(TAPGeneratorOptions* opts):
   TVAnalysedPulseGenerator("TemplateConvolve",opts),
    fPedestal(EventNavigator::Instance().GetSetupRecord().GetPedestal(GetChannel())),
    fTicksPerNs(EventNavigator::Instance().GetSetupRecord().GetTickLength(GetChannel())),
    //fMuScTimeOffset(SetupNavigator::Instance()->GetCoarseTimeOffset(GetSource())),
    fMuScTimeOffset(0.),
    fExpectPileUp(false), fIntegralRatio(NULL)
{
   // get the templates from the archive
   if(!fTemplateArchive){
     fTemplateArchive=new TemplateArchive(opts->GetString("template_archive").c_str(),"READ");
   }
   fTemplate=fTemplateArchive->GetTemplate(GetChannel());
   fTemplate->RebinToOriginalSampling();
   fTemplate->NormaliseToSumSquares();

   int fit_peak= opts->GetInt("n_quad_fit",5);
   int left= opts->GetInt("left",20);
   int right= opts->GetInt("right",120);
   fConvolver=new TemplateConvolver(GetChannel(), fTemplate, fit_peak, left, right);
   TH1* tpl=(TH1*)fTemplate->GetHisto()->Clone("Template");
   tpl->SetDirectory(gDirectory);
   fConvolver->CharacteriseTemplate();

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
}

int TemplateConvolveAPGenerator::ProcessPulses( 
    const PulseIslandList& pulseList,
    AnalysedPulseList& analysedList){

  // Loop over all the TPIs given to us
  double integral, ratio;
  TTemplateConvolveAnalysedPulse* tap;
  for (PulseIslandList::const_iterator tpi=pulseList.begin();
       tpi!=pulseList.end(); tpi++){

    // Check integral ratio if we're supposed to
    if(fIntegralRatio && !PassesIntegralRatio(*tpi, integral,ratio)){
       continue;
    }
    Algorithm::TpiMinusPedestal_iterator waveform((*tpi)->GetSamples().begin(),fPedestal);
    Algorithm::TpiMinusPedestal_iterator end((*tpi)->GetSamples().end());

    // convolve with the template
    int n_peaks=fConvolver->Convolve(waveform,end );
    if(n_peaks<0) {
      if(Debug())cout<<"Waveform too small to analyze"<<endl;
      continue;
    }
    n_peaks= fConvolver->FindPeaks(fExpectPileUp , waveform);

    const TemplateConvolver::PeaksVector& peaks=fConvolver->GetPeaks();
    int count=0;
    for(TemplateConvolver::PeaksVector::const_iterator i_tap=peaks.begin();
          i_tap!=peaks.end(); ++i_tap){
    //for(int i_peak=0; i_peak<3 || fConvolver->GetPeaks().size()==0 ; ++i_peak){
    //   // get the first pulse to be found
    //   const TemplateConvolver::FoundPeaks& i_tap=*fConvolver->GetPeaks().begin();
    
       // Make a new TAP to store the data.  This method makes a TAP and sets the parent TPI info.  It needs
       // the index of the parent TPI in the container as an argument
       tap = MakeNewTAP<TTemplateConvolveAnalysedPulse>(tpi-pulseList.begin());
       if(fIntegralRatio){
         tap->SetIntegral(integral);
         tap->SetIntegralRatio(ratio);
       }
       tap->SetNPeaks(n_peaks);
       tap->SetPeakRank(count);
       tap->SetEnergyConvolve(fConvolver->GetEnergyConvolution());
       tap->SetTimeConvolve(fConvolver->GetTimeConvolution());
       tap->SetTimeOffset(i_tap->time);
       tap->SetAmplitudeScale(i_tap->amplitude);
       tap->SetAmplitude(i_tap->amplitude*fConvolver->GetAmplitudeScale());
       tap->SetQuadraticFit(i_tap->quad,i_tap->linear, i_tap->constant);
       tap->SetTemplate(fTemplate);
       tap->SetPedestal(fPedestal);
       double time= CalibrateTime(i_tap->time + fConvolver->GetTimeShift() +0.5, *tpi);
       tap->SetTime(time);
       //DEBUG_VALUE(i_tap->amplitude, i_tap->time, i_tap->pedestal);

       // Finally add the new TAP to the output list
       analysedList.push_back(tap);

       //// subtract template from first peak and convolve again
       //waveform.AddHistogram(fConvolver->GetTemplateACF(), i_tap.amplitude, -i_tap.time+fConvolver->GetLeftSafety());
       //n_peaks= fConvolver->Convolve(waveform,end );
       //if(n_peaks==0) break;
       count++;
    }
  }

  return 0;
}

bool TemplateConvolveAPGenerator::PassesIntegralRatio(const TPulseIsland* pulse, double& integral, double& ratio)const{
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

double TemplateConvolveAPGenerator::CalibrateTime(double clockTime, const TPulseIsland* tpi)const{
  return (clockTime + (double)tpi->GetTimeStamp()) * fTicksPerNs - fMuScTimeOffset;
}

ALCAP_TAP_GENERATOR(TemplateConvolve, template_archive, n_quad_fit, left, right, use_IR_cut, min_integral, max_integral, min_ratio, max_ratio );
