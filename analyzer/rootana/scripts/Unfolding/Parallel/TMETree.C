#define TMETree_cxx
// The class definition in TMETree.h has been generated automatically
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
// root> T->Process("TMETree.C")
// root> T->Process("TMETree.C","some options")
// root> T->Process("TMETree.C+")
//

#include "TMETree.h"
#include <TNtuple.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <TMath.h>


Double_t TMETree::LikelihoodProbabilityCut(Double_t X, Double_t mu, Double_t sigma) {
	Double_t Z = (X - mu)/sigma;
	return exp(-Z*Z/2) / (sqrt(2*TMath::Pi() ) * sigma);
}

void TMETree::Begin(TTree * /*tree*/)
{
   // The Begin() function is called at the start of the query.
   // When running with PROOF Begin() is only called on the client.
   // The tree argument is deprecated (on PROOF 0 is passed).
	TString option = GetOption();

	TF1 *p_led = new TF1("left_proton", "[0]*(1-[1]/x)**[2]*exp(-x/[3])", 1.5, 10);
	p_led->SetParameter(0, 0.585205);
	p_led->SetParameter(1, -0.329325);
	p_led->SetParameter(2, 8.51648);
	p_led->SetParameter(3, 16.3396);
	fitFunctions["left_proton"] = p_led;
	
	TF1 *d_led = new TF1("left_deuteron", "[0]*(1-[1]/x)**[2]*exp(-x/[3])", 2.0, 11);
	d_led->SetParameter(0, 0.704607);
	d_led->SetParameter(1, -2.26743);
	d_led->SetParameter(2, 2.13257);
	d_led->SetParameter(3, 24.0647);
	fitFunctions["left_deuteron"] = d_led;
	
	TF1 *t_led = new TF1("left_triton", "[0]*(1-[1]/x)**[2]*exp(-x/[3])", 2.5, 12);
	t_led->SetParameter(0, 0.941740);
	t_led->SetParameter(1, -2.34857);
	t_led->SetParameter(2, 2.10046);
	t_led->SetParameter(3, 24.3845);
	fitFunctions["left_triton"] = t_led;
	
	TF1 *p_red = new TF1("right_proton", "[0]*(1-[1]/x)**[2]*exp(-x/[3])", 1.5, 10);
	p_red->SetParameter(0, 0.62285);
	p_red->SetParameter(1, -0.251733);
	p_red->SetParameter(2, 11.1821);
	p_red->SetParameter(3, 12.538);
	fitFunctions["right_proton"] = p_red;
	
	TF1 *d_red = new TF1("right_deuteron", "[0]*(1-[1]/x)**[2]*exp(-x/[3])", 2.0, 11);
	d_red->SetParameter(0, 0.839878);
	d_red->SetParameter(1, -0.119888);
	d_red->SetParameter(2, 29.9925);
	d_red->SetParameter(3, 19.1545);
	fitFunctions["right_deuteron"] = d_red;
	
	TF1 *t_red = new TF1("right_triton", "[0]*(1-[1]/x)**[2]*exp(-x/[3])", 2.5, 12);
	t_red->SetParameter(0, 0.964517);
	t_red->SetParameter(1, -2.73694);
	t_red->SetParameter(2, 2.04411);
	t_red->SetParameter(3, 21.1987);
	fitFunctions["right_triton"] = t_red;

   for(pair<const char *, TF1 *> f: fitFunctions) {
      fOutput->Add(f.second);
   }
}

void TMETree::SlaveBegin(TTree * /*tree*/)
{
   // The SlaveBegin() function is called after the Begin() function.
   // When running with PROOF SlaveBegin() is called on each slave server.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();
   tNNUnfolding = new TTree("tNNUnfolding", "NNUnfolding");
   tNNUnfolding->Branch("SiL1_E", &SiL1_E, "SiL1_E/D");
   tNNUnfolding->Branch("SiL3_E", &SiL3_E, "SiL3_E/D");
   tNNUnfolding->Branch("SiR1_E", &SiR1_E, "SiR1_E/D");
   tNNUnfolding->Branch("SiR2_E", &SiR2_E, "SiR2_E/D");
   tNNUnfolding->Branch("SiR3_E", &SiR3_E, "SiR3_E/D");

   hSiL_tDiff = new TH1D("hSiL_tDiff", "SiL tDiff; t [ns]", 1000, -10000, 10000);
   hSiR_tDiff = new TH1D("hSiR_tDiff", "SiR tDiff; t [ns]", 1000, -10000, 10000);
   hSiL_EvdE = new TH2D("hSiL_EvdE", "SiL EvdE; E+dE [MeV]; dE [MeV]", 4096, 0, 24.576, 4096, 0, 8.192);
   hSiL_EvdE_proton = new TH2D("hSiL_EvdE_proton", "SiL EvdE proton; E+dE [MeV]; dE [MeV]", 4096, 0, 24.576, 4096, 0, 8.192);
   hSiL_EvdE_deuteron = new TH2D("hSiL_EvdE_deuteron", "SiL EvdE deuteron; E+dE [MeV]; dE [MeV]", 4096, 0, 24.576, 4096, 0, 8.192);
   hSiL_EvdE_triton = new TH2D("hSiL_EvdE_triton", "SiL EvdE triton; E+dE [MeV]; dE [MeV]", 4096, 0, 24.576, 4096, 0, 8.192);
   hSiR_EvdE = new TH2D("hSiR_EvdE", "SiR EvdE; E+dE [MeV]; dE [MeV]", 4096, 0, 24.576, 4096, 0, 8.192);
   hSiR_EvdE_proton = new TH2D("hSiR_EvdE_proton", "SiR EvdE proton; E+dE [MeV]; dE [MeV]", 4096, 0, 24.576, 4096, 0, 8.192);
   hSiR_EvdE_deuteron = new TH2D("hSiR_EvdE_deuteron", "SiR EvdE deuteron; E+dE [MeV]; dE [MeV]", 4096, 0, 24.576, 4096, 0, 8.192);
   hSiR_EvdE_triton = new TH2D("hSiR_EvdE_triton", "SiR EvdE triton; E+dE [MeV]; dE [MeV]", 4096, 0, 24.576, 4096, 0, 8.192);
   hSiT_t = new TH1D("hSiT_t", "SiT time; t [ns]", 1000, -10000, 10000);
   hSiT_E = new TH1D("hSiT_E", "SiT energy; E [MeV]", 4096, 0, 8.192);

   TH1D *hSiL_PD_proton = new TH1D("h_SiL_PD_proton", "SiL proton probability distribution; dE [MeV]", 4096, 0, 8.192); 
   TH1D *hSiL_PD_deuteron = new TH1D("h_SiL_PD_deuteron", "SiL deuteron probability distribution; dE [MeV]", 4096, 0, 8.192); 
   TH1D *hSiL_PD_triton = new TH1D("h_SiL_PD_triton", "SiL triton probability distribution; dE [MeV]", 4096, 0, 8.192); 
   TH1D *hSiR_PD_proton = new TH1D("h_SiR_PD_proton", "SiR proton probability distribution; dE [MeV]", 4096, 0, 8.192); 
   TH1D *hSiR_PD_deuteron = new TH1D("h_SiR_PD_deuteron", "SiR deuteron probability distribution; dE [MeV]", 4096, 0, 8.192); 
   TH1D *hSiR_PD_triton = new TH1D("h_SiR_PD_triton", "SiR triton probability distribution; dE [MeV]", 4096, 0, 8.192); 
   hProbabilityDistribution["sil_proton"] = hSiL_PD_proton;
   hProbabilityDistribution["sil_deuteron"] = hSiL_PD_deuteron;
   hProbabilityDistribution["sil_triton"] = hSiL_PD_triton;
   hProbabilityDistribution["sir_proton"] = hSiR_PD_proton;
   hProbabilityDistribution["sir_deuteron"] = hSiR_PD_deuteron;
   hProbabilityDistribution["sir_triton"] = hSiR_PD_triton;

   vector<TH2D *> vSiL_time;
   vector<TH2D *> vSiL_energy;
   vector<TH2D *> vLLPL_proton;
   vector<TH2D *> vLLPL_deuteron;
   vector<TH2D *> vLLPL_triton;
   for(unsigned i=0; i<n_SiL1_channels; i++) {
      TH2D *h = new TH2D(Form("hSiL1_%d_tDiff", i+2), Form("hSiL1_%d tDiff; t[ns]; E+dE [MeV]", i+2), 1000, -10000, 10000, 4096, 0, 8.192);
      vSiL_time.push_back(h);
      h = new TH2D(Form("hSiL1_%d_EvdE", i+2), Form("SiL1_%d EvdE; E+dE [MeV]; dE [MeV]", i+2), 4096, 0, 24.576, 4096, 0, 8.192);
      vSiL_energy.push_back(h);
      h = new TH2D(Form("hSiL1_%d_LLP_proton", i+2), Form("SiL1_%d LLP proton; E+dE [MeV]; dE [MeV]", i+2), 4096, 0, 24.576, 4096, 0, 8.192);
      vLLPL_proton.push_back(h);
      h = new TH2D(Form("hSiL1_%d_LLP_deuteron", i+2), Form("SiL1_%d LLP deuteron; E+dE [MeV]; dE [MeV]", i+2), 4096, 0, 24.576, 4096, 0, 8.192);
      vLLPL_deuteron.push_back(h);
      h = new TH2D(Form("hSiL1_%d_LLP_triton", i+2), Form("SiL1_%d LLP triton; E+dE [MeV]; dE [MeV]", i+2), 4096, 0, 24.576, 4096, 0, 8.192);
      vLLPL_triton.push_back(h);
   }
   hSiL["time"] = vSiL_time;
   hSiL["energy"] = vSiL_energy;
   hSiL["llp_proton"] = vLLPL_proton;
   hSiL["llp_deuteron"] = vLLPL_deuteron;
   hSiL["llp_triton"] = vLLPL_triton;

   vector<TH2D *> vSiR_time;
   vector<TH2D *> vSiR_energy;
   vector<TH2D *> vLLPR_proton;
   vector<TH2D *> vLLPR_deuteron;
   vector<TH2D *> vLLPR_triton;
   for(unsigned i=0; i<n_SiR1_channels; i++) {
      TH2D *h = new TH2D(Form("hSiR1_%d_tDiff", i+1), Form("hSiR1_%d tDiff; t [ns]; E+dE [MeV]", i+1), 1000, -10000, 10000, 4096, 0, 8.192);
      vSiR_time.push_back(h);
      h = new TH2D(Form("hSiR1_%d_EvdE", i+1), Form("SiR1_%d EvdE; E+dE [MeV]; dE [MeV]", i+1), 4096, 0, 24.576, 4096, 0, 8.192);
      vSiR_energy.push_back(h);
      h = new TH2D(Form("hSiR1_%d_LLP_proton", i+1), Form("SiR1_%d LLP proton; E+dE [MeV]; dE [MeV]", i+1), 4096, 0, 24.576, 4096, 0, 8.192);
      vLLPR_proton.push_back(h);
      h = new TH2D(Form("hSiR1_%d_LLP_deuteron", i+1), Form("SiR1_%d LLP deuteron; E+dE [MeV]; dE [MeV]", i+1), 4096, 0, 24.576, 4096, 0, 8.192);
      vLLPR_deuteron.push_back(h);
      h = new TH2D(Form("hSiR1_%d_LLP_triton", i+1), Form("SiR1_%d LLP triton; E+dE [MeV]; dE [MeV]", i+1), 4096, 0, 24.576, 4096, 0, 8.192);
      vLLPR_triton.push_back(h);
   }
   hSiR["time"] = vSiR_time;
   hSiR["energy"] = vSiR_energy;
   hSiR["llp_proton"] = vLLPR_proton;
   hSiR["llp_deuteron"] = vLLPR_deuteron;
   hSiR["llp_triton"] = vLLPR_triton;

   vector<TH1D *> vSiT_time;
   vector<TH1D *> vSiT_energy;
   for(unsigned i=0; i<n_SiT_channels; i++) {
      TH1D *t = new TH1D(Form("hSiT%d_time", i+1), Form("hSiT%d time; t [ns]", i+1), 1000, -10000, 10000);
      TH1D *e = new TH1D(Form("hSiT%d_energy", i+1), Form("hSiT%d energy; E [MeV]", i+1), 4096, 0, 8.192);
      vSiT_time.push_back(t);
      vSiT_energy.push_back(e);
   }
   hSiT["time"] = vSiT_time;
   hSiT["energy"] = vSiT_energy;

   hGeLoGain["time"] = new TH1D("hGeLoGain_time", "GeLoGain time; t[ns]", 1000, -10000, 10000);
   hGeLoGain["energy"] = new TH1D("hGeLoGain_energy", "GeLoGain energy; E [MeV]", 4096, 0, 3.000);
   hGeHiGain["time"] = new TH1D("hGeHiGain_time", "GeHiGain time; t[ns]", 1000, -10000, 10000);
   hGeHiGain["energy"] = new TH1D("hGeHiGain_energy", "GeHiGain energy; E [MeV]", 4096, 0, 3.000);
}

Bool_t TMETree::Process(Long64_t entry)
{
   // The Process() function is called for each entry in the tree (or possibly
   // keyed object in the case of PROOF) to be processed. The entry argument
   // specifies which entry in the currently loaded tree is to be processed.
   // When processing keyed objects with PROOF, the object is already loaded
   // and is available via the fObject pointer.
   //
   // This function should contain the \"body\" of the analysis. It can contain
   // simple or elaborate selection criteria, run algorithms on the data
   // of the event and typically fill histograms.
   //
   // The processing can be stopped by calling Abort().
   //
   // Use fStatus to set the return value of TTree::Process().
   //
   // The return value is currently not used.

	fReader.SetEntry(entry);
	vector<SimplePulse> vSiL1[n_SiL1_channels] = {
		*SiL1_2, *SiL1_3, *SiL1_4, *SiL1_5, *SiL1_6,
		*SiL1_7, *SiL1_8, *SiL1_9, *SiL1_10, *SiL1_11,
		*SiL1_12, *SiL1_13, *SiL1_14, *SiL1_15
	};
	vector<SimplePulse> vSiR1[n_SiR1_channels] = {
		*SiR1_1, *SiR1_2, *SiR1_3, *SiR1_4
	};
	vector<SimplePulse> vSiT[n_SiT_channels] = {
		*SiT_1, *SiT_2, *SiT_3, *SiT_4
	};
Double_t probability_cut = 1.1;
	if(SiL3->size() ) {
		for(int i_chn=0 ; i_chn < n_SiL1_channels; i_chn++) {
			SiL1_E=0; SiL3_E=0; SiR1_E=0; SiR2_E=0; SiR3_E=0;
//			if(vSiL1[i_chn].size() && vSiR1[0].size()==0 && vSiR1[1].size()==0 && vSiR1[2].size()==0 && vSiR1[3].size()==0) {
			if(vSiL1[i_chn].size() ) {
				Double_t t1 = vSiL1[i_chn].at(0).tTME;
				Double_t t2 = SiL3->at(0).tTME;
				//require if there is a hit in SiL then no hit in SiR and vice versa
				//////
				if(fabs(t2-t1) < 200.0f) {
					//Double_t dE = vSiL1[i_chn].at(0).Amp * left_gradient_fix[i_chn] + left_offset_fix[i_chn];
					Double_t dE = vSiL1[i_chn].at(0).Amp * 2.056969 + 1.370678;
					Double_t E = SiL3->at(0).E;
					dE = dE *0.001;
					E = E *0.001;
					if(dE>0.3f && E>0.1f) {
						hSiL["time"].at(i_chn)->Fill(t2 - t1, E+dE);
						hSiL["energy"].at(i_chn)->Fill(E+dE, dE);
						hSiL_tDiff->Fill(t2 - t1);
						hSiL_EvdE->Fill(E+dE, dE);

						hProbabilityDistribution["sil_proton"]->Fill(LikelihoodProbabilityCut(dE, fitFunctions["left_proton"]->Eval(E+dE), 0.08511) );
						hProbabilityDistribution["sil_deuteron"]->Fill(LikelihoodProbabilityCut(dE, fitFunctions["left_deuteron"]->Eval(E+dE), 0.08511) );
						hProbabilityDistribution["sil_triton"]->Fill(LikelihoodProbabilityCut(dE, fitFunctions["left_triton"]->Eval(E+dE), 0.08511) );
						if(LikelihoodProbabilityCut(dE, fitFunctions["left_proton"]->Eval(E+dE), 0.08511) > probability_cut) {
							hSiL_EvdE_proton->Fill(E+dE, dE);
							hSiL["llp_proton"].at(i_chn)->Fill(E+dE, dE);
						}
						if(LikelihoodProbabilityCut(dE, fitFunctions["left_deuteron"]->Eval(E+dE), 0.08511) > probability_cut) {
							hSiL_EvdE_deuteron->Fill(E+dE, dE);
							hSiL["llp_deuteron"].at(i_chn)->Fill(E+dE, dE);
						}
						if(LikelihoodProbabilityCut(dE, fitFunctions["left_triton"]->Eval(E+dE), 0.08511) > probability_cut) {
							hSiL_EvdE_triton->Fill(E+dE, dE);
							hSiL["llp_triton"].at(i_chn)->Fill(E+dE, dE);
						}

						SiL1_E = dE; SiL3_E = E;
						tNNUnfolding->Fill();
					}
				}
			}
		}
	}
	if(SiR2->size() ) {
		for(int i_chn=0 ; i_chn < n_SiR1_channels; i_chn++) {
			SiL1_E=0; SiL3_E=0; SiR1_E=0; SiR2_E=0; SiR3_E=0;
//			if(vSiR1[i_chn].size() && vSiL1[0].size()==0 && vSiL1[1].size()==0 && vSiL1[2].size()==0 && vSiL1[3].size()==0 && vSiL1[4].size()==0 && vSiL1[5].size()==0 && vSiL1[6].size()==0 && vSiL1[7].size()==0 && vSiL1[8].size()==0 && vSiL1[9].size()==0 && vSiL1[10].size()==0 && vSiL1[11].size()==0 && vSiL1[12].size()==0 && vSiL1[13].size()==0) {
			if(vSiR1[i_chn].size() ) {
				Double_t t1 = vSiR1[i_chn].at(0).tTME;
				Double_t t2 = SiR2->at(0).tTME;
				if(fabs(t2-t1) < 200.0f) {
					Double_t dE = vSiR1[i_chn].at(0).E;
					Double_t E = SiR2->at(0).E;
					dE = dE *0.001;
					E = E *0.001;
					hSiR["time"].at(i_chn)->Fill(t2 - t1, E+dE);
					hSiR["energy"].at(i_chn)->Fill(E+dE, dE);
					hSiR_tDiff->Fill(t2 - t1);
					hSiR_EvdE->Fill(E+dE, dE);

					hProbabilityDistribution["sir_proton"]->Fill(LikelihoodProbabilityCut(dE, fitFunctions["right_proton"]->Eval(E+dE), 0.08511) );
					hProbabilityDistribution["sir_deuteron"]->Fill(LikelihoodProbabilityCut(dE, fitFunctions["right_deuteron"]->Eval(E+dE), 0.08511) );
					hProbabilityDistribution["sir_triton"]->Fill(LikelihoodProbabilityCut(dE, fitFunctions["right_triton"]->Eval(E+dE), 0.08511) );
					if(LikelihoodProbabilityCut(dE, fitFunctions["right_proton"]->Eval(E+dE), 0.08511) > probability_cut) {
						hSiR_EvdE_proton->Fill(E+dE, dE);
						hSiR["llp_proton"].at(i_chn)->Fill(E+dE, dE);
					}
					if(LikelihoodProbabilityCut(dE, fitFunctions["right_deuteron"]->Eval(E+dE), 0.08511) > probability_cut) {
						hSiR_EvdE_deuteron->Fill(E+dE, dE);
						hSiR["llp_deuteron"].at(i_chn)->Fill(E+dE, dE);
					}
					if(LikelihoodProbabilityCut(dE, fitFunctions["right_triton"]->Eval(E+dE), 0.08511) > probability_cut) {
						hSiR_EvdE_triton->Fill(E+dE, dE);
						hSiR["llp_triton"].at(i_chn)->Fill(E+dE, dE);
					}
					SiR1_E = dE; SiR2_E = E; if(SiR3->size()>0) SiR3_E = SiR3->at(0).E * 0.001;
					tNNUnfolding->Fill();
				}
			}
		}
	}
	for(int i_chn=0; i_chn < n_SiT_channels; i_chn++) {
		if(vSiT[i_chn].size() ) {
			Double_t E = vSiT[i_chn].at(0).E * 0.001;
			hSiT["time"].at(i_chn)->Fill(vSiT[i_chn].at(0).tTME);
			hSiT["energy"].at(i_chn)->Fill(E);
			hSiT_t->Fill(vSiT[i_chn].at(0).tTME);
			hSiT_E->Fill(E);
		}
	}
	if(GeLoGain->size() ) hGeLoGain["time"]->Fill(GeLoGain->at(0).tTME);
	if(GeLoGain->size() ) hGeLoGain["energy"]->Fill(GeLoGain->at(0).E * 0.001);
	if(GeHiGain->size() ) hGeHiGain["time"]->Fill(GeHiGain->at(0).tTME);
	if(GeHiGain->size() ) hGeHiGain["energy"]->Fill(GeHiGain->at(0).E * 0.001);
	return kTRUE;
}

void TMETree::SlaveTerminate()
{
   // The SlaveTerminate() function is called after all entries or objects
   // have been processed. When running with PROOF SlaveTerminate() is called
   // on each slave server.
   fOutput->Add(hSiL_tDiff);
   fOutput->Add(hSiR_tDiff);
   fOutput->Add(hSiL_EvdE);
   fOutput->Add(hSiR_EvdE);
   fOutput->Add(hSiT_t);
   fOutput->Add(hSiT_E);
   fOutput->Add(hSiL_EvdE_proton);
   fOutput->Add(hSiL_EvdE_deuteron);
   fOutput->Add(hSiL_EvdE_triton);
   fOutput->Add(hSiR_EvdE_proton);
   fOutput->Add(hSiR_EvdE_deuteron);
   fOutput->Add(hSiR_EvdE_triton);

   fOutput->Add(tNNUnfolding);

for(auto const& obj : hProbabilityDistribution) {
	fOutput->Add(obj.second);
}

   for(unsigned i=0; i<n_SiL1_channels; i++) {
      fOutput->Add(hSiL["time"].at(i) );
      fOutput->Add(hSiL["energy"].at(i) );
      fOutput->Add(hSiL["llp_proton"].at(i) );
      fOutput->Add(hSiL["llp_deuteron"].at(i) );
      fOutput->Add(hSiL["llp_triton"].at(i) );
   }
   for(unsigned i=0; i<n_SiR1_channels; i++) {
      fOutput->Add(hSiR["time"].at(i) );
      fOutput->Add(hSiR["energy"].at(i) );
      fOutput->Add(hSiR["llp_proton"].at(i) );
      fOutput->Add(hSiR["llp_deuteron"].at(i) );
      fOutput->Add(hSiR["llp_triton"].at(i) );
   }
   for(unsigned i=0; i<n_SiT_channels; i++) {
      fOutput->Add(hSiT["time"].at(i) );
      fOutput->Add(hSiT["energy"].at(i) );
   }
   fOutput->Add(hGeLoGain["time"]);
   fOutput->Add(hGeLoGain["energy"]);
   fOutput->Add(hGeHiGain["time"]);
   fOutput->Add(hGeHiGain["energy"]);
}

void TMETree::Terminate()
{
   // The Terminate() function is the last function to be called during
   // a query. It always runs on the client, it can be used to present
   // the results graphically or save the results to file.

	try {
//		TCanvas *cMain = new TCanvas("General", "General");
//		cMain->Divide(2,2);
//		cMain->cd(1);
//		fOutput->FindObject("hSiL_tDiff")->Draw();
//		cMain->cd(2);
//		fOutput->FindObject("hSiR_tDiff")->Draw();
//		cMain->cd(3);
//		fOutput->FindObject("hSiL_EvdE")->Draw("colz");
//		cMain->cd(4);
//		fOutput->FindObject("hSiR_EvdE")->Draw("colz");

		//Print everytihing in fOutput
		fOutputFile = new TFile("tproof-output.root", "RECREATE");
		for(TObject* obj: *fOutput) {
			Info("Terminate", "%s", obj->GetName() );
			gDirectory->Add(obj);
		}
		fOutputFile->Write();
	} catch (int e) {
		cerr << "Exception: " << e << endl;
	}
}
