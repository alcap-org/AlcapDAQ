void DrawTimeCoin(std::string target) {
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
	TH1D *hpSiR = new TH1D("hpSiR", ";t_{2}-t_{1}[ns];Counts", 100, -250, 250);
	TH1D *hdSiR = new TH1D("hdSiR", ";t_{2}-t_{1}[ns];Counts", 100, -250, 250);
	TH1D *htSiR = new TH1D("htSiR", ";t_{2}-t_{1}[ns];Counts", 100, -250, 250);
	TH1D *haSiR = new TH1D("haSiR", ";t_{2}-t_{1}[ns];Counts", 100, -250, 250);
	TH1D *hpSiL = new TH1D("hpSiL", ";t_{2}-t_{1}[ns];Counts", 100, -250, 250);
	TH1D *hdSiL = new TH1D("hdSiL", ";t_{2}-t_{1}[ns];Counts", 100, -250, 250);
	TH1D *htSiL = new TH1D("htSiL", ";t_{2}-t_{1}[ns];Counts", 100, -250, 250);
	TH1D *haSiL = new TH1D("haSiL", ";t_{2}-t_{1}[ns];Counts", 100, -250, 250);
	for(Int_t i=0; i < tree->GetEntries(); ++i) {
		tree->GetEntry(i);
		if(timeToPrevTME < 10e3 || timeToNextTME < 10e3) continue;
		if(t2>10e3) continue;
		if(a2>3980) continue;
		if(channel->Contains("SiR") ) {
			if(channel->Contains("SiR1_1") && a1>3850. ) continue;
			if(channel->Contains("SiR1_2") && a1>3972. ) continue;
			if(channel->Contains("SiR1_3") && a1>3956. ) continue;
			if(channel->Contains("SiR1_4") && a1>3986. ) continue;
			if(sig2->Contains("proton") ) hpSiR->Fill(t2-t1);
			if(sig2->Contains("deuteron") ) hdSiR->Fill(t2-t1);
			if(sig2->Contains("triton") ) htSiR->Fill(t2-t1);
			if(sig2->Contains("alpha") ) haSiR->Fill(t2-t1);
		} else if(channel->Contains("SiL") ) {
			if(sig2->Contains("proton") ) hpSiL->Fill(t2-t1);
			if(sig2->Contains("deuteron") ) hdSiL->Fill(t2-t1);
			if(sig2->Contains("triton") ) htSiL->Fill(t2-t1);
			if(sig2->Contains("alpha") ) haSiL->Fill(t2-t1);
		}
	}

	const char *FigsDir = getenv("R15b_OUT");
	TCanvas *r = new TCanvas("r", "r");
	hpSiR->Draw();       hpSiR->SetLineColor(kRed);
	hdSiR->Draw("SAME"); hdSiR->SetLineColor(kBlue);
	htSiR->Draw("SAME"); htSiR->SetLineColor(kGreen);
	haSiR->Draw("SAME"); haSiR->SetLineColor(kMagenta);
	TLegend *legend = new TLegend(0.606017, 0.624204, 0.859599, 0.85138);
	legend->SetHeader("#bf{AlCap} Al 50#mum");
	//legend->AddEntry("", Form("%s#mum (Right 2#sigma)", target.c_str() ),"");
	legend->AddEntry(hpSiR, "proton","l");
	legend->AddEntry(hdSiR, "deuteron","l");
	legend->AddEntry(htSiR, "triton","l");
	legend->AddEntry(haSiR, "alpha","l");
	legend->Draw();
	r->SaveAs(Form("%s/AlCapData_%sDataset_TimeCoincidence_RightArm.pdf", FigsDir, target.c_str() ) );

	TCanvas *l = new TCanvas("l", "l");
	hpSiL->Draw();       hpSiL->SetLineColor(kRed);
	hdSiL->Draw("SAME"); hdSiL->SetLineColor(kBlue);
	htSiL->Draw("SAME"); htSiL->SetLineColor(kGreen);
	haSiL->Draw("SAME"); haSiL->SetLineColor(kMagenta);
	TLegend *legend1 = new TLegend(0.606017, 0.624204, 0.859599, 0.85138);
	legend1->SetHeader("#bf{AlCap} TOF");
	legend1->AddEntry("", Form("%s#mum (Left 2#sigma)", target.c_str() ),"");
	legend1->AddEntry(hpSiL, "proton","l");
	legend1->AddEntry(hdSiL, "deuteron","l");
	legend1->AddEntry(htSiL, "triton","l");
	legend1->AddEntry(haSiL, "alpha","l");
	legend1->Draw();
	l->SaveAs(Form("%s/AlCapData_%sDataset_TimeCoincidence_LeftArm.pdf", FigsDir, target.c_str() ) );
}
