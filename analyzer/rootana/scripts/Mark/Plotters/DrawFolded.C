void DrawFolded(std::string target) {
	TFile *fData = new TFile(Form("%s/%s.root", getenv("R15b_DATA"), target.c_str() ), "READ");
	target[0] = toupper(target[0]);
	TTree *tree = (TTree *)fData->Get("tree");
	Double_t t1, t2, t3, e1, e2, e3, timeToPrevTME, timeToNextTME;
	Int_t a1, a2, a3;
	TString *channel = new TString();
	TString *sig2 = new TString();
	tree->SetBranchAddress("timeToPrevTME", &timeToPrevTME);
	tree->SetBranchAddress("timeToNextTME", &timeToNextTME);
	tree->SetBranchAddress("a1", &a1);
	tree->SetBranchAddress("e1", &e1);
	tree->SetBranchAddress("t1", &t1);
	tree->SetBranchAddress("a2", &a2);
	tree->SetBranchAddress("e2", &e2);
	tree->SetBranchAddress("t2", &t2);
	tree->SetBranchAddress("a3", &a3);
	tree->SetBranchAddress("e3", &e3);
	tree->SetBranchAddress("t3", &t3);
	tree->SetBranchAddress("channel", &channel);
	tree->SetBranchAddress("sig2", &sig2);

	TFile *fOutput = new TFile(Form("%s/%s-output.root", getenv("R15b_OUT"), target.c_str() ), "RECREATE");
	TH1D *hpSiR = new TH1D("hpSiR", "Proton;E [MeV];Counts / 0.5 MeV", 40, 0, 20);
	TH1D *hdSiR = new TH1D("hdSiR", "Deuteron;E [MeV];Counts / 0.5 MeV", 40, 0, 20);
	TH1D *htSiR = new TH1D("htSiR", "Triton;E [MeV];Counts / 0.5 MeV", 40, 0, 20);
	TH1D *haSiR = new TH1D("haSiR", "Alpha;E [MeV];Counts / 0.5 MeV", 40, 0, 20);
	TH1D *hpSiL = new TH1D("hpSiL", "Proton;E [MeV];Counts / 0.5 MeV", 40, 0, 20);
	TH1D *hdSiL = new TH1D("hdSiL", "Deuteron;E [MeV];Counts / 0.5 MeV", 40, 0, 20);
	TH1D *htSiL = new TH1D("htSiL", "Triton;E [MeV];Counts / 0.5 MeV", 40, 0, 20);
	TH1D *haSiL = new TH1D("haSiL", "Alpha;E [MeV];Counts / 0.5 MeV", 40, 0, 20);
	for(Int_t i=0; i < tree->GetEntries(); ++i) {
		tree->GetEntry(i);
		if(timeToPrevTME < 10e3 || timeToNextTME < 10e3) continue;
		if(t2<500) continue;
		if(t2>10e3) continue;
		if(t2-t1-13 > 60 || t2-t1-13 < -60) continue;
		if(!TMath::IsNaN(e3) ) continue;
		if(a2>3980) continue;
		if(channel->Contains("SiR") ) {
			if(channel->Contains("SiR1_1") && a1>3850. ) continue;
			if(channel->Contains("SiR1_2") && a1>3972. ) continue;
			if(channel->Contains("SiR1_3") && a1>3956. ) continue;
			if(channel->Contains("SiR1_4") && a1>3986. ) continue;
			if(sig2->Contains("proton") ) {
				hpSiR->Fill(e1+e2);
			}
			if(sig2->Contains("deuteron") ) {
				hdSiR->Fill(e1+e2);
			}
			if(sig2->Contains("triton") ) {
				htSiR->Fill(e1+e2);
			}
			if(sig2->Contains("alpha") ) {
				haSiR->Fill(e1+e2);
			}
		} else if(channel->Contains("SiL") ) {
			if(sig2->Contains("proton") ) {
				hpSiL->Fill(e1+e2);
			}
			if(sig2->Contains("deuteron") ) {
				hdSiL->Fill(e1+e2);
			}
			if(sig2->Contains("triton") ) {
				htSiL->Fill(e1+e2);
			}
			if(sig2->Contains("alpha") ) {
				haSiL->Fill(e1+e2);
			}

		}
	}
	std::cout << "SiL\tSiR" << std::endl;
	std::cout << "p : " << hpSiL->GetEntries() << "\t " << hpSiR->GetEntries() << std::endl;
	std::cout << "d : " << hdSiL->GetEntries() << "\t " << hdSiR->GetEntries() << std::endl;
	std::cout << "t : " << htSiL->GetEntries() << "\t " << htSiR->GetEntries() << std::endl;
	std::cout << "a : " << haSiL->GetEntries() << "\t " << haSiR->GetEntries() << std::endl;
	const char *FigsDir = getenv("R15b_OUT");
	{
		TCanvas *c = new TCanvas("r", "r");
		c->SetLogy();
		hpSiR->Draw();       hpSiR->SetLineColor(kRed);
		hdSiR->Draw("SAME"); hdSiR->SetLineColor(kBlue);
		htSiR->Draw("SAME"); htSiR->SetLineColor(kGreen);
		haSiR->Draw("SAME"); haSiR->SetLineColor(kMagenta);
		TLegend *legend = new TLegend(0.65, 0.66, 0.86, 0.85);
		legend->SetHeader("#bf{AlCap} Folded");
		legend->AddEntry("", Form("%s#mum (Right 2#sigma)", target.c_str() ),"");
		legend->AddEntry(hpSiR, "proton","l");
		legend->AddEntry(hdSiR, "deuteron","l");
		legend->AddEntry(htSiR, "triton","l");
		legend->AddEntry(haSiR, "alpha","l");
		legend->Draw();
		c->SaveAs(Form("%s/AlCapData_%sDataset_Folded_RightArm.pdf", FigsDir, target.c_str() ) );
	}
	{
		TCanvas *c = new TCanvas("l", "l");
		c->SetLogy();
		hpSiL->Draw();       hpSiL->SetLineColor(kRed);
		hdSiL->Draw("SAME"); hdSiL->SetLineColor(kBlue);
		htSiL->Draw("SAME"); htSiL->SetLineColor(kGreen);
		haSiL->Draw("SAME"); haSiL->SetLineColor(kMagenta);
		TLegend *legend = new TLegend(0.65, 0.66, 0.86, 0.85);
		legend->SetHeader("#bf{AlCap} #it{Preliminary}");
		legend->AddEntry("", Form("%s#mum (Left 2#sigma)", target.c_str() ),"");
		legend->AddEntry(hpSiL, "proton","l");
		legend->AddEntry(hdSiL, "deuteron","l");
		legend->AddEntry(htSiL, "triton","l");
		legend->AddEntry(haSiL, "alpha","l");
		legend->Draw();
		c->SaveAs(Form("%s/AlCapData_%sDataset_Folded_LeftArm.pdf", FigsDir, target.c_str() ) );
	}
	fOutput->Write();
	fOutput->Close();
}
