#include "TemplateCreator.h"
#include "RegisterModule.inc"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"

#include "definitions.h"

#include "PulseCandidateFinder.h"
#include "ExportPulse.h"

#include <iostream>
using std::cout;
using std::endl;

TemplateCreator::TemplateCreator(modules::options* opts):
   FillHistBase("TemplateCreator",opts){

  // Do something with opts here.  Has the user specified any
  // particular configuration that you want to know?
  // For example, perhaps this module wants an axis range:
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

  // Loop over each detector
  for(it = gData->fPulseIslandToChannelMap.begin(); it != gData->fPulseIslandToChannelMap.end(); ++it){
    
    // Get the bank and detector names for this detector
    bankname = it->first;
    detname = setup->GetDetectorName(bankname);

    // Get the TPIs
    thePulseIslands = it->second;
    if (thePulseIslands.size() == 0) continue; // no pulses here..

    // Loop through all the pulses
    for (PulseIslandList::iterator pulseIter = thePulseIslands.begin(); pulseIter != thePulseIslands.end(); ++pulseIter) {

      // First we will see how many candidate pulses there are on the TPI
      PulseCandidateFinder* pulse_candidate_finder = new PulseCandidateFinder(*pulseIter, 300); // TODO: what values do we need for these parameters?
      int n_pulse_candidates = pulse_candidate_finder->GetNPulseCandidates();

      std::vector<TPulseIsland*> pulse_candidates = pulse_candidate_finder->GetPulseCandidates();

      if (n_pulse_candidates == 0) {
	ExportPulse::Instance()->AddToExportList(detname, pulseIter - thePulseIslands.begin());
      }

      if (Debug() && n_pulse_candidates > 0) {
	std::cout << "TemplateCreator::ProcessEntry: There are " << n_pulse_candidates << " pulse candidates on this TPI" << std::endl;
	for (std::vector<TPulseIsland*>::const_iterator candidateIter = pulse_candidates.begin(); candidateIter != pulse_candidates.end(); ++candidateIter) {
	  std::cout << "Candidate #" << candidateIter - pulse_candidates.begin() << ": timestamp = " << (*candidateIter)->GetTimeStamp() << std::endl;
	}
      }
      // if there is more than one, we skip to the next TPI

      // if there is only one, we continue
        // we will add the first pulse directly to the template (although we may try and choose a random pulse to start with)
        // all the other pulses will be fitted to the template and then added to it
        // we keep on adding pulses until adding pulses has no effect on the template
        // then we save the template to the file
    }
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

// The following macro registers this module to be useable in the config file.
// The first argument is compulsory and gives the name of this module
// All subsequent arguments will be used as names for arguments given directly 
// within the modules file.
//ALCAP_REGISTER_MODULE(TemplateCreator ,slow_gen,fast_gen);
ALCAP_REGISTER_MODULE(TemplateCreator);
