#include "TemplateCreator.h"
#include "RegisterModule.inc"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "definitions.h"
#include "EventNavigator.h"
#include "SetupNavigator.h"
#include "ExportPulse.h"
#include "debug_tools.h"

#include "TH1D.h"
#include "TMath.h"

#include <iostream>
#include <sstream>
#include <stdexcept>
using std::cout;
using std::endl;


TemplateCreator::TemplateCreator(modules::options* opts):
  BaseModule("TemplateCreator",opts), fOpts(opts),
  fPulseDebug(false), fAnalyseAllChannels(false)
{
  fRefineFactor = opts->GetInt("refine_factor", 5);
  fPulseDebug = opts->GetBool("pulse_debug", false);
  opts->GetVectorStringsByDelimiter("channels",fRequestedChannels);
  fArchiveName=opts->GetString("file_name","templates.root");
  if(fRequestedChannels.empty()) fAnalyseAllChannels=true;

  // Prepare integral ratio cuts
  fCutIntegralRatio=opts->GetBool("use_IR_cut",false);
  if(fCutIntegralRatio){
    fIntegralMax=opts->GetDouble("max_integral");
    fIntegralMin=opts->GetDouble("min_integral");
    fIntegralRatioMax=opts->GetDouble("max_ratio");
    fIntegralRatioMin=opts->GetDouble("min_ratio");
  }
  fSinglePulse = opts->GetBool("single_pulse", false);
}

TemplateCreator::~TemplateCreator(){
///  delete fTemplateArchive;
///  fChannels.clear();
}

TemplateCreator::ChannelSet::ChannelSet(const std::string& det, const std::string& bank,
  modules::options* opts, int refine):
    detname(det), bankname(bank),
    fit_successes(0),fit_attempts(0),
    trigger_polarity(TSetupData::Instance()->GetTriggerPolarity(bank)),
    pulse_finder(NULL),
    integralRatio(new Algorithm::IntegralRatio(
                          opts->GetInt("start_integral",0),
                          opts->GetInt("start_tail",60),
                          opts->GetInt("stop_integral",0),
                          trigger_polarity))
{
       // Create the pulse candidate finder for this detector
       if(!opts->GetFlag("no_PCF_check")){
          pulse_finder = new PulseCandidateFinder_TSpectrum(detname, opts);
       }

       // Create the TemplateMultiFitter that we will use for this channel
       fitter = new TemplateMultiFitter(detname, refine);
       
       template_pulse=new TTemplate(detname,refine,trigger_polarity,opts->GetFlag("debug"));
}

void TemplateCreator::ChannelSet::Clear(){
  delete fitter;
  delete template_pulse;
  if(pulse_finder) delete pulse_finder;
  if(integralRatio) delete integralRatio;
}

// Called before the main event loop
// Can be used to set things up, like histograms etc
// Return non-zero to indicate a problem
int TemplateCreator::BeforeFirstEntry(TGlobalData* gData, const TSetupData* setup){

  // Prepare the template archive
  if(fArchiveName==EventNavigator::Instance().GetOutputFileName()){
    fTemplateArchive = new TemplateArchive(GetDirectory());
  } else{
    fTemplateArchive = new TemplateArchive(fArchiveName.c_str(), "RECREATE");
  }

  // Set all the converged statuses to false
  StringPulseIslandMap::const_iterator it;
  for(it = gData->fPulseIslandToChannelMap.begin(); it != gData->fPulseIslandToChannelMap.end(); ++it){
       const std::string bankname = it->first;
       const std::string detname = TSetupData::Instance()->GetDetectorName(bankname);
       if(!fAnalyseAllChannels &&
          std::find(fRequestedChannels.begin(), fRequestedChannels.end(), detname)
              ==fRequestedChannels.end()) {
           continue;
       }
       if(Debug()) cout<<"TemplateCreator::BeforeFirstEntry: Will make template for '"<<detname<<"'"<<endl;

       fChannels.push_back(ChannelSet(detname,bankname,fOpts,fRefineFactor));
  }

       fIntegralRatioHist = new TH1F("hIntegralRatioHist", "", 1000,0,1);
  return 0;
}

// Called once for each event in the main event loop
// Return non-zero to indicate a problem and terminate the event loop
int TemplateCreator::ProcessEntry(TGlobalData* gData, const TSetupData* setup){

  // Loop over each detector
  unsigned no_converged=0;
  for(ChannelList::iterator i_ch=fChannels.begin(); i_ch!=fChannels.end(); ++i_ch){

    // See if we already have a converged template for this detector
    if (i_ch->template_pulse->HasConverged() || (fSinglePulse && !i_ch->template_pulse->Empty())) {
      no_converged++;
      continue;
    }

    // Get the bank and detector names for this detector
    const std::string& bankname = i_ch->bankname;
    const std::string& detname = i_ch->detname;
    const PulseIslandList& thePulseIslands= gData->fPulseIslandToChannelMap.at(bankname);
    fTemplateFitter=i_ch->fitter;

    // Get the TPIs
    if (thePulseIslands.size() == 0) continue; // no pulses here..

    // Loop through all the pulses
    for (PulseIslandList::const_iterator pulseIter = thePulseIslands.begin();
          pulseIter != thePulseIslands.end(); ++pulseIter) {

      TPulseIsland* pulse = *pulseIter;
      
      // Loop through the samples and check for digitizer overflow
      int over_under_flow=HasPulseOverflowed(pulse,i_ch->bankname);
      if(over_under_flow!=0){
	if (Debug()) {
	  cout << "TemplateCreator: Pulse #" << pulseIter - thePulseIslands.begin() << " has ";
	  if(over_under_flow>0) cout << "overflowed";
	  else cout<<"undeflowed";
	  cout <<" the digitizer and won't be added to the template" << endl;
	}
        continue; // skip this pulse
      }

      // Check that the PulseCandidateFinder only sees 1 peak
      if(i_ch->pulse_finder){
	// First we will see how many candidate pulses there are on the TPI
	i_ch->pulse_finder->FindPulseCandidates(pulse);
	int n_pulse_candidates = i_ch->pulse_finder->GetNPulseCandidates();
	
	// only continue if there is one pulse candidate on the TPI
	if (n_pulse_candidates != 1) {
          if (Debug()) {
            cout << "TemplateCreator: Pulse #" << pulseIter - thePulseIslands.begin() << " has " << n_pulse_candidates << " pulse candidates and so won't be added to the template" << std::endl;
          }
	  continue;
	}
      }

      // check it has a nice amplitude
      const std::vector<int>& theSamples = (pulse)->GetSamples();
      int n_samples = theSamples.size();
      double max_sample = -9999999;
      int trigger_polarity = TSetupData::Instance()->GetTriggerPolarity(bankname);
      for (int i = 0; i < n_samples; ++i) {
	int sample_value = trigger_polarity*theSamples.at(i);
	if (sample_value > max_sample) {
	  max_sample = sample_value;
	}
      }
      double pedestal = SetupNavigator::Instance()->GetPedestal(detname);
      if (max_sample-(trigger_polarity*pedestal) < 1000 || max_sample-(trigger_polarity*pedestal) > 1500) {
	if (Debug()) {
	  cout << "TemplateCreator: Pulse #" << pulseIter - thePulseIslands.begin()
	       << " has a too low or high peak sample value (" << max_sample << ")" << std::endl;
	}
	continue;
      }
      
      // check this pulse is safely within the waveform
      int pulse_length = pulse->GetSamples().size();
      if (pulse->GetPeakSample() >= pulse_length - 2*pulse_length/5.0) {
	if (Debug()) {
	  cout << "TemplateCreator: Pulse #" << pulseIter - thePulseIslands.begin()
	       << " is too close to one end of the island and so won't be used as "
	    "the first pulse in the template." << endl;
	}
	//	return NULL;
	continue;
      }

      //      if(fCutIntegralRatio){
      /*         try{
           (*i_ch->integralRatio)(pulse);
         }catch(std::out_of_range& e){
           continue;
         }
         const double& integral=i_ch->integralRatio->GetTotal();
         const double& ratio=i_ch->integralRatio->GetRatio();
	 std::cout << "Integral = " << integral << ", Tail = " << i_ch->integralRatio->GetTail() << ", ratio = " << ratio << std::endl;
	 fIntegralRatioHist->Fill(ratio);
	 //if (ratio < 0.34 || ratio > 0.45) {
	 if (ratio < 0.28 || ratio > 0.36){
	   cout << "TemplateCreator: Pulse #" << pulseIter - thePulseIslands.begin()
		<< " has an integral ratio that is too low (" << ratio << ")" << std::endl;
	   continue;
	 }
      */
	 //	 if( fIntegralMax < integral || fIntegralMin > integral
	 //	     || fIntegralRatioMax < ratio || fIntegralRatioMin > ratio) {
	 //	   continue;
	 //	 }
	 //      }


      // Add the first pulse directly to the template (although we may try and choose a random pulse to start with)
      if (i_ch->template_pulse->Empty()) {
        // Create the refined pulse waveform
        int pulseID=pulseIter- thePulseIslands.begin();
        TH1D* tpl=StartTemplate(pulseID, pulse,i_ch->detname);
        if(tpl) { 
	  i_ch->template_pulse->Initialize(pulseID,tpl,GetDirectory());
	  //	  i_ch->template_pulse->NormaliseToAmplitude();
	  fTemplateFitter->AddTemplate(i_ch->template_pulse);
	  fTemplateFitter->Init();

	  if (fPulseDebug) {
	    std::stringstream newhistname;
	    newhistname << "hTemplate_" << i_ch->template_pulse->PulsesMerged() << "Pulses_" << detname;
	    i_ch->template_pulse->GetHisto()->Clone(newhistname.str().c_str())->Write();
	  }
	  if (Debug()) {
	    cout << "TemplateCreator: Initial Template Pedestal = " 
		 << i_ch->template_pulse->GetPedestal() << " ADC, Amplitude = "
		 << i_ch->template_pulse->GetAmplitude() << " ADC, Time = "
		 << i_ch->template_pulse->GetTime() << " template time bins" << std::endl << std::endl;

	  }
	}
	if (fSinglePulse) {
	  break; // we only want a one-pulse template so go to the next channel
	}
	else {
	  continue; // add more pulses
	}
      }


      // Create the refined pulse waveform
      TH1D* hPulseToFit = CreateRefinedPulseHistogram(pulse, "hPulseToFit", "hPulseToFit", false);

      // all the other pulses will be fitted to the template and then added to it
      // Get some initial estimates for the fitter
      double template_pedestal = i_ch->template_pulse->GetPedestal();
      double template_amplitude = i_ch->template_pulse->GetAmplitude();
      double template_time = i_ch->template_pulse->GetTime();

      int n_bins_for_pedestal = 5*fRefineFactor;
      double total = 0;
      for (int iBin = 1; iBin <= n_bins_for_pedestal; ++iBin) {
	total += hPulseToFit->GetBinContent(iBin);
      }
      double pulse_pedestal = total / n_bins_for_pedestal;
      double pulse_amplitude = definitions::DefaultValue;
      double pulse_time = definitions::DefaultValue;

      // now we're dealing with actual pulses since we subtract the template_pedestal in the transformation
      double pedestal_offset_estimate = pulse_pedestal - template_pedestal; 
      double amplitude_scale_factor_estimate = definitions::DefaultValue;
      double time_offset_estimate = definitions::DefaultValue;

      // Define the values to scale and shift things by 
      //      if (TSetupData::Instance()->GetTriggerPolarity(bankname) >0) { 
        pulse_amplitude = (hPulseToFit->GetMaximum() - pulse_pedestal);
        pulse_time = hPulseToFit->GetMaximumBin() - 1;
	//      }
	//      else if (TSetupData::Instance()->GetTriggerPolarity(bankname) <0) {
	//        pulse_amplitude = (pulse_pedestal - hPulseToFit->GetMinimum());
	//        pulse_time = hPulseToFit->GetMinimumBin() - 1; // go from bin numbering (1, n_samples) to clock ticks (0, n_samples-1)
	//      }
      amplitude_scale_factor_estimate = pulse_amplitude / template_amplitude;  // estimated scale factor
      time_offset_estimate = pulse_time - template_time;

      //      fTemplateFitter->SetInitialParameterEstimates(pedestal_offset_estimate, amplitude_scale_factor_estimate, time_offset_estimate);
      //      std::cout << "TemplateCreator: Template: Pedestal = " << template_pedestal << " ADC, Amplitude = " << template_amplitude << " ADC, Time = " << template_time << " template time bins" << std::endl;
      //      std::cout << "TemplateCreator: Pulse: Pedestal = " << pulse_pedestal << " ADC, Amplitude = " << pulse_amplitude << " ADC, Time = " << pulse_time << " template time bins" << std::endl;
      fTemplateFitter->SetPedestalEstimate(pulse_pedestal);
      fTemplateFitter->SetPulseEstimates(0, pulse_amplitude, pulse_time);

      //      std::cout << "TemplateCreator: Initial Estimates: PedOffset = " << pedestal_offset_estimate << ", AmpScaleFactor = " << amplitude_scale_factor_estimate << ", TimeOffset = " << time_offset_estimate << std::endl;
      
      //      fTemplateFitter->SetPedestal(pedestal_offset_estimate);
      //      fTemplateFitter->SetPulseEstimates(0, amplitude_scale_factor_estimate, time_offset_estimate);
      
      //      int fit_status = fTemplateFitter->FitPulseToTemplate(i_ch->template_pulse, hPulseToFit, bankname);
      int fit_status = fTemplateFitter->FitWithOneTimeFree(0, hPulseToFit, 5*fRefineFactor);
      ++i_ch->fit_attempts;
      if (fit_status != 0) {
        if (Debug()) {
          std::cout << "TemplateCreator: Problem with fit (status = " << fit_status << ")" << std::endl;
        }
        delete hPulseToFit; // delete this here since it is no longer needed
        continue;
      }
      ++i_ch->fit_successes;

      if (Debug()) {
        cout << "TemplateCreator: Fitted Parameters: Pedestal = " 
             << fTemplateFitter->GetPedestal() << " ADC, "
	     << "Amplitude = " << fTemplateFitter->GetAmplitude(0) << " ADC (SF = " << fTemplateFitter->GetAmplitudeScaleFactor(0) << " ADC), "
	     << "Time = " << fTemplateFitter->GetTime(0) << " template time bins (Offset = " << fTemplateFitter->GetTimeOffset(0) << " template time bins), "
	     << std::endl << "Chi2 = " << fTemplateFitter->GetChi2() << ", NDoF = "
             << fTemplateFitter->GetNDoF() << ", Prob = "
             << TMath::Prob(fTemplateFitter->GetChi2(), fTemplateFitter->GetNDoF()) << std::endl << std::endl;
      }

      // Add the pulse to the template (we'll do correct the sample values there)
      i_ch->template_pulse->AddPulse(
				     //                  fTemplateFitter->GetTimeOffset(),
				     //                  fTemplateFitter->GetAmplitudeScaleFactor(),
				     //                  fTemplateFitter->GetPedestalOffset(),
				     fTemplateFitter->GetTime(0), // get the time of the 0th template (should be only one for the template creator)
				     fTemplateFitter->GetAmplitude(0),
				     fTemplateFitter->GetPedestal(),
                  hPulseToFit);

      if (fPulseDebug) {
        // Print out some templates as we go along
	//        if (i_ch->template_pulse->PulsesMerged() <= 10 
	//             || ( i_ch->template_pulse->PulsesMerged() <= 100 
	//                     && i_ch->template_pulse->PulsesMerged()%10 == 0)
	//             || ( i_ch->template_pulse->PulsesMerged()%100 == 0) ) {
          std::stringstream newhistname;
          newhistname << "hTemplate_" << i_ch->template_pulse->PulsesMerged() << "Pulses_" << detname;
          i_ch->template_pulse->GetHisto()->Clone(newhistname.str().c_str())->Write();

	  newhistname.str("");
	  newhistname << "hAddedPulse_" << i_ch->template_pulse->PulsesMerged() << "Pulses_" << detname;
          hPulseToFit->Clone(newhistname.str().c_str())->Write();
	  //        }
      }


      // we keep on adding pulses until adding pulses has no effect on the template
      if (i_ch->template_pulse->CheckConverged()) {
        cout << "TemplateCreator: " << detname << " template terminated containing " 
             << i_ch->template_pulse->PulsesMerged() <<" pulses "<< std::endl;
        break; // break from the for loop
      }
    } // end for loop over TPIs
  } //end for loop through channels
  
  if(no_converged==fChannels.size()){
  cout<<"All channels converged so end run"<<endl;
  return -1;
  }

  return 0;
}

// Called just after the main event loop
// Can be used to write things out, dump a summary etc
// Return non-zero to indicate a problem
int TemplateCreator::AfterLastEntry(TGlobalData* gData, const TSetupData* setup){

  // Print to stdout the percentage of successful fit for each channel
  for(ChannelList::iterator i_ch=fChannels.begin(); i_ch!=fChannels.end(); ++i_ch){

    if (i_ch->template_pulse->Empty()) { // if there's no template been created for this channel
      continue;
    }
    
    cout << "TemplateCreator: " << i_ch->detname 
         << ": " << i_ch->fit_attempts << " fits attempted with "
         << i_ch->fit_successes << " successful (" 
         << ((double)i_ch->fit_successes/(double)i_ch->fit_attempts)*100 << "%)" << endl;

    // Normalise the templates
    i_ch->template_pulse->NormaliseToAmplitude();
    //    i_ch->template_pulse->CreateTemplateErrorsHist();
    i_ch->template_pulse->AddToDirectory(GetDirectory("../"), GetDirectory());
    
    // Save the template to the file
    fTemplateArchive->SaveTemplate(i_ch->template_pulse);
    
    // tidy up
    i_ch->Clear();
  }

  return 0;
}

int TemplateCreator::HasPulseOverflowed(const TPulseIsland* pulse, const std::string& bankname){
    // Get the samples so we can check for digitiser overflow
    const std::vector<int>& theSamples = (pulse)->GetSamples();
    int n_samples = theSamples.size();

    // Calculate the maximum ADC value for this digitiser
    int n_bits = TSetupData::Instance()->GetNBits(bankname);
    double max_adc_value = std::pow(2, n_bits);
    for (int i = 0; i < n_samples; ++i) {
      int sample_value = theSamples.at(i);

      if (sample_value >= max_adc_value-1 && sample_value <= max_adc_value+1) {
         return 1;
      }
      else if (sample_value == 0) {
         return -1;
      }
    }
    // no under or overflow
    return 0;
}

TH1D* TemplateCreator::StartTemplate(int pulseID, const TPulseIsland* pulse,const std::string& detname){

  if (Debug()) {
    cout << "TemplateCreator: Adding " << detname << " Pulse #" 
         << pulseID << " directly to the template" << endl;
  }

  std::string histname = "hTemplate_" + detname;
  std::string histtitle = "Template Histogram for the " + detname + " channel";
  return CreateRefinedPulseHistogram(pulse, histname.c_str(), histtitle.c_str(), true);
}

// The following macro registers this module to be useable in the config file.
// The first argument is compulsory and gives the name of this module
// All subsequent arguments will be used as names for arguments given directly 
// within the modules file.
ALCAP_REGISTER_MODULE(TemplateCreator);
