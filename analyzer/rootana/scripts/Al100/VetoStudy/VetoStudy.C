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
    TH1D *hD1 = new TH1D("hD1", "Data SiR;E [keV];E [keV]", 40, 0, 20000);
    TH1D *hD2 = new TH1D("hD2", "MeasDataRight;E [keV];E [keV]", 40, 0, 20000);
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
        } else if(channel->Contains("SiR1") ) {
            if(g->IsInside(e1+e2, e1) ) {
                if(TMath::IsNaN(e3) ) {
                    hD1->Fill(e1+e2);
                } else {
                    hD2->Fill(e1+e2);
                }
            }
        }
    }
    TH1D *ratio = (TH1D*)hD1->Clone();
    ratio->SetTitle("data;E [keV]");
    ratio->Divide(hD2);
    ratio->Draw("E");
    ratio->SetLineColor(kRed);
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

    TH1D *h1 = new TH1D("h1", "MC", 40, 0, 20000);
    TH1D *h2 = new TH1D("h2", "h2", 40, 0, 20000);
    for(Int_t i=0; i < tree->GetEntries(); ++i) {
        tree->GetEntry(i);
        if(channel->Contains("SiR") ) {
            if(e3 == 0) {
                h1->Fill(e1+e2);
            } else {
                h2->Fill(e1+e2);
            }
        }
    }
    TH1D *ratio = (TH1D*)h1->Clone();
    ratio->SetTitle("mc;E [keV]");
    ratio->Divide(h2);
    ratio->Draw("ESAMES");
}
void VetoStudy() {
    gStyle->SetOptStat(0);
    TCanvas *c = new TCanvas("c", "c");
    Data();
    MC();
    c->BuildLegend();
}
