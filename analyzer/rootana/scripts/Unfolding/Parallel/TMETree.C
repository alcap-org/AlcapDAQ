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
#include <TMath.h>
#include <algorithm>
#include <iterator>
#include "TSQLiteServer.h"
#include "TSQLiteResult.h"
#include "TSQLiteRow.h"

void TMETree::LoadCuts()
{
	//Electron cut
	TCutG *electronCut = new TCutG("electronCut", 5);
	electronCut->SetPoint(0, 0.0891429, -0.447298);
	electronCut->SetPoint(1, 0.766338, -0.0178962);
	electronCut->SetPoint(2, 1.2, -0.4);
	electronCut->SetPoint(3, 0.35, -1.25);
	electronCut->SetPoint(4, 0.0891429, -0.447298);

	TFile *fCuts = new TFile("/home/m-wong/tree/al100-cuts.root", "READ");
	TString option = GetOption();
	if(option.CompareTo("al50") == 0) {
		fCuts = new TFile("/home/m-wong/tree/al50-cuts.root", "READ");
	}
	TFile *fMuonCuts = new TFile("/home/m-wong/tree/muon-cuts.root", "READ");
	TFile *fAlphaCuts = new TFile("/home/m-wong/tree/alpha-cuts.root", "READ");
	for(int sigma=1; sigma < 5; ++sigma) {
		cutSiL["proton"][Form("%dsigma", sigma)] = (TCutG *)fCuts->Get(Form("hLg_SiL_EvDeltaE_proton_%dsigma", sigma) );
		cutSiL["deuteron"][Form("%dsigma", sigma)] = (TCutG *)fCuts->Get(Form("hLg_SiL_EvDeltaE_deuteron_%dsigma", sigma) );
		cutSiL["triton"][Form("%dsigma", sigma)] = (TCutG *)fCuts->Get(Form("hLg_SiL_EvDeltaE_triton_%dsigma", sigma) );
		cutSiL["alpha"][Form("%dsigma", sigma)] = (TCutG *)fAlphaCuts->Get(Form("hLg_SiL_EvDeltaE_alpha_%dsigma", sigma) );


		cutSiR["electron"][Form("%dsigma", sigma)] = electronCut;
		cutSiR["muon"][Form("%dsigma", sigma)] = (TCutG *)fMuonCuts->Get(Form("hLg_SiR_EvDeltaE_muon_%dsigma", sigma) );
		cutSiR["proton"][Form("%dsigma", sigma)] = (TCutG *)fCuts->Get(Form("hLg_SiR_EvDeltaE_proton_%dsigma", sigma) );
		cutSiR["deuteron"][Form("%dsigma", sigma)] = (TCutG *)fCuts->Get(Form("hLg_SiR_EvDeltaE_deuteron_%dsigma", sigma) );
		cutSiR["triton"][Form("%dsigma", sigma)] = (TCutG *)fCuts->Get(Form("hLg_SiR_EvDeltaE_triton_%dsigma", sigma) );
		cutSiR["alpha"][Form("%dsigma", sigma)] = (TCutG *)fAlphaCuts->Get(Form("hLg_SiR_EvDeltaE_alpha_%dsigma", sigma) );
	}
}
void TMETree::RunQuality()
{
	silver = {9410, 9411, 9412, 9518, 9553, 9607, 9656, 9657, 9659, 9679};
}
void TMETree::ResetValues() {
	tpi_id1 = -1;
	tpi_id2 = -1;
	tpi_id3 = -1;
	t1 = nan("nanl"); 
	t2 = nan("nanl");
	t3 = nan("nanl");
	e1 = nan("nanl");
	e2 = nan("nanl");
	e3 = nan("nanl");
	a1 = nan("nanl");
	a2 = nan("nanl");
	a3 = nan("nanl");
	channel = TString("");
	sig1 = TString("");
	sig2 = TString("");
	sig3 = TString("");
	sig4 = TString("");
}
TMETree::~TMETree() {
}
void TMETree::Begin(TTree * /*tree*/)
{
	// The Begin() function is called at the start of the query.
	// When running with PROOF Begin() is only called on the client.
	// The tree argument is deprecated (on PROOF 0 is passed).
	TString option = GetOption();
}

void TMETree::SlaveBegin(TTree * /*tree*/)
{
	// The SlaveBegin() function is called after the Begin() function.
	// When running with PROOF SlaveBegin() is called on each slave server.
	// The tree argument is deprecated (on PROOF 0 is passed).
	TString option = GetOption();
	fProofFile = new TProofOutputFile(Form("%s.root", option.Data() ), "M");
	TNamed *out = (TNamed *) fInput->FindObject("PROOF_OUTPUTFILE");
	if (out) fProofFile->SetOutputFileName(out->GetTitle());
	TDirectory *savedir = gDirectory;
	fFile = fProofFile->OpenFile("RECREATE");
	if (fFile && fFile->IsZombie()) SafeDelete(fFile);
	savedir->cd();

	tree = new TTree("tree", "tree");
	tree->Branch("timeToPrevTME", &nTimeToPrevTME);
	tree->Branch("timeToNextTME", &nTimeToNextTME);
	tree->Branch("runId", &nRunId);
	tree->Branch("blockId", &nBlockId);
	tree->Branch("tmeId", &nTMEId);
	tree->Branch("channel", &channel);
	tree->Branch("tpi_id1", &tpi_id1);
	tree->Branch("tpi_id2", &tpi_id2);
	tree->Branch("tpi_id3", &tpi_id3);
	tree->Branch("a1", &a1);
	tree->Branch("a2", &a2);
	tree->Branch("a3", &a3);
	tree->Branch("e1", &e1);
	tree->Branch("e2", &e2);
	tree->Branch("e3", &e3);
	tree->Branch("t1", &t1);
	tree->Branch("t2", &t2);
	tree->Branch("t3", &t3);
	tree->Branch("sig1", &sig1);
	tree->Branch("sig2", &sig2);
	tree->Branch("sig3", &sig3);
	tree->Branch("sig4", &sig4);
	tree->Branch("quality", &quality);
	tree->SetDirectory(fFile);
	tree->AutoSave();
	LoadCuts();
	RunQuality();

	//Ge 511keV gain shift
        TSQLiteServer *db = new TSQLiteServer("sqlite:///home/m-wong/tree/finetune.db");
        TSQLiteResult * result = (TSQLiteResult*)db->Query("SELECT * FROM FineGeCalib;");
        TSQLiteRow * row = (TSQLiteRow *) result->Next();
	Int_t run; std::string channel; Double_t gain;
        while (row != NULL) {
                run = atoi(row->GetField(0) );
                channel = row->GetField(1);
                gain = atof(row->GetField(2) );
                calib[run][channel] = gain;
                row = (TSQLiteRow*) result->Next();
        }

}	

Bool_t TMETree::Process(Long64_t entry)
{
	//Double_t SiL1TimeFineTuneMean = 8.16967e+02;
	//Double_t SiL3TimeFineTuneMean = 2.79161e+02;
	//Double_t SiR1TimeFineTuneMean = 2.81795e+02;
	//Double_t SiR2TimeFineTuneMean = 2.72388e+02;
	//Double_t SiR3TimeFineTuneMean = 3.11400e+02;
	Double_t SiL1TimeFineTuneMean = 0.;
	Double_t SiL3TimeFineTuneMean = 0.;
	Double_t SiR1TimeFineTuneMean = 0.;
	Double_t SiR2TimeFineTuneMean = 0.;
	Double_t SiR3TimeFineTuneMean = 0.;
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
	TString option = GetOption();
	fReader.SetEntry(entry);
	nRunId = *runId;
	nBlockId = *blockId;
	//assign silver runs
	quality = TString("");
	if(std::find(std::begin(silver), std::end(silver), nRunId) != std::end(silver) ) {
		quality = TString("s");
	}
	nTMEId = *TMEId;
	nTimeToPrevTME = *timeToPrevTME;
	nTimeToNextTME = *timeToNextTME;
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
	ResetValues();
	for(int i_chn=0 ; i_chn < n_SiL1_channels; i_chn++) {
		if(vSiL1[i_chn].size() ) {
			for(unsigned j=0; j < vSiL1[i_chn].size(); j++) {
				tpi_id1 = vSiL1[i_chn].at(j).tpi_id;
				t1 = vSiL1[i_chn].at(j).tTME - SiL1TimeFineTuneMean;
//				e1 = vSiL1[i_chn].at(j).E * 0.001; //al100
//				if(option.CompareTo("al50") == 0) {
				e1 = (vSiL1[i_chn].at(j).Amp * 2.056969 + 1.370678) * 0.001; //al50
//				}
				a1 = vSiL1[i_chn].at(j).Amp;
				if(SiL3->size() ) {
					for(unsigned k=0; k < SiL3->size(); k++) {
						tpi_id2 = SiL3->at(k).tpi_id;
						t2 = SiL3->at(k).tTME - SiL3TimeFineTuneMean;
						//e1 = (vSiL1[i_chn].at(j).Amp * 2.056969 + 1.370678) * 0.001; //al50
						e2 = SiL3->at(k).E * 0.001;
						a2 = SiL3->at(k).Amp;
						sig1 = TString("");
						sig2 = TString("");
						sig3 = TString("");
						sig4 = TString("");
						for(std::map<const char *, std::map<const char *, TCutG *> >::iterator it = cutSiL.begin(); it != cutSiL.end(); ++it) {
							for(std::map<const char *, TCutG *>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
								if(cutSiL[it->first][it2->first]->IsInside(0.7071 * (TMath::Log10(e1+e2) - TMath::Log10(e1) ), 0.7071 * (TMath::Log10(e1+e2) + TMath::Log10(e1) ) ) ) {
									if(strcmp(it2->first, "1sigma") == 0) sig1 += TString(it->first);
									if(strcmp(it2->first, "2sigma") == 0) sig2 += TString(it->first);
									if(strcmp(it2->first, "3sigma") == 0) sig3 += TString(it->first);
									if(strcmp(it2->first, "4sigma") == 0) sig4 += TString(it->first);
								}
							}
						}
						channel = TString(all_SiL1_channel_names[i_chn]);
						tree->Fill();
					}
				}
			}
		}
	}
	ResetValues();
	for(int i_chn=0 ; i_chn < n_SiR1_channels; i_chn++) {
		if(vSiR1[i_chn].size() ) {
			for(unsigned j=0; j < vSiR1[i_chn].size(); j++) {
				tpi_id1 = vSiR1[i_chn].at(j).tpi_id;
				t1 = vSiR1[i_chn].at(j).tTME - SiR1TimeFineTuneMean;
				e1 = vSiR1[i_chn].at(j).E * 0.001;
				a1 = vSiR1[i_chn].at(j).Amp;
				if(SiR2->size() ) {
					for(unsigned k=0; k < SiR2->size(); k++) {
						tpi_id2 = SiR2->at(k).tpi_id;
						t2 = SiR2->at(k).tTME - SiR2TimeFineTuneMean;
						e2 = SiR2->at(k).E * 0.001;
						a2 = SiR2->at(k).Amp;
						if(SiR3->size() ) {
							for(unsigned p=0; p < SiR3->size(); p++) {
								tpi_id3 = SiR3->at(p).tpi_id;
								t3 = SiR3->at(p).tTME - SiR3TimeFineTuneMean;
								e3 = SiR3->at(p).E * 0.001;
								a3 = SiR3->at(p).Amp;
							}
						}
						sig1 = TString("");
						sig2 = TString("");
						sig3 = TString("");
						sig4 = TString("");

						for(std::map<const char *, std::map<const char *, TCutG *> >::iterator it = cutSiR.begin(); it != cutSiR.end(); ++it) {
							for(std::map<const char *, TCutG *>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
								if(cutSiR[it->first][it2->first]->IsInside(0.7071 * (TMath::Log10(e1+e2) - TMath::Log10(e1) ), 0.7071 * (TMath::Log10(e1+e2) + TMath::Log10(e1) ) ) ) {
									if(strcmp(it2->first, "1sigma") == 0) sig1 += TString(it->first);
									if(strcmp(it2->first, "2sigma") == 0) sig2 += TString(it->first);
									if(strcmp(it2->first, "3sigma") == 0) sig3 += TString(it->first);
									if(strcmp(it2->first, "4sigma") == 0) sig4 += TString(it->first);
								}
							}
						}
						channel = TString(all_SiR1_channel_names[i_chn]);
						tree->Fill();
					}
				}
			}
		}
	}
	ResetValues();
	if(GeLoGain->size() ) {
		for(unsigned i=0; i<GeLoGain->size(); i++) {
			tpi_id1 = GeLoGain->at(i).tpi_id;
			a1 = GeLoGain->at(i).Amp;
			e1 = GeLoGain->at(i).Amp * calib[nRunId]["GeLoGain"] + 0.2387;
			t1 = GeLoGain->at(i).tTME;
			channel = TString("GeLoGain");
			tree->Fill();
		}
	}
	ResetValues();
	if(GeHiGain->size() ) {
		for(unsigned i=0; i<GeHiGain->size(); i++) {
			tpi_id1 = GeHiGain->at(i).tpi_id;
			a1 = GeHiGain->at(i).Amp;
			e1 = GeHiGain->at(i).Amp * calib[nRunId]["GeHiGain"] + 0.1238;
			t1 = GeHiGain->at(i).tTME;
			channel = TString("GeHiGain");
			tree->Fill();
		}
	}
	return kTRUE;
}

void TMETree::SlaveTerminate()
{
	// The SlaveTerminate() function is called after all entries or objects
	// have been processed. When running with PROOF SlaveTerminate() is called
	// on each slave server.
   if (fFile) {
      TDirectory *savedir = gDirectory;
      fFile->cd();
      tree->Write();
      gDirectory = savedir;
      fFile->Close();
      Info("SlaveTerminate", "objects saved into '%s%s': sending related TProofOutputFile ...", fProofFile->GetFileName(), fProofFile->GetOptionsAnchor());
      fProofFile->Print();
      fOutput->Add(fProofFile);
   }

}

void TMETree::Terminate()
{
	// The Terminate() function is the last function to be called during
	// a query. It always runs on the client, it can be used to present
	// the results graphically or save the results to file.
//	try {
//		//Print everything in fOutput
//		TString option = GetOption();
//		fOutputFile = new TFile(Form("%s.root", option.Data() ), "RECREATE");
//		for(TObject* obj: *fOutput) {
//			gDirectory->Add(obj);
//		}
//		fOutputFile->Write();
//	} catch (int e) {
//		cerr << "Exception: " << e << endl;
//	}
}
