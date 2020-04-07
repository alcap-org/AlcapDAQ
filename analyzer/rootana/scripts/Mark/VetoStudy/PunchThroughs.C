void Data() {
    TCutG *g = new TCutG("wpt", 12);
    g->SetPoint(0, 14841, 15122.3);
    g->SetPoint(1, 16259.3, 15626.6);
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

    TCutG *vic = new TCutG("vic", 8);
vic->SetPoint(0, 20385.4, 10079.9);
vic->SetPoint(1, 21501.9, 8855.76);
vic->SetPoint(2, 19094.4, 8246.19);
vic->SetPoint(3, 14060.3, 7162.5);
vic->SetPoint(4, 6330.95, 3493.29);
vic->SetPoint(5, 6356.03, 4690.34);
vic->SetPoint(6, 12520.1, 8699.17);
vic->SetPoint(7, 20385.4, 10079.9);



    TFile *fData = new TFile("al50.root", "READ");
    TTree *tree = (TTree *)fData->Get("tree");

    Double_t e1, e2, e3, t1, t2, t3, timeToPrevTME, timeToNextTME;
    TString *channel = new TString("");
    TString  *sig3 = new TString("");;
    TH1D *hD1 = new TH1D("wPT", "Data SiR wPT;E [keV];E [keV]", 80, 0, 40000);
    TH1D *hD2 = new TH1D("noPT", "Data SiR noPT;E [keV];E [keV]", 80, 0, 40000);
    TH1D *hD3 = new TH1D("vic", "Data SiR2-SiR3;E [keV];E [keV]", 80, 0, 40000);
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
        if(channel->Contains("SiR1") ) {
            if(g->IsInside(e3+e2, e2) ) {
                hD1->Fill(e2);
            }
            if(h->IsInside(e3+e2, e2) ) {
                hD2->Fill(e2);
            }
	    if(vic->IsInside(e3+e2, e2) ) {
		    hD3->Fill(38000 - (e3+e2+e1) );
	    }
        }
    }
    hD1->Draw();
    hD1->SetLineColor(kRed);
    hD2->Draw("SAME");
    hD2->SetLineColor(kBlue);
//vic->Draw("SAME");
//h->Draw("SAME");
//g->Draw("SAME");

//hD3->Draw("SAME");
//hD3->SetLineColor(kBlack);

//    TF1 *f = new TF1("fit", "gaus(0)+gaus(3)", 5000, 25000);
//    f->SetLineStyle(kDashed);
//    f->SetParameter(0, 4.45671e+01);
//    f->SetParameter(1, 1.18496e+04);
//    f->SetParameter(2, 2.65155e+03);
//    f->SetParameter(3, 1.11044e+02);
//    f->SetParameter(4, 1.78286e+04);
//    f->SetParameter(5, 1.48539e+03);
//    hD3->Fit("fit");
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

    TH1D *h1 = new TH1D("MC", "MC", 40, 0, 20000);
    for(Int_t i=0; i < tree->GetEntries(); ++i) {
        tree->GetEntry(i);
        if(channel->Contains("SiR") ) {
            if(e3 != 0) {
                h1->Fill(e3+e2);
            }
        }
    }
    h1->Draw("SAME");
    h1->Scale(1/h1->GetEntries() );
}
void PunchThroughs() {
    TCanvas *c = new TCanvas("c", "c");
    Data();
//    MC();
    c->BuildLegend();
}
