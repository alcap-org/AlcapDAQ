void Data() {
    TCutG *g = new TCutG("wpt", 5);
    g->SetPoint(0, 9500, 75);
    g->SetPoint(1, 10500, 75);
    g->SetPoint(2, 10500, 275);
    g->SetPoint(3, 9500, 275);
    g->SetPoint(4, 9500, 75);

    TCutG *h = new TCutG("wpt2", 5);
    h->SetPoint(0, 18000, 9500);
    h->SetPoint(1, 22000, 9500);
    h->SetPoint(2, 22000, 10500);
    h->SetPoint(3, 18000, 10500);
    h->SetPoint(4, 18000, 9500);

    TFile *fData = new TFile("al50.root", "READ");
    TTree *tree = (TTree *)fData->Get("tree");

    Double_t e1, e2, e3, t1, t2, t3, timeToPrevTME, timeToNextTME;
    TString *channel = new TString("");
    TString  *sig3 = new TString("");
    TH2D *_hD1 = new TH2D("_hD1", "SiR1-SiR2;E [keV];E [keV]", 40, 0, 20000, 300, 0, 3000);
    TH2D *_hD2 = new TH2D("_hD2", "SiR2-SiR3;E [keV];E [keV]", 60, 0, 30000, 2000, 0, 20000);
    TH2D *hD1 = new TH2D("hD1", "SiR1-SiR2;E [keV];E [keV]", 40, 0, 20000, 300, 0, 3000);
    TH2D *hD2 = new TH2D("hD2", "SiR2-SiR3;E [keV];E [keV]", 60, 0, 30000, 2000, 0, 20000);
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
    for(Long64_t i=0; i < tree->GetEntries(); i++) {
        tree->GetEntry(i);
        e1 *=1e3; e2 *=1e3; e3*=1e3;
        if(timeToPrevTME < 10e3 || timeToNextTME < 10e3) continue;
        if(t2<500) continue;
        if(t2>10e3) continue;
        if(t2-t1 > 200 || t2-t1 < -200) continue;
        if(t3-t2 > 200 || t3-t2 < -200) continue;
        if(channel->Contains("SiR") ) {
		_hD1->Fill(e1+e2, e1);
		if(g->IsInside(e1+e2, e1) ) {
			hD1->Fill(e1+e2, e1);
		}
		_hD2->Fill(e3+e2, e2);
		if(h->IsInside(e3+e2, e2) ) {
			hD2->Fill(e3+e2, e2);
		}
        }
    }
    TCanvas *c = new TCanvas("c", "c", 1000, 500);
    c->Divide(2, 1);
    c->cd(1);
    hD1->Draw();
    hD1->SetMarkerColor(kRed);
    g->SetLineColor(kRed);
    g->Draw("SAME");
    _hD1->Draw("SAME");
    c->cd(2);
    hD2->Draw();
    hD2->SetMarkerColor(kRed);
    h->SetLineColor(kRed);
    h->Draw("SAME");
    _hD2->Draw("SAME");
}
void CountCheck() {
    Data();
}
