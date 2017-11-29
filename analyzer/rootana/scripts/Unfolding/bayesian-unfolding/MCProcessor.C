#define MCProcessor_cxx
// The class definition in MCProcessor.h has been generated automatically
// by the ROOT utility TTree::MakeSelector(). This class is derived
// from the ROOT class TSelector. For more information on the TSelector
// framework see $ROOTSYS/README/README.SELECTOR or the ROOT User Manual.

// The following methods are defined in this file:
//    Begin():        called every time a loop on the tree starts,
//                    a convenient place to create your histograms.
//    SlaveBegin():   called after Begin(), when on PROOF called only on the
//                    slave servers.
//    Process():      called for each event, in this function you decide what
//                    to read and fill your histograms.
//    SlaveTerminate: called at the end of the loop on the tree, when on PROOF
//                    called only on the slave servers.
//    Terminate():    called at the end of the loop on the tree,
//                    a convenient place to draw/fit your histograms.
//
// To use this file, try the following session on your Tree T:
//
// Root > T->Process("MCProcessor.C")
// Root > T->Process("MCProcessor.C","some options")
// Root > T->Process("MCProcessor.C+")
//

#include <iostream>
#include "MCProcessor.h"
#include <TTree.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TLine.h>
#include <TH2.h>
#include <TH1.h>
#include <TStyle.h>


void MCProcessor::Begin(TTree * /*tree*/)
{
	// The Begin() function is called at the start of the query.
	// When running with PROOF Begin() is only called on the client.
	// The tree argument is deprecated (on PROOF 0 is passed).
	TString option = GetOption();
	TObjArray* opts = option.Tokenize(" ");
        inputAbsolutePath = ((TObjString* )opts->At(0))->GetString().Data();
        particle = ((TObjString* )opts->At(1))->GetString().Data();

	Info("Begin", "Loading from file:%s, processing particle:%s", inputAbsolutePath.Data(), particle.Data() );
	TString fileName(inputAbsolutePath(inputAbsolutePath.Last('/')+1, inputAbsolutePath.Length() ) );
	fOutputFile = new TFile("bayesian.unfolding." + fileName, "RECREATE");
}

void MCProcessor::SlaveBegin(TTree * /*tree*/)
{
	// The SlaveBegin() function is called after the Begin() function.
	// When running with PROOF SlaveBegin() is called on each slave server.
	// The tree argument is deprecated (on PROOF 0 is passed).

	TString option = GetOption();
	tUnfolding = new TTree("Unfolding", "Unfolding");
	tUnfolding->Branch("truth_E", &truth_E, "truth_E/D");
	tUnfolding->Branch("SiR1_E", &SiR1_E, "SiR1_E/D");
	tUnfolding->Branch("SiR2_E", &SiR2_E, "SiR2_E/D");
	tUnfolding->Branch("SiR3_E", &SiR3_E, "SiR3_E/D");
	tUnfolding->Branch("SiL1_E", &SiL1_E, "SiL1_E/D");
	tUnfolding->Branch("SiL3_E", &SiL3_E, "SiL3_E/D");

	/** Left detector */
	Int_t binSize = 256;
	hEvdELeft=new TH2D("hEvdELeft","Left detector; E [MeV]; dE [MeV]",binSize,0,24,binSize,0,24);
	hEvdEStoppedLeft=new TH2D("hEvdEStoppedLeft","Left detector stopped particle; E [MeV]; dE [MeV]",binSize,0,24,binSize,0,24);
	hEvdEPunchLeft=new TH2D("hEvdEPunchLeft","Left detector punch-through particle; Total Energy [MeV]; Energy depositied in Thin [MeV]",binSize,0,24,binSize,0,24);
	hTransferLeft=new TH2D("hTransferLeft","Response matrix; Energy at left detector [MeV]; Energy at Target [MeV]",binSize,0,24,binSize,0,24);
	hTransferPTLeft=new TH2D("hTransferPTLeft","Punch through response matrix; Energy at left detector [MeV]; Energy at Target [MeV]",binSize,0,24,binSize,0,24);
	hInputLeft=new TH1D("hInputLeft","Left detector input energy; Energy (MeV)",binSize,0,24);
	hInputPTLeft=new TH1D("hInputPTLeft","Left detector punch through input energy; Energy (MeV)",binSize,0,24);
	hAcceptanceLeft=new TH1D("hAcceptanceLeft","Left detector acceptance; E [MeV]; Acceptance (%)",binSize,0,24);
	/** Right detector */
	hEvdERight=new TH2D("hEvdERight","Right detector; E [MeV]; dE [MeV]",binSize,0,24,binSize,0,24);
	hEvdEStoppedRight=new TH2D("hEvdEStoppedRight","Right detector stopped particle; E [MeV]; dE [MeV]",binSize,0,24,binSize,0,24);
	hEvdE2StoppedRight=new TH2D("hEvdEStoppedRight2","Right detector stopped particle with SiR3 veto; E [MeV]; dE [MeV]",binSize,0,24,binSize,0,24);
	hEvdEPunchRight=new TH2D("hEvdEPunchRight","Right detector punch-through particle; Total Energy [MeV]; Energy depositied in Thin [MeV]",binSize,0,24,binSize,0,24);
	hTransferRight=new TH2D("hTransferRight","Response matrix; Energy at left detector [MeV]; Energy at Target [MeV]",binSize,0,24,binSize,0,24);
	hTransferPTRight=new TH2D("hTransferPTRight","Punch through response matrix; Energy at left detector [MeV]; Energy at Target [MeV]",binSize,0,24,binSize,0,24);
	hInputRight=new TH1D("hInputRight","Right detector input energy; Energy (MeV)",binSize,0,24);
	hInputPTRight=new TH1D("hInputPTRight","Right detector punch through input energy; Energy (MeV)",binSize,0,24);
	hAcceptanceRight=new TH1D("hAcceptanceRight","Right detector acceptance; E [MeV]; Acceptance (%)",binSize,0,24);

	/** Target */
	hDepth=new TH1D("hDepth", "Emitted particle starting depth; z [mm];", 200,  -0.055, 0.055);
}

Bool_t MCProcessor::Process(Long64_t entry)
{
	// The Process() function is called for each entry in the tree (or possibly
	// keyed object in the case of PROOF) to be processed. The entry argument
	// specifies which entry in the currently loaded tree is to be processed.
	// It can be passed to either MCProcessor::GetEntry() or TBranch::GetEntry()
	// to read either all or the required parts of the data. When processing
	// keyed objects with PROOF, the object is already loaded and is available
	// via the fObject pointer.
	//
	// This function should contain the "body" of the analysis. It can contain
	// simple or elaborate selection criteria, run algorithms on the data
	// of the event and typically fill histograms.
	//
	// The processing can be stopped by calling Abort().
	//
	// Use fStatus to set the return value of TTree::Process().
	//
	// The return value is currently not used.
	b_M_particleName->GetEntry(entry);
	b_M_volName->GetEntry(entry);
	b_M_ovolName->GetEntry(entry);
	b_M_oprocess->GetEntry(entry);
	b_M_edep->GetEntry(entry);
	b_M_stopped->GetEntry(entry);
	b_i_px->GetEntry(entry);
	b_i_py->GetEntry(entry);
	b_i_pz->GetEntry(entry);
	b_M_local_x->GetEntry(entry);
	b_M_local_z->GetEntry(entry);

	SiR1_E=0; SiR2_E=0; SiR3_E=0; SiL1_E=0; SiL3_E=0;
	r_thick_stop = false;
	r_thick_stop_2 = false;
	l_thick_stop = false;
	r_N_thin=0; r_N_thick=0; r_N_thick2=0;
	l_N_thin=0; l_N_thick=0;
	if (entry % 10000 == 0)
		Info("Process", "%d / %d", entry, fChain->GetEntries() );

	if(isnan(i_px) || isnan(i_py) || isnan(i_pz) ) return kTRUE;
	if(strcmp(particle, "proton") ==0) M = 938.27;
	if(strcmp(particle, "deuteron") ==0) M = 1875.6;
	if(strcmp(particle, "triton") ==0) M = 2808.9;
	if(strcmp(particle, "alpha") ==0) M = 3727.4;
	if(strcmp(particle, "e-") ==0) M = 0.511;
	if(strcmp(particle, "mu-") ==0) M = 104.5;
	if(strcmp(particle, "pi-") ==0) M = 139.6;
	const double E_k_orig=sqrt(i_px*i_px + i_py*i_py + i_pz*i_pz + M*M) - M;
	//const double theta=std::atan2(rho,px)*degree;
	for (unsigned iElement = 0; iElement < M_particleName->size();  ++iElement) {
		// Check we have a proton
		if(M_particleName->at(iElement)!=particle) continue;
		if(M_volName->at(iElement)=="Target") {
			hDepth->Fill(M_local_z->at(iElement) * 10 * -1 );
		}

		// Check it came from the target or as a primary particle
		if(M_oprocess->at(iElement)!="NULL" && M_volName->at(iElement)!="Target") continue;
		double i_edep = M_edep->at(iElement)*1e3; // convert to MeV

		// If this is a hit in the SiR thick or thin:
		if(M_volName->at(iElement)=="SiR2"){
			++r_N_thick;
			SiR2_E+=i_edep;
			// Since this is thick, check if the proton stopped
			if(M_stopped->at(iElement)) r_thick_stop=true;
		} else if(M_volName->at(iElement)=="SiR1"){
			++r_N_thin;
			SiR1_E+=i_edep;
		} else if(M_volName->at(iElement)=="SiR3"){
			++r_N_thick2;
			SiR3_E+=i_edep;
			if(M_stopped->at(iElement)) r_thick_stop_2=true;
		}

		// If this is a hit in the SiL thick or thin:
		else if(M_volName->at(iElement)=="SiL3"){
			++l_N_thick;
			SiL3_E+=i_edep;
			// Since this is thick, check if the proton stopped
			if(M_stopped->at(iElement)) l_thick_stop=true;
		} else if(M_volName->at(iElement)=="SiL1"){
			++l_N_thin;
			SiL1_E+=i_edep;
		}
		else {
			continue;
		}
	}
	if(r_N_thin >0 && r_N_thick>0){
		hEvdERight->Fill(SiR2_E+SiR1_E, SiR1_E);
		if(r_thick_stop) {
			hEvdEStoppedRight->Fill(SiR2_E+SiR1_E, SiR1_E);
			hTransferRight->Fill(SiR2_E+SiR1_E, E_k_orig);
			hAcceptanceRight->Fill(E_k_orig);
		} else {
			hEvdEPunchRight->Fill(SiR2_E+SiR1_E, SiR1_E);
			hTransferPTRight->Fill(SiR2_E+SiR1_E, E_k_orig);
		}
		if(r_N_thick2==0) {
			hEvdE2StoppedRight->Fill(SiR2_E+SiR1_E, SiR1_E);
		}
	}
	if(l_N_thin >0 && l_N_thick>0){
		hEvdELeft->Fill(SiL3_E+SiL1_E, SiL1_E);
		if(l_thick_stop) {
			hEvdEStoppedLeft->Fill(SiL3_E+SiL1_E, SiL1_E);
			hTransferLeft->Fill(SiL3_E+SiL1_E, E_k_orig);
			hAcceptanceLeft->Fill(E_k_orig);
		} else {
			hEvdEPunchLeft->Fill(SiL3_E+SiL1_E, SiL1_E);
			hTransferPTLeft->Fill(SiL3_E+SiL1_E, E_k_orig);
		}
	}
	if(i_px>0) hInputLeft->Fill(E_k_orig);
	if(i_px<0) hInputRight->Fill(E_k_orig);

	truth_E = E_k_orig;
	if((SiR2_E!=0 && SiR1_E!=0) || (SiL3_E!=0 && SiL1_E!=0) )
		tUnfolding->Fill();

	return kTRUE;
}
void MCProcessor::SlaveTerminate()
{
	// The SlaveTerminate() function is called after all entries or objects
	// have been processed. When running with PROOF SlaveTerminate() is called
	// on each slave server.
	hAcceptanceLeft->Divide(hInputLeft);
	hAcceptanceLeft->Scale(100);
	hAcceptanceRight->Divide(hInputRight);
	hAcceptanceRight->Scale(100);

	hEvdERight_px=hEvdEStoppedRight->ProjectionX("_px");
	hEvdELeft_px=hEvdEStoppedLeft->ProjectionX("_px");

}

void MCProcessor::Terminate()
{
	// The Terminate() function is the last function to be called during
	// a query. It always runs on the client, it can be used to present
	// the results graphically or save the results to file.
	gDirectory->cd(0);
	fOutputFile->Write();
}
