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
#include <TH1D.h>
#include <TH2D.h>
#include <TH3D.h>
#include <TMath.h>
#include "TSQLiteServer.h"
#include "TSQLiteResult.h"
#include "TSQLiteRow.h"
#include <algorithm>

void TMETree::LoadAl100Cuts()
{
}
void TMETree::LoadAl50Cuts()
{
   SiL_cutg = new TCutG("left", 53);
   SiL_cutg->SetPoint(0,-0.000973585,0.320611);
   SiL_cutg->SetPoint(1,0.175661,0.320611);
   SiL_cutg->SetPoint(2,0.175661,0.329771);
   SiL_cutg->SetPoint(3,0.300834,0.329771);
   SiL_cutg->SetPoint(4,0.300834,0.329771);
   SiL_cutg->SetPoint(5,0.396801,0.329771);
   SiL_cutg->SetPoint(6,0.396801,0.334351);
   SiL_cutg->SetPoint(7,0.476078,0.334351);
   SiL_cutg->SetPoint(8,0.476078,0.334351);
   SiL_cutg->SetPoint(9,0.542837,0.334351);
   SiL_cutg->SetPoint(10,0.542837,0.343511);
   SiL_cutg->SetPoint(11,0.601252,0.343511);
   SiL_cutg->SetPoint(12,0.601252,0.348092);
   SiL_cutg->SetPoint(13,0.652712,0.348092);
   SiL_cutg->SetPoint(14,0.652712,0.348092);
   SiL_cutg->SetPoint(15,0.698609,0.348092);
   SiL_cutg->SetPoint(16,0.698609,0.352672);
   SiL_cutg->SetPoint(17,0.740334,0.352672);
   SiL_cutg->SetPoint(18,0.740334,0.348092);
   SiL_cutg->SetPoint(19,0.777886,0.348092);
   SiL_cutg->SetPoint(20,0.777886,0.348092);
   SiL_cutg->SetPoint(21,0.812656,0.348092);
   SiL_cutg->SetPoint(22,0.812656,0.357252);
   SiL_cutg->SetPoint(23,0.844645,0.357252);
   SiL_cutg->SetPoint(24,0.844645,0.361832);
   SiL_cutg->SetPoint(25,0.873853,0.361832);
   SiL_cutg->SetPoint(26,0.873853,0.352672);
   SiL_cutg->SetPoint(27,0.90306,0.352672);
   SiL_cutg->SetPoint(28,0.90306,0.370992);
   SiL_cutg->SetPoint(29,0.928095,0.370992);
   SiL_cutg->SetPoint(30,0.928095,0.357252);
   SiL_cutg->SetPoint(31,0.953129,0.357252);
   SiL_cutg->SetPoint(32,0.953129,0.338931);
   SiL_cutg->SetPoint(33,0.976773,0.338931);
   SiL_cutg->SetPoint(34,0.976773,0.30687);
   SiL_cutg->SetPoint(35,0.999026,0.30687);
   SiL_cutg->SetPoint(36,0.999026,0.60916);
   SiL_cutg->SetPoint(37,0.976773,0.60916);
   SiL_cutg->SetPoint(38,0.976773,0.577099);
   SiL_cutg->SetPoint(39,0.953129,0.577099);
   SiL_cutg->SetPoint(40,0.953129,0.558779);
   SiL_cutg->SetPoint(41,0.928095,0.558779);
   SiL_cutg->SetPoint(42,0.928095,0.549618);
   SiL_cutg->SetPoint(43,0.90306,0.549618);
   SiL_cutg->SetPoint(44,0.90306,0.549618);
   SiL_cutg->SetPoint(45,0.873853,0.549618);
   SiL_cutg->SetPoint(46,0.873853,0.540458);
   SiL_cutg->SetPoint(47,0.844645,0.540458);
   SiL_cutg->SetPoint(48,0.844645,0.535878);
   SiL_cutg->SetPoint(49,0.812656,0.535878);
   SiL_cutg->SetPoint(50,0.812656,0.535878);
   SiL_cutg->SetPoint(51,0.777886,0.535878);
   SiL_cutg->SetPoint(52,0.777886,0.526718);
   SiL_cutg->SetPoint(53,0.740334,0.526718);
   SiL_cutg->SetPoint(54,0.740334,0.508397);
   SiL_cutg->SetPoint(55,0.698609,0.508397);
   SiL_cutg->SetPoint(56,0.698609,0.508397);
   SiL_cutg->SetPoint(57,0.652712,0.508397);
   SiL_cutg->SetPoint(58,0.652712,0.499237);
   SiL_cutg->SetPoint(59,0.601252,0.499237);
   SiL_cutg->SetPoint(60,0.601252,0.490076);
   SiL_cutg->SetPoint(61,0.542837,0.490076);
   SiL_cutg->SetPoint(62,0.542837,0.485496);
   SiL_cutg->SetPoint(63,0.476078,0.485496);
   SiL_cutg->SetPoint(64,0.476078,0.471756);
   SiL_cutg->SetPoint(65,0.396801,0.471756);
   SiL_cutg->SetPoint(66,0.396801,0.462595);
   SiL_cutg->SetPoint(67,0.300834,0.462595);
   SiL_cutg->SetPoint(68,0.300834,0.458015);
   SiL_cutg->SetPoint(69,0.175661,0.458015);
   SiL_cutg->SetPoint(70,0.175661,0.462595);
   SiL_cutg->SetPoint(71,-0.000973585,0.462595);
   SiL_cutg->SetPoint(72,-0.000973585,0.320611);

   SiR_cutg = new TCutG("right",53);
   SiR_cutg->SetPoint(0,0,0.325777);
   SiR_cutg->SetPoint(1,0.176091,0.325777);
   SiR_cutg->SetPoint(2,0.176091,0.31409);
   SiR_cutg->SetPoint(3,0.30103,0.31409);
   SiR_cutg->SetPoint(4,0.30103,0.316238);
   SiR_cutg->SetPoint(5,0.39794,0.316238);
   SiR_cutg->SetPoint(6,0.39794,0.314057);
   SiR_cutg->SetPoint(7,0.477121,0.314057);
   SiR_cutg->SetPoint(8,0.477121,0.313126);
   SiR_cutg->SetPoint(9,0.544068,0.313126);
   SiR_cutg->SetPoint(10,0.544068,0.317628);
   SiR_cutg->SetPoint(11,0.60206,0.317628);
   SiR_cutg->SetPoint(12,0.60206,0.313673);
   SiR_cutg->SetPoint(13,0.653213,0.313673);
   SiR_cutg->SetPoint(14,0.653213,0.306342);
   SiR_cutg->SetPoint(15,0.69897,0.306342);
   SiR_cutg->SetPoint(16,0.69897,0.306031);
   SiR_cutg->SetPoint(17,0.740363,0.306031);
   SiR_cutg->SetPoint(18,0.740363,0.306819);
   SiR_cutg->SetPoint(19,0.778151,0.306819);
   SiR_cutg->SetPoint(20,0.778151,0.301657);
   SiR_cutg->SetPoint(21,0.812913,0.301657);
   SiR_cutg->SetPoint(22,0.812913,0.299958);
   SiR_cutg->SetPoint(23,0.845098,0.299958);
   SiR_cutg->SetPoint(24,0.845098,0.299003);
   SiR_cutg->SetPoint(25,0.875061,0.299003);
   SiR_cutg->SetPoint(26,0.875061,0.291806);
   SiR_cutg->SetPoint(27,0.90309,0.291806);
   SiR_cutg->SetPoint(28,0.90309,0.291006);
   SiR_cutg->SetPoint(29,0.929419,0.291006);
   SiR_cutg->SetPoint(30,0.929419,0.283458);
   SiR_cutg->SetPoint(31,0.954243,0.283458);
   SiR_cutg->SetPoint(32,0.954243,0.28613);
   SiR_cutg->SetPoint(33,0.977724,0.28613);
   SiR_cutg->SetPoint(34,0.977724,0.292055);
   SiR_cutg->SetPoint(35,1,0.292055);
   SiR_cutg->SetPoint(36,1,0.545801);
   SiR_cutg->SetPoint(37,0.977724,0.545801);
   SiR_cutg->SetPoint(38,0.977724,0.546931);
   SiR_cutg->SetPoint(39,0.954243,0.546931);
   SiR_cutg->SetPoint(40,0.954243,0.551506);
   SiR_cutg->SetPoint(41,0.929419,0.551506);
   SiR_cutg->SetPoint(42,0.929419,0.540567);
   SiR_cutg->SetPoint(43,0.90309,0.540567);
   SiR_cutg->SetPoint(44,0.90309,0.539397);
   SiR_cutg->SetPoint(45,0.875061,0.539397);
   SiR_cutg->SetPoint(46,0.875061,0.537827);
   SiR_cutg->SetPoint(47,0.845098,0.537827);
   SiR_cutg->SetPoint(48,0.845098,0.523948);
   SiR_cutg->SetPoint(49,0.812913,0.523948);
   SiR_cutg->SetPoint(50,0.812913,0.524472);
   SiR_cutg->SetPoint(51,0.778151,0.524472);
   SiR_cutg->SetPoint(52,0.778151,0.514817);
   SiR_cutg->SetPoint(53,0.740363,0.514817);
   SiR_cutg->SetPoint(54,0.740363,0.505916);
   SiR_cutg->SetPoint(55,0.69897,0.505916);
   SiR_cutg->SetPoint(56,0.69897,0.507663);
   SiR_cutg->SetPoint(57,0.653213,0.507663);
   SiR_cutg->SetPoint(58,0.653213,0.498447);
   SiR_cutg->SetPoint(59,0.60206,0.498447);
   SiR_cutg->SetPoint(60,0.60206,0.490391);
   SiR_cutg->SetPoint(61,0.544068,0.490391);
   SiR_cutg->SetPoint(62,0.544068,0.488872);
   SiR_cutg->SetPoint(63,0.477121,0.488872);
   SiR_cutg->SetPoint(64,0.477121,0.483566);
   SiR_cutg->SetPoint(65,0.39794,0.483566);
   SiR_cutg->SetPoint(66,0.39794,0.480657);
   SiR_cutg->SetPoint(67,0.30103,0.480657);
   SiR_cutg->SetPoint(68,0.30103,0.490433);
   SiR_cutg->SetPoint(69,0.176091,0.490433);
   SiR_cutg->SetPoint(70,0.176091,0.511148);
   SiR_cutg->SetPoint(71,0,0.511148);
   SiR_cutg->SetPoint(72,0,0.325777);
}

void TMETree::Begin(TTree * /*tree*/)
{
	// The Begin() function is called at the start of the query.
	// When running with PROOF Begin() is only called on the client.
	// The tree argument is deprecated (on PROOF 0 is passed).
	TString option = GetOption();
	Info("Begin", "Output filename: %s", option.Data() );
}

void TMETree::SlaveBegin(TTree * /*tree*/)
{
	Int_t run;
	std::string channel;
	Double_t mean;
	TSQLiteServer *db = new TSQLiteServer("sqlite:///home/m-wong/alcap-analysis/calibration.sqlite");
	TSQLiteResult * result = (TSQLiteResult*)db->Query("SELECT * FROM CoarseTimeOffset");
	TSQLiteRow * row = (TSQLiteRow *) result->Next();
	while (row != NULL) {
		run = atoi(row->GetField(0) );
		channel = row->GetField(1);
		std::replace(channel.begin(), channel.end(), '-', '_');
		channel = channel.substr(0, channel.length()-2);
		mean = atof(row->GetField(2) );
		calib[run][channel] = mean;
		row = (TSQLiteRow*) result->Next();
	}
	// The SlaveBegin() function is called after the Begin() function.
	// When running with PROOF SlaveBegin() is called on each slave server.
	// The tree argument is deprecated (on PROOF 0 is passed).
	//int min=9410;
	//int max=9682;
	int min=9890;
        int max=10127;
	
	h1D["runId"] = new TH1D("hRunId", "RunId", max-min+1, min, max+1);
	h1D["runId-wPP"] = new TH1D("hRunIdwPP", "RunId", max-min+1, min, max+1);
	h3D["SiL"] = new TH3D("hSiL", "SiL;E+#DeltaE [MeV];#DeltaE [MeV];t [ns]", 100, 0, 24., 100, 0, 8., 100, -20000, 20000);
	h3D["SiR"] = new TH3D("hSiR", "SiR;E+#DeltaE [MeV];#DeltaE [MeV];t [ns]", 100, 0, 24., 100, 0, 8., 100, -20000, 20000);
	h1D["SiL_tDiff"] = new TH1D("hSiL_tDiff", "SiL t_{diff}; t [ns]", 2000, -20000, 20000);
	h1D["SiR_tDiff"] = new TH1D("hSiR_tDiff", "SiR t_{diff}; t [ns]", 2000, -20000, 20000);
	h1D["SiL3_tTME"] = new TH1D("hSiL3_tTME", "SiL tTME;t [ns]", 2000, -10000, 10000);
	h1D["SiR2_tTME"] = new TH1D("hSiR2_tTME", "SiR tTME;t [ns]", 2000, -10000, 10000);
	h1D["Lg-SiL-proton-TME"] = new TH1D("hLg_SiL_proton_TME", "SiL proton time;t [ns]", 2000, -10000, 10000);
	h1D["Lg-SiR-proton-TME"] = new TH1D("hLg_SiR_proton_TME", "SiR proton time;t [ns]", 2000, -10000, 10000);
	h1D["Lg-SiL-proton-E"] = new TH1D("hLg_SiL_proton_E", "SiL proton energy;t [ns]", 20, 0, 10);
	h1D["Lg-SiR-proton-E"] = new TH1D("hLg_SiR_proton_E", "SiR proton energy;t [ns]", 20, 0, 10);

	h2D["SiL_EvdE"] = new TH2D("hSiL_EvDeltaE", "SiL Ev#DeltaE;E+#DeltaE [MeV];#DeltaE [MeV]", 4096, 0, 24.576, 4096, 0, 8.192);
	h2D["SiR_EvdE"] = new TH2D("hSiR_EvDeltaE", "SiR Ev#DeltaE;E+#DeltaE [MeV];#DeltaE [MeV]", 4096, 0, 24.576, 4096, 0, 8.192);
	h2D["Lg-SiL_EvdE"] = new TH2D("hLg_SiL_EvDeltaE", "SiL Ev#DeltaE;Lg E+#DeltaE / #sqrt{2} [MeV];Lg #DeltaE / #sqrt{2} [MeV]", 500, -0.1, 1.5, 500, -0.9, 1.5);
	h2D["Lg-SiR_EvdE"] = new TH2D("hLg_SiR_EvDeltaE", "SiR Ev#DeltaE;Lg E+#DeltaE / #sqrt{2} [MeV];Lg #DeltaE / #sqrt{2} [MeV]", 500, -0.1, 1.5, 500, -0.9, 1.5);
	h2D["SiL_EvdE-wPP"] = new TH2D("hSiL_EvDeltaE_wPP", "SiL Ev#DeltaE with PP;E+#DeltaE [MeV];#DeltaE [MeV]", 4096, 0, 24.576, 4096, 0, 8.192);
	h2D["SiR_EvdE-wPP"] = new TH2D("hSiR_EvDeltaE_wPP", "SiR Ev#DeltaE with PP;E+#DeltaE [MeV];#DeltaE [MeV]", 4096, 0, 24.576, 4096, 0, 8.192);
	h2D["SiL_EvdE-wPP-gt400ns"] = new TH2D("hSiL_EvDeltaE_wPP_gt400ns", "SiL Ev#DeltaE with PP & 400ns time cut;E+#DeltaE [MeV];#DeltaE [MeV]", 4096, 0, 24.576, 4096, 0, 8.192);
	h2D["SiR_EvdE-wPP-gt400ns"] = new TH2D("hSiR_EvDeltaE_wPP_gt400ns", "SiR Ev#DeltaE with PP & 400ns time cut;E+#DeltaE [MeV];#DeltaE [MeV]", 4096, 0, 24.576, 4096, 0, 8.192);
	h2D["Lg-SiL_EvdE-wPP-gt400ns"] = new TH2D("hLg_SiL_EvDeltaE_wPP_gt400ns", "SiL Ev#DeltaE;Lg E+#DeltaE / #sqrt{2} [MeV];Lg #DeltaE / #sqrt{2} [MeV]", 500, -0.1, 1.5, 500, -0.9, 1.5);
	h2D["Lg-SiR_EvdE-wPP-gt400ns"] = new TH2D("hLg_SiR_EvDeltaE_wPP_gt400ns", "SiR Ev#DeltaE;Lg E+#DeltaE / #sqrt{2} [MeV];Lg #DeltaE / #sqrt{2} [MeV]", 500, -0.1, 1.5, 500, -0.9, 1.5);
	h2D["SiL_EvdE_proton"] = new TH2D("hSiL_EvDeltaE_proton", "SiL Ev#DeltaE proton;E+#DeltaE [MeV];#DeltaE [MeV]", 4096, 0, 24.576, 4096, 0, 8.192);
	h2D["SiR_EvdE_proton"] = new TH2D("hSiR_EvDeltaE_proton", "SiR Ev#DeltaE proton;E+#DeltaE [MeV];#DeltaE [MeV]", 4096, 0, 24.576, 4096, 0, 8.192);

	h2D["SiT"] = new TH2D("hSiT", "SiT;t [ns];E [keV]", 2000, -20000, 20000, 4096, 0, 4096);
	h2D["GeLoGain"] = new TH2D("hGeLoGain", "GeLoGain;t [ns];E [keV];", 2000, -20000, 20000, 6200, 0, 6200);
	h2D["GeHiGain"] = new TH2D("hGeHiGain", "GeHiGain;t [ns];E [keV];", 2000, -20000, 20000, 2800, 0, 2800);
	h2D["SiT-wPP"] = new TH2D("hSiTwPP", "SiT;t [ns];E [keV]", 2000, -20000, 20000, 4096, 0, 4096);
	h2D["GeLoGain-wPP"] = new TH2D("hGeLoGainwPP", "GeLoGain;t [ns];E [keV];", 2000, -20000, 20000, 6200, 0, 6200);
	h2D["GeHiGain-wPP"] = new TH2D("hGeHiGainwPP", "GeHiGain;t [ns];E [keV];", 2000, -20000, 20000, 2800, 0, 2800);

	LoadAl50Cuts();
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
	h1D["runId"]->Fill(*runId);
	if(*timeToPrevTME > 10e3 && *timeToNextTME > 10e3) {
		h1D["runId-wPP"]->Fill(*runId);
	}
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
	Double_t t1 = 0; 
	Double_t t2 = 0;
	Double_t dE = 0;
	Double_t E=0;
	if(SiL3->size() ) {
		for(int i_chn=0 ; i_chn < n_SiL1_channels; i_chn++) {
			if(vSiL1[i_chn].size() ) {
				for(unsigned j=0; j < vSiL1[i_chn].size(); j++) {
					for(unsigned k=0; k < SiL3->size(); k++) {
//						t1 = vSiL1[i_chn].at(j).tTME + calib[*runId][all_SiL1_channel_names[i_chn] ]; //al100
						t1 = vSiL1[i_chn].at(j).tTME; //al50
						t2 = SiL3->at(k).tTME;
//						dE = vSiL1[i_chn].at(j).E * 0.001; //al100
						dE = (vSiL1[i_chn].at(j).Amp * 2.056969 + 1.370678) * 0.001; //al50
						E = SiL3->at(k).E * 0.001;
						h1D["SiL_tDiff"]->Fill(t2 - t1);
						h1D["SiL3_tTME"]->Fill(t2);
						h2D["SiL_EvdE"]->Fill(E+dE, dE);
						h2D["Lg-SiL_EvdE"]->Fill(0.7071 * (TMath::Log10(E+dE) - TMath::Log10(dE) ), 0.7071 * (TMath::Log10(E+dE) + TMath::Log10(dE) ) );
						if(t2-t1 > -1000 && t2-t1 < 1000) {
							h2D["SiL_EvdE-wPP"]->Fill(E+dE, dE);
							if(t2>400) {
								h2D["SiL_EvdE-wPP-gt400ns"]->Fill(E+dE, dE);
								h2D["Lg-SiL_EvdE-wPP-gt400ns"]->Fill(0.7071 * (TMath::Log10(E+dE) - TMath::Log10(dE) ), 0.7071 * (TMath::Log10(E+dE) + TMath::Log10(dE) ) );
							}
							//3 sigmas
							if(SiL_cutg->IsInside(0.7071 * (TMath::Log10(E+dE) - TMath::Log10(dE) ), 0.7071 * (TMath::Log10(E+dE) + TMath::Log10(dE) ) ) ) {
								h1D["Lg-SiL-proton-TME"]->Fill(t2);
								h2D["SiL_EvdE_proton"]->Fill(E+dE, dE);
								h1D["Lg-SiL-proton-E"]->Fill(E+dE);

							}
						}
						h3D["SiL"]->Fill(E+dE, dE, t2-t1);						
					}
				}
			}
		}
	}
	t1 = 0; 
	t2 = 0;
	dE = 0;
	E = 0;
	if(SiR2->size() ) {
		for(int i_chn=0 ; i_chn < n_SiR1_channels; i_chn++) {
			if(vSiR1[i_chn].size() ) {
				for(unsigned j=0; j < vSiR1[i_chn].size(); j++) {
					for(unsigned k=0; k < SiR2->size(); k++) {
						t1 = vSiR1[i_chn].at(j).tTME;
						t2 = SiR2->at(k).tTME;
						dE = vSiR1[i_chn].at(j).E * 0.001;
						E = SiR2->at(k).E * 0.001;
						h1D["SiR_tDiff"]->Fill(t2 - t1);
						h1D["SiR2_tTME"]->Fill(t2);
						h2D["SiR_EvdE"]->Fill(E+dE, dE);
						h2D["Lg-SiR_EvdE"]->Fill(0.7071 * (TMath::Log10(E+dE) - TMath::Log10(dE) ), 0.7071 * (TMath::Log10(E+dE) + TMath::Log10(dE) ) );
						if(t2-t1 > -1000 && t2-t1 < 1000) {
							h2D["SiR_EvdE-wPP"]->Fill(E+dE, dE);
							if(t2>400) {
								h2D["SiR_EvdE-wPP-gt400ns"]->Fill(E+dE, dE);
								h2D["Lg-SiR_EvdE-wPP-gt400ns"]->Fill(0.7071 * (TMath::Log10(E+dE) - TMath::Log10(dE) ), 0.7071 * (TMath::Log10(E+dE) + TMath::Log10(dE) ) );
							}
							//3 sigmas
							if(SiR_cutg->IsInside(0.7071 * (TMath::Log10(E+dE) - TMath::Log10(dE) ), 0.7071 * (TMath::Log10(E+dE) + TMath::Log10(dE) ) ) ) {
								h1D["Lg-SiR-proton-TME"]->Fill(t2);
								h2D["SiR_EvdE_proton"]->Fill(E+dE, dE);
								h1D["Lg-SiR-proton-E"]->Fill(E+dE);
							}
						}
						h3D["SiR"]->Fill(E+dE, dE, t2-t1);	
					}
				}
			}
		}
	}
	E=0;
	Double_t tTME=0;
	for(int i_chn=0; i_chn < n_SiT_channels; i_chn++) {
		if(vSiT[i_chn].size() ) {
			for(unsigned i=0; i < vSiT[i_chn].size(); i++) {
				E = vSiT[i_chn].at(i).E;
				tTME = vSiT[i_chn].at(i).tTME;
				h2D["SiT"]->Fill(tTME, E);
				if(*timeToPrevTME > 10e3 && *timeToNextTME > 10e3) {
					h2D["SiT-wPP"]->Fill(tTME, E);
				}
			}
		}
	}
	E=0;
	tTME=0;
	if(GeLoGain->size() ) {
		for(unsigned i=0; i<GeLoGain->size(); i++) {
			E = GeLoGain->at(i).E;
			tTME = GeLoGain->at(i).tTME;
			h2D["GeLoGain"]->Fill(tTME, E);
			if(*timeToPrevTME > 10e3 && *timeToNextTME > 10e3) {
				h2D["GeLoGain-wPP"]->Fill(tTME, E);
			}
		}
	}
	E=0;
	tTME=0;
	if(GeHiGain->size() ) {
		for(unsigned i=0; i<GeHiGain->size(); i++) {
			E = GeHiGain->at(i).E;
			tTME = GeHiGain->at(i).tTME;
			h2D["GeHiGain"]->Fill(tTME, E);
			if(*timeToPrevTME > 10e3 && *timeToNextTME > 10e3) {
				h2D["GeHiGain-wPP"]->Fill(tTME, E);
			}
		}
	}
	return kTRUE;
}

void TMETree::SlaveTerminate()
{
	// The SlaveTerminate() function is called after all entries or objects
	// have been processed. When running with PROOF SlaveTerminate() is called
	// on each slave server.
	for(std::map<const char *, TH1D *>::iterator it = h1D.begin(); it != h1D.end(); it++) {
		fOutput->Add(it->second);
	}
	for(std::map<const char *, TH2D *>::iterator it = h2D.begin(); it != h2D.end(); it++) {
		fOutput->Add(it->second);
	}
	for(std::map<const char *, TH3D *>::iterator it = h3D.begin(); it != h3D.end(); it++) {
		fOutput->Add(it->second);
	}
}

void TMETree::Terminate()
{
	// The Terminate() function is the last function to be called during
	// a query. It always runs on the client, it can be used to present
	// the results graphically or save the results to file.
	try {
		//Print everytihing in fOutput
		TString option = GetOption();
		fOutputFile = new TFile(option.Data(), "RECREATE");
		for(TObject* obj: *fOutput) {
			gDirectory->Add(obj);
		}
		fOutputFile->Write();
	} catch (int e) {
		cerr << "Exception: " << e << endl;
	}
}
