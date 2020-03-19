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
	TFile *fCuts = new TFile(Form("%s/Parallel/al100-cuts.root", getenv("ANALYSIS_HOME") ), "READ");
	TFile *fSiR3Cuts = 0;
	TString option = GetOption();
	if(option.CompareTo("al50") == 0) {
		fCuts = new TFile(Form("%s/Parallel/al50-cuts.root", getenv("ANALYSIS_HOME") ), "READ");
		fSiR3Cuts = new TFile(Form("%s/Parallel/al50-sir3-cuts.root", getenv("ANALYSIS_HOME") ), "READ");
	} else if(option.CompareTo("si16b") == 0) {
		fCuts = new TFile(Form("%s/Parallel/si16b-cuts.root", getenv("ANALYSIS_HOME") ), "READ");
		fSiR3Cuts = new TFile(Form("%s/Parallel/si16b-sir3-cuts.root", getenv("ANALYSIS_HOME") ), "READ");
	} else if(option.CompareTo("ti50") == 0) {
		fCuts = new TFile(Form("%s/Parallel/ti50-cuts.root", getenv("ANALYSIS_HOME") ), "READ");
		fSiR3Cuts = new TFile(Form("%s/Parallel/ti50-sir3-cuts.root", getenv("ANALYSIS_HOME") ), "READ");
	}
	for(int sigma=1; sigma < 5; ++sigma) {
		const char * sigmaStr = Form("%dsigma", sigma);
		cutSiL["proton"][sigmaStr] = (TCutG *)fCuts->Get(Form("hLg_SiL_EvDeltaE_proton_%dsigma", sigma) );
		cutSiL["deuteron"][sigmaStr] = (TCutG *)fCuts->Get(Form("hLg_SiL_EvDeltaE_deuteron_%dsigma", sigma) );
		cutSiL["triton"][sigmaStr] = (TCutG *)fCuts->Get(Form("hLg_SiL_EvDeltaE_triton_%dsigma", sigma) );
		cutSiL["alpha"][sigmaStr] = (TCutG *)fCuts->Get(Form("hLg_SiL_EvDeltaE_alpha_%dsigma", sigma) );

		cutSiR["proton"][sigmaStr] = (TCutG *)fCuts->Get(Form("hLg_SiR_EvDeltaE_proton_%dsigma", sigma) );
		cutSiR["deuteron"][sigmaStr] = (TCutG *)fCuts->Get(Form("hLg_SiR_EvDeltaE_deuteron_%dsigma", sigma) );
		cutSiR["triton"][sigmaStr] = (TCutG *)fCuts->Get(Form("hLg_SiR_EvDeltaE_triton_%dsigma", sigma) );
		cutSiR["alpha"][sigmaStr] = (TCutG *)fCuts->Get(Form("hLg_SiR_EvDeltaE_alpha_%dsigma", sigma) );
	
		if(option.CompareTo("al100") != 0) {
			cutSiRPT["proton"][sigmaStr] = (TCutG *)fSiR3Cuts->Get(Form("sir3_hLg_SiR_EvDeltaE_proton_%dsigma", sigma) );
			cutSiRPT["extended"][sigmaStr] = (TCutG *)fSiR3Cuts->Get(Form("sir3_pt_hLg_SiR_EvDeltaE_proton_%dsigma", sigma) );
		}
	}
	//check
	for(std::map<const char *, std::map<const char *, TCutG *> >::iterator it = cutSiR.begin(); it != cutSiR.end(); ++it) {
		for(std::map<const char *, TCutG *>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
			std::cout << it->first << " " << it2->first << " " << it2->second->GetName() << std::endl;
		}
	}
		if(option.CompareTo("al50") == 0) {
			for(std::map<const char *, std::map<const char *, TCutG *> >::iterator it = cutSiRPT.begin(); it != cutSiRPT.end(); ++it) {
				for(std::map<const char *, TCutG *>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
					std::cout << it->first << " " << it2->first << " " << it2->second->GetName() << std::endl;
				}
			}
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
	pt1 = TString("");
	pt2 = TString("");
	pt3 = TString("");
	pt4 = TString("");
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
	tree->Branch("pt1", &pt1);
	tree->Branch("pt2", &pt2);
	tree->Branch("pt3", &pt3);
	tree->Branch("pt4", &pt4);
	tree->Branch("quality", &quality);
	tree->SetDirectory(fFile);
	tree->AutoSave();
	LoadCuts();
	RunQuality();

	//Ge 511keV gain shift
        TSQLiteServer *db = new TSQLiteServer(Form("sqlite://%s/finetune.db", getenv("PWD") ) );
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
		quality = TString("silver");
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
				e1 = vSiL1[i_chn].at(j).E * 0.001; //al100
				if(option.CompareTo("al50") == 0 || option.CompareTo("ti50") == 0) {
					e1 = (vSiL1[i_chn].at(j).Amp * 2.056969 + 1.370678) * 0.001; //al50
				}
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

						for(std::map<const char *, std::map<const char *, TCutG *> >::iterator it = cutSiR.begin(); it != cutSiR.end(); ++it) {
							for(std::map<const char *, TCutG *>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
								if((it2->second)->IsInside(0.7071 * (TMath::Log10(e1+e2) - TMath::Log10(e1) ), 0.7071 * (TMath::Log10(e1+e2) + TMath::Log10(e1) ) ) ) {
									if(strcmp(it2->first, "1sigma") == 0) sig1.Append(it->first);
									if(strcmp(it2->first, "2sigma") == 0) sig2.Append(it->first);
									if(strcmp(it2->first, "3sigma") == 0) sig3.Append(it->first);
									if(strcmp(it2->first, "4sigma") == 0) sig4.Append(it->first);
								}
							}
						}

						if(option.CompareTo("al100") != 0) {
							if(SiR3->size() ) {
								for(unsigned p=0; p < SiR3->size(); p++) {
									tpi_id3 = SiR3->at(p).tpi_id;
									t3 = SiR3->at(p).tTME - SiR3TimeFineTuneMean;
									e3 = SiR3->at(p).E * 0.001;
									a3 = SiR3->at(p).Amp;

									for(std::map<const char *, std::map<const char *, TCutG *> >::iterator _it = cutSiRPT.begin(); _it != cutSiRPT.end(); ++_it) {
										for(std::map<const char *, TCutG *>::iterator _it2 = _it->second.begin(); _it2 != _it->second.end(); ++_it2) {
											if((_it2->second)->IsInside(0.7071 * (TMath::Log10(e2+e3) - TMath::Log10(e2) ), 0.7071 * (TMath::Log10(e2+e3) + TMath::Log10(e2) ) ) ) {
												if(strcmp(_it2->first, "1sigma") == 0) pt1.Append(_it->first);
												if(strcmp(_it2->first, "2sigma") == 0) pt2.Append(_it->first);
												if(strcmp(_it2->first, "3sigma") == 0) pt3.Append(_it->first);
												if(strcmp(_it2->first, "4sigma") == 0) pt4.Append(_it->first);
											}
										}
									}
								}
							}
						}

						channel = TString(all_SiR1_channel_names[i_chn]);
						tree->Fill();
						sig1 = TString("");
						sig2 = TString("");
						sig3 = TString("");
						sig4 = TString("");
						pt1 = TString("");
						pt2 = TString("");
						pt3 = TString("");
						pt4 = TString("");
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
			if(calib[nRunId]["GeLoGain"]) {
				e1 = GeLoGain->at(i).Amp * calib[nRunId]["GeLoGain"] + 0.2387;
			} else {
				e1 = GeLoGain->at(i).E;
			}
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
			if(calib[nRunId]["GeHiGain"]) {
				e1 = GeHiGain->at(i).Amp * calib[nRunId]["GeHiGain"] + 0.1238;
			} else {
				e1 = GeHiGain->at(i).E;
			}
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
