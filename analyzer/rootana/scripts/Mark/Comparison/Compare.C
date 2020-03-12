void Compare() {
	gStyle->SetOptStat(0);
	Double_t twist_protons_x[19] = {3442.5,5521.9,7727.2,9869.6,12137,14342,16736,19004,21460,23916,26246,28766,31222,33615,36071,38465,40984,43314,45770};

	Double_t twist_protons_y[19] = {0.0061659,0.0040113,0.0025747,0.0016527,0.0011043,0.00073794,0.00049308,0.00033844,0.00023228,0.00015520,0.00011090,0.000076112,0.000052944,0.000036829,0.000024940,0.000017349,0.000012068,0.0000083945,0.0000057615};
	TGraph *gTProtons = new TGraph(19, twist_protons_x, twist_protons_y);
	gTProtons->SetTitle("TWIST Al 75 #mum");
	gTProtons->SetLineColor(kBlue);
	gTProtons->SetLineStyle(kDashed);
	gTProtons->SetLineWidth(2);

	TFile *fJohn = new TFile("en.p.t400ns.dt200ns.rebin5.pp10us.elo2e3keV.ehi12e3keV.bay5.pidcorr.root", "READ");
	TFile *fAl50 = new TFile("unfolded.al50.proton.root", "READ");
	TFile *fAl100 = new TFile("unfolded.al100.root", "READ");

	TH1D *h_John_r = (TH1D *)fJohn->Get("hr_e_reco_bay");
	TH1D *h_John_l = (TH1D *)fJohn->Get("hl_e_reco_bay");

	TH1D *h_Al50_r = (TH1D *)fAl50->Get("hRecoRight"); h_Al50_r->Rebin(2);
	TH1D *h_Al50_l = (TH1D *)fAl50->Get("hRecoLeft");

	TH1D *h_Al100_r = (TH1D *)fAl100->Get("hRecoRight"); h_Al100_r->Rebin(2);
	TH1D *h_Al100_l = (TH1D *)fAl100->Get("hRecoLeft");
	for(int i=17; i < 21; ++i) {
		h_Al100_r->SetBinContent(i, 0);
		h_Al100_r->SetBinError(i, 0);
	}

	{
	TCanvas *c = new TCanvas("c", "c");
//	h_John_r->Scale(1/(0.56*0.609*1.61E+8) ); 
//	h_John_r->Draw();
//	h_John_r->SetTitle("John Al50");
//	h_John_r->SetLineWidth(2);
	h_Al50_r->Draw("");
	h_Al50_r->SetTitle("Al 50 #mum");
	h_Al50_r->SetLineColor(kBlack); h_Al50_r->SetLineWidth(2);
	h_Al100_r->Draw("SAME");
	h_Al100_r->SetTitle("Al 100 #mum");
	h_Al100_r->SetLineColor(kRed); h_Al100_r->SetLineWidth(2);

	gTProtons->Draw("LSAME");

	c->BuildLegend();
	alcaphistogram(h_Al50_r);
	alcaphistogram(h_Al50_l);
	alcapPreliminary(h_Al50_r);
	h_John_r->SetTitle("50#mum and 100#mum Al;E [keV];Protons/captured #mu/500 keV");
	h_John_r->GetYaxis()->SetMaxDigits(3);
	}
/**
	{
	TCanvas *d = new TCanvas("d", "d");
//	h_John_l->Scale(1/(0.56*0.609*1.61E+8) ); 
//	h_John_l->Draw();
//	h_John_l->SetTitle("John Al50");
//	h_John_l->SetLineWidth(2);
	h_Al50_l->Draw("");
	h_Al50_l->SetTitle("Al50");
	h_Al50_l->SetLineColor(kBlack); h_Al50_l->SetLineWidth(2);
	h_Al100_l->Draw("SAME");
	h_Al100_l->SetTitle("Al100");
	h_Al100_l->SetLineColor(kRed); h_Al100_l->SetLineWidth(2);

	d->BuildLegend();
	h_John_l->SetTitle("50#mum and 100#mum Al;E [keV];Protons/captured #mu/500 keV");
	h_John_l->GetYaxis()->SetMaxDigits(3);
	}
**/
}
