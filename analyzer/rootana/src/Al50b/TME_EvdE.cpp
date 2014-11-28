#include "TME_EvdE.h"
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
#include <cmath>
using std::cout;
using std::endl;

extern MuonEventList gMuonEvents;

TME_EvdE::TME_EvdE(modules::options* opts):
   BaseModule("TME_EvdE",opts),fNullCount(0),fTdpCount(0){
}

TME_EvdE::~TME_EvdE(){
}

int TME_EvdE::BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup){
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

    fLeftArm.thin = fSiL1;
    fLeftArm.thick = fSiL2;
    fRightArm.thin = fSiR1;
    fRightArm.thick = fSiR2;

    fArms.push_back(fLeftArm);
    fArms.push_back(fRightArm);

    //    fSiL1.push_back(IDs::channel (kMuSc   , kNotApplicable ));
    TH2F* hSiL_EvdE = new TH2F("hSiL_EvdE", "EvdE plot for SiL", 100,0,10000, 100,0,10000);
    hSiL_EvdE->SetXTitle("[keV]");
    fEvdEPlots.push_back(hSiL_EvdE);

    TH2F* hSiR_EvdE = new TH2F("hSiR_EvdE", "EvdE plot for SiR", 100,0,10000, 100,0,10000);
    hSiR_EvdE->SetXTitle("[keV]");
    fEvdEPlots.push_back(hSiR_EvdE);

    TH1F* hSiL_Time = new TH1F("hSiL_Time", "Time distribution in SiL", 2500,0,5000);
    hSiL_Time->SetXTitle("[ns]");
    fTimePlots.push_back(hSiL_Time);

    TH1F* hSiR_Time = new TH1F("hSiR_Time", "Time distribution in SiR", 2500,0,5000);
    hSiR_Time->SetXTitle("[ns]");
    fTimePlots.push_back(hSiR_Time);

  return 0;
}

int TME_EvdE::ProcessEntry(TGlobalData* gData,const TSetupData *setup){

    for(MuonEventList::const_iterator i_tme=gMuonEvents.begin();
            i_tme!=gMuonEvents.end(); ++i_tme){

      // First, check for pile-up in the muSc
      if ( (*i_tme)->HasMuonPileup()) {
	continue;
      }

      double tme_time= (*i_tme)->GetTime(); // this is the same as the muSc time

      // Loop through the arms and plots
      std::vector<TH2F*>::iterator i_evde_plot = fEvdEPlots.begin();
      std::vector<TH1F*>::iterator i_time_plot = fTimePlots.begin();
      for (std::vector<Arm>::const_iterator i_arm = fArms.begin(); 
	   i_arm != fArms.end() || i_evde_plot != fEvdEPlots.end() || i_time_plot != fTimePlots.end(); 
	   ++i_arm, ++i_evde_plot, ++i_time_plot) {
	// Now loop through the SiL1
	DetectorList si_thin = (*i_arm).thin;
	IDs::channel* si_thick = (*i_arm).thick;

	for(DetectorList::const_iterator i_det=si_thin.begin();
	    i_det!=si_thin.end(); ++i_det){
	  // pulses per channel
	  int si_thin_source_index=(*i_tme)->GetSourceIndex(*i_det);
	  while(si_thin_source_index>-1){
	    const IDs::source& si_thin_source=(*i_tme)->GetSource(si_thin_source_index);
	    
	    int n_si_thin = (*i_tme)->NumPulses(si_thin_source);
	    //	      std::cout << si_thin_source << " has " << n_si_thin << " pulses" << std::endl;
	    for(int i=0; i<n_si_thin; ++i){
	      const TDetectorPulse* tdp_si_thin=(*i_tme)->GetPulse(si_thin_source,i);
	      double thin_energy = tdp_si_thin->GetTAP(TDetectorPulse::kSlow)->GetEnergy();
	      double thin_time = tdp_si_thin->GetTime();

	      // Loop trhough the si_thick pulses
	      int si_thick_source_index=(*i_tme)->GetSourceIndex(*si_thick);
	      while (si_thick_source_index>-1) {
		const IDs::source& si_thick_source=(*i_tme)->GetSource(si_thick_source_index);
		int n_si_thick = (*i_tme)->NumPulses(si_thick_source);
		for (int j=0; j<n_si_thick; ++j) {
		  const TDetectorPulse* tdp_si_thick=(*i_tme)->GetPulse(si_thick_source,j);
		  double thick_energy = tdp_si_thick->GetTAP(TDetectorPulse::kSlow)->GetEnergy();
		  double thick_time = tdp_si_thick->GetTime();
		  
		  if ( std::fabs(thin_time - tme_time) > 200 ) { 
		    (*i_evde_plot)->Fill(thick_energy+thin_energy, thin_energy);
		    (*i_time_plot)->Fill(tme_time - thin_time);
		  }
		}
		si_thick_source_index=(*i_tme)->GetSourceIndex(*si_thick,si_thick_source_index+1);
	      }
	    }
	    si_thin_source_index=(*i_tme)->GetSourceIndex(*i_det,si_thin_source_index+1);
	  }
	}
      }
    }

  return 0;
}

int TME_EvdE::AfterLastEntry(TGlobalData* gData,const TSetupData *setup){

  return 0;
}

ALCAP_REGISTER_MODULE(TME_EvdE);
