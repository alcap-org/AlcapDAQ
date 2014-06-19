#include "PulseCandidateFinder_InvestigateParameters.h"
#include "RegisterModule.inc"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "definitions.h"

#include <iostream>
#include <cmath>
#include <sstream>
using std::cout;
using std::endl;

#include "utils/PulseCandidateFinder.h"
#include "ExportPulse.h"

PulseCandidateFinder_InvestigateParameters::PulseCandidateFinder_InvestigateParameters(modules::options* opts):
   BaseModule("PulseCandidateFinder_InvestigateParameters",opts){
  
}

PulseCandidateFinder_InvestigateParameters::~PulseCandidateFinder_InvestigateParameters(){
}

// Called before the main event loop
// Can be used to set things up, like histograms etc
// Return non-zero to indicate a problem
int PulseCandidateFinder_InvestigateParameters::BeforeFirstEntry(TGlobalData* gData,TSetupData *setup){
  // Print extra info if we're debugging this module:
  if(Debug()){
     cout<<"-----PulseCandidateFinder_InvestigateParameters::BeforeFirstEntry(): I'm debugging!"<<endl;
  }

  return 0;
}

// Called once for each event in the main event loop
// Return non-zero to indicate a problem and terminate the event loop
int PulseCandidateFinder_InvestigateParameters::ProcessEntry(TGlobalData* gData,TSetupData *setup){

  // Prepare a few variables
  std::string bankname, detname;
  PulseIslandList thePulseIslands;
  StringPulseIslandMap::const_iterator it;

  // Create the pulse candidate finder here, since we can use the same instance for all channels
  PulseCandidateFinder* pulse_candidate_finder = new PulseCandidateFinder();

  // Loop over each detector
  for(it = gData->fPulseIslandToChannelMap.begin(); it != gData->fPulseIslandToChannelMap.end(); ++it){
    
    // Get the bank and detector names for this detector
    bankname = it->first;
    detname = setup->GetDetectorName(bankname);

    // Get the TPIs
    thePulseIslands = it->second;
    if (thePulseIslands.size() == 0) continue; // no pulses here..

    // Create the histogram that will store all the parameter values that we will look at to determine the best values
    if (fParameterHistograms.find(detname) == fParameterHistograms.end()) {
      int max_value = 10000;
      std::string histname = "fParameterHistogram_" + detname;
      TH1D* histogram = new TH1D(histname.c_str(), histname.c_str(), max_value, 0, max_value);
      fParameterHistograms[detname] = histogram;
    }

    // Create the histogram to store the parameters in
    TH1D* parameter_histogram = fParameterHistograms[detname];

    // Loop through all the pulses
    for (PulseIslandList::iterator pulseIter = thePulseIslands.begin(); pulseIter != thePulseIslands.end(); ++pulseIter) {

      pulse_candidate_finder->FindPulseCandidates(*pulseIter);
      pulse_candidate_finder->FillParameterHistogram(parameter_histogram);

      // Use to export pulses (NB you will only want to run rootana on a few events to stop the output being large)
      int n_pulse_candidates = pulse_candidate_finder->GetNPulseCandidates();
      if (Debug()) {
	if (n_pulse_candidates > 0) {
	  ExportPulse::Instance()->AddToExportList(detname, pulseIter - thePulseIslands.begin());
	  if (n_pulse_candidates > 1) {
	    std::cout << detname << "(" << bankname << "): Pulse #" << pulseIter - thePulseIslands.begin() << " has " << n_pulse_candidates << " pulse candidates\n"; 
	  }
	}
      }

    }
  }

  return 0;
}

// Called just after the main event loop
// Can be used to write things out, dump a summary etc
// Return non-zero to indicate a problem
int PulseCandidateFinder_InvestigateParameters::AfterLastEntry(TGlobalData* gData,TSetupData *setup){

  // Print extra info if we're debugging this module:
  if(Debug()){
     cout<<"-----PulseCandidateFinder_InvestigateParameters::AfterLastEntry(): I'm debugging!"<<endl;
  }

  return 0;
}

// The following macro registers this module to be useable in the config file.
// The first argument is compulsory and gives the name of this module
// All subsequent arguments will be used as names for arguments given directly 
// within the modules file.  See the github wiki for more.
ALCAP_REGISTER_MODULE(PulseCandidateFinder_InvestigateParameters);
