void SiLApp() {
    TCutG *g = new TCutG("g", 13);
    g->SetPoint(0, 1553.01, 1687.85);
    g->SetPoint(1, 2699.14, 957.299);
    g->SetPoint(2, 4504.3, 605.55);
    g->SetPoint(3, 8000, 399.912);
    g->SetPoint(4, 8000, 172.628);
    g->SetPoint(5, 14275.1, 172.628);
    g->SetPoint(6, 15421.2, 362.031);
    g->SetPoint(7, 13730.7, 459.438);
    g->SetPoint(8, 10607.4, 551.434);
    g->SetPoint(9, 7283.67, 724.603);
    g->SetPoint(10, 4103.15, 1103.41);
    g->SetPoint(11, 2011.46, 1996.31);
    g->SetPoint(12, 1553.01, 1687.85);


    Double_t SiLMCDerivedVeto[13] = {
	    3.7530864,
	    3.8750000,
	    3.2465753,
	    3.3500000,
	    2.6612903,
	    3.2884615,
	    3.6666667,
	    3.5227273,
	    7.5294118,
	    4.9090909,
	    10.250000,
	    6.0000000,
	    11.500000
    };

    TFile *fData = new TFile("al50.root", "READ");
    TTree *tree = (TTree *)fData->Get("tree");

    Double_t e1, e2, e3, t1, t2, timeToPrevTME, timeToNextTME;
    TString *channel = new TString("");
    TString  *sig3 = new TString("");;
    TH1D *hLMCDerivedVeto = new TH1D("LeftMCDerivedVeto", "MCDerivedVeto Data (SiL);E [keV];E [keV]", 40, 0, 20000);
    TH1D *hLNormal = new TH1D("LeftNormal", "Normal Data (SiL);E [keV];E [keV]", 40, 0, 20000);
    tree->SetBranchAddress("timeToPrevTME", &timeToPrevTME);
    tree->SetBranchAddress("timeToNextTME", &timeToNextTME);
    tree->SetBranchAddress("e1", &e1);
    tree->SetBranchAddress("e2", &e2);
    tree->SetBranchAddress("t1", &t1);
    tree->SetBranchAddress("t2", &t2);
    tree->SetBranchAddress("channel", &channel);
    tree->SetBranchAddress("sig3", &sig3);
    for(Long64_t i=0; i < tree->GetEntries(); ++i) {
        tree->GetEntry(i);
        e1 *=1e3; e2 *=1e3; e3*=1e3;
        if(timeToPrevTME < 10e3 || timeToNextTME < 10e3) continue;
        if(t2<500) continue;
        if(t2>10e3) continue;
        if(t2-t1 > 200 || t2-t1 < -200) continue;
        if(channel->Contains("SiL1") ) {
		if(g->IsInside(e1+e2, e1) ) {
			hLMCDerivedVeto->Fill(e1+e2);
		}
		if(sig3->Contains("proton") ) {
			hLNormal->Fill(e1+e2);
		}
	}
    }
    for(Int_t i=17; i < 30; ++i) {
	    std::cout << hLMCDerivedVeto->GetBinContent(i) << " " << SiLMCDerivedVeto[i-17] << std::endl;
	hLMCDerivedVeto->SetBinContent(i, hLMCDerivedVeto->GetBinContent(i) * SiLMCDerivedVeto[i-17] / (SiLMCDerivedVeto[i-17] + 1) );
    }
    TCanvas *c = new TCanvas("c", "c");
    hLNormal->Draw();
    hLMCDerivedVeto->Draw("SAMES");
    hLMCDerivedVeto->SetLineColor(kRed);

    for(Int_t i=17; i < 30; ++i) {
	    std::cout << i << " " <<  hLNormal->GetBinContent(i) - hLMCDerivedVeto->GetBinContent(i) << std::endl;
    }

    c->BuildLegend();
}
