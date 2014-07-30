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
#include <stdexcept>
using std::cout;
using std::endl;

extern Long64_t* gEntryNumber;

TemplateCreator::TemplateCreator(modules::options* opts):
  BaseModule("TemplateCreator",opts), fOpts(opts){

  fRefineFactor = opts->GetInt("refine_factor", 5);
  fPulseDebug = opts->GetBool("pulse_debug", false);
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

  // Set all the converged statuses to false
  StringPulseIslandMap::const_iterator it;
  for(it = gData->fPulseIslandToChannelMap.begin(); it != gData->fPulseIslandToChannelMap.end(); ++it){
    std::string bankname = it->first;
    std::string detname = TSetupData::Instance()->GetDetectorName(bankname);

    fConvergedStatuses[detname] = false;
  }
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

    // See if we already have a converged template for this detector
    if (fConvergedStatuses[detname] == true) {
      continue;
    }

    // Create the pulse candidate finder for this detector
    PulseCandidateFinder* pulse_candidate_finder = new PulseCandidateFinder(detname, fOpts);

    // Create the TemplateFitter that we will use for this channel
    fTemplateFitter = new TemplateFitter(detname, fRefineFactor);

    // Get the TPIs
    thePulseIslands = it->second;
    if (thePulseIslands.size() == 0) continue; // no pulses here..

    // Try and get the template (it may have been created in a previous event)
    std::string template_name = "hTemplate_" + detname;
    TH1D* hTemplate = NULL;
    if (fTemplates.find(detname) != fTemplates.end()) {
      hTemplate = fTemplates[detname];
    }

    // Store a couple of numbers to get an idea of how many successful fits there are
    int& n_fit_attempts = fNFitAttempts[detname]; // number of pulses we try to fit to
    int& n_successful_fits = fNSuccessfulFits[detname];
    int& n_pulses_in_template = fNPulsesInTemplate[detname];

    // Loop through all the pulses
    for (PulseIslandList::iterator pulseIter = thePulseIslands.begin(); pulseIter != thePulseIslands.end(); ++pulseIter) {

      // First we will see how many candidate pulses there are on the TPI
      pulse_candidate_finder->FindPulseCandidates(*pulseIter);
      int n_pulse_candidates = pulse_candidate_finder->GetNPulseCandidates();

      // only continue if there is one pulse candidate on the TPI
      if (n_pulse_candidates == 1) {

	TPulseIsland* pulse = *pulseIter;

        // Add the first pulse directly to the template (although we may try and choose a random pulse to start with)
	if (hTemplate == NULL) {
	  std::string histname = "hTemplate_" + detname;
	  std::string histtitle = "Template Histogram for the " + detname + " channel";

	  int pulse_length = pulse->GetSamples().size();
	  if (pulse->GetPeakSample() >= pulse_length - pulse_length/5.0) {
	    if (Debug()) {
	      std::cout << "TemplateCreator: Pulse #" << pulseIter - thePulseIslands.begin() << " is too close to one end of the island and so won't be used as the first pulse in the template." << std::endl;
	    }
	    continue;
	  }
	  hTemplate = CreateRefinedPulseHistogram(pulse, histname.c_str(), histtitle.c_str(), true);
	  ++n_pulses_in_template;

	  if (Debug()) {
	    std::cout << "TemplateCreator: Adding " << detname << " Pulse #" << pulseIter - thePulseIslands.begin() << " directly to the template" << std::endl;
	  }
	  fTemplates[detname] = hTemplate;
	  continue;
	}

	// Get the samples so we can check for digitiser overflow
	const std::vector<int>& theSamples = (pulse)->GetSamples();
	int n_samples = theSamples.size();

	// Calculate the maximum ADC value for this digitiser
	int n_bits = TSetupData::Instance()->GetNBits(bankname);
	double max_adc_value = std::pow(2, n_bits);

	// Loop through the samples and check for digitizer overflow
	bool overflowed = false;
	for (int i = 0; i < n_samples; ++i) {
	  int sample_value = theSamples.at(i);
	  if (sample_value >= max_adc_value-1 && sample_value <= max_adc_value+1) {
	    if (Debug()) {
	      std::cout << "TemplateCreator: Pulse #" << pulseIter - thePulseIslands.begin() << " has overflowed the digitizer and won't be added to the template" << std::endl;
	    }
	    overflowed = true;
	    break;
	  }
	  else if (sample_value == 0) {
	    if (Debug()) {
	      std::cout << "TemplateCreator: Pulse #" << pulseIter - thePulseIslands.begin() << " has underflowed the digitizer and won't be added to the template" << std::endl;
	    }
	    overflowed = true;
	    break;
	  }
	}
	if (overflowed) {
	  continue; // skip this pulse
	}

	// Create the refined pulse waveform
	TH1D* hPulseToFit = CreateRefinedPulseHistogram(pulse, "hPulseToFit", "hPulseToFit", false);

	// Create some histograms that monitor the progression of the template
	if (fErrorVsPulseAddedHistograms.find(detname) == fErrorVsPulseAddedHistograms.end()) {
	  std::string error_histname = "hErrorVsPulseAdded_" + detname;
	  std::string error_histtitle = "Plot of the Error as each new Pulse is added to the template for the " + detname + " channel";
	  int n_bins = 10000;
	  TH1D* error_hist = new TH1D(error_histname.c_str(), error_histtitle.c_str(), n_bins,0,n_bins);
	  fErrorVsPulseAddedHistograms[detname] = error_hist;
	}

	// all the other pulses will be fitted to the template and then added to it
	// Get some initial estimates for the fitter
	double template_pedestal = hTemplate->GetBinContent(1);
	double template_amplitude;
	double template_time;

	double pulse_pedestal = hPulseToFit->GetBinContent(1);
	double pulse_amplitude;
	double pulse_time;

	double pedestal_offset_estimate = pulse_pedestal; // now we're dealing with actual pulses since we subtract the template_pedestal in the transformation
	double amplitude_scale_factor_estimate;
	double time_offset_estimate;
	if (TSetupData::Instance()->GetTriggerPolarity(bankname) == 1) { 
	  template_amplitude = (hTemplate->GetMaximum() - template_pedestal);
	  template_time = hTemplate->GetMaximumBin() - 1; // go from bin numbering (1, n_samples) to clock ticks (0, n_samples-1)

	  pulse_amplitude = (hPulseToFit->GetMaximum() - pulse_pedestal);
	  pulse_time = hPulseToFit->GetMaximumBin() - 1;

	  amplitude_scale_factor_estimate = pulse_amplitude / template_amplitude;  // estimated scale factor
	  time_offset_estimate = pulse_time - template_time;
	}
	else if (TSetupData::Instance()->GetTriggerPolarity(bankname) == -1) {
	  template_amplitude = (template_pedestal - hTemplate->GetMinimum());
	  template_time = hTemplate->GetMinimumBin() - 1; // go from bin numbering (1, n_samples) to clock ticks (0, n_samples-1)

	  pulse_amplitude = (pulse_pedestal - hPulseToFit->GetMinimum());
	  pulse_time = hPulseToFit->GetMinimumBin() - 1; // go from bin numbering (1, n_samples) to clock ticks (0, n_samples-1)

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

	int fit_status = fTemplateFitter->FitPulseToTemplate(hTemplate, hPulseToFit, bankname);
	++n_fit_attempts;
	if (fit_status != 0) {
	  if (Debug()) {
	    std::cout << "TemplateCreator: Problem with fit (status = " << fit_status << ")" << std::endl;
	  }
	  delete hPulseToFit; // delete this here since it is no longer needed
	  continue;
	}
	++n_successful_fits;

	if (Debug()) {
	  std::cout << "Template Creator: Fitted Parameters: PedOffset = " << fTemplateFitter->GetPedestalOffset() << ", AmpScaleFactor = " << fTemplateFitter->GetAmplitudeScaleFactor()
	            << ", TimeOffset = " << fTemplateFitter->GetTimeOffset() << ", Chi2 = " << fTemplateFitter->GetChi2() << ", NDoF = " << fTemplateFitter->GetNDoF() 
		    << ", Prob = " << TMath::Prob(fTemplateFitter->GetChi2(), fTemplateFitter->GetNDoF()) << std::endl << std::endl;
	}

	if (fPulseDebug) {
	  // Print out some templates as we go along
	  if (n_pulses_in_template <= 10 || 
	      (n_pulses_in_template <= 100 && n_pulses_in_template%10 == 0) ||
	      (n_pulses_in_template%100 == 0) ) {
	    std::stringstream newhistname;
	    newhistname << "hTemplate_" << n_pulses_in_template << "Pulses_" << detname;
	    TH1D* new_template = (TH1D*) hTemplate->Clone(newhistname.str().c_str());
	  }
	}

	// Add the pulse to the template (we'll do correct the sample values there)
	AddPulseToTemplate(hTemplate, hPulseToFit, bankname);
	++n_pulses_in_template;
	

	if (fPulseDebug) {
	  // Print out the uncorrected and corrected pulse that has been added to the template

	  // Create the histograms that we will use to plot the corrected and uncorrected pulses
	  std::stringstream histname;
	  histname << template_name << "_Event" << *gEntryNumber << "_Pulse" << pulseIter - thePulseIslands.begin() << "_" << n_pulses_in_template << "Added";
	  TH1D* hUncorrectedPulse = (TH1D*) hPulseToFit->Clone(histname.str().c_str());
	  histname << "_Corrected";
	  TH1D* hCorrectedPulse = (TH1D*) hPulseToFit->Clone(histname.str().c_str());
	  hCorrectedPulse->SetEntries(0); // set entries back to 0
	  
	  double pedestal_error = SetupNavigator::Instance()->GetPedestalError(bankname);

	  // Loop through the bins of the uncorrected pulse and set the values in the corrected pulse histogram
	  for (int iPulseBin = 1; iPulseBin <= hPulseToFit->GetNbinsX(); ++iPulseBin) {
	    
	    double uncorrected_value = hPulseToFit->GetBinContent(iPulseBin);
	    double corrected_value = CorrectSampleValue(uncorrected_value, template_pedestal);
	    
	    hCorrectedPulse->SetBinContent(iPulseBin +0.5 - fTemplateFitter->GetTimeOffset(), corrected_value); 
	    hCorrectedPulse->SetBinError(iPulseBin +0.5 - fTemplateFitter->GetTimeOffset(), pedestal_error);
	  }
	}

	delete hPulseToFit;

	// we keep on adding pulses until adding pulses has no effect on the template
	bool converged = CheckConvergence(hTemplate, bankname);
	if (converged) {
	  fConvergedStatuses[detname] = true;
	  std::cout << "TemplateCreator: " << detname << " template terminated at iteration " << n_pulses_in_template << std::endl;
	  break; // break from the for loop
	}
      } // end if only one pulse candidate
    } //end for loop through channels
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

  // Print to stdout the percentage of successful fit for each channel
  StringPulseIslandMap::const_iterator it;
  for(it = gData->fPulseIslandToChannelMap.begin(); it != gData->fPulseIslandToChannelMap.end(); ++it){
    std::string bankname = it->first;
    std::string detname = setup->GetDetectorName(bankname);

    TH1D* hTemplate = fTemplates[detname];
    
    if (!hTemplate) { // if there's no template been created for this channel
      continue;
    }
    
    int& n_fit_attempts = fNFitAttempts[detname]; // number of pulses we try to fit to
    int& n_successful_fits = fNSuccessfulFits[detname];
    std::cout << "TemplateCreator: " << detname << ": " << n_fit_attempts << " fits attempted with " << n_successful_fits << " successful (" << ((double)n_successful_fits/(double)n_fit_attempts)*100 << "%)" << std::endl;

    // Normalise the template so that it has pedestal=0 and amplitude=1
    // Work out the pedestal of the template from the first 5 bins
    int n_bins_for_template_pedestal = 5;
    double total = 0;
    for (int iBin = 1; iBin <= n_bins_for_template_pedestal; ++iBin) {
      total += hTemplate->GetBinContent(iBin);
    }
    double template_pedestal = total / n_bins_for_template_pedestal;
    //    std::cout << detname << ": Template Pedestal = " << template_pedestal << std::endl;
   
    // Subtract off the pedesal
    for (int iBin = 1; iBin <= hTemplate->GetNbinsX(); ++iBin) {
      double old_value = hTemplate->GetBinContent(iBin);
      double new_value = old_value - template_pedestal;

      hTemplate->SetBinContent(iBin, new_value);
    }

    // Integrate over the histogram and scale to give an area of 1
    double integral = std::fabs(hTemplate->Integral()); // want the absolute value for the integral because of the negative polarity pulses
    hTemplate->Scale(1.0/integral);
    integral = std::fabs(hTemplate->Integral());

    // Save the template to the file
    fTemplateArchive->SaveTemplate(hTemplate);
  }

  // Clean up the template archive
  delete fTemplateArchive;

  return 0;
}

/// AddPulseToTemplate()
/// Adds the given pulse to the template
void TemplateCreator::AddPulseToTemplate(TH1D* & hTemplate, TH1D* & hPulse, std::string bankname) {

  std::string detname = TSetupData::Instance()->GetDetectorName(bankname);

  int n_pulses = fNPulsesInTemplate.at(detname);
  if (Debug()) {
    std::cout << "AddPulseToTemplate(): n_pulses = " << n_pulses << std::endl;
  }

  double template_pedestal = hTemplate->GetBinContent(1);

  // Loop through the pulse histogram
  for (int iPulseBin = 1; iPulseBin < hPulse->GetNbinsX(); ++iPulseBin) {

    // First, get the corrected sample value based on the fitted parameters
    double uncorrected_value = hPulse->GetBinContent(iPulseBin);
    double corrected_value = CorrectSampleValue(uncorrected_value, template_pedestal);

    // Get the corrected bin number based on the fitted time offset
    int bin_number = iPulseBin + 0.5 - fTemplateFitter->GetTimeOffset(); // +0.5 so that we round to the nearest integer and subtract time offset because this value might not want to go direct into the template

    // Only change the bin contents of bins within the range of the template histogram
    if (bin_number < 1 || bin_number > hTemplate->GetNbinsX()) {
      continue;
    }

    // Store the old bin content and error for later
    double old_bin_content = hTemplate->GetBinContent(bin_number);
    double old_bin_error = hTemplate->GetBinError(bin_number);

    // Calculate the new bin content...
    double new_bin_content = n_pulses * old_bin_content + corrected_value;
    new_bin_content /= (n_pulses + 1);

    /// ...and new bin error
    double new_bin_error = ((n_pulses - 1)*old_bin_error*old_bin_error) + (corrected_value - old_bin_content)*(corrected_value - new_bin_content);
    new_bin_error = std::sqrt(new_bin_error / n_pulses);

    if (Debug()) {
      std::cout << "TemplateCreator::AddPulseToTemplate(): Bin #" << bin_number << ": Corrected Sample Value = " << corrected_value << std::endl
		<< "\t\t\tOld Value (Error) = " << old_bin_content << "(" << old_bin_error << ")" << std::endl
		<< "\t\t\tNew Value (Error) = " << new_bin_content << "(" << new_bin_error << ")" << std::endl;
    }

    // Set the new bin content and error of the template
    hTemplate->SetBinContent(bin_number, new_bin_content);
    hTemplate->SetBinError(bin_number, new_bin_error);
  }
}

double TemplateCreator::CorrectSampleValue(double old_value, double template_pedestal) {

  double new_value = old_value - fTemplateFitter->GetPedestalOffset();
  new_value /= fTemplateFitter->GetAmplitudeScaleFactor();
  new_value += template_pedestal;

  return new_value;
}

// Creates a histogram with sub-bin resolution
TH1D* TemplateCreator::CreateRefinedPulseHistogram(const TPulseIsland* pulse, std::string histname, std::string histtitle, bool interpolate) {

  // Get a few things first
  std::string bankname = pulse->GetBankName();
  const std::vector<int>& theSamples = pulse->GetSamples();
  int n_samples = theSamples.size();
  int n_bins = fRefineFactor*n_samples; // number of bins in the template

  // Create the higher resolution histogram
  TH1D* hist = new TH1D(histname.c_str(), histtitle.c_str(), n_bins, 0, n_samples);

  double pedestal_error = SetupNavigator::Instance()->GetPedestalError(bankname);

  // Go through the bins in the high-resolution histogram
  // NB sample numbers go grom 0 to n-1 and bins go from 1 to n
  for (int i = 0; i < n_bins; ++i) {
    int bin = i+1; // bins go from 1 to n rather than 0 to n-1
    int sample_number = i / fRefineFactor;
    double remainder = i % fRefineFactor;
    double sample_value;

    // We may want to interpolate between the samples in the samples vector
    if (interpolate) {
      try {
	sample_value = theSamples.at(sample_number) + (remainder / fRefineFactor)*(theSamples.at(sample_number+1) - theSamples.at(sample_number));
      }
      catch (const std::out_of_range& oor) { // if we'll be going out of range of the samples vector
	sample_value = theSamples.at(sample_number);
      }
    }
    else {
      sample_value = theSamples.at(sample_number);
    }

    // Set the bin contents and bin error
    hist->SetBinContent( bin, sample_value);
    hist->SetBinError( bin, pedestal_error);
  }

  return hist;
}


bool TemplateCreator::CheckConvergence(TH1D* hTemplate, std::string bankname) {

  std::string detname = TSetupData::Instance()->GetDetectorName(bankname);
  int n_pulses_in_template = fNPulsesInTemplate[detname];
  int trigger_polarity = TSetupData::Instance()->GetTriggerPolarity(bankname);

  TH1D* error_histogram = fErrorVsPulseAddedHistograms[detname];
  double error = 0;
  if (trigger_polarity == -1) {
    error = hTemplate->GetBinError(hTemplate->GetMinimumBin());
  }
  else if (trigger_polarity == 1) {
    error = hTemplate->GetBinError(hTemplate->GetMaximumBin());
  }
  error_histogram->Fill(n_pulses_in_template, error);

  // Check the difference between this iteration and previous ones and, if it's small, the template has converged
  int n_bins_to_check = 10;
  double convergence_limit = 0.1;
  bool converged = false;
  for (int iPrevBin = 0; iPrevBin < n_bins_to_check; ++iPrevBin) {
    double previous_error = error_histogram->GetBinContent(n_pulses_in_template-iPrevBin); // we have just added error to bin number n_pulses_in_template+1

    if ( std::fabs(previous_error - error) < convergence_limit) {
      converged = true;
    }
    else {
      converged = false;
      break;
    }
  }

  return converged;
  //  std::cout << detname << ": #" << n_pulses_in_template << ", Error (Inserted) = " << error << ", Error (Retrieved) = " << error_histogram->GetBinContent(n_pulses_in_template+1) << std::endl;

}
// The following macro registers this module to be useable in the config file.
// The first argument is compulsory and gives the name of this module
// All subsequent arguments will be used as names for arguments given directly 
// within the modules file.
//ALCAP_REGISTER_MODULE(TemplateCreator ,slow_gen,fast_gen);
ALCAP_REGISTER_MODULE(TemplateCreator);
