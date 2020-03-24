void DrawUnfolded(std::string target) {
	const char *FigsDir = getenv("R15b_OUT");
	TFile *fData = new TFile(Form("%s/unfolded.%s.root", getenv("R15b_OUT"), target.c_str() ), "READ");
	TH1D *hpSiL = (TH1D *)fData->Get("hproton_SiL"); hpSiL->GetYaxis()->SetMaxDigits(3);
	TH1D *hpSiR = (TH1D *)fData->Get("hproton_SiR"); hpSiR->GetYaxis()->SetMaxDigits(3);
	TH1D *hdSiL = (TH1D *)fData->Get("hdeuteron_SiL"); hdSiL->GetYaxis()->SetMaxDigits(3);
	TH1D *hdSiR = (TH1D *)fData->Get("hdeuteron_SiR"); hdSiR->GetYaxis()->SetMaxDigits(3);
	TH1D *htSiL = (TH1D *)fData->Get("htriton_SiL"); htSiL->GetYaxis()->SetMaxDigits(3);
	TH1D *htSiR = (TH1D *)fData->Get("htriton_SiR"); htSiR->GetYaxis()->SetMaxDigits(3);
	TH1D *haSiL = (TH1D *)fData->Get("halpha_SiL"); haSiL->GetYaxis()->SetMaxDigits(3);
	TH1D *haSiR = (TH1D *)fData->Get("halpha_SiR"); haSiR->GetYaxis()->SetMaxDigits(3);

	target[0] = toupper(target[0]);
	TCanvas *l = new TCanvas("l", "l");
	l->SetLogy();
	hpSiL->Draw();       hpSiL->SetLineColor(kRed); hpSiL->SetTitle(";E[keV];Counts / captured #mu^{-} / 500 keV");
	hdSiL->Draw("SAME"); hdSiL->SetLineColor(kBlue);
	htSiL->Draw("SAME"); htSiL->SetLineColor(kGreen);
	haSiL->Draw("SAME"); haSiL->SetLineColor(kMagenta);
	TLegend *legend = new TLegend(0.645, 0.671, 0.857, 0.862);
	legend->SetHeader("#bf{AlCap} Unfolded");
	legend->AddEntry("", Form("%s#mum (Left 2#sigma)", target.c_str() ), "");
	legend->AddEntry(hpSiL,"proton","l");
	legend->AddEntry(hdSiL,"deuteron","l");
	legend->AddEntry(htSiL,"triton","l");
	legend->AddEntry(haSiL,"alpha","l");
	legend->Draw();
	l->SaveAs(Form("%s/AlCapData_%sDataset_Unfolded_LeftArm.pdf", FigsDir, target.c_str() ) );

	TCanvas *r = new TCanvas("r", "r");
	r->SetLogy();
	hpSiR->Draw();       hpSiR->SetLineColor(kRed); hpSiR->SetTitle(";E[keV];Counts / captured #mu^{-} / 500 keV");
	hdSiR->Draw("SAME"); hdSiR->SetLineColor(kBlue);
	htSiR->Draw("SAME"); htSiR->SetLineColor(kGreen);
	haSiR->Draw("SAME"); haSiR->SetLineColor(kMagenta);
	TLegend *legend1 = new TLegend(0.64, 0.66, 0.85, 0.85);
	legend1->SetHeader("#bf{AlCap} Unfolded");
	legend1->AddEntry("", Form("%s#mum (Right 2#sigma)", target.c_str() ), "");
	legend1->AddEntry(hpSiR,"proton","l");
	legend1->AddEntry(hdSiR,"deuteron","l");
	legend1->AddEntry(htSiR,"triton","l");
	legend1->AddEntry(haSiR,"alpha","l");
	legend1->Draw();
	r->SaveAs(Form("%s/AlCapData_%sDataset_Unfolded_RightArm.pdf", FigsDir, target.c_str() ) );
}
