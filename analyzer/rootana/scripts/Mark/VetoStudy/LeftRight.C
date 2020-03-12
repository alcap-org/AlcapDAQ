void Data() {
    TCutG *g = new TCutG("g", 9);
    g->SetPoint(0, 4067.52, 1270.17);
    g->SetPoint(1, 4120.51, 73.2179);
    g->SetPoint(2, 16467.3, 66.8511);
    g->SetPoint(3, 16202.4, 378.823);
    g->SetPoint(4, 14533.2, 410.657);
    g->SetPoint(5, 11380.2, 499.792);
    g->SetPoint(6, 8041.82, 671.695);
    g->SetPoint(7, 5206.82, 996.4);
    g->SetPoint(8, 4067.52, 1270.17);

    TFile *fData = new TFile("al50.root", "READ");
    TTree *tree = (TTree *)fData->Get("tree");

    Double_t e1, e2, e3, t1, t2, timeToPrevTME, timeToNextTME;
    TString *channel = new TString("");
    TString  *sig3 = new TString("");;
    TH1D *hL = new TH1D("hL", "MeasDataLeft;E [keV];E [keV]", 40, 0, 20000);
    TH1D *hR = new TH1D("hR", "MeasDataRight;E [keV];E [keV]", 40, 0, 20000);
    tree->SetBranchAddress("timeToPrevTME", &timeToPrevTME);
    tree->SetBranchAddress("timeToNextTME", &timeToNextTME);
    tree->SetBranchAddress("e1", &e1);
    tree->SetBranchAddress("e2", &e2);
    tree->SetBranchAddress("e3", &e3);
    tree->SetBranchAddress("t1", &t1);
    tree->SetBranchAddress("t2", &t2);
    tree->SetBranchAddress("channel", &channel);
    tree->SetBranchAddress("sig3", &sig3);
    for(Long64_t i=0; i < tree->GetEntries(); i++) {
        tree->GetEntry(i);
        e1 *=1e3; e2 *=1e3; e3*=1e3;
        if(timeToPrevTME < 10e3 || timeToNextTME < 10e3) continue;
        if(t2<500) continue;
        if(t2>10e3) continue;
        if(t2-t1 > 200 || t2-t1 < -200) continue;
        if(channel->Contains("SiL1") ) {
            if(g->IsInside(e1+e2, e1) ) {
                    hL->Fill(e1+e2);
            }
        } else if(channel->Contains("SiR1") ) {
            if(g->IsInside(e1+e2, e1) ) {
                    hR->Fill(e1+e2);
            }
        }
    }
    TCanvas *c = new TCanvas("c", "c");
    hL->Draw();
    hR->Draw("SAME");
    hR->SetLineColor(kRed);
    c->BuildLegend();
}
void MC() {
    TFile *f = new TFile("transfer.sf1.02.al50.hungerford.root", "READ");
    TTree *tree = (TTree *)f->Get("oTree");

    Double_t e1, e2, e3;
    TString *pid = new TString();
    TString *channel = new TString();

    tree->SetBranchAddress("e1", &e1);
    tree->SetBranchAddress("e2", &e2);
    tree->SetBranchAddress("e3", &e3);
    tree->SetBranchAddress("pid", &pid);
    tree->SetBranchAddress("channel", &channel);

    TH1D *hMCR = new TH1D("hMCR", "hMCR", 40, 0, 20000);
    TH1D *hMCL = new TH1D("hMCL", "hMCL", 40, 0, 20000);
    TH1D *hMCRpt = new TH1D("hMCRpt", "hMCR PT", 40, 0, 20000);
    TH1D *hMCLpt = new TH1D("hMCLpt", "hMCL PT", 40, 0, 20000);
    for(Int_t i=0; i < tree->GetEntries(); ++i) {
        tree->GetEntry(i);
	if(channel->Contains("SiR") ) {
		if(e3 == 0) {
			hMCR->Fill(e1+e2);
		} else {
			hMCRpt->Fill(e1+e2);
		}
	} else if(channel->Contains("SiL") ) {
		if(e3 == 0) {
			hMCL->Fill(e1+e2);
		} else {
			hMCLpt->Fill(e1+e2);
		}
	}
    }
    TCanvas *c2 = new TCanvas("c2", "c2");
    hMCL->Draw();
    hMCLpt->Draw("SAME");
    c2->BuildLegend();
    TCanvas *c3 = new TCanvas("c3", "c3");
    hMCR->Draw();
    hMCRpt->Draw("SAME");
    c3->BuildLegend();
}
void GetLeftRatios() {
    TFile *f = new TFile("transfer.sf1.02.al50.hungerford.root", "READ");
    TTree *tree = (TTree *)f->Get("oTree");

    Double_t e1, e2, e3;
    TString *pid = new TString();
    TString *channel = new TString();

    tree->SetBranchAddress("e1", &e1);
    tree->SetBranchAddress("e2", &e2);
    tree->SetBranchAddress("e3", &e3);
    tree->SetBranchAddress("pid", &pid);
    tree->SetBranchAddress("channel", &channel);

    TH1D *hMCL = new TH1D("hMCL", "hMCL", 40, 0, 20000);
    TH1D *hMCLpt = new TH1D("hMCLpt", "hMCL PT", 40, 0, 20000);
    for(Int_t i=0; i < tree->GetEntries(); ++i) {
        tree->GetEntry(i);
	if(channel->Contains("SiL") ) {
		if(e3 == 0) {
			hMCL->Fill(e1+e2);
		} else {
			hMCLpt->Fill(e1+e2);
		}
	}
    }
    hMCL->Divide(hMCLpt);
    TCanvas *c2 = new TCanvas("c2", "c2");
    hMCL->Draw();

}
void LeftRight() {
//	Data();
//	MC();
	GetLeftRatios();
}
