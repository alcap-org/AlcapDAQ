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

// IsTimeOrdered()
// -- Returns true of the first pulse is before the second
// Static so that only this file sees it
// Useful for sorting the pulses
static bool IsTimeOrdered(TAnalysedPulse* a, TAnalysedPulse* b) {
  // Want a to be before b
  return ( a->GetTime() < b->GetTime() );
}


TemplateFitAPGenerator::TemplateFitAPGenerator(TAPGeneratorOptions* opts):
   TVAnalysedPulseGenerator("TemplateFitAPGenerator",opts),
   fInitPedestal(SetupNavigator::Instance()->GetPedestal(GetChannel())),
    fIntegralRatio(NULL),
   fMaxTemplates(3),
   fMaxBin(fInitPedestal, TSetupData::Instance()->GetTriggerPolarity(GetBank())),
   fCFTime(fInitPedestal,  TSetupData::Instance()->GetTriggerPolarity(GetBank()),
	   TSetupData::Instance()->GetClockTick(GetBank()),
	   opts->GetBool("no_time_shift", false) ? 0. : SetupNavigator::Instance()->GetCoarseTimeOffset(GetSource()),
	   TSetupData::GetDownSampling(GetBank().c_str(), SetupNavigator::Instance()->GetRunNumber()),
	   opts->GetDouble("constant_fraction")),
   fNoTimeShift(opts->GetBool("no_time_shift", false)),
   fSyncPulses(opts->GetBool("sync_pulses", false))
{
   // get the templates from the archive
   if(!fTemplateArchive){
     fTemplateArchive=new TemplateArchive(opts->GetString("template_archive").c_str(),"READ");
   }
   /*   if(opts->HasOption("template_archive_2")){
     std::string template_2_src=opts->GetString("template_archive_2");
     if (template_2_src!=fTemplateArchive->GetName() && !fTemplateArchive2)
        fTemplateArchive2=new TemplateArchive(opts->GetString("template_archive_2").c_str(),"READ");
     else if(!fTemplateArchive2){
        fTemplateArchive2=fTemplateArchive;
     }
   }
   */
   fTemplate=fTemplateArchive->GetTemplate(GetChannel());

   // make the fitters
   //   fFitter = new TemplateFitter(GetChannel().str(), 1);
   fPCF = new PulseCandidateFinder_TSpectrum(GetChannel().str(), opts);

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

  fMultiFitter = new TemplateMultiFitter(GetChannel().str());

  // chi2 to determine when we refit with 2 pulses
  //  fChi2MinToRefit=opts->GetDouble("min_chi2_to_refit",2e5);
  fChi2MinToRefit=opts->GetDouble("min_chi2_to_refit",200);
}

TemplateFitAPGenerator::~TemplateFitAPGenerator(){
    
  if(fIntegralRatio) delete fIntegralRatio;
  if(fTemplateArchive) {
    delete fTemplateArchive;
    fTemplateArchive=NULL;
  }
  delete fMultiFitter;
}

int TemplateFitAPGenerator::ProcessPulses( 
      const PulseIslandList& pulseList,
      AnalysedPulseList& analysedList){

  // Loop over all the TPIs given to us
  TTemplateFitAnalysedPulse* tap;
  double integral, ratio;
  bool debug = false;
  for (PulseIslandList::const_iterator tpi=pulseList.begin();
       tpi!=pulseList.end(); tpi++){

    /*    if ( tpi-pulseList.begin() == 31){
      debug = true;
    }
    else {
      debug = false;
    }
    */
    
    
    if (debug) {
      std::cout << std::endl << "New TPI! (#" << tpi-pulseList.begin() << ")" << std::endl;
      std::cout << "========" << std::endl;
    }

    if(fIntegralRatio && !PassesIntegralRatio(*tpi, integral,ratio)){
       continue;
    }

    // Get a couple of histograms that we will use for all fit attempts
    int trigger_polarity = TSetupData::Instance()->GetTriggerPolarity((*tpi)->GetBankName());
    TH1D* hPulseUnrefined = InterpolatePulse(*tpi,"hPulseUnrefined","hPulseUnrefined",false,1,true);
    fPCF->FindPulseCandidates(*tpi, hPulseUnrefined);
    std::vector<PulseCandidateFinder_TSpectrum::PeakLocation> peak_locations = fPCF->GetPeakLocations(); // NB these are NOT pedestal subtracted but not in template time bins

    if (debug) {
      std::cout << "AE: Peak Locations.size() = " << peak_locations.size() << std::endl;
      for (int i_peak = 0; i_peak < peak_locations.size(); ++i_peak) {
	std::cout << "Peak #" << i_peak+1 << ": t = " << peak_locations.at(i_peak).time << ", A = " << peak_locations.at(i_peak).amplitude << std::endl;
      }
    }
    TH1D* hPulseToFit=InterpolatePulse(*tpi,"hPulseToFit","hPulseToFit",false,fTemplate->GetRefineFactor());

    double init_amp=fMaxBin(*tpi);///fTemplateAmp;
    double init_time=  fMaxBin.GetTime() * fTemplate->GetRefineFactor(); //- fTemplateTime ;
    if (debug) {
      std::cout << "Init Amp = "<< init_amp << ", Init Time = " << init_time << ", Init Pedestal = " << fInitPedestal << std::endl;
    }
    

    // Go in order trying to fit with an additional template each time
    double best_chi2 = 9e7;
    std::vector<TTemplateFitAnalysedPulse*> best_taps;
    
    for (int n_templates = 1; n_templates <= fMaxTemplates; ++n_templates) {
      if (debug) {
	std::cout << std::endl << "Attempting " << n_templates << " templates..." << std::endl;
      }
      // Create the fitter with the correct number of templates
      fMultiFitter->Reset();
      for (int i_template = 0; i_template < n_templates; ++i_template) {
	fMultiFitter->AddTemplate(fTemplate);
      }
      fMultiFitter->Init();

      // Get an estimate for the initial template parameters
      if (peak_locations.size() < n_templates) {
	//	std::cout << "TSpectrum found fewer peaks than the number of templates requested" << std::endl;
	//	std::cout << "AE: " << peak_locations.size() << ", " << n_templates << std::endl;
	continue;
      }
      if (debug) {
	std::cout << "Initial Template Estimates:" << std::endl;
      }
      for (int i_template = 0; i_template < n_templates; ++i_template) {
	fMultiFitter->SetPulseEstimates(i_template, 
				  peak_locations.at(i_template).amplitude - trigger_polarity*fInitPedestal, 
				  (peak_locations.at(i_template).time+0.5)*fTemplate->GetRefineFactor());

	if (debug) {
	  std::cout << "\t Template #" << i_template << ": A = " << fMultiFitter->GetAmplitude(i_template) << ", t = " << fMultiFitter->GetTime(i_template) << std::endl;
	}
      }
      if (debug) {
	std::cout << "\t Pedestal = " << fMultiFitter->GetPedestal() << ", Chi^2 = " << fMultiFitter->GetChi2() << std::endl;
      }

      fMultiFitter->SetPedestalEstimate(trigger_polarity*fInitPedestal);

      if (debug) {
	std::cout << "Fitting with all times fixed (1)..." << std::endl;
      }
      int fit_status = fMultiFitter->FitWithAllTimesFixed(hPulseToFit);

      if (debug) {
	std::cout << "Final parameters:" << std::endl;
	for (int i_template = 0; i_template < n_templates; ++i_template) {
	  std::cout << "\t Template #" << i_template << ": A = " << fMultiFitter->GetAmplitude(i_template) << ", t = " << fMultiFitter->GetTime(i_template) << std::endl;
	}
	std::cout << "\t Pedestal = " << fMultiFitter->GetPedestal() << ", Chi^2 / NdF = " << fMultiFitter->GetChi2() << " / " << fMultiFitter->GetNDoF() << " = " << fMultiFitter->GetChi2() / fMultiFitter->GetNDoF() << std::endl;
      }

      // Now fit each individual peak on its own
      if (debug) {
	std::cout << "Fitting each template individually..." << std::endl;
      }

      for (int i_template = 0; i_template < n_templates; ++i_template) {
	double range = 5;

	if (fMultiFitter->GetTime(i_template) > (*tpi)->GetSamples().size()-5) { // if we are close to the end of the TPI
	  range = 10; // allow it to go out a bit further
	}
	fit_status = fMultiFitter->FitWithOneTimeFree(i_template, hPulseToFit, range*fTemplate->GetRefineFactor());

	if (debug) {
	  std::cout << "\t Template #" << i_template << ": status = " << fit_status << ", A = " << fMultiFitter->GetAmplitude(i_template) << ", t = " << fMultiFitter->GetTime(i_template) << " template bins, P = " << fMultiFitter->GetPedestal() << ", Chi^2 / NdF = " << fMultiFitter->GetChi2() << " / " << fMultiFitter->GetNDoF() << " = " << fMultiFitter->GetChi2() / fMultiFitter->GetNDoF() << std::endl;
	}
      }

      if (debug) {
	std::cout << "Final parameters:" << std::endl;
	for (int i_template = 0; i_template < n_templates; ++i_template) {
	  std::cout << "\t Template #" << i_template << ": A = " << fMultiFitter->GetAmplitude(i_template) << ", t = " << fMultiFitter->GetTime(i_template) << std::endl;
	}
	std::cout << "\t Pedestal = " << fMultiFitter->GetPedestal() << ", Chi^2 / NdF = " << fMultiFitter->GetChi2() << " / " << fMultiFitter->GetNDoF() << " = " << fMultiFitter->GetChi2() / fMultiFitter->GetNDoF() << std::endl;
      }
	//	std::cout << "AE: chi^2 = " << fMultiFitter->GetChi2()  << std::endl;
      if (fMultiFitter->GetChi2() < best_chi2 && fit_status==0) {	
	for (std::vector<TTemplateFitAnalysedPulse*>::const_iterator i_best_tap = best_taps.begin(); i_best_tap != best_taps.end(); ++i_best_tap) {
	  delete *i_best_tap;
	}
	best_taps.clear();
	
	for (int i_template = 0; i_template < n_templates; ++i_template) {
	  TTemplateFitAnalysedPulse* tap = MakeNewTAP<TTemplateFitAnalysedPulse>(tpi-pulseList.begin(), i_template);
	  tap->SetTemplate(fTemplate);
	  tap->SetPedestal(fMultiFitter->GetPedestal());
	  tap->SetAmplitude(fMultiFitter->GetAmplitude(i_template));

	  double clock_tick_in_ns = TSetupData::Instance()->GetClockTick(TSetupData::Instance()->GetBankName(GetChannel().str()));
	  double time_shift;
	  if (fNoTimeShift) {
	    time_shift = 0;
	  }
	  else {
	    time_shift = SetupNavigator::Instance()->GetCoarseTimeOffset(GetSource());
	  }
	  
	  int down_samp = TSetupData::GetDownSampling(GetBank().c_str(), SetupNavigator::Instance()->GetRunNumber());
	  double time = (*tpi)->GetTimeStamp() + ( (fTemplate->GetCFTime() / fTemplate->GetRefineFactor()) * down_samp );
	  double tpl_time_offset = fMultiFitter->GetTimeOffset(i_template); // in template time bins
	  tpl_time_offset /= fTemplate->GetRefineFactor(); // convert to clock ticks
	  tpl_time_offset *= down_samp; // take into account the down sampling

	  tap->SetTemplateTimeOffset(tpl_time_offset);
	  //	  tpl_time_offset += time_shift;
	  
	  time += tpl_time_offset;
	  time *= clock_tick_in_ns; // convert to ns
	  time -= time_shift;

	  if (fSyncPulses) {
	    if (time < 10e3 || time > 96e6) {
	      continue;
	    }
	  }

	  tap->SetTime(time);
	  tap->SetChi2(fMultiFitter->GetChi2());
	  tap->SetNDoF(fMultiFitter->GetNDoF());
	  tap->SetFitStatus(fit_status);
	  //	tap->SetIntegral(integral);
	  //	tap->SetIntegralRatio(ratio);

	  best_taps.push_back(tap);

	  best_chi2 = fMultiFitter->GetChi2();
	}
      }

      if (best_chi2 < fChi2MinToRefit) {
	break;
      }
      else if (peak_locations.size() < n_templates+1) {
	// Look for the residual
	PulseCandidateFinder_TSpectrum::PeakLocation add_location;
	bool large_residual = fMultiFitter->FindLargeResidual(hPulseToFit, add_location.amplitude, add_location.time);

	if (large_residual) {
	  peak_locations.push_back(add_location);
	  //	  std::cout << "Added peak... t = " << add_location.time << ", A = " << add_location.amplitude << std::endl;
	}
      }
    }


    for (std::vector<TTemplateFitAnalysedPulse*>::const_iterator i_best_tap = best_taps.begin(); i_best_tap != best_taps.end(); ++i_best_tap) {
      analysedList.push_back(*i_best_tap);

      /*      	  if ((*i_best_tap)->GetParentID() == 71 ||(*i_best_tap)->GetParentID() == 124 || (*i_best_tap)->GetParentID() == 175 || (*i_best_tap)->GetParentID() == 335
	      || (*i_best_tap)->GetParentID() == 462 || (*i_best_tap)->GetParentID() == 626 || (*i_best_tap)->GetParentID() == 649 || (*i_best_tap)->GetParentID() == 832
	      || (*i_best_tap)->GetParentID() == 840 || (*i_best_tap)->GetParentID() == 907 || (*i_best_tap)->GetParentID() == 915 || (*i_best_tap)->GetParentID() == 969) {
	    
	    std::cout << "AE: "  << (*i_best_tap)->GetParentID() << ": " << (*i_best_tap)->GetTime() << " " << (*i_best_tap)->GetAmplitude() << std::endl;
	  }
      */  
    }

    delete hPulseUnrefined;
    delete hPulseToFit;
  }
  std::sort(analysedList.begin(), analysedList.end(), IsTimeOrdered);

  // returning 0 tells the caller that we were successful, return non-zero if not
  return 0;
}

bool TemplateFitAPGenerator::PassesIntegralRatio(const TPulseIsland* pulse, double& integral, double& ratio)const{
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

bool TemplateFitAPGenerator::RefitWithTwo(TH1D* tpi, TTemplateFitAnalysedPulse*& tap_one, TTemplateFitAnalysedPulse*& tap_two)const{

  // find the max bin after the current fitted tap
  int second_time=-9999999;
  double second_scale=-9999999;
  //  InitializeSecondPulse(tpi,tap_one,second_time,second_scale);

  TemplateMultiFitter* fitter = new TemplateMultiFitter(GetChannel().str());
  fitter->AddTemplate(fTemplate);
  fitter->AddTemplate(fTemplate);
  fitter->Init();


  // fit the second template around the second peak
  fitter->SetPedestalEstimate(tap_one->GetPedestal());
  double init_amp= tpi->GetBinContent(tpi->GetMaximumBin());///fTemplateAmp;
  double init_time=  tpi->GetXaxis()->GetBinLowEdge(tpi->GetMaximumBin()) * fTemplate->GetRefineFactor(); //- fTemplateTime ;
  fitter->SetPulseEstimates( 0, tap_one->GetAmplitude(), tap_one->GetTime() * fTemplate->GetRefineFactor());
  fitter->SetPulseEstimates( 1, tap_two->GetAmplitude(), tap_two->GetTime() * fTemplate->GetRefineFactor());
  //  fitter->SetPulseEstimates( 0, init_amp, init_time);
  //  fitter->SetPulseEstimates( 1, init_amp, init_time);
  //  fitter->SetPulseEstimates( 1, second_scale, second_time);
  std::cout << "Inital estimates for the double fitter:" << std::endl;
  std::cout << "Tempalte #0: A = " << tap_one->GetAmplitude() << ", t = " << tap_one->GetTime() << " template bins" << std::endl;
  std::cout << "Tempalte #1: A = " << tap_two->GetAmplitude() << ", t = " << tap_two->GetTime() << " template bins" << std::endl;
  //  std::cout << "Tempalte #0: A = " << init_amp << ", t = " << init_time << " template bins" << std::endl;
  //  std::cout << "Tempalte #1: A = " << second_scale << ", t = " << second_time << " template bins" << std::endl;
  int fit_status = fitter->FitWithOneTimeFree(1, tpi, 5*fTemplate->GetRefineFactor());
  //  int fit_status = fitter->FitWithOneTimeFree(0, tpi);

  // fit the first pulse again
  fit_status = fitter->FitWithOneTimeFree(0, tpi, 5*fTemplate->GetRefineFactor());
  //  fit_status = fitter->FitWithOneTimeFree(1, tpi);

  // fit the second pulse again
  fit_status = fitter->FitWithAllTimesFixed(tpi);
  //  int fit_status = fitter->FitWithTwoTimesFree(0, 1, tpi);
  //  std::cout << "Fit Status (double fitter) = " << fit_status << std::endl;

  if(fit_status==0){
    // Has the double fit improved the chi-2 per NDoF
    double old_chi2=tap_one->GetChi2()/tap_one->GetNDoF();
    double new_chi2=fitter->GetChi2()/fitter->GetNDoF();
    std::cout << "Old Chi^2 = " << old_chi2 << ", New Chi^2 = " << new_chi2 << std::endl;
    if(old_chi2 > new_chi2){
      // refill tap_one's values
      std::cout << "Creating 2 TAPs" << std::endl;
      tap_one->SetPedestal(fitter->GetPedestal());
      tap_one->SetAmplitude(fitter->GetAmplitude(0));
      tap_one->SetTime(fitter->GetTime(0));
      tap_one->SetChi2(fitter->GetChi2());
      tap_one->SetNDoF(fitter->GetNDoF());
      tap_one->SetFitStatus(fit_status);

      // make the second tap
      //      tap_two = MakeNewTAP<TTemplateFitAnalysedPulse>(tap_one->GetParentID());
      tap_two->SetTemplate(fTemplate);
      tap_two->SetPedestal(fitter->GetPedestal());
      tap_two->SetAmplitude(fitter->GetAmplitude(1));
      tap_two->SetTime(fitter->GetTime(1));
      tap_two->SetChi2(fitter->GetChi2());
      tap_two->SetNDoF(fitter->GetNDoF());
      tap_two->SetFitStatus(fit_status);

      std::cout << "\tTap 1: A = " << tap_one->GetAmplitude() << ", t = " << tap_one->GetTime() << ", P = " << tap_one->GetPedestal() << std::endl;
      std::cout << "\tTap 2: A = " << tap_two->GetAmplitude() << ", t = " << tap_two->GetTime() << ", P = " << tap_one->GetPedestal() << std::endl;
      // link the two pulses together
      tap_one->SetOtherPulse(tap_two);
      tap_two->SetOtherPulse(tap_one);
      tap_two->IsPileUpPulse();

      delete fitter;
      return true;
    }
  } 

  return false;
}

void TemplateFitAPGenerator::InitializeSecondPulse(
          TH1D* tpi, const TTemplateFitAnalysedPulse* tap_one,
          int& second_time, double& second_scale)const{
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
    second_time=max_bin;// - fTemplate2->GetTime();
    second_scale=max_residual;// / fTemplate2->GetAmplitude();
    std::cout << "Initializing second pulse to A = " << second_scale << " and t = " << second_time << std::endl;
}

ALCAP_TAP_GENERATOR(TemplateFit,template_archive,constant_fraction,no_time_shift,sync_pulses, use_IR_cut, min_integral, max_integral, min_ratio, max_ratio, template_archive_2,min_chi2_to_refit);
