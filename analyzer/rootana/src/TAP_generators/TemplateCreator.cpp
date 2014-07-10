#include "TemplateCreator.h"
#include "RegisterModule.inc"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"

#include "definitions.h"

#include "SetupNavigator.h"
#include "ExportPulse.h"

#include "TMath.h"

#include <iostream>
#include <sstream>
using std::cout;
using std::endl;

TemplateCreator::TemplateCreator(modules::options* opts):
  BaseModule("TemplateCreator",opts), fOpts(opts){

  // Do something with opts here.  Has the user specified any
  // particular configuration that you want to know?
  // For example, perhaps this module wants an axis range:
}

TemplateCreator::~TemplateCreator(){
}

// Called before the main event loop
// Can be used to set things up, like histograms etc
// Return non-zero to indicate a problem
int TemplateCreator::BeforeFirstEntry(TGlobalData* gData, const TSetupData* setup){
  // Print extra info if we're debugging this module:
  if(Debug()){
     cout<<"-----I'm debugging TemplateCreator::BeforeFirstEntry() "<<endl;
  }

  // Prepare the template archive
  fTemplateArchive = new TemplateArchive("templates.root", "RECREATE");

  return 0;
}

// Called once for each event in the main event loop
// Return non-zero to indicate a problem and terminate the event loop
int TemplateCreator::ProcessEntry(TGlobalData* gData, const TSetupData* setup){

  // Prepare a few variables
  std::string bankname, detname;
  PulseIslandList thePulseIslands;
  StringPulseIslandMap::const_iterator it;

  // Loop over each detector
  for(it = gData->fPulseIslandToChannelMap.begin(); it != gData->fPulseIslandToChannelMap.end(); ++it){

    // Get the bank and detector names for this detector
    bankname = it->first;
    detname = setup->GetDetectorName(bankname);

    // Create the pulse candidate finder for this detector
    PulseCandidateFinder* pulse_candidate_finder = new PulseCandidateFinder(detname, fOpts);

    // Create the TemplateFitter that we will use for this channel
    fTemplateFitter = new TemplateFitter(detname);

    // Get the TPIs
    thePulseIslands = it->second;
    if (thePulseIslands.size() == 0) continue; // no pulses here..

    // Try and get the template (it may have been created in a previous event)
    std::string template_name = "hTemplate_" + detname;
    TH1D* hTemplate = fTemplateArchive->GetTemplate(template_name.c_str());

    // Store a couple of numbers to get an idea of how many successful fits there are
    int& n_fit_attempts = fNFitAttempts[detname]; // number of pulses we try to fit to
    int& n_successful_fits = fNSuccessfulFits[detname];
    int& n_pulses_in_template = fNPulsesInTemplate[detname];

    // Loop through all the pulses
    for (PulseIslandList::iterator pulseIter = thePulseIslands.begin(); pulseIter != thePulseIslands.end(); ++pulseIter) {

      // First we will see how many candidate pulses there are on the TPI
      pulse_candidate_finder->FindPulseCandidates(*pulseIter);
      int n_pulse_candidates = pulse_candidate_finder->GetNPulseCandidates();

      std::vector<TPulseIsland*> pulse_candidates = pulse_candidate_finder->GetPulseCandidates();

      // only continue if there is one pulse candidate on the TPI
      if (n_pulse_candidates == 1) {

	TPulseIsland* pulse = *pulseIter;

        // Add the first pulse directly to the template (although we may try and choose a random pulse to start with)
	if (hTemplate == NULL) {
	  AddPulseToTemplate(hTemplate, pulse);
	  ++n_pulses_in_template;

	  const std::vector<int>& theSamples = (pulse)->GetSamples();
	  std::stringstream histname;
	  histname << template_name << "_Pulse" << pulseIter - thePulseIslands.begin();
	  TH1D* hUncorrectedPulse = new TH1D(histname.str().c_str(), histname.str().c_str(), theSamples.size(), 0, theSamples.size());

	  double pedestal_error = SetupNavigator::Instance()->GetPedestalError(bankname);
	  for (std::vector<int>::const_iterator sampleIter = theSamples.begin(); sampleIter != theSamples.end(); ++sampleIter) {

	    double uncorrected_value = (*sampleIter);
	    
	    hUncorrectedPulse->SetBinContent(sampleIter - theSamples.begin()+1, uncorrected_value); // +1 because bins start at 1
	    hUncorrectedPulse->SetBinError(sampleIter - theSamples.begin()+1, pedestal_error);
	  }

	  if (Debug()) {
	    std::cout << "TemplateCreator: Adding " << detname << " Pulse #" << pulseIter - thePulseIslands.begin() << " directly to the template" << std::endl;
	  }
	  continue;
	}

	// all the other pulses will be fitted to the template and then added to it
	// Get some initial estimates for the fitter
	double template_pedestal = hTemplate->GetBinContent(1);
	double template_amplitude;
	double template_time;

	double pulse_pedestal = (pulse)->GetSamples().at(0);
	double pulse_amplitude = (pulse)->GetAmplitude();
	double pulse_time = (pulse)->GetPeakSample(); // between 0 and n_samples-1

	double pedestal_offset_estimate = template_pedestal - pulse_pedestal;
	double amplitude_scale_factor_estimate;
	double time_offset_estimate;
	if (TSetupData::Instance()->GetTriggerPolarity(bankname) == 1) { 
	  template_amplitude = (hTemplate->GetMaximum() - template_pedestal);
	  template_time = hTemplate->GetMaximumBin() - 1; // go from bin numbering (1, n_samples) to clock ticks (0, n_samples-1)

	  amplitude_scale_factor_estimate = pulse_amplitude / template_amplitude;  // estimated scale factor
	  time_offset_estimate = pulse_time - template_time;
	}
	else if (TSetupData::Instance()->GetTriggerPolarity(bankname) == -1) {
	  template_amplitude = (template_pedestal - hTemplate->GetMinimum());
	  template_time = hTemplate->GetMinimumBin() - 1; // go from bin numbering (1, n_samples) to clock ticks (0, n_samples-1)

	  amplitude_scale_factor_estimate = pulse_amplitude / template_amplitude;  // estimated scale factor
	  time_offset_estimate = pulse_time - template_time;
	}

	fTemplateFitter->SetInitialParameterEstimates(pedestal_offset_estimate, amplitude_scale_factor_estimate, time_offset_estimate);
	
	if (Debug()) {
	  std::cout << "TemplateCreator: " << detname << "(" << bankname << "): Pulse #" << pulseIter - thePulseIslands.begin() << ": " << std::endl
		    << "TemplateCreator: Template: pedestal = " << template_pedestal << ", amplitude = " << template_amplitude << ", time = " << template_time << std::endl
		    << "TemplateCreator: Pulse: pedestal = " << pulse_pedestal << ", amplitude = " << pulse_amplitude << ", time = " << pulse_time << std::endl
		    << "TemplateCreator: Initial Estimates: pedestal = " << pedestal_offset_estimate << ", amplitude = " << amplitude_scale_factor_estimate 
		    << ", time = " << time_offset_estimate << std::endl;
	}

	int fit_status = fTemplateFitter->FitPulseToTemplate(hTemplate, pulse);
	++n_fit_attempts;
	if (fit_status != 0) {
	  if (Debug()) {
	    std::cout << "TemplateCreator: Problem with fit (status = " << fit_status << ")" << std::endl;
	  }
	  continue;
	}
	++n_successful_fits;
	//	ExportPulse::Instance()->AddToExportList(detname, pulseIter - thePulseIslands.begin());

	if (Debug()) {
	  std::cout << "Template Creator: Fitted Parameters: PedOffset = " << fTemplateFitter->GetPedestalOffset() << ", AmpScaleFactor = " << fTemplateFitter->GetAmplitudeScaleFactor()
	            << ", TimeOffset = " << fTemplateFitter->GetTimeOffset() << ", Chi2 = " << fTemplateFitter->GetChi2() << ", NDoF = " << fTemplateFitter->GetNDoF() 
		    << ", Prob = " << TMath::Prob(fTemplateFitter->GetChi2(), fTemplateFitter->GetNDoF()) << std::endl << std::endl;
	}

	// Now add the fitted pulse to the template

	if (n_pulses_in_template <= 10 || 
	    (n_pulses_in_template <= 100 && n_pulses_in_template%10 == 0) ) {
	  std::stringstream newhistname;
	  newhistname << "hTemplate_" << n_pulses_in_template << "Pulses_" << detname;
	  TH1D* new_template = (TH1D*) hTemplate->Clone(newhistname.str().c_str());
	}

	// Add the pulse to the template (we'll do the correcting there)
	AddPulseToTemplate(hTemplate, pulse);
	++n_pulses_in_template;
	double error_of_max_bin;
	if (TSetupData::Instance()->GetTriggerPolarity(bankname) == 1) {
	  error_of_max_bin = hTemplate->GetBinError(hTemplate->GetMaximumBin());
	}
	else if (TSetupData::Instance()->GetTriggerPolarity(bankname) == -1) {
	  error_of_max_bin = hTemplate->GetBinError(hTemplate->GetMinimumBin());
	}
	fErrorVsPulseAddedHistograms.at(detname)->Fill(n_pulses_in_template, error_of_max_bin);
	double prob = TMath::Prob(fTemplateFitter->GetChi2(), fTemplateFitter->GetNDoF());
	if (prob != 0) {
	  fProbVsPulseAddedHistograms.at(detname)->Fill(n_pulses_in_template, -(TMath::Log(prob)));
	}
	
	// Create the corrected pulse
	const std::vector<int>& theSamples = (pulse)->GetSamples();
	std::stringstream histname;
	histname << template_name << "_Pulse" << pulseIter - thePulseIslands.begin();
	TH1D* hUncorrectedPulse = new TH1D(histname.str().c_str(), histname.str().c_str(), theSamples.size(), 0, theSamples.size());
	histname << "_Corrected";
	TH1D* hCorrectedPulse = new TH1D(histname.str().c_str(), histname.str().c_str(), theSamples.size(), 0, theSamples.size());

	double pedestal_error = SetupNavigator::Instance()->GetPedestalError(bankname);
	for (std::vector<int>::const_iterator sampleIter = theSamples.begin(); sampleIter != theSamples.end(); ++sampleIter) {

	  double uncorrected_value = (*sampleIter);
	  double corrected_value = CorrectSampleValue(uncorrected_value);

	  hUncorrectedPulse->SetBinContent(sampleIter - theSamples.begin()+1, uncorrected_value); // +1 because bins start at 1
	  hUncorrectedPulse->SetBinError(sampleIter - theSamples.begin()+1, pedestal_error);
	  hCorrectedPulse->SetBinContent(sampleIter - theSamples.begin()+1 +0.5 - fTemplateFitter->GetTimeOffset(), corrected_value); 
	  hCorrectedPulse->SetBinError(sampleIter - theSamples.begin()+1 +0.5 - fTemplateFitter->GetTimeOffset(), pedestal_error);
	}
	// we keep on adding pulses until adding pulses has no effect on the template
      }
    }
    // We will want to normalise to template to have pedestal=0 and amplitude=1
    // Save the template to the file
    fTemplateArchive->SaveTemplate(hTemplate);
  }

  return 0;
}

// Called just after the main event loop
// Can be used to write things out, dump a summary etc
// Return non-zero to indicate a problem
int TemplateCreator::AfterLastEntry(TGlobalData* gData, const TSetupData* setup){

  // Print extra info if we're debugging this module:
  if(Debug()){
     cout<<"-----I'm debugging TemplateCreator::AfterLastEntry()"<<endl;
  }

  StringPulseIslandMap::const_iterator it;

  // Loop over each detector
  for(it = gData->fPulseIslandToChannelMap.begin(); it != gData->fPulseIslandToChannelMap.end(); ++it){
    std::string bankname = it->first;
    std::string detname = setup->GetDetectorName(bankname);

    int& n_fit_attempts = fNFitAttempts[detname]; // number of pulses we try to fit to
    int& n_successful_fits = fNSuccessfulFits[detname];
    std::cout << "TemplateCreator: " << detname << ": " << n_fit_attempts << " fits attempted with " << n_successful_fits << " successful (" << ((double)n_successful_fits/(double)n_fit_attempts)*100 << "%)" << std::endl;
  }

  // Clean up the template archive
  delete fTemplateArchive;

  return 0;
}

/// AddPulseToTemplate()
/// Adds the given pulse to the template
void TemplateCreator::AddPulseToTemplate(TH1D* & hTemplate, const TPulseIsland* pulse) {

  // Get the samples so that we can add them to the template
  const std::vector<int>& theSamples = pulse->GetSamples();
  int n_samples = theSamples.size();

  std::string bankname = pulse->GetBankName();
  std::string detname = TSetupData::Instance()->GetDetectorName(bankname);

  // Wnat to increase the bin resolution (this may become a module option at some point)
  int refine_factor = 5;

  // If the template histogram is NULL, then create it
  if (hTemplate == NULL) {

    // Names for the histograms
    std::string histname = "hTemplate_" + detname;
    std::string histtitle = "Template Histogram for the " + detname + " channel";

    hTemplate = new TH1D(histname.c_str(), histtitle.c_str(), n_samples, 0, n_samples);

    // Add the first pulse and the correct initial errors
    double pedestal_error = SetupNavigator::Instance()->GetPedestalError(bankname);
    for (std::vector<int>::const_iterator sampleIter = theSamples.begin(); sampleIter != theSamples.end(); ++sampleIter) {
      hTemplate->SetBinContent( sampleIter - theSamples.begin()+1, *sampleIter); // +1 because ROOT numbers bins from 1
      hTemplate->SetBinError( sampleIter - theSamples.begin()+1, pedestal_error);
    }

    std::string error_histname = "hErrorVsPulseAdded_" + detname;
    std::string error_histtitle = "Plot of the Error as each new Pulse is added to the template for the " + detname + " channel";
    int n_bins = 1000000;
    TH1D* error_hist = new TH1D(error_histname.c_str(), error_histtitle.c_str(), n_bins,0,n_bins);
    error_hist->Fill(1, pedestal_error);
    fErrorVsPulseAddedHistograms[detname] = error_hist;

    std::string prob_histname = "hProbVsPulseAdded_" + detname;
    std::string prob_histtitle = "Plot of the Prob as each new Pulse is added to the template for the " + detname + " channel";
    TH1D* prob_hist = new TH1D(prob_histname.c_str(), prob_histtitle.c_str(), n_bins,0,n_bins);
    fProbVsPulseAddedHistograms[detname] = prob_hist;
  }

  else { // add subsequent pulses
    /*    double x, x_old;
    double e, e_old;
    for (int i = 0; i < fNBins; ++i) {
      x_old = fTemplate->GetBinContent(i+1);
      e_old = fTemplate->GetBinError(i+1);
      x = (double)fNPulses * x_old + reshaped_pulse[i];
      x /= (double)(fNPulses + 1);
      e = (double)(fNPulses - 1) * std::pow(e_old, 2.) + ((double)reshaped_pulse[i] - x_old) * ((double)reshaped_pulse[i] - x);
      e = std::sqrt(e / fNPulses);
      fTemplate->SetBinContent(i+1, x);
      fTemplate->SetBinError(i+1, e);
    }
    */
    int n_pulses = fNPulsesInTemplate.at(detname);
    if (Debug()) {
      std::cout << "AddPulseToTemplate(): n_pulses = " << n_pulses << std::endl;
    }

    // Loop through the pulse samples
    for (std::vector<int>::const_iterator sampleIter = theSamples.begin(); sampleIter != theSamples.end(); ++sampleIter) {

      double corrected_value = CorrectSampleValue(*sampleIter);

      int bin_number = sampleIter - theSamples.begin() + 1 + 0.5 - fTemplateFitter->GetTimeOffset(); // +1 because ROOT numbers bins from 1, +0.5 to round to the nearest integer and subtract time offset because this value might not want to go direct into the template

      // Only change the bin contents of bins within the range of the template histogram
      if (bin_number < 1 || bin_number > hTemplate->GetNbinsX()) {
	continue;
      }
      double old_bin_content = hTemplate->GetBinContent(bin_number);
      double old_bin_error = hTemplate->GetBinError(bin_number);

      double new_bin_content = n_pulses * old_bin_content + corrected_value;
      new_bin_content /= (n_pulses + 1);

      double new_bin_error = ((n_pulses - 1)*old_bin_error*old_bin_error) + (corrected_value - old_bin_content)*(corrected_value - new_bin_content);
      new_bin_error = std::sqrt(new_bin_error / n_pulses);

      if (Debug()) {
	std::cout << "TemplateCreator::AddPulseToTemplate(): Bin #" << bin_number << ": Corrected Sample Value = " << corrected_value << std::endl
		  << "\t\t\tOld Value (Error) = " << old_bin_content << "(" << old_bin_error << ")" << std::endl
		  << "\t\t\tNew Value (Error) = " << new_bin_content << "(" << new_bin_error << ")" << std::endl;
      }

      hTemplate->SetBinContent(bin_number, new_bin_content);
      hTemplate->SetBinError(bin_number, new_bin_error);
    }
  }

  // Now loop through the samples and add to the template
  /*  for (int iBin = 0; iBin < refine_factor*n_samples; ++iBin) {

    int sample_element = iBin / refine_factor;
    double value_to_fill = 0;

    if (sample_element != theSamples.size()-1) { // if this element is not going to be outside of the samples vector
      int iSubBin = iBin % refine_factor;
      
      int this_sample = theSamples.at(sample_element);
      int next_sample = theSamples.at(sample_element+1);
      double sample_difference = next_sample - this_sample;
      
      double interpolated_value = this_sample + iSubBin*(sample_difference / refine_factor);
      value_to_fill = interpolated_value;
    }
    else {
      value_to_fill = theSamples.at(sample_element);
    }
    //    std::cout << "Bin#" << iBin << ": " << value_to_fill << std::endl;
    hTemplate->SetBinContent( iBin, value_to_fill);
  }
  */
}

double TemplateCreator::CorrectSampleValue(double old_value) {

  double new_value = old_value / fTemplateFitter->GetAmplitudeScaleFactor();
  new_value -= fTemplateFitter->GetPedestalOffset();

  return new_value;
}

// The following macro registers this module to be useable in the config file.
// The first argument is compulsory and gives the name of this module
// All subsequent arguments will be used as names for arguments given directly 
// within the modules file.
//ALCAP_REGISTER_MODULE(TemplateCreator ,slow_gen,fast_gen);
ALCAP_REGISTER_MODULE(TemplateCreator);
