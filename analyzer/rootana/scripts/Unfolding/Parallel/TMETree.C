#define TMETree_cxx
#include "TMETree.h"
#include "TFile.h"
#include "TTree.h"
#include "AlCapConstants.h"
#include "TH2.h"
#include "TMath.h"

#include <iostream>

Double_t TMETree::LikelihoodProbabilityCut(Double_t X, Double_t mu, Double_t sigma) {
	Double_t Z = (X - mu)/sigma;
	return exp(-Z*Z/2) / ((sqrt(2 * 3.1415) ) * sigma);
}

void TMETree::SlaveBegin(TTree* /*tree*/) {
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

	std::vector<TH2D *> vSiL_time;
	std::vector<TH2D *> vSiL_energy;
	std::vector<TH2D *> vLLPL_proton;
	std::vector<TH2D *> vLLPL_deuteron;
	std::vector<TH2D *> vLLPL_triton;
	for(int i=0; i<n_SiL1_channels; i++) {
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

	std::vector<TH2D *> vSiR_time;
	std::vector<TH2D *> vSiR_energy;
	std::vector<TH2D *> vLLPR_proton;
	std::vector<TH2D *> vLLPR_deuteron;
	std::vector<TH2D *> vLLPR_triton;
	for(int i=0; i<n_SiR1_channels; i++) {
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

	std::vector<TH1D *> vSiT_time;
	std::vector<TH1D *> vSiT_energy;
	for(int i=0; i<n_SiT_channels; i++) {
		TH1D *t = new TH1D(Form("hSiT%d_time", i+1), Form("hSiT%d time; t [ns]", i+1), 1000, -10000, 10000);
		TH1D *e = new TH1D(Form("hSiT%d_energy", i+1), Form("hSiT%d energy; E [MeV]", i+1), 4096, 0, 10.24);
		vSiT_time.push_back(t);
		vSiT_energy.push_back(e);
	}
	hSiT["time"] = vSiT_time;
	hSiT["energy"] = vSiT_energy;
	hSiTCheck = new TH2D("hSiTCheck", "SiT Check; E [keV]; ADC value", 4096, 0, 10240, 4096, 0, 10240);

	hGeLoGain["time"] = new TH1D("hGeLoGain_time", "GeLoGain time; t[ns]", 1000, -10000, 10000);
	hGeLoGain["energy"] = new TH1D("hGeLoGain_energy", "GeLoGain energy; E [keV]", 16384, 0, 6200);
	hGeLoGain["prompt"] = new TH1D("hGeLoGain_energy_prompt", "GeLoGain prompt energy; E [keV]", 16384, 0, 6200);
	hGeLoGain["semiprompt"] = new TH1D("hGeLoGain_energy_semiprompt", "GeLoGain semiprompt energy; E [keV]", 16384, 0, 6200);
	hGeLoGain["delayed"] = new TH1D("hGeLoGain_energy_delayed", "GeLoGain delayed energy; E [keV]", 16384, 0, 6200);
	hGeHiGain["time"] = new TH1D("hGeHiGain_time", "GeHiGain time; t[ns]", 1000, -10000, 10000);
	hGeHiGain["energy"] = new TH1D("hGeHiGain_energy", "GeHiGain energy; E [keV]", 16384, 0, 2800);
	hGeHiGain["prompt"] = new TH1D("hGeHiGain_energy_prompt", "GeHiGain prompt energy; E [keV]", 16384, 0, 2800);
	hGeHiGain["semiprompt"] = new TH1D("hGeHiGain_energy_semiprompt", "GeHiGain semiprompt energy; E [keV]", 16384, 0, 2800);
	hGeHiGain["delayed"] = new TH1D("hGeHiGain_energy_delayed", "GeHiGain delayed energy; E [keV]", 16384, 0, 2800);

	hGeLoGainDrift511 = new TH2D("hGeLoGainDrift511", "GeLoGain Drift 511keV; Run Id; E [keV]", 300, 9400, 9699, 200, 490, 529);
	hGeHiGainDrift511 = new TH2D("hGeHiGainDrift511", "GeHiGain Drift 511keV; Run Id; E [keV]", 300, 9400, 9699, 200, 490, 529);
	hGeLoGainDrift1460 = new TH2D("hGeLoGainDrift1460", "GeLoGain Drift K-40 1460keV; Run Id; E [keV]", 300, 9400, 9699, 400, 1400, 1499);
	hGeHiGainDrift1460 = new TH2D("hGeHiGainDrift1460", "GeHiGain Drift K-40 1460keV; Run Id; E [keV]", 300, 9400, 9699, 400, 1400, 1499);

}

Bool_t TMETree::Process(Long64_t entry) {
	fChain->GetEntry(entry);
	if(entry % 100000 == 0) {
		Info("Process", "%llu / %llu", entry, fChain->GetEntries() );
	}
	if(anyDoubleCountedPulses) return kTRUE;
	std::vector<SimplePulse> vSiL1[n_SiL1_channels] = {
		*SiL1_2, *SiL1_3, *SiL1_4, *SiL1_5, *SiL1_6,
		*SiL1_7, *SiL1_8, *SiL1_9, *SiL1_10, *SiL1_11,
		*SiL1_12, *SiL1_13, *SiL1_14, *SiL1_15
	};
	std::vector<SimplePulse> vSiR1[n_SiR1_channels] = {
		*SiR1_1, *SiR1_2, *SiR1_3, *SiR1_4
	};
	std::vector<SimplePulse> vSiT[n_SiT_channels] = {
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
					hSiTCheck->Fill(vSiT[i_chn].at(0).E, vSiT[i_chn].at(0).Amp);
				}
			}
			Double_t E=0;
			Double_t tTME=0;
			if(GeLoGain->size() ) {
				E = GeLoGain->at(0).E;
				tTME = GeLoGain->at(0).tTME;
				hGeLoGain["time"]->Fill(GeLoGain->at(0).tTME);
				hGeLoGain["energy"]->Fill(E);
				if(E>490 && E<530)
					hGeLoGainDrift511->Fill(runId, E);
				if(tTME>=-200 && tTME<200)
					hGeLoGain["prompt"]->Fill(GeLoGain->at(0).E);
				if(tTME>=200 && tTME<4000)
					hGeLoGain["semiprompt"]->Fill(GeLoGain->at(0).E);
				if(tTME>=4000) {
					hGeLoGain["delayed"]->Fill(GeLoGain->at(0).E);
					if(E>1400 && E<1500)
						hGeLoGainDrift1460->Fill(runId, E);
				}
			}
			E=0;
			tTME=0;
			if(GeHiGain->size() ) {
				E = GeHiGain->at(0).E;
				tTME = GeHiGain->at(0).tTME;
				hGeHiGain["time"]->Fill(GeHiGain->at(0).tTME);
				hGeHiGain["energy"]->Fill(E);
				if(E>490 && E<530)
					hGeHiGainDrift511->Fill(runId, E);
				if(tTME>=-200 && tTME<200)
					hGeHiGain["prompt"]->Fill(GeHiGain->at(0).E);
				if(tTME>=200 && tTME<4000)
					hGeHiGain["semiprompt"]->Fill(GeHiGain->at(0).E);
				if(tTME>=4000) {
					hGeHiGain["delayed"]->Fill(GeHiGain->at(0).E);
					if(E>1400 && E<1500)
						hGeHiGainDrift1460->Fill(runId, E);
				}
			}
			return kTRUE;
		}

		void TMETree::SlaveTerminate() {
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

			for(std::map<const char *, TH1D *>::iterator it = hProbabilityDistribution.begin(); it != hProbabilityDistribution.end(); it++) {
				fOutput->Add(it->second);
			}

			for(int i=0; i<n_SiL1_channels; i++) {
				fOutput->Add(hSiL["time"].at(i) );
				fOutput->Add(hSiL["energy"].at(i) );
				fOutput->Add(hSiL["llp_proton"].at(i) );
				fOutput->Add(hSiL["llp_deuteron"].at(i) );
				fOutput->Add(hSiL["llp_triton"].at(i) );
			}
			for(int i=0; i<n_SiR1_channels; i++) {
				fOutput->Add(hSiR["time"].at(i) );
				fOutput->Add(hSiR["energy"].at(i) );
				fOutput->Add(hSiR["llp_proton"].at(i) );
				fOutput->Add(hSiR["llp_deuteron"].at(i) );
				fOutput->Add(hSiR["llp_triton"].at(i) );
			}
			for(int i=0; i<n_SiT_channels; i++) {
				fOutput->Add(hSiT["time"].at(i) );
				fOutput->Add(hSiT["energy"].at(i) );
			}
			fOutput->Add(hSiTCheck);

			for(std::map<const char *, TH1D *>::iterator it = hGeLoGain.begin(); it != hGeLoGain.end(); it++) {
				fOutput->Add(it->second);
			}
			for(std::map<const char *, TH1D *>::iterator it = hGeHiGain.begin(); it != hGeHiGain.end(); it++) {
				fOutput->Add(it->second);
			}

			fOutput->Add(hGeLoGainDrift511);
			fOutput->Add(hGeHiGainDrift511);
			fOutput->Add(hGeLoGainDrift1460);
			fOutput->Add(hGeHiGainDrift1460);
		}

		void TMETree::Terminate() {
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
				for(Int_t i=0; i < fOutput->GetSize(); i++) {
					gDirectory->Add(fOutput->At(i) );
				}
				fOutputFile->Write();
			} catch (int e) {
				cerr << "Exception: " << e << endl;
			}

		}
