#include "TME_Al50_EvdE.h"
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

TME_Al50_EvdE::TME_Al50_EvdE(modules::options* opts):
  BaseModule("TME_Al50_EvdE",opts),fNullCount(0),fTdpCount(0){

  fStoppedProtonCut=opts->GetBool("stopped_proton_cut", false);
}

TME_Al50_EvdE::~TME_Al50_EvdE(){
}

int TME_Al50_EvdE::BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup){

  fLeftArm.detname = "SiL";
  fLeftArmQuad1.detname = "SiL_Quad1";
  fLeftArmQuad2.detname = "SiL_Quad2";
  fLeftArmQuad3.detname = "SiL_Quad3";
  fLeftArmQuad4.detname = "SiL_Quad4";
  fRightArm.detname = "SiR";
  fRightArmQuad1.detname = "SiR_Quad1";
  fRightArmQuad2.detname = "SiR_Quad2";
  fRightArmQuad3.detname = "SiR_Quad3";
  fRightArmQuad4.detname = "SiR_Quad4";


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
  DetectorList left_quad_1; left_quad_1.push_back(IDs::channel (kSiL1_1 , kNotApplicable));
  fLeftArmQuad1.thin = left_quad_1;
  fLeftArmQuad1.thick = fSiL2;
  DetectorList left_quad_2; left_quad_2.push_back(IDs::channel (kSiL1_2 , kNotApplicable));
  fLeftArmQuad2.thin = left_quad_2;
  fLeftArmQuad2.thick = fSiL2;
  DetectorList left_quad_3; left_quad_3.push_back(IDs::channel (kSiL1_3 , kNotApplicable));
  fLeftArmQuad3.thin = left_quad_3;
  fLeftArmQuad3.thick = fSiL2;
  DetectorList left_quad_4; left_quad_4.push_back(IDs::channel (kSiL1_4 , kNotApplicable));
  fLeftArmQuad4.thin = left_quad_4;
  fLeftArmQuad4.thick = fSiL2;

  fRightArm.thin = fSiR1;
  fRightArm.thick = fSiR2;
  DetectorList right_quad_1; right_quad_1.push_back(IDs::channel (kSiR1_1 , kNotApplicable));
  fRightArmQuad1.thin = right_quad_1;
  fRightArmQuad1.thick = fSiR2;
  DetectorList right_quad_2; right_quad_2.push_back(IDs::channel (kSiR1_2 , kNotApplicable));
  fRightArmQuad2.thin = right_quad_2;
  fRightArmQuad2.thick = fSiR2;
  DetectorList right_quad_3; right_quad_3.push_back(IDs::channel (kSiR1_3 , kNotApplicable));
  fRightArmQuad3.thin = right_quad_3;
  fRightArmQuad3.thick = fSiR2;
  DetectorList right_quad_4; right_quad_4.push_back(IDs::channel (kSiR1_4 , kNotApplicable));
  fRightArmQuad4.thin = right_quad_4;
  fRightArmQuad4.thick = fSiR2;


  // Hard-coded for the time being
  fLeftArm.lower_time_cut = 0;
  fLeftArm.upper_time_cut = 10000;
  fLeftArmQuad1.lower_time_cut = 0;
  fLeftArmQuad1.upper_time_cut = 999999999;
  fLeftArmQuad2.lower_time_cut = 0;
  fLeftArmQuad2.upper_time_cut = 999999999;
  fLeftArmQuad3.lower_time_cut = 0;
  fLeftArmQuad3.upper_time_cut = 999999999;
  fLeftArmQuad4.lower_time_cut = 0;
  fLeftArmQuad4.upper_time_cut = 999999999;

  fRightArm.lower_time_cut = 0;
  fRightArm.upper_time_cut = 10000;
  fRightArmQuad1.lower_time_cut = 0;
  fRightArmQuad1.upper_time_cut = 999999999;
  fRightArmQuad2.lower_time_cut = 0;
  fRightArmQuad2.upper_time_cut = 999999999;
  fRightArmQuad3.lower_time_cut = 0;
  fRightArmQuad3.upper_time_cut = 999999999;
  fRightArmQuad4.lower_time_cut = 0;
  fRightArmQuad4.upper_time_cut = 999999999;

  fArms.push_back(fLeftArm);
  //  fArms.push_back(fLeftArmQuad1);
  //  fArms.push_back(fLeftArmQuad2);
  //  fArms.push_back(fLeftArmQuad3);
  //  fArms.push_back(fLeftArmQuad4);
  fArms.push_back(fRightArm);
  //  fArms.push_back(fRightArmQuad1);
  //  fArms.push_back(fRightArmQuad2);
  //  fArms.push_back(fRightArmQuad3);
  //  fArms.push_back(fRightArmQuad4);

  // Create the histograms for each arm
  for (std::vector<Arm>::iterator i_arm = fArms.begin(); i_arm != fArms.end(); ++i_arm) {
    std::string evde_histname = i_arm->detname + "_EvdE";
    std::string evde_histtitle = "The E v dE plot for " + i_arm->detname;
    i_arm->h_EvdE = new TH2F(evde_histname.c_str(), evde_histtitle.c_str(), 250,0,25000, 100,0,10000);
    i_arm->h_EvdE->SetXTitle("E + dE [keV]");
    i_arm->h_EvdE->SetYTitle("dE [keV]");

    std::string time_histname = i_arm->detname + "_Time";
    std::string time_histtitle = "The time of hits in " + i_arm->detname;
    i_arm->h_Time = new TH1F(time_histname.c_str(), time_histtitle.c_str(), 2500,0,10000);
    i_arm->h_Time->SetXTitle("Time [ns]");
    i_arm->h_Time->SetYTitle("Count");    

    std::string vetoed_thin_energy_histname = i_arm->detname + "_Vetoed_Thin_EnergyTime";
    std::string vetoed_thin_energy_histtitle = "The vetoed_thin_energy and time of hits in " + i_arm->detname;
    i_arm->h_VetoedThinEnergyTime = new TH2F(vetoed_thin_energy_histname.c_str(), vetoed_thin_energy_histtitle.c_str(), 5000,-10000,10000, 100,0,5000);
    i_arm->h_VetoedThinEnergyTime->SetYTitle("Energy [keV]");
    i_arm->h_VetoedThinEnergyTime->SetXTitle("t_{thin} - t_{tme} [ns]");

    if (fStoppedProtonCut) {
      TTree* pid_cuts_tree = new TTree();
      std::string pid_cuts_filename = "/gpfs/home/edmonds_a/AlcapDAQ/analyzer/rootana/src/Al50/pid-cuts-" + i_arm->detname + ".txt";
      pid_cuts_tree->ReadFile(pid_cuts_filename.c_str());
      //      pid_cuts_tree->Print();

      TBranch* energy_branch = (TBranch*) pid_cuts_tree->GetBranch("energy");
      double energy;
      energy_branch->SetAddress(&energy);

      TBranch* stopped_proton_mean_branch = (TBranch*) pid_cuts_tree->GetBranch("p_stop_mean");
      double stopped_proton_mean;
      stopped_proton_mean_branch->SetAddress(&stopped_proton_mean);

      TBranch* stopped_proton_rms_branch = (TBranch*) pid_cuts_tree->GetBranch("p_stop_rms");
      double stopped_proton_rms;
      stopped_proton_rms_branch->SetAddress(&stopped_proton_rms);

      std::string profile_histname = i_arm->detname + "_StoppedProtonProfile";
      std::string profile_histtitle = "Probability that at a given (E, dE) that the hit is a proton in " + i_arm->detname;
      i_arm->h_stopped_proton_profile = new TH1F(profile_histname.c_str(), profile_histtitle.c_str(), 100,0,10000);
      i_arm->h_stopped_proton_profile->SetXTitle("E+dE [keV]");
      i_arm->h_stopped_proton_profile->SetYTitle("dE [keV]");

      for (int i_entry = 0; i_entry < pid_cuts_tree->GetEntries(); ++i_entry) {
	pid_cuts_tree->GetEntry(i_entry);
	int i_bin = i_arm->h_stopped_proton_profile->FindBin(energy);
	i_arm->h_stopped_proton_profile->SetBinContent(i_bin, stopped_proton_mean);
	i_arm->h_stopped_proton_profile->SetBinError(i_bin, stopped_proton_rms);
      }
      delete pid_cuts_tree;
    }
    else {
      i_arm->h_stopped_proton_profile = NULL;
    }

  }

  return 0;
}

int TME_Al50_EvdE::ProcessEntry(TGlobalData* gData,const TSetupData *setup){

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

	// Fill the vetoed thin energy plot
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
	      double thin_time = tdp_si_thin->GetTime();

	      //	      if (thin_energy < 500) {
	      //		continue;
	      //	      }
	      /*	      std::cout << "TME: #" << i_tme - gMuonEvents.begin() << " "
		        << i_arm->detname << " "
			<< "Q" << i_det - si_thin.begin() << " "
			<< "#" << i_thin_pulse << " "
			<< thin_energy << " keV, t = " << thin_time << " ns" << std::endl;
	      */
	      //	      std::cout << "Time to central muon = " << thin_time - tme_time << std::endl;

	      // Go through the thick detector and see if there's a hit within +- 5ns
	      int si_thick_source_index=(*i_tme)->GetSourceIndex(*si_thick);

	      // While there are thick detector sources still around
	      bool veto = false;
	      while (si_thick_source_index>-1) {
		const IDs::source& si_thick_source=(*i_tme)->GetSource(si_thick_source_index);
		int n_si_thick = (*i_tme)->NumPulses(si_thick_source);		
		// Loop through the pulses in the thick detector
		for (int i_thick_pulse=0; i_thick_pulse<n_si_thick; ++i_thick_pulse) {
		  const TDetectorPulse* tdp_si_thick=(*i_tme)->GetPulse(si_thick_source,i_thick_pulse);
		  double thick_time = tdp_si_thick->GetTime();
		  if (std::fabs(thick_time - thin_time) <= 100) {
		    veto = true;
		    break;
		  }
		}
		si_thick_source_index=(*i_tme)->GetSourceIndex(*si_thick,si_thick_source_index+1);
	      }
	      if (!veto) {
		i_arm->h_VetoedThinEnergyTime->Fill(thin_time - tme_time, thin_energy);
	      }
	    }
	    si_thin_source_index=(*i_tme)->GetSourceIndex(*i_det,si_thin_source_index+1);
	  }
	}
	
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
	    // Skip to next pulse if energy <100 keV
	    if (thick_energy < 100) {
	      continue;
	    }

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

		  if (thin_energy < 100) {
		    continue;
		  }
		  double thin_amplitude = tdp_si_thin->GetAmplitude();
		  double thin_time = tdp_si_thin->GetTime();
		  
		  bool passes_cuts = false;

		  double arrival_time = thick_time - tme_time;
		  if ( arrival_time > i_arm->lower_time_cut && arrival_time < i_arm->upper_time_cut ) { 
		    // Now check if this passes our proton cut
		    if (fStoppedProtonCut) {
		      int bin = i_arm->h_stopped_proton_profile->FindBin(thick_energy+thin_energy);
		      double mc_mean = i_arm->h_stopped_proton_profile->GetBinContent(bin);
		      double mc_rms = i_arm->h_stopped_proton_profile->GetBinError(bin);

		      if (std::fabs(mc_mean - thin_energy) < mc_rms) {
			passes_cuts = true;
		      }
		    }
		    else { // everything passes if we're not cutting on protons
		      passes_cuts = true;
		    }
		  }

		  if (std::abs(thin_time - thick_time) > 500) {
		    //		    std::cout << "abs(t_thin - t_thick) = " << std::fabs(thin_time - thick_time) << std::endl;
		    passes_cuts=false;
		  }
		  if (fStoppedProtonCut && (thick_energy+thin_energy < 1500 || thick_energy+thin_energy > 8000)) {
		    passes_cuts=false; // only accept proton if E is between 1.5 and 8 MeV (like Nam's cut)
		  }
		  if (passes_cuts) {
		    //		    if (fStoppedProtonCut) {
		    //		      std::cout << "Amplitude --> Energy (thick): " << thick_amplitude << " --> " << thick_energy << std::endl;
		    //		      std::cout << "Amplitude --> Energy (thin): " << thin_amplitude << " --> " << thin_energy << std::endl;
		    //		      std::cout << "Plotting: " << thick_energy+thin_energy << ", " << thin_energy << std::endl;
		      //		    }
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

int TME_Al50_EvdE::AfterLastEntry(TGlobalData* gData,const TSetupData *setup){

  return 0;
}

ALCAP_REGISTER_MODULE(TME_Al50_EvdE, stopped_proton_cut);
