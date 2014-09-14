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
    fIntegralRatio(NULL)
{
   // get the templates from the archive
   if(!fTemplateArchive){
     fTemplateArchive=new TemplateArchive(opts->GetString("template_archive").c_str(),"READ");
   }
   fTemplate=fTemplateArchive->GetTemplate(GetChannel());

   fConvolver=new TemplateConvolver(GetChannel(), fTemplate, opts->GetDouble("pulse_cut",1e6));

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

    // convolve with the template
    int n_peaks= fConvolver->Convolve(*tpi);

    // Make a new TAP to store the data.  This method makes a TAP and sets the parent TPI info.  It needs
    // the index of the parent TPI in the container as an argument
    tap = MakeNewTAP<TTemplateConvolveAnalysedPulse>(tpi-pulseList.begin());
    if(fIntegralRatio){
      tap->SetIntegral(integral);
      tap->SetIntegralRatio(ratio);
    }
    tap->SetNPeaks(n_peaks);
    tap->SetEnergyConvolve(fConvolver->GetEnergyConvolution());
    tap->SetTimeConvolve(fConvolver->GetTimeConvolution());
    tap->SetTime(fConvolver->GetTime());
    tap->SetAmplitude(fConvolver->GetAmplitude());

    // Finally add the new TAP to the output list
    analysedList.push_back(tap);
  }

  return 0;
}

bool TemplateConvolveAPGenerator::PassesIntegralRatio(const TPulseIsland* pulse, double& integral, double& ratio)const{
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


ALCAP_TAP_GENERATOR(TemplateConvolve, template_archive, peak_cut, use_IR_cut, min_integral, max_integral, min_ratio, max_ratio );
