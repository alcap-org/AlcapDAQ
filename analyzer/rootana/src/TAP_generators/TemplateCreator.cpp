#include "TemplateCreator.h"
#include "RegisterModule.inc"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"

#include "definitions.h"

#include "utils/TemplateFitter.h"
#include "ExportPulse.h"

#include <iostream>
using std::cout;
using std::endl;

TemplateCreator::TemplateCreator(modules::options* opts):
   BaseModule("TemplateCreator",opts){

  // Do something with opts here.  Has the user specified any
  // particular configuration that you want to know?
  // For example, perhaps this module wants an axis range:

  fPulseCandidateFinder = new PulseCandidateFinder(opts);
}

TemplateCreator::~TemplateCreator(){
}

// Called before the main event loop
// Can be used to set things up, like histograms etc
// Return non-zero to indicate a problem
int TemplateCreator::BeforeFirstEntry(TGlobalData* gData,TSetupData *setup){
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
int TemplateCreator::ProcessEntry(TGlobalData* gData,TSetupData *setup){

  // Prepare a few variables
  std::string bankname, detname;
  PulseIslandList thePulseIslands;
  StringPulseIslandMap::const_iterator it;

  // Create the TemplateFitter that we will use
  TemplateFitter* template_fitter = new TemplateFitter();

  // Loop over each detector
  for(it = gData->fPulseIslandToChannelMap.begin(); it != gData->fPulseIslandToChannelMap.end(); ++it){
    
    // Get the bank and detector names for this detector
    bankname = it->first;
    detname = setup->GetDetectorName(bankname);

    // Get the TPIs
    thePulseIslands = it->second;
    if (thePulseIslands.size() == 0) continue; // no pulses here..

    // Try and get the template (it may have been created in a previous event)
    std::string template_name = "hTemplate_" + detname;
    TH1D* hTemplate = fTemplateArchive->GetTemplate(template_name.c_str());

    // Loop through all the pulses
    for (PulseIslandList::iterator pulseIter = thePulseIslands.begin(); pulseIter != thePulseIslands.end(); ++pulseIter) {

      // First we will see how many candidate pulses there are on the TPI
      fPulseCandidateFinder->FindPulseCandidates(*pulseIter);
      int n_pulse_candidates = fPulseCandidateFinder->GetNPulseCandidates();

      std::vector<TPulseIsland*> pulse_candidates = fPulseCandidateFinder->GetPulseCandidates();

      // we only continue if there is more than one pulse candidate on the TPI
      /*      if (n_pulse_candidates == 1) {

        // Add the first pulse directly to the template (although we may try and choose a random pulse to start with)
	if (Debug() && hTemplate == NULL) { // for debugging, just print add one pulse to the template
	  AddPulseToTemplate(hTemplate, *pulseIter);
	}

	// all the other pulses will be fitted to the template and then added to it
	template_fitter->FitPulseToTemplate(hTemplate, *pulseIter);
	if (Debug()) {
	  std::cout << detname << "(" << bankname << "): Pulse #" << pulseIter - thePulseIslands.begin() << ": "
	            << "Fitted Parameters: Ped = " << template_fitter->GetPedestal() << ", Amp = " << template_fitter->GetAmplitude()
	            << ", Time = " << template_fitter->GetTime() << ", Chi2 = " << template_fitter->GetChi2() << std::endl;
	}
	// we keep on adding pulses until adding pulses has no effect on the template
	}*/
    }
    // Save the template to the file
    fTemplateArchive->SaveTemplate(hTemplate);
  }

  return 0;
}

// Called just after the main event loop
// Can be used to write things out, dump a summary etc
// Return non-zero to indicate a problem
int TemplateCreator::AfterLastEntry(TGlobalData* gData,TSetupData *setup){

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
  for (std::vector<int>::const_iterator sampleIter = theSamples.begin(); sampleIter != theSamples.end(); ++sampleIter) {
    hTemplate->Fill(sampleIter - theSamples.begin(), *sampleIter);
  }
}

// The following macro registers this module to be useable in the config file.
// The first argument is compulsory and gives the name of this module
// All subsequent arguments will be used as names for arguments given directly 
// within the modules file.
//ALCAP_REGISTER_MODULE(TemplateCreator ,slow_gen,fast_gen);
ALCAP_REGISTER_MODULE(TemplateCreator);
