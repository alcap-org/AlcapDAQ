void DrawTimeCoin()
{
	TFile *fData = new TFile("al100.root", "READ");
	TH1D *hSiL = new TH1D("hSiL", "SiL", 100, -500, 500);
	TH1D *hSiR = new TH1D("hSiR", "SiR", 100, -500, 500);
	Double_t t1, t2, timeToPrevTME, timeToNextTME;
	TString *channel = new TString("");
	TTree *tree = (TTree *)fData->Get("tree");
	tree->SetBranchAddress("timeToPrevTME", &timeToPrevTME);
	tree->SetBranchAddress("timeToNextTME", &timeToNextTME);
	tree->SetBranchAddress("t1", &t1);
	tree->SetBranchAddress("t2", &t2);
	tree->SetBranchAddress("channel", &channel);
	for(Long64_t i=0; i < tree->GetEntries(); i++) {
		tree->GetEntry(i);
		if(timeToPrevTME<10e3 || timeToNextTME<10e3) continue;
		if(abs(t2)>10000) continue;
		if(channel->Contains("SiL") ) {
			hSiL->Fill(t2-t1);
		} else if(channel->Contains("SiR") ) {
			hSiR->Fill(t2-t1);
		}
	}
	TCanvas *c = new TCanvas("SiL", "SiL");
	hSiL->Draw();

	TCanvas *d = new TCanvas("SiR", "SiR");
	hSiR->Draw();
}
