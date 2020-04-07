void SiRApp() {
    TCutG *g = new TCutG("g", 13);
g->SetPoint(0, 1680.22, 1806.97);
g->SetPoint(1, 2991.36, 917.356);
g->SetPoint(2, 4748.85, 544.291);
g->SetPoint(3, 8000, 324.279);
g->SetPoint(4, 8000, 94.7009);
g->SetPoint(5, 16353.9, 104.267);
g->SetPoint(6, 16326, 372.108);
g->SetPoint(7, 13787.4, 429.502);
g->SetPoint(8, 10774.5, 506.028);
g->SetPoint(9, 7036.38, 754.738);
g->SetPoint(10, 3632.99, 1261.72);
g->SetPoint(11, 2321.84, 2189.6);
g->SetPoint(12, 1680.22, 1806.97);



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
    TH1D *hRMCDerivedVeto = new TH1D("RightMCDerivedVeto", "MCDerivedVeto Data (SiR);E [keV]", 40, 0, 20000);
    TH1D *hRNormal = new TH1D("RightNormal", "Normal Data (SiR);E [keV]", 40, 0, 20000);
TH1D *hPT = new TH1D("RightPT", "PT Data (SiR); E[keV]", 40, 0, 20000);
    tree->SetBranchAddress("timeToPrevTME", &timeToPrevTME);
    tree->SetBranchAddress("timeToNextTME", &timeToNextTME);
    tree->SetBranchAddress("e1", &e1);
    tree->SetBranchAddress("e2", &e2);
    tree->SetBranchAddress("e3", &e3);
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
        if(channel->Contains("SiR1") ) {
		if(g->IsInside(e1+e2, e1) ) {
			hRMCDerivedVeto->Fill(e1+e2);
			if(!TMath::IsNaN(e3) ) {
				hPT->Fill(e1+e2);
			}
		}
		if(sig3->Contains("proton") ) {
			hRNormal->Fill(e1+e2);
		}
	}
    }
//    for(Int_t i=17; i < 30; ++i) {
//	    std::cout << hLMCDerivedVeto->GetBinContent(i) << " " << SiLMCDerivedVeto[i-17] << std::endl;
//	hLMCDerivedVeto->SetBinContent(i, hLMCDerivedVeto->GetBinContent(i) * SiLMCDerivedVeto[i-17] / (SiLMCDerivedVeto[i-17] + 1) );
//    }
    TCanvas *c = new TCanvas("c", "c");
    hRNormal->Draw();
    hRMCDerivedVeto->Draw("SAMES");
    hRMCDerivedVeto->SetLineColor(kRed);
hPT->Draw("SAMES");
hPT->SetLineColor(kBlack);

//    for(Int_t i=17; i < 30; ++i) {
//	    std::cout << i << " " <<  hLNormal->GetBinContent(i) - hLMCDerivedVeto->GetBinContent(i) << std::endl;
//    }

    c->BuildLegend();
}
