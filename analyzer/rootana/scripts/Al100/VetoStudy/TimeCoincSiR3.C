void TimeCoincSiR3(const char * particle = "proton") {
	gStyle->SetOptFit(1);
	gStyle->SetOptStat(0);
	Int_t min = -300;
	Int_t max = 300;

	TFile *fData = new TFile("al50.root", "READ");
	Double_t t1, t2, t3, timeToPrevTME, timeToNextTME, e1, e2, e3;
	TString *channel = new TString("");
	TString *pt3= new TString("");
	TFile *fOutput = new TFile("tof_output.root", "UPDATE");
	TH1D *hMeasDataRight = new TH1D(Form("%sSiR3", particle), "Coincidence time;t_{3}-t_{1} [ns];Counts/10 ns", 60, min, max);
	TTree *tree = (TTree *)fData->Get("tree");
	tree->SetBranchAddress("t1", &t1);
	tree->SetBranchAddress("t2", &t2);
	tree->SetBranchAddress("t3", &t3);
	tree->SetBranchAddress("e1", &e1);
	tree->SetBranchAddress("e2", &e2);
	tree->SetBranchAddress("e3", &e3);
	tree->SetBranchAddress("timeToPrevTME", &timeToPrevTME);
	tree->SetBranchAddress("timeToNextTME", &timeToNextTME);
	tree->SetBranchAddress("channel", &channel);
	tree->SetBranchAddress("pt3", &pt3);
	for(Long64_t i=0; i < tree->GetEntries(); i++) {
		tree->GetEntry(i);
		if(!pt3->Contains(particle) ) continue;
		if(timeToPrevTME < 10e3 || timeToNextTME < 10e3) continue;
		if(abs(t2)>10e3) continue;
		if(t2<500) continue;
		if(TMath::IsNaN(e3) ) continue;
		if(channel->Contains("SiR1") ) {
			hMeasDataRight->Fill(t3-t1);
		}
	}
	hMeasDataRight->Draw("E1");
	hMeasDataRight->Fit("gaus");
	alcaphistogram(hMeasDataRight);
	alcaplabel("AlCap", hMeasDataRight);
}
