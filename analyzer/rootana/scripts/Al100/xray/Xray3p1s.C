void Xray3p1s() {
	TFile *fData = new TFile("al100.geshifted.root", "READ");
	TTree *tree = (TTree *)fData->Get("tree");
	Double_t t1, e1, timeToPrevTME, timeToNextTME;
	TString *channel = new TString("");
	tree->SetBranchAddress("timeToPrevTME", &timeToPrevTME);
	tree->SetBranchAddress("timeToNextTME", &timeToNextTME);
	tree->SetBranchAddress("e1", &e1);
	tree->SetBranchAddress("t1", &t1);
	tree->SetBranchAddress("channel", &channel);

	Int_t min = 402, max = 424;
	Int_t bins = max-min;
	TFile *fOutput = new TFile("xray_output.root", "RECREATE");
	TH1D *hGeLoGain = new TH1D("hGeLoGain", "GeLoGain;E [keV];Counts / 1 keV", bins, min, max);
	TH1D *hGeHiGain = new TH1D("hGeHiGain", "GeHiGain;E [keV];Counts / 1 keV", bins, min, max);

	TH1D *hGeLoGainRes = new TH1D("hGeLoGainRes", "GeLoGain Residual", bins, min, max);
	TH1D *hGeHiGainRes = new TH1D("hGeHiGainRes", "GeHiGain Residual", bins, min, max);
	for(Long64_t i=0; i < tree->GetEntries(); ++i) {
		tree->GetEntry(i);
		if(timeToPrevTME < 10e3 || timeToNextTME < 10e3) continue;
		if(t1>10e3) continue;
		if(e1 < min || e1 > max) continue;
		Int_t sigma = 3;
		if(channel->Contains("GeLoGain") ) {
			if(t1 > sigma*93.86 || t1 < -sigma*93.86) continue;
			hGeLoGain->Fill(e1);
		} else if(channel->Contains("GeHiGain") ) {
			if(t1 > sigma*93.77 || t1 < -sigma*93.77) continue;
			hGeHiGain->Fill(e1);
		}
	}
	TF1 *fit = new TF1("fit", "[0]*TMath::Exp(-0.5 * ((x-[1])/[2])^2) + [3]*TMath::Exp(-0.5 * ((x-[4])/[2])^2) + TMath::Exp([5]+[6]*x)", min, max);
	fit->SetParName(0, "Al const.");
	fit->SetParName(1, "Al mean");
	fit->SetParName(2, "Sigma");
	fit->SetParName(3, "Pb const.");
	fit->SetParName(4, "Pb mean ");
	fit->SetParName(5, "Bkg. const.");
	fit->SetParName(6, "Bkg. slope");

//	fit->SetParameters(0, 3.11004e+03);
//fit->SetParLimits(0, 0, 1e4);
//	fit->SetParameters(1, 4.12342e+02);
//	fit->SetParameters(2, 1.12209e+00);
//	fit->SetParLimits(2, 0, 3);
//	fit->SetParameters(3, 2.20069e+03);
//	fit->SetParLimits(3, 0, 5e3);
//	fit->SetParameters(4, 4.15962e+02);
//	fit->SetParameters(5, 7.21917e+00);
//	fit->SetParameters(6, -1.53014e-03);
	fit->SetParLimits(1, 411, 414);
	fit->SetParLimits(4, 413, 417);

fit->SetParameter(0,  2.09157e+03 );
fit->SetParameter(1,  4.13014e+02 );
fit->SetParameter(2,  1.20547e+00 );
fit->SetParameter(3,  1.07240e+03 );
fit->SetParameter(4,  4.15502e+02 );
fit->SetParameter(5,   9.01129e+00);
fit->SetParameter(6, -4.21168e-03 );

	gStyle->SetOptFit(1);
	gStyle->SetOptStat(1000000001); //only name of histogram
	//GeLoGain fit
	fit->SetLineColor(kBlue);
	hGeLoGain->Fit("fit", "SR+");
	Double_t areaGeLoGain = TMath::Sqrt(2*TMath::Pi() ) * fit->GetParameter(0) * fit->GetParameter(2);
	Double_t areaErrGeLoGain = TMath::Sqrt(2*TMath::Pi() ) * areaGeLoGain * ( fit->GetParError(0)/fit->GetParameter(0) + fit->GetParError(2)/fit->GetParameter(2) );
	for(int i=1; i<hGeLoGain->GetSize() - 1; ++i) {
		Double_t res = (hGeLoGain->GetBinContent(i) - fit->Eval(min+i-0.5) )/hGeLoGain->GetBinContent(i);
		hGeLoGainRes->SetBinContent(i-1, res);
	}
	//GeHiGain fit
	fit->SetLineColor(kRed);
	hGeHiGain->Fit("fit", "SR+");
	Double_t areaGeHiGain = TMath::Sqrt(2*3.142) * fit->GetParameter(0) * fit->GetParameter(2);
	Double_t areaErrGeHiGain = TMath::Sqrt(2*TMath::Pi() ) * areaGeHiGain * ( fit->GetParError(0)/fit->GetParameter(0) + fit->GetParError(2)/fit->GetParameter(2) );
	for(int i=1; i<hGeHiGain->GetSize() - 1; ++i) {
		Double_t res = (hGeHiGain->GetBinContent(i) - fit->Eval(min+i-0.5) )/hGeHiGain->GetBinContent(i);
		hGeHiGainRes->SetBinContent(i-1, res);
	}

	hGeLoGainRes->Fit("pol0");
	hGeHiGainRes->Fit("pol0");
	fOutput->Write();

	Double_t xrayIntensity = 0.077, xrayIntensityErr = 0.002;
	Double_t geHiEfficiency = 5.73e-4, geLoEfficiency = 5.67e-4, geEfficiencyErr = 0.1e-4; //needs to be updated
	printf("GeLoGain:%f±%f GeHiGain:%f±%f\n", areaGeLoGain, areaErrGeLoGain, areaGeHiGain, areaErrGeHiGain);

	Double_t finalErrGeLoGain = TMath::Power((areaErrGeLoGain/areaGeLoGain), 2) + TMath::Power((xrayIntensityErr/xrayIntensity), 2) + TMath::Power((geEfficiencyErr/geLoEfficiency), 2);
	finalErrGeLoGain = areaGeLoGain/xrayIntensity/geLoEfficiency * TMath::Sqrt(finalErrGeLoGain);
	Double_t finalErrGeHiGain = TMath::Power((areaErrGeHiGain/areaGeHiGain), 2) + TMath::Power((xrayIntensityErr/xrayIntensity), 2) + TMath::Power((geEfficiencyErr/geHiEfficiency), 2);
	finalErrGeHiGain = areaGeHiGain/xrayIntensity/geHiEfficiency * TMath::Sqrt(finalErrGeHiGain);

	printf("GeLoGain:%f±%f GeHiGain:%f±%f\n", areaGeLoGain/xrayIntensity/geLoEfficiency, finalErrGeLoGain, areaGeHiGain/xrayIntensity/geHiEfficiency, finalErrGeHiGain);

	TCanvas *c = new TCanvas("c", "c");
	alcaphistogram(hGeHiGain);
	alcaphistogram(hGeLoGain);
	hGeHiGain->Draw("E1");
	hGeHiGain->SetTitle("3p-1s X-ray Spectrum (Al100)");
	hGeHiGain->SetLineColor(kRed);
	hGeLoGain->Draw("E1 SAMES"); hGeLoGain->SetLineColor(kBlue);

	TLegend *legend = new TLegend(.6, .5, .87, .87, "AlCap #bf{#it{X-ray} }");
	legend->AddEntry((TObject*)0, "#bf{N_{#mui} = 2.28 #times 10^{8}}", "");
	legend->AddEntry((TObject*)0, "#bf{#muAl 3p #rightarrow 1s}", "");
	legend->AddEntry(hGeHiGain, Form("#bf{N_{hs} = %.2f(%.0f) #times 10^{8} }", areaGeHiGain/xrayIntensity/geHiEfficiency/1e8, finalErrGeHiGain/1e6), "LE");
	legend->AddEntry(hGeLoGain, Form("#bf{N_{ls} = %.2f(%.0f) #times 10^{8} }", areaGeLoGain/xrayIntensity/geLoEfficiency/1e8, finalErrGeLoGain/1e6), "LE");
	legend->Draw("SAME");

//	TCanvas *d = new TCanvas("LoGainRes", "LoGainRes");
//	hGeLoGainRes->Draw("E");
//
//	TCanvas *e = new TCanvas("HiGainRes", "HiGainRes");
//	hGeHiGainRes->Draw("E");
}
