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

  //    fSiL1.push_back(IDs::channel (kMuSc   , kNotApplicable ));
  TH2F* hSiL_EvdE = new TH2F("hSiL_EvdE", "EvdE plot for SiL", 100,0,10000, 100,0,10000);
  hSiL_EvdE->SetXTitle("[keV]");
  fEvdEPlots.push_back(hSiL_EvdE);

  TH2F* hSiR_EvdE = new TH2F("hSiR_EvdE", "EvdE plot for SiR", 100,0,10000, 100,0,10000);
  hSiR_EvdE->SetXTitle("[keV]");
  fEvdEPlots.push_back(hSiR_EvdE);

  TH1F* hSiL_Time = new TH1F("hSiL_Time", "Time distribution in SiL", 2500,0,10000);
  hSiL_Time->SetXTitle("[ns]");
  fTimePlots.push_back(hSiL_Time);

  TH1F* hSiR_Time = new TH1F("hSiR_Time", "Time distribution in SiR", 2500,0,10000);
  hSiR_Time->SetXTitle("[ns]");
  fTimePlots.push_back(hSiR_Time);

  if (fStoppedProtonCut) {
    TTree* pid_cuts_tree = new TTree();
    pid_cuts_tree->ReadFile("/gpfs/home/edmonds_a/AlcapDAQ/analyzer/rootana/src/Al50/pid-cuts.txt");
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

    fStoppedProtonProbHist = new TH2F("hStoppedProtonProbHist", "hStoppedProtonProbHist", 100,0,10000, 100,0,10000);
    fStoppedProtonProbHist->SetXTitle("E [keV]");
    fStoppedProtonProbHist->SetYTitle("dE [keV]");
    fStoppedProtonProbHist->SetTitle("Probability that a given (E, dE) is a proton");

    for (int i_entry = 0; i_entry < pid_cuts_tree->GetEntries(); ++i_entry) {
      pid_cuts_tree->GetEntry(i_entry);
      fStoppedProtonProbHist->Fill(energy, d_energy, stopped_proton_prob);
    }
  }
  else {
    fStoppedProtonProbHist = NULL;
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

      // Create some iterators for the plot vectors
      std::vector<TH2F*>::iterator i_evde_plot = fEvdEPlots.begin();
      std::vector<TH1F*>::iterator i_time_plot = fTimePlots.begin();

      // Loop through the arms and plots
      for (std::vector<Arm>::const_iterator i_arm = fArms.begin(); 
	   i_arm != fArms.end() || i_evde_plot != fEvdEPlots.end() || i_time_plot != fTimePlots.end(); 
	   ++i_arm, ++i_evde_plot, ++i_time_plot) {

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
	    double thick_energy = tdp_si_thick->GetTAP(TDetectorPulse::kSlow)->GetEnergy();
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
		  double thin_energy = tdp_si_thin->GetTAP(TDetectorPulse::kSlow)->GetEnergy();
		  double thin_amplitude = tdp_si_thin->GetAmplitude();
		  double thin_time = tdp_si_thin->GetTime();
		  
		  bool passes_cuts = false;

		  double arrival_time = thin_time - tme_time;
		  if ( arrival_time > i_arm->lower_time_cut && arrival_time < i_arm->upper_time_cut ) { 
		    // Now check if this passes our proton cut
		    if (fStoppedProtonCut) {
		      int bin = fStoppedProtonProbHist->FindBin(thick_energy+thin_energy, thin_energy);
		      double probability = fStoppedProtonProbHist->GetBinContent(bin);
		      if (probability > 0.99) {
			passes_cuts = true;
		      }
		    }
		    else { // everything passes if we're not cutting on protons
		      passes_cuts = true;
		    }
		  }

		  if (thick_time - thin_time > 0) {
		    //		    std::cout << "abs(t_thin - t_thick) = " << std::fabs(thin_time - thick_time) << std::endl;
		    passes_cuts=true;
		  }
		  if (passes_cuts) {
		    //		    std::cout << "Amplitude --> Energy (thick): " << thick_amplitude << " --> " << thick_energy << std::endl;
		    //		    std::cout << "Amplitude --> Energy (thin): " << thin_amplitude << " --> " << thin_energy << std::endl;
		    //		    std::cout << "Plotting: " << thick_energy+thin_energy << ", " << thin_energy << std::endl;
		    (*i_evde_plot)->Fill(thick_energy+thin_energy, thin_energy);
		    (*i_time_plot)->Fill(arrival_time);
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
