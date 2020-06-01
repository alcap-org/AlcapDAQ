void ThinThick() {
	const char *FigsDir = getenv("R15b_OUT");
	TFile *fAl50 = new TFile(Form("%s/al50-systematics.root", getenv("R15b_OUT") ), "READ");
	TFile *fAl100 = new TFile(Form("%s/al100-systematics.root", getenv("R15b_OUT") ), "READ");
	TH1D *hpAl50SiL = (TH1D *)fAl50->Get("hproton_SiL_al50"); hpAl50SiL->GetYaxis()->SetMaxDigits(3); hpAl50SiL->SetMarkerStyle(kFullCircle); hpAl50SiL->SetMarkerColor(kRed); hpAl50SiL->SetLineColor(kRed);
	TH1D *hpAl50SiR = (TH1D *)fAl50->Get("hproton_SiR_al50"); hpAl50SiR->GetYaxis()->SetMaxDigits(3); hpAl50SiR->SetMarkerStyle(kFullCircle);  hpAl50SiR->SetMarkerColor(kBlue); hpAl50SiR->SetLineColor(kBlue);
	TH1D *hpAl100SiL = (TH1D *)fAl100->Get("hproton_SiL_al100"); hpAl100SiL->GetYaxis()->SetMaxDigits(3); hpAl100SiL->SetMarkerStyle(kOpenSquare); hpAl100SiL->SetMarkerColor(kBlack); hpAl100SiL->SetLineColor(kBlack);
	TH1D *hpAl100SiR = (TH1D *)fAl100->Get("hproton_SiR_al100"); hpAl100SiR->GetYaxis()->SetMaxDigits(3); hpAl100SiR->SetMarkerStyle(kOpenSquare); hpAl100SiR->SetMarkerColor(kOrange); hpAl100SiR->SetLineColor(kOrange);

	hpAl50SiL->Rebin(2);
	hpAl50SiR->Rebin(2);
	hpAl100SiL->Rebin(2);
	hpAl100SiR->Rebin(2);
	hpAl50SiR->GetXaxis()->SetRangeUser(4, 20);
	hpAl50SiL->GetXaxis()->SetRangeUser(4, 16);
	hpAl100SiL->GetXaxis()->SetRangeUser(4, 16);
	hpAl100SiR->GetXaxis()->SetRangeUser(4, 16);

	hpAl50SiR->SetTitle(";E [MeV];Protons / muons captured / MeV");

	TCanvas *c = new TCanvas("c", "c");
	hpAl50SiR->Draw("E1");
	hpAl50SiL->Draw("E1 SAME");
	hpAl100SiL->Draw("E1 SAME");
	hpAl100SiR->Draw("E1 SAME");

	TLegend *legend = new TLegend(0.65, 0.66, 0.86, 0.85);
	legend->AddEntry(hpAl50SiL, "50 #mum left","lep");
	legend->AddEntry(hpAl50SiR, "50 #mum right","lep");
	legend->AddEntry(hpAl100SiL, "100 #mum left","lep");
	legend->AddEntry(hpAl100SiR, "100 #mum right","lep");
	legend->Draw("SAME");

	c->SaveAs(Form("%s/AlCapData_ThickThinAl.png", getenv("R15b_OUT") ) );
}
