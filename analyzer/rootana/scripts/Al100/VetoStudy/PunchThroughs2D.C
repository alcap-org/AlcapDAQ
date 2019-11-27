void Data() {
    TCutG *g = new TCutG("wpt", 12);
    g->SetPoint(0, 15000, 13763.2);
    g->SetPoint(1, 16511.7, 15493.4);
    g->SetPoint(2, 17562.3, 12919.6);
    g->SetPoint(3, 21501.9, 8855.76);
    g->SetPoint(4, 19094.4, 8246.19);
    g->SetPoint(5, 14060.3, 7162.5);
    g->SetPoint(6, 6330.95, 3493.29);
    g->SetPoint(7, 6356.03, 4690.34);
    g->SetPoint(8, 13053.5, 8415.51);
    g->SetPoint(9, 18131.3, 9736.26);
    g->SetPoint(10, 15329.8, 12919.6);
    g->SetPoint(11, 15000, 13763.2);

    TCutG *h = new TCutG("nopt", 8);
    h->SetPoint(0, 15000, 13763.2);
    h->SetPoint(1, 16128.2, 14763.2);
    h->SetPoint(2, 17471.3, 13078.9);
    h->SetPoint(3, 21501.9, 8855.76);
    h->SetPoint(4, 19889, 8236.84);
    h->SetPoint(5, 18131.3, 9736.26);
    h->SetPoint(6, 15329.8, 12919.6);
    h->SetPoint(7, 15000, 13763.2);

    TFile *fData = new TFile("al50.root", "READ");
    TTree *tree = (TTree *)fData->Get("tree");

    Double_t e1, e2, e3, t1, t2, t3, timeToPrevTME, timeToNextTME;
    TString *channel = new TString("");
    TString  *sig3 = new TString("");;
 //   TH2D *hD1 = new TH2D("hD1", "MeasDataRight;E [keV];E [keV]", 30, 15000, 30000, 500, 6000, 14000);
 TH2D *hD1 = new TH2D("hLg_SiR_EvDeltaE", "SiR Ev#DeltaE;Lg E+#DeltaE / #sqrt{2} [MeV];Lg #DeltaE / #sqrt{2} [MeV]", 250, -0.1, 0.29, 100, 0.9, 2);
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
//        e1 *=1e3; e2 *=1e3; e3*=1e3;
        if(timeToPrevTME < 10e3 || timeToNextTME < 10e3) continue;
        if(t2<500) continue;
        if(abs(t2)>10e3) continue;
        if(t3-t2 > 200 || t3-t2 < -200) continue;
	if(TMath::IsNaN(e3) ) continue;
        if(channel->Contains("SiL") ) {
        } else if(channel->Contains("SiR") ) {
//                hD1->Fill(e3+e2, e2);
		hD1->Fill(0.7071 * (TMath::Log10(e2+e3) - TMath::Log10(e2) ), 0.7071 * (TMath::Log10(e2+e3) + TMath::Log10(e2) ) );
        }
    }
    hD1->Draw();
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

    TH2D *h1 = new TH2D("MC", "h1", 40, 0, 30000, 500, 0, 20000);
    for(Int_t i=0; i < tree->GetEntries(); ++i) {
        tree->GetEntry(i);
        if(channel->Contains("SiR") ) {
            if(e3 != 0) {
                h1->Fill(e3+e2, e2);
            }
        }
    }
    h1->Draw("SAME");
    h1->SetMarkerColor(kBlue);
}
void PunchThroughs2D() {
    gStyle->SetOptStat(0);
    TCanvas *c = new TCanvas("c", "c");
    Data();
//    MC();
    c->BuildLegend();
}
