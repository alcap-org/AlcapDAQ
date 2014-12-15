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

  fStoppedProtonCut=opts->GetBool("stopped_proton_cut", false);
}

TME_EvdE::~TME_EvdE(){
}

int TME_EvdE::BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup){

  fLeftArm.detname = "SiL";
  fRightArm.detname = "SiR";

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

  // Hard-coded for the time being
  fLeftArm.lower_time_cut = 600;
  fLeftArm.upper_time_cut = 6000;
  fRightArm.lower_time_cut = 500;
  fRightArm.upper_time_cut = 4000;

  fArms.push_back(fLeftArm);
  fArms.push_back(fRightArm);

  // Create the histograms for each arm
  for (std::vector<Arm>::iterator i_arm = fArms.begin(); i_arm != fArms.end(); ++i_arm) {
    std::string evde_histname = i_arm->detname + "_EvdE";
    std::string evde_histtitle = "The E v dE plot for " + i_arm->detname;
    i_arm->h_EvdE = new TH2F(evde_histname.c_str(), evde_histtitle.c_str(), 100,0,10000, 100,0,10000);
    i_arm->h_EvdE->SetXTitle("E + dE [keV]");
    i_arm->h_EvdE->SetYTitle("dE [keV]");

    std::string time_histname = i_arm->detname + "_Time";
    std::string time_histtitle = "The time of hits in " + i_arm->detname;
    i_arm->h_Time = new TH1F(time_histname.c_str(), time_histtitle.c_str(), 2500,0,10000);
    i_arm->h_Time->SetXTitle("Time [ns]");
    i_arm->h_Time->SetYTitle("Count");    

    if (fStoppedProtonCut) {
      TTree* pid_cuts_tree = new TTree();
      std::string pid_cuts_filename = "/gpfs/home/edmonds_a/AlcapDAQ/analyzer/rootana/src/Al50/pid-cuts-" + i_arm->detname + ".txt";
      pid_cuts_tree->ReadFile(pid_cuts_filename.c_str());
      pid_cuts_tree->Print();

      TBranch* energy_branch = (TBranch*) pid_cuts_tree->GetBranch("energy");
      double energy;
      energy_branch->SetAddress(&energy);

      TBranch* d_energy_branch = (TBranch*) pid_cuts_tree->GetBranch("dEnergy");
      double d_energy;
      d_energy_branch->SetAddress(&d_energy);

      TBranch* stopped_proton_prob_branch = (TBranch*) pid_cuts_tree->GetBranch("p_stop_prob");
      double stopped_proton_prob;
      stopped_proton_prob_branch->SetAddress(&stopped_proton_prob);

      std::string prob_histname = i_arm->detname + "_StoppedProtonProb";
      std::string prob_histtitle = "Probability that at a given (E, dE) that the hit is a proton in " + i_arm->detname;
      i_arm->h_stopped_proton_prob = new TH2F(prob_histname.c_str(), prob_histtitle.c_str(), 100,0,10000, 100,0,10000);
      i_arm->h_stopped_proton_prob->SetXTitle("E [keV]");
      i_arm->h_stopped_proton_prob->SetYTitle("dE [keV]");

      for (int i_entry = 0; i_entry < pid_cuts_tree->GetEntries(); ++i_entry) {
	pid_cuts_tree->GetEntry(i_entry);
	i_arm->h_stopped_proton_prob->Fill(energy, d_energy, stopped_proton_prob);
	//	std::cout << i_arm->detname << ": Filling @ (" << energy << ", " << d_energy << "): " << stopped_proton_prob << std::endl;
      }
      delete pid_cuts_tree;
    }
    else {
      i_arm->h_stopped_proton_prob = NULL;
    }

  }

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
      for (std::vector<Arm>::const_iterator i_arm = fArms.begin(); i_arm != fArms.end(); ++i_arm) {

	// Check that there are pulses in both the thin and the thick silicon detectors
	DetectorList si_thin = (*i_arm).thin;
	IDs::channel* si_thick = (*i_arm).thick;

	// Get the thick detector
	int si_thick_source_index=(*i_tme)->GetSourceIndex(*si_thick);

	// While there are thick detector sources still around
	while (si_thick_source_index>-1) {
	  const IDs::source& si_thick_source=(*i_tme)->GetSource(si_thick_source_index);
	  int n_si_thick = (*i_tme)->NumPulses(si_thick_source);

	  // Loop through the pulses in the thick detector
	  for (int i_thick_pulse=0; i_thick_pulse<n_si_thick; ++i_thick_pulse) {
	    const TDetectorPulse* tdp_si_thick=(*i_tme)->GetPulse(si_thick_source,i_thick_pulse);
	    double thick_energy = tdp_si_thick->GetTAP(TDetectorPulse::kFast)->GetEnergy();
	    double thick_amplitude = tdp_si_thick->GetAmplitude();
	    double thick_time = tdp_si_thick->GetTime();

	    // Loop through the thin quadrants
	    for(DetectorList::const_iterator i_det=si_thin.begin();
		i_det!=si_thin.end(); ++i_det){

	      // Get the thin quadrant and loop through its sources
	      int si_thin_source_index=(*i_tme)->GetSourceIndex(*i_det);
	      while(si_thin_source_index>-1){
		const IDs::source& si_thin_source=(*i_tme)->GetSource(si_thin_source_index);
		int n_si_thin = (*i_tme)->NumPulses(si_thin_source);

		// Loop through the pulses in this quadrant
		for(int i_thin_pulse=0; i_thin_pulse<n_si_thin; ++i_thin_pulse){
		  const TDetectorPulse* tdp_si_thin=(*i_tme)->GetPulse(si_thin_source,i_thin_pulse);
		  double thin_energy = tdp_si_thin->GetTAP(TDetectorPulse::kFast)->GetEnergy();
		  double thin_amplitude = tdp_si_thin->GetAmplitude();
		  double thin_time = tdp_si_thin->GetTime();
		  
		  bool passes_cuts = false;

		  double arrival_time = thin_time - tme_time;
		  if ( arrival_time > i_arm->lower_time_cut && arrival_time < i_arm->upper_time_cut ) { 
		    // Now check if this passes our proton cut
		    if (fStoppedProtonCut) {
		      int bin = i_arm->h_stopped_proton_prob->FindBin(thick_energy+thin_energy, thin_energy);
		      double probability = i_arm->h_stopped_proton_prob->GetBinContent(bin);
		      //		      std::cout << "(E+dE, dE) = (" << thick_energy+thin_energy << ", " << thin_energy << "): Prob = " << probability << std::endl;
		      if (probability > 0.99) {
			passes_cuts = true;
		      }
		    }
		    else { // everything passes if we're not cutting on protons
		      passes_cuts = true;
		    }
		  }

		  //		  if (thick_time - thin_time > 0) {
		    //		    std::cout << "abs(t_thin - t_thick) = " << std::fabs(thin_time - thick_time) << std::endl;
		  //		    passes_cuts=true;
		  //		  }
		  if (passes_cuts) {
		    //		    std::cout << "Amplitude --> Energy (thick): " << thick_amplitude << " --> " << thick_energy << std::endl;
		    //		    std::cout << "Amplitude --> Energy (thin): " << thin_amplitude << " --> " << thin_energy << std::endl;
		    //		    std::cout << "Plotting: " << thick_energy+thin_energy << ", " << thin_energy << std::endl;
		    i_arm->h_EvdE->Fill(thick_energy+thin_energy, thin_energy);
		    i_arm->h_Time->Fill(arrival_time);
		  }

		}
		si_thin_source_index=(*i_tme)->GetSourceIndex(*i_det,si_thin_source_index+1);
	      }
	    }
	  }
	  si_thick_source_index=(*i_tme)->GetSourceIndex(*si_thick,si_thick_source_index+1);
	}
      }
    }
    
    return 0;
}

int TME_EvdE::AfterLastEntry(TGlobalData* gData,const TSetupData *setup){

  return 0;
}

ALCAP_REGISTER_MODULE(TME_EvdE, stopped_proton_cut);
