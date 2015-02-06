#include "TME_SiArmTDiff.h"
#include "RegisterModule.inc"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "definitions.h"
#include "TMuonEvent.h"
#include "debug_tools.h"

#include <TH1F.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <TApplication.h>

#include <iostream>
using std::cout;
using std::endl;

extern MuonEventList gMuonEvents;

TME_SiArmTDiff::TME_SiArmTDiff(modules::options* opts):
   BaseModule("TME_SiArmTDiff",opts),fNullCount(0),fTdpCount(0){
}

TME_SiArmTDiff::~TME_SiArmTDiff(){
}

int TME_SiArmTDiff::BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup){
    using namespace IDs;
    fSiL1.push_back(IDs::channel (kSiL1_1 , kNotApplicable ));
    fSiL1.push_back(IDs::channel (kSiL1_2 , kNotApplicable ));
    fSiL1.push_back(IDs::channel (kSiL1_3 , kNotApplicable ));
    fSiL1.push_back(IDs::channel (kSiL1_4 , kNotApplicable ));
    fSiL2 = new IDs::channel (kSiL2   , kNotApplicable );
    fSiR1.push_back(IDs::channel (kSiR1_1 , kNotApplicable ));
    fSiR1.push_back(IDs::channel (kSiR1_2 , kNotApplicable ));
    fSiR1.push_back(IDs::channel (kSiR1_3 , kNotApplicable ));
    fSiR1.push_back(IDs::channel (kSiR1_4 , kNotApplicable ));
    fSiR2 = new IDs::channel (kSiR2   , kNotApplicable );
    //    fSiL1.push_back(IDs::channel (kMuSc   , kNotApplicable ));
    fSiLTDiffs = new TH1F("hSiLTDiffs", "TDiffs Between thick and thin SiL", 5000, -5e3,5e3);
    fSiLTDiffs->SetXTitle("t_thick - t_thin (ns)");

    fSiRTDiffs = new TH1F("hSiRTDiffs", "TDiffs Between thick and thin SiR", 5000, -5e3,5e3);
    fSiRTDiffs->SetXTitle("t_thick - t_thin (ns)");

  return 0;
}

int TME_SiArmTDiff::ProcessEntry(TGlobalData* gData,const TSetupData *setup){

    for(MuonEventList::const_iterator i_tme=gMuonEvents.begin();
            i_tme!=gMuonEvents.end(); ++i_tme){

      // First, check for pile-up in the muSc
      if ( (*i_tme)->HasMuonPileup()) {
	continue;
      }

      double tme_time= (*i_tme)->GetTime(); // this is the same as the muSc time

      // Now loop through the SiL1
      for(DetectorList::const_iterator i_det=fSiL1.begin();
                i_det!=fSiL1.end(); ++i_det){
	// pulses per channel
	int SiL1_source_index=(*i_tme)->GetSourceIndex(*i_det);
	while(SiL1_source_index>-1){
	  const IDs::source& SiL1_source=(*i_tme)->GetSource(SiL1_source_index);
	  
	  int n_SiL1 = (*i_tme)->NumPulses(SiL1_source);
	  //	      std::cout << SiL1_source << " has " << n_SiL1 << " pulses" << std::endl;
	  for(int i=0; i<n_SiL1; ++i){
	    const TDetectorPulse* tdp=(*i_tme)->GetPulse(SiL1_source,i);
	    double thin_time = tdp->GetTime();

	    // Loop trhough the SiL2 pulses
	    int SiL2_source_index=(*i_tme)->GetSourceIndex(*fSiL2);
	    while (SiL2_source_index>-1) {
	      const IDs::source& SiL2_source=(*i_tme)->GetSource(SiL2_source_index);
	      int n_SiL2 = (*i_tme)->NumPulses(SiL2_source);
	      for (int j=0; j<n_SiL2; ++j) {
		const TDetectorPulse* tdp_SiL2=(*i_tme)->GetPulse(SiL2_source,j);
		double thick_time = tdp_SiL2->GetTime();
		fSiLTDiffs->Fill(thick_time - thin_time);
	      }
	      SiL2_source_index=(*i_tme)->GetSourceIndex(*fSiL2,SiL2_source_index+1);
	    }
	  }
	  SiL1_source_index=(*i_tme)->GetSourceIndex(*i_det,SiL1_source_index+1);
	}
      }

      // Now loop through the SiR1
      for(DetectorList::const_iterator i_det=fSiR1.begin();
                i_det!=fSiR1.end(); ++i_det){
	// pulses per channel
	int SiR1_source_index=(*i_tme)->GetSourceIndex(*i_det);
	while(SiR1_source_index>-1){
	  const IDs::source& SiR1_source=(*i_tme)->GetSource(SiR1_source_index);
	  
	  int n_SiR1 = (*i_tme)->NumPulses(SiR1_source);
	  //	      std::cout << SiR1_source << " has " << n_SiR1 << " pulses" << std::endl;
	  for(int i=0; i<n_SiR1; ++i){
	    const TDetectorPulse* tdp=(*i_tme)->GetPulse(SiR1_source,i);
	    double thin_time = tdp->GetTime();

	    // Loop trhough the SiR2 pulses
	    int SiR2_source_index=(*i_tme)->GetSourceIndex(*fSiR2);
	    while (SiR2_source_index>-1) {
	      const IDs::source& SiR2_source=(*i_tme)->GetSource(SiR2_source_index);
	      int n_SiR2 = (*i_tme)->NumPulses(SiR2_source);
	      for (int j=0; j<n_SiR2; ++j) {
		const TDetectorPulse* tdp_SiR2=(*i_tme)->GetPulse(SiR2_source,j);
		double thick_time = tdp_SiR2->GetTime();
		fSiRTDiffs->Fill(thick_time - thin_time);
	      }
	      SiR2_source_index=(*i_tme)->GetSourceIndex(*fSiR2,SiR2_source_index+1);
	    }
	  }
	  SiR1_source_index=(*i_tme)->GetSourceIndex(*i_det,SiR1_source_index+1);
	}
      }
    }

  return 0;
}

int TME_SiArmTDiff::AfterLastEntry(TGlobalData* gData,const TSetupData *setup){

  return 0;
}

ALCAP_REGISTER_MODULE(TME_SiArmTDiff);
