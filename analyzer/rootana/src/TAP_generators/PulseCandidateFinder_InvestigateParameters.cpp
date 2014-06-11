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

#include "PulseCandidateFinder.h"

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

  // Loop over each detector
  for(it = gData->fPulseIslandToChannelMap.begin(); it != gData->fPulseIslandToChannelMap.end(); ++it){
    
    // Get the bank and detector names for this detector
    bankname = it->first;
    detname = setup->GetDetectorName(bankname);

    // Get the TPIs
    thePulseIslands = it->second;
    if (thePulseIslands.size() == 0) continue; // no pulses here..

    // Create the histogram that will store all the sample differences that we will look at to determine the best values for
    // the "rise" and "fall" parameters (if it doesn't exist)
    if (fSampleDifferenceHistograms.find(detname) == fSampleDifferenceHistograms.end()) {
      int max_value = 10000;
      std::string histname = "fSampleDifferenceHistograms_" + detname;
      std::stringstream histtitle;
      histtitle << "Plot of the Sample Differences for " << detname << " for Run " << 2808;
      TH1D* histogram = new TH1D(histname.c_str(), histtitle.str().c_str(), max_value, 0, max_value);
      histogram->GetYaxis()->SetTitle("");
      histogram->GetXaxis()->SetTitle("Difference Between Consecutive Samples [ADC]"); 
      fSampleDifferenceHistograms[detname] = histogram;
    }

    // Loop through all the pulses
    TH1D* difference_histogram = fSampleDifferenceHistograms[detname];
    for (PulseIslandList::iterator pulseIter = thePulseIslands.begin(); pulseIter != thePulseIslands.end(); ++pulseIter) {

      // Create the pulse candidate finder and histogram to store the sample differences in
      PulseCandidateFinder* pulse_candidate_finder = new PulseCandidateFinder(*pulseIter);

      //      pulse_candidate_finder->FillSampleDifferencesHistogram(difference_histogram);
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
