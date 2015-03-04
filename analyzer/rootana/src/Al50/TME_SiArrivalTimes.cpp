#include "TME_SiArrivalTimes.h"
#include "RegisterModule.inc"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "definitions.h"

#include "TMuonEvent.h"

#include <iostream>
using std::cout;
using std::endl;
#include <cmath>

extern MuonEventList gMuonEvents;

TME_SiArrivalTimes::TME_SiArrivalTimes(modules::options* opts):
   BaseModule("TME_SiArrivalTimes",opts){

}

TME_SiArrivalTimes::~TME_SiArrivalTimes(){
}

// Called before the main event loop
// Can be used to set things up, like histograms etc
// Return non-zero to indicate a problem
int TME_SiArrivalTimes::BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup){
  // Print extra info if we're debugging this module:
  if(Debug()){
     cout<<"-----TME_SiArrivalTimes::BeforeFirstEntry(): I'm debugging!"<<endl;
  }

  using namespace IDs;
  fSiDetectors.push_back(IDs::channel (kSiL1_1 , kNotApplicable ));
  fSiDetectors.push_back(IDs::channel (kSiL1_2 , kNotApplicable ));
  fSiDetectors.push_back(IDs::channel (kSiL1_3 , kNotApplicable ));
  fSiDetectors.push_back(IDs::channel (kSiL1_4 , kNotApplicable ));
  fSiDetectors.push_back(IDs::channel (kSiL2   , kNotApplicable ));
  fSiDetectors.push_back(IDs::channel (kSiR1_1 , kNotApplicable ));
  fSiDetectors.push_back(IDs::channel (kSiR1_2 , kNotApplicable ));
  fSiDetectors.push_back(IDs::channel (kSiR1_3 , kNotApplicable ));
  fSiDetectors.push_back(IDs::channel (kSiR1_4 , kNotApplicable ));
  fSiDetectors.push_back(IDs::channel (kSiR2   , kNotApplicable ));

  fArrivalTimesHist = new TH1F("hArrivalTimes", "hArrivalTimes", 500,0,10000);
 
  return 0;
}

// Called once for each event in the main event loop
// Return non-zero to indicate a problem and terminate the event loop
int TME_SiArrivalTimes::ProcessEntry(TGlobalData* gData,const TSetupData *setup){


  for(MuonEventList::iterator i_tme=gMuonEvents.end()-1;
      i_tme!=gMuonEvents.begin()-1; --i_tme){ // loop through backwards because I will be deleting TMEs as I go

    double tme_time= (*i_tme)->GetTime(); // get the time of the central muon

    // Loop through the silicon detectors
    for(DetectorList::const_iterator i_det=fSiDetectors.begin();
	i_det!=fSiDetectors.end(); ++i_det){

      // Get this silicon detector and loop through its sources
      int source_index=(*i_tme)->GetSourceIndex(*i_det);
      while(source_index>-1){
	const IDs::source& si_source=(*i_tme)->GetSource(source_index);
	int n_pulses = (*i_tme)->NumPulses(si_source);

	// Loop through the pulses in this silicon detector
	for(int i_pulse=0; i_pulse<n_pulses; ++i_pulse){
	  const TDetectorPulse* si_tdp=(*i_tme)->GetPulse(si_source,i_pulse);
	  double si_time = si_tdp->GetTime();

	  double arrival_time= std::fabs(si_time - tme_time);
	  fArrivalTimesHist->Fill(arrival_time);
	}

	// Get the next source
	source_index=(*i_tme)->GetSourceIndex(*i_det,source_index+1);
      }
    }

  }
  return 0;
}

// Called just after the main event loop
// Can be used to write things out, dump a summary etc
// Return non-zero to indicate a problem
int TME_SiArrivalTimes::AfterLastEntry(TGlobalData* gData,const TSetupData *setup){

  // Print extra info if we're debugging this module:
  if(Debug()){
     cout<<"-----TME_SiArrivalTimes::AfterLastEntry(): I'm debugging!"<<endl;
  }

  return 0;
}

// The following macro registers this module to be useable in the config file.
// The first argument is compulsory and gives the name of this module
// All subsequent arguments will be used as names for arguments given directly 
// within the modules file.  See the github wiki for more.
ALCAP_REGISTER_MODULE(TME_SiArrivalTimes);
