void DrawTransferMatrix(std::string target, std::string particle) {
	const char *transferMatrixPath = getenv("R15b_TM");
	TFile *responseMatrixFile = 0;
	if(target.compare("al50") ==0) {
		responseMatrixFile = new TFile(Form("%s/transfer.sf1.02.al50.%s.root", transferMatrixPath, particle.c_str() ), "READ");
		if(particle.compare("proton") == 0) {
			responseMatrixFile = new TFile(Form("%s/transfer.sf1.02.al50.%s3.root", transferMatrixPath, particle.c_str() ), "READ");
		}
	} else if(target.compare("al100") ==0 ) {
		responseMatrixFile = new TFile(Form("%s/transfer.sf1.035.al100.%s.root", transferMatrixPath, particle.c_str() ), "READ");
	} else if(target.compare("ti50") ==0) {
		responseMatrixFile = new TFile(Form("%s/transfer.sf1.03.ti50.%s.root", transferMatrixPath, particle.c_str() ), "READ");
	}
	target[0] = toupper(target[0]);
	particle[0] = toupper(particle[0]);
	RooUnfoldResponse *L_TM = (RooUnfoldResponse *)responseMatrixFile->Get("SiL500_TM");
	RooUnfoldResponse *R_TM = (RooUnfoldResponse *)responseMatrixFile->Get("SiR500_TM");

	const char *FigsDir = getenv("R15b_OUT");
	TCanvas *l = new TCanvas("l", "l");
	TH2 *hl = L_TM->Hresponse();
	hl->GetYaxis()->SetMaxDigits(3);
	hl->GetXaxis()->SetMaxDigits(3);
	hl->SetTitle(";E_{meas}[MeV];E_{true}[MeV]");
	hl->Draw("col");
	TLegend *legend = new TLegend(0.600287, 0.218487, 0.810888, 0.407563);

	legend->SetHeader("#bf{AlCap} Transfer matrix");
	legend->AddEntry("", Form("%s#mum %s  (Left)", target.c_str(), particle.c_str() ),"");
	legend->AddEntry(hl, Form("Entries = %.0f", hl->GetEntries() ), "p");
	legend->Draw();
	l->SaveAs(Form("%s/AlCapMC_%s_TM_%s_Left.pdf", FigsDir, target.c_str(), particle.c_str()  ) );
	
	TCanvas *r = new TCanvas("r", "r");
	TH2 *hr = R_TM->Hresponse();
	hr->RebinX(1);
	hr->RebinY(1);
	hr->GetYaxis()->SetMaxDigits(3);
	hr->GetXaxis()->SetMaxDigits(3);
	hr->SetTitle(";E_{meas}[MeV];E_{true}[MeV]");
	hr->Draw("colz");
	TLegend *legend1 = new TLegend(0.600287, 0.218487, 0.810888, 0.407563);
	legend1->SetHeader("#bf{AlCap} Transfer matrix");
	legend1->AddEntry("", Form("%s#mum %s  (Right)", target.c_str(), particle.c_str() ),"");
	legend1->AddEntry(hl, Form("Entries = %.0f", hr->GetEntries() ), "p");
	//legend1->Draw();
	r->SaveAs(Form("%s/AlCapMC_%s_TM_%s_Right.pdf", FigsDir, target.c_str(), particle.c_str() ) );

	delete l;
	delete r;
}
