#define Muons_cxx
// The class definition in Muons.h has been generated automatically
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
// root> T->Process("Muons.C")
// root> T->Process("Muons.C","some options")
// root> T->Process("Muons.C+")
//

#include "Muons.h"
#include <TMath.h>
#include <algorithm>
#include <iterator>

void Muons::ResetValues() {
	tpi_id1 = -1;
	t1 = nan("nanl"); 
	e1 = nan("nanl");
	a1 = nan("nanl");
	channel = TString("");
}
Muons::~Muons() {
}
void Muons::Begin(TTree * /*tree*/)
{
	// The Begin() function is called at the start of the query.
	// When running with PROOF Begin() is only called on the client.
	// The tree argument is deprecated (on PROOF 0 is passed).
	TString option = GetOption();
}

void Muons::SlaveBegin(TTree * /*tree*/)
{
	// The SlaveBegin() function is called after the Begin() function.
	// When running with PROOF SlaveBegin() is called on each slave server.
	// The tree argument is deprecated (on PROOF 0 is passed).
	TString option = GetOption();
	fProofFile = new TProofOutputFile(Form("muons-%s.root", option.Data() ), "M");
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
	tree->Branch("a1", &a1);
	tree->Branch("e1", &e1);
	tree->Branch("t1", &t1);
	tree->SetDirectory(fFile);
	tree->AutoSave();

	hSiT1 = new TH1D("hSiT1", "Muons", 1000, 0, 10e3);
	hSiT2 = new TH1D("hSiT2", "Muons", 1000, 0, 10e3);
	hSiT3 = new TH1D("hSiT3", "Muons", 1000, 0, 10e3);
	hSiT4 = new TH1D("hSiT4", "Muons", 1000, 0, 10e3);
}	

Bool_t Muons::Process(Long64_t entry)
{
	TString option = GetOption();
	fReader.SetEntry(entry);
	nRunId = *runId;
	nBlockId = *blockId;
	//assign silver runs
	nTMEId = *TMEId;
	nTimeToPrevTME = *timeToPrevTME;
	nTimeToNextTME = *timeToNextTME;
	std::vector<SimplePulse> vSiT[n_SiT_channels] = {
		*SiT_1, *SiT_2, *SiT_3, *SiT_4
	};
	ResetValues();
	for(int i_chn=0; i_chn < n_SiT_channels; i_chn++) {
		if(vSiT[i_chn].size() ) {
			for(unsigned j=0; j < vSiT[i_chn].size(); ++j) {
				tpi_id1 = vSiT[i_chn].at(j).tpi_id;
				t1 = vSiT[i_chn].at(j).tTME;
				e1 = vSiT[i_chn].at(j).E;
				a1 = vSiT[i_chn].at(j).Amp;
				channel = TString(all_SiT_channel_names[i_chn]);
//				tree->Fill();
				if(i_chn == 0)
					hSiT1->Fill(e1);
				if(i_chn == 1)
					hSiT2->Fill(e1);
				if(i_chn == 2)
					hSiT3->Fill(e1);
				if(i_chn == 3)
					hSiT4->Fill(e1);
			}
		}
	}
	return kTRUE;
}

void Muons::SlaveTerminate()
{
	// The SlaveTerminate() function is called after all entries or objects
	// have been processed. When running with PROOF SlaveTerminate() is called
	// on each slave server.
	if (fFile) {
		TDirectory *savedir = gDirectory;
		fFile->cd();
		//      tree->Write();
		hSiT1->Write();
		hSiT2->Write();
		hSiT3->Write();
		hSiT4->Write();
		gDirectory = savedir;
		fFile->Close();
		Info("SlaveTerminate", "objects saved into '%s%s': sending related TProofOutputFile ...", fProofFile->GetFileName(), fProofFile->GetOptionsAnchor());
		fProofFile->Print();
		fOutput->Add(fProofFile);
	}

}

void Muons::Terminate()
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
