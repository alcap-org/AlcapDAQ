#include "TemplateCreator.h"
#include "RegisterModule.inc"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"

#include "definitions.h"

#include "utils/TemplateFitter.h"
#include "ExportPulse.h"

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
    TemplateFitter* template_fitter = new TemplateFitter(detname);

    // Get the TPIs
    thePulseIslands = it->second;
    if (thePulseIslands.size() == 0) continue; // no pulses here..

    // Try and get the template (it may have been created in a previous event)
    std::string template_name = "hTemplate_" + detname;
    TH1D* hTemplate = fTemplateArchive->GetTemplate(template_name.c_str());

    // Loop through all the pulses
    for (PulseIslandList::iterator pulseIter = thePulseIslands.begin(); pulseIter != thePulseIslands.end(); ++pulseIter) {

      // First we will see how many candidate pulses there are on the TPI
      pulse_candidate_finder->FindPulseCandidates(*pulseIter);
      int n_pulse_candidates = pulse_candidate_finder->GetNPulseCandidates();

      std::vector<TPulseIsland*> pulse_candidates = pulse_candidate_finder->GetPulseCandidates();

      // only continue if there is one pulse candidate on the TPI
      if (n_pulse_candidates == 1) {

        // Add the first pulse directly to the template (although we may try and choose a random pulse to start with)
	if (hTemplate == NULL) { // for debugging, just print add one pulse to the template
	  AddPulseToTemplate(hTemplate, *pulseIter);
	  continue;
	}

	// all the other pulses will be fitted to the template and then added to it
	// Get some initial estimates for the fitter
	double pedestal_estimate = 0;
	double amplitude_estimate = 0;
	double time_estimate = (*pulseIter)->GetPeakSample() - hTemplate->GetMaximumBin();
	std::cout << "Estimates: pedestal = " << pedestal_estimate << ", amplitude = " << amplitude_estimate << ", time = " << time_estimate << std::endl;
	template_fitter->SetInitialParameterEstimates(pedestal_estimate, amplitude_estimate, time_estimate);
	
	template_fitter->FitPulseToTemplate(hTemplate, *pulseIter);
	ExportPulse::Instance()->AddToExportList(detname, pulseIter - thePulseIslands.begin());
	if (Debug()) {
	  std::cout << detname << "(" << bankname << "): Pulse #" << pulseIter - thePulseIslands.begin() << ": "
	            << "Fitted Parameters: PedOffset = " << template_fitter->GetPedestalOffset() << ", AmpScaleFactor = " << template_fitter->GetAmplitudeScaleFactor()
	            << ", TimeOffset = " << template_fitter->GetTimeOffset() << ", Chi2 = " << template_fitter->GetChi2() << std::endl << std::endl;
	}

	// Create the corrected pulse
	const std::vector<int>& theSamples = (*pulseIter)->GetSamples();
	std::stringstream histname;
	histname << template_name << "_Pulse" << pulseIter - thePulseIslands.begin();
	TH1D* hUncorrectedPulse = new TH1D(histname.str().c_str(), histname.str().c_str(), theSamples.size(), 0, theSamples.size());
	histname << "_Corrected";
	TH1D* hCorrectedPulse = new TH1D(histname.str().c_str(), histname.str().c_str(), theSamples.size(), 0, theSamples.size());

	for (std::vector<int>::const_iterator sampleIter = theSamples.begin(); sampleIter != theSamples.end(); ++sampleIter) {

	  double uncorrected_value = (*sampleIter);
	  std::cout << "Uncorrected value = " << uncorrected_value << std::endl;
	  double corrected_value = uncorrected_value * template_fitter->GetAmplitudeScaleFactor();
	  std::cout << " x " << template_fitter->GetAmplitudeScaleFactor() << " = " << corrected_value << std::endl;
	  corrected_value += template_fitter->GetPedestalOffset();
	  std::cout << " + " << template_fitter->GetPedestalOffset() << " = " << corrected_value << std::endl;

	  hUncorrectedPulse->Fill(sampleIter - theSamples.begin(), uncorrected_value);
	  hCorrectedPulse->Fill(sampleIter - theSamples.begin() + template_fitter->GetTimeOffset(), corrected_value);
	  //	  std::cout << "Uncorrected Value = " << (*sampleIter) << ", Corrected Value = " << (*sampleIter)*template_fitter->GetAmplitudeScaleFactor() + template_fitter->GetPedestalOffset() << std::endl;
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

  // Wnat to increase the bin resolution (this may become a module option at some point)
  int refine_factor = 5;

  // If the template histogram is NULL, then create it
  if (hTemplate == NULL) {

    // Names for the histograms
    std::string bankname = pulse->GetBankName();
    std::string detname = TSetupData::Instance()->GetDetectorName(bankname);
    std::string histname = "hTemplate_" + detname;
    std::string histtitle = "Template Histogram for the " + detname + " channel";

    hTemplate = new TH1D(histname.c_str(), histtitle.c_str(), n_samples, 0, n_samples);
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

  for (std::vector<int>::const_iterator sampleIter = theSamples.begin(); sampleIter != theSamples.end(); ++sampleIter) {
    hTemplate->Fill( sampleIter - theSamples.begin(), *sampleIter);
  }
}

// The following macro registers this module to be useable in the config file.
// The first argument is compulsory and gives the name of this module
// All subsequent arguments will be used as names for arguments given directly 
// within the modules file.
//ALCAP_REGISTER_MODULE(TemplateCreator ,slow_gen,fast_gen);
ALCAP_REGISTER_MODULE(TemplateCreator);
