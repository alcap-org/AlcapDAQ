{
	TFile *fData = new TFile("../al50.geshifted.root", "READ");
	TTree *tree = (TTree *)fData->Get("tree");
	Double_t t1, e1, timeToPrevTME, timeToNextTME;
	TString *channel;
	tree->SetBranchAddress("timeToPrevTME", &timeToPrevTME);
	tree->SetBranchAddress("timeToNextTME", &timeToNextTME);
	tree->SetBranchAddress("e1", &e1);
	tree->SetBranchAddress("t1", &t1);
	tree->SetBranchAddress("channel", &channel);

	Int_t min = 403, max = 423;
	Int_t bins = max-min;
	TFile *fOutput = new TFile("xray_output.root", "RECREATE");
	TH1D *hGeLoGain = new TH1D("hGeLoGain", "GeLoGain;E [keV];Counts / 1 keV", bins, min, max);
	TH1D *hGeHiGain = new TH1D("hGeHiGain", "GeHiGain;E [keV];Counts / 1 keV", bins, min, max);

	TH1D *hGeLoGainRes = new TH1D("hGeLoGainRes", "GeLoGain Residual", bins, min, max);
	TH1D *hGeHiGainRes = new TH1D("hGeHiGainRes", "GeHiGain Residual", bins, min, max);
	for(Long64_t i=0; i < tree->GetEntries(); ++i) {
		tree->GetEntry(i);
		if(timeToPrevTME < 10e3 || timeToNextTME < 10e3) continue;
		if(t1 > 200 || t1 < -200) continue;
		if(e1 < min || e1 > max) continue;
		if(channel->Contains("GeLoGain") ) {
			hGeLoGain->Fill(e1);
		} else if(channel->Contains("GeHiGain") ) {
			hGeHiGain->Fill(e1);
		}
	}
//not constraints on sigma
//	TF1 *fit = new TF1("fit", "gaus(0)+gaus(3)+expo(6)", min, max);
//	fit->SetParameter(0,  2.99905e+04);
//	fit->SetParameter(1,  3.44548e+02);
//	fit->SetParameter(2,  1.12945e+00);
//	fit->SetParameter(3,  6.38096e+03);
//	fit->SetParameter(4,  3.48731e+02);
//	fit->SetParameter(5,  1.11865e+00);
//	fit->SetParameter(6,  1.19627e+01);
//	fit->SetParameter(7, -1.04991e-02);
	
//sigma for signal and background set to be the same

	TF1 *fit = new TF1("fit", "[0]*TMath::Exp(-0.5 * ((x-[1])/[2])^2) + [3]*TMath::Exp(-0.5 * ((x-[4])/[2])^2) + TMath::Exp([5]+[6]*x)", min, max);
	fit->SetParName(0, "Al const.");
	fit->SetParName(1, "Al mean");
	fit->SetParName(2, "Sigma");
	fit->SetParName(3, "Pb const.");
	fit->SetParName(4, "Pb mean ");
	fit->SetParName(5, "Bkg. const.");
	fit->SetParName(6, "Bkg. slope");

	fit->SetParameters(0, 3.11004e+03);
fit->SetParLimits(0, 0, 1e4);
	fit->SetParameters(1, 4.12342e+02);
	fit->SetParameters(2, 1.12209e+00);
	fit->SetParLimits(2, 0, 3);
	fit->SetParameters(3, 2.20069e+03);
	fit->SetParLimits(3, 0, 5e3);
	fit->SetParameters(4, 4.15962e+02);
	fit->SetParameters(5, 7.21917e+00);
	fit->SetParameters(6, 1.53014e-03);
	fit->SetParLimits(1, 411, 414);
	fit->SetParLimits(4, 413, 417);

	gStyle->SetOptFit(1);
	gStyle->SetOptStat(1000000001); //only name of histogram
	//GeLoGain fit
	hGeLoGain->Fit("fit", "SR+");
	Double_t areaGeLoGain = TMath::Sqrt(2*TMath::Pi() ) * fit->GetParameter(0) * fit->GetParameter(2);
	Double_t areaErrGeLoGain = TMath::Sqrt(2*TMath::Pi() ) * areaGeLoGain * ( fit->GetParError(0)/fit->GetParameter(0) + fit->GetParError(2)/fit->GetParameter(2) );
	for(int i=1; i<hGeLoGain->GetSize() - 1; ++i) {
		Double_t res = (hGeLoGain->GetBinContent(i) - fit->Eval(min+i-0.5) )/hGeLoGain->GetBinContent(i);
		hGeLoGainRes->SetBinContent(i-1, res);
	}
	//GeHiGain fit
	hGeHiGain->Fit("fit", "SR+");
	Double_t areaGeHiGain = TMath::Sqrt(2*3.142) * fit->GetParameter(0) * fit->GetParameter(2);
	Double_t areaErrGeHiGain = TMath::Sqrt(2*TMath::Pi() ) * areaGeLoGain * ( fit->GetParError(0)/fit->GetParameter(0) + fit->GetParError(2)/fit->GetParameter(2) );
	for(int i=1; i<hGeHiGain->GetSize() - 1; ++i) {
		Double_t res = (hGeHiGain->GetBinContent(i) - fit->Eval(min+i-0.5) )/hGeHiGain->GetBinContent(i);
		hGeHiGainRes->SetBinContent(i-1, res);
	}
	fOutput->Write();

	Double_t xrayIntensity = 0.077, xrayIntensityErr = 0.002;
	Double_t geEfficiency = 5.7e-4, geEfficiencyErr = 0.1e-4; //needs to be updated, using efficiency at 2p-1s energy
	printf("GeLoGain:%f±%f GeHiGain:%f±%f\n", areaGeLoGain, areaErrGeLoGain, areaGeHiGain, areaErrGeHiGain);

	Double_t finalErrGeLoGain = TMath::Power((areaErrGeLoGain/areaGeLoGain), 2) + TMath::Power((xrayIntensityErr/xrayIntensity), 2) + TMath::Power((geEfficiencyErr/geEfficiency), 2);
	finalErrGeLoGain = areaGeLoGain/xrayIntensity/geEfficiency * TMath::Sqrt(finalErrGeLoGain);
	Double_t finalErrGeHiGain = TMath::Power((areaErrGeHiGain/areaGeHiGain), 2) + TMath::Power((xrayIntensityErr/xrayIntensity), 2) + TMath::Power((geEfficiencyErr/geEfficiency), 2);
	finalErrGeHiGain = areaGeHiGain/xrayIntensity/geEfficiency * TMath::Sqrt(finalErrGeHiGain);

	printf("GeLoGain:%f±%f GeHiGain:%f±%f\n", areaGeLoGain/xrayIntensity/geEfficiency, finalErrGeLoGain, areaGeHiGain/xrayIntensity/geEfficiency, finalErrGeHiGain);

	TCanvas *c = new TCanvas("c", "c");
	hGeHiGain->Draw("E");
	hGeHiGain->SetTitle("3p-1s X-ray Spectrum (Al50) #pm200ns");
	hGeHiGain->SetLineColor(kRed);
	hGeHiGain->GetYaxis()->SetTitleOffset(1.35);
	hGeLoGain->Draw("E SAMES"); hGeLoGain->SetLineColor(kBlue);

	TLatex latex;
	latex.SetTextSize(.025);
	latex.DrawLatex(410, 2500, Form("#splitline{HiGain %.2e#pm%.2e}{LoGain %.2e#pm%.2e}", areaGeHiGain/xrayIntensity/geEfficiency, finalErrGeHiGain, areaGeLoGain/xrayIntensity/geEfficiency, finalErrGeLoGain) );
}
