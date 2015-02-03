#include "TME_RemoveScatteredMuons.h"
#include "RegisterModule.inc"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "definitions.h"

#include "TMuonEvent.h"

#include <iostream>
using std::cout;
using std::endl;

extern MuonEventList gMuonEvents;

TME_RemoveScatteredMuons::TME_RemoveScatteredMuons(modules::options* opts):
   BaseModule("TME_RemoveScatteredMuons",opts){

  // Do something with opts here.  Has the user specified any
  // particular configuration that you want to know?

  fTimeCut = opts->GetDouble("time_cut", 0); // by default not used
}

TME_RemoveScatteredMuons::~TME_RemoveScatteredMuons(){
}

// Called before the main event loop
// Can be used to set things up, like histograms etc
// Return non-zero to indicate a problem
int TME_RemoveScatteredMuons::BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup){
  // Print extra info if we're debugging this module:
  if(Debug()){
     cout<<"-----TME_RemoveScatteredMuons::BeforeFirstEntry(): I'm debugging!"<<endl;
     cout<<"-----TME_RemoveScatteredMuons::BeforeFirstEntry(): time_cut = " << fTimeCut << std::endl;
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


  return 0;
}

// Called once for each event in the main event loop
// Return non-zero to indicate a problem and terminate the event loop
int TME_RemoveScatteredMuons::ProcessEntry(TGlobalData* gData,const TSetupData *setup){

  for(MuonEventList::const_iterator i_tme=gMuonEvents.begin();
      i_tme!=gMuonEvents.end(); ++i_tme){

    double tme_time= (*i_tme)->GetTime(); // this is the same as the muSc time

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

	  std::cout << si_time << " - " << tme_time << " = " << si_time - tme_time << std::endl;
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
int TME_RemoveScatteredMuons::AfterLastEntry(TGlobalData* gData,const TSetupData *setup){

  // Print extra info if we're debugging this module:
  if(Debug()){
     cout<<"-----TME_RemoveScatteredMuons::AfterLastEntry(): I'm debugging!"<<endl;
  }

  return 0;
}

// The following macro registers this module to be useable in the config file.
// The first argument is compulsory and gives the name of this module
// All subsequent arguments will be used as names for arguments given directly 
// within the modules file.  See the github wiki for more.
ALCAP_REGISTER_MODULE(TME_RemoveScatteredMuons,time_cut);
