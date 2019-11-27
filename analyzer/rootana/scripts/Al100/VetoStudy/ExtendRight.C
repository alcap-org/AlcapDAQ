void Data() {
	TFile *fData = new TFile("al50.root", "READ");
	TTree *tree = (TTree *)fData->Get("tree");

	Double_t e1, e2, e3, t1, t2, t3, timeToPrevTME, timeToNextTME;
	TString *channel = new TString("");
	TString  *sig3 = new TString("");
	TString  *pt3 = new TString("");
	TH1D *hD1 = new TH1D("hD1", "SiR2 stops;E [keV];Counts / 0.5 MeV", 40, 0, 20);
	TH1D *hD2 = new TH1D("hD2", "SiR3 stops;E [keV];Counts / 0.5 MeV", 40, 0, 20);
	TH1D *hD3 = new TH1D("hD3", "SiR3 PT;E [keV]; Counts / 0.5 MeV", 40, 0, 20);
	tree->SetBranchAddress("timeToPrevTME", &timeToPrevTME);
	tree->SetBranchAddress("timeToNextTME", &timeToNextTME);
	tree->SetBranchAddress("e1", &e1);
	tree->SetBranchAddress("e2", &e2);
	tree->SetBranchAddress("e3", &e3);
	tree->SetBranchAddress("t1", &t1);
	tree->SetBranchAddress("t2", &t2);
	tree->SetBranchAddress("t3", &t3);
	tree->SetBranchAddress("channel", &channel);
	tree->SetBranchAddress("sig3", &sig3);
	tree->SetBranchAddress("pt3", &pt3);
	for(Long64_t i=0; i < tree->GetEntries(); i++) {
		tree->GetEntry(i);
		if(timeToPrevTME < 10e3 || timeToNextTME < 10e3) continue;
		if(t2<500) continue;
		if(abs(t2)>10e3) continue;
		if(t2-t1 > 200 || t2-t1 < -200) continue;
		if(channel->Contains("SiR1") ) {
			if(e2 > 17e3) continue;
			if(TMath::IsNaN(e3) ) {
				if(sig3->Contains("proton") ) {
					hD1->Fill(e1+e2);
				}
			} else {
				if(pt3->Contains("proton") ) {
					hD2->Fill(e1+e2+e3);
				}
			}
		}
	}
	hD2->Scale(1/0.77);
	hD1->Add(hD2);
	hD1->Draw();
	hD2->Draw("SAME");
	hD2->SetLineColor(kRed);
}
void ExtendRight() {
	gStyle->SetOptStat(0);
	TCanvas *c = new TCanvas("c", "c");
	Data();
	c->BuildLegend();
}
