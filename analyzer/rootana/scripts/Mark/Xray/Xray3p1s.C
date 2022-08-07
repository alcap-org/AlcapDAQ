void Xray3p1s(std::string target, std::string uChannel = "GeHiGain") {
	TFile *fData = new TFile(Form("%s/%s.root", getenv("R15b_DATA"), target.c_str() ), "READ");
	TTree *tree = (TTree *)fData->Get("tree");
	Double_t t1, e1, timeToPrevTME, timeToNextTME;
	TString * channel = new TString("");
	tree->SetBranchAddress("timeToPrevTME", &timeToPrevTME);
	tree->SetBranchAddress("timeToNextTME", &timeToNextTME);
	tree->SetBranchAddress("e1", &e1);
	tree->SetBranchAddress("t1", &t1);
	tree->SetBranchAddress("channel", &channel);

	Int_t min, max;
	if(target.compare("al50") == 0 || target.compare("al100") == 0) {
		min = 402;
		max = 424;
	} else if(target.compare("ti50") == 0) {
		min = 919;
		max = 953;
	}
	Int_t bins = max-min;
	TH1D *hGeHiGain = new TH1D("hGeHiGain", ";;Counts / 1 keV", bins, min, max);

	TH1D *hGeHiGainRes = new TH1D("hGeHiGainRes", "GeHiGain Residual;E[keV];#frac{Entries-Fit}{Entries}[%]", bins, min, max);
	for(Long64_t i=0; i < tree->GetEntries(); ++i) {
		tree->GetEntry(i);
		if(timeToPrevTME < 10e3 || timeToNextTME < 10e3) continue;
		if(abs(t1)>10e3) continue;
		if(e1 < min || e1 > max) continue;
		if(channel->Contains(uChannel) ) {
			hGeHiGain->Fill(e1);
		}
	}
	TF1 *fit = new TF1("fit", "[0]*TMath::Exp(-0.5 * ((x-[1])/[2])^2) + [3]*TMath::Exp(-0.5 * ((x-[4])/[2])^2) + TMath::Exp([5]+[6]*x)", min, max);
	fit->SetParName(0, "Al const.");
	fit->SetParName(1, "Al mean");
	fit->SetParName(2, "Sigma");
	fit->SetParName(3, "Pb const.");
	fit->SetParName(4, "Pb mean");
	fit->SetParName(5, "Bkg. const.");
	fit->SetParName(6, "Bkg. slope");

	if(target.compare("al50") == 0 || target.compare("al100") == 0) {
		fit->SetParameter(0,  2.09157e+03 );
		fit->SetParameter(1,  4.13014e+02 );
		fit->SetParameter(2,  1.20547e+00 );
		fit->SetParameter(3,  1.07240e+03 );
		fit->SetParameter(4,  4.15502e+02 );
		fit->SetParameter(5,   9.01129e+00);
		fit->SetParameter(6, -4.21168e-03 );
		fit->SetParLimits(1, 411, 414);
		fit->SetParLimits(2, 0, 2);
		fit->SetParLimits(4, 413, 417);
	} else if(target.compare("ti50") == 0) {
		fit->SetParameter(0, 3.57101e+03);
		fit->SetParameter(1, 9.31683e+02);
		fit->SetParameter(2, 1.98952e+00);
		fit->SetParameter(3, 2.87069e+03);
		fit->SetParameter(4, 9.37861e+02);
		fit->SetParameter(5, 1.30813e+01);
		fit->SetParameter(6, -6.91330e-03);
		fit->SetParLimits(1, 927, 934);
		fit->SetParLimits(2, 1, 2.5);
		fit->SetParLimits(4, 936, 939);
	}
//	gStyle->SetOptFit(1);
//	gStyle->SetOptStat(1000000001); //only name of histogram

	//GeHiGain fit
	fit->SetLineColor(kBlue);
	hGeHiGain->Fit("fit", "SR+");
	Double_t areaGeHiGain = TMath::Sqrt(2*3.142) * fit->GetParameter(0) * fit->GetParameter(2);
	printf("Area\t%f\n", areaGeHiGain);
	printf("Chi2 / NDF %f / %d \n", fit->GetChisquare(), fit->GetNDF() );
	Double_t areaErrGeHiGain = TMath::Sqrt(2*TMath::Pi() ) * areaGeHiGain * ( fit->GetParError(0)/fit->GetParameter(0) + fit->GetParError(2)/fit->GetParameter(2) );
	for(int i=0; i<hGeHiGain->GetSize()- 1; ++i) {
		Double_t res = 100 * (hGeHiGain->GetBinContent(i) - fit->Eval(min+i-0.5) )/hGeHiGain->GetBinContent(i);
		hGeHiGainRes->SetBinContent(i, res);
	}
	Double_t xrayIntensity, xrayIntensityErr;
	Double_t geHiEfficiency, geEfficiencyErr;
	Double_t geLoEfficiency;
	if(target.compare("al50") == 0 || target.compare("al100") == 0) {
		xrayIntensity = 0.077;
		xrayIntensityErr = 0.002;
		geHiEfficiency = 5.73e-4;
		geLoEfficiency = 5.67e-4;
		geEfficiencyErr = 0.1e-4;
	} else if(target.compare("ti50") == 0) {
		xrayIntensity = 0.752;
		xrayIntensityErr = 0.008;
		geHiEfficiency = 2.60e-4;
		geHiEfficiency = 2.00e-4;
		geEfficiencyErr = 0.1e-4;
	}
	Double_t geEfficiency;
	if(uChannel.compare("GeHiGain") == 0) {
		geEfficiency = geHiEfficiency;
	} else if(uChannel.compare("GeLoGain") == 0) {
		geEfficiency = geLoEfficiency;
	}
	Double_t finalErrGeHiGain = TMath::Power((areaErrGeHiGain/areaGeHiGain), 2) + TMath::Power((xrayIntensityErr/xrayIntensity), 2) + TMath::Power((geEfficiencyErr/geEfficiency), 2);
	finalErrGeHiGain = areaGeHiGain/xrayIntensity/geEfficiency * TMath::Sqrt(finalErrGeHiGain);

	TF1 *result = new TF1("fit", "[0]*TMath::Exp(-0.5 * ((x-[1])/[2])^2)", min, max);
	result->SetParameter(0, fit->GetParameter(0) );
	result->SetParameter(1, fit->GetParameter(1) );
	result->SetParameter(2, fit->GetParameter(2) );
	result->SetLineColor(kBlue);
	result->SetLineStyle(kDashed);

	TCanvas *c = new TCanvas("c", "c");
	TPad *p1 = new TPad("p1", "p1",0,0.3,1,1, kWhite, 0, 0);
	p1->SetBottomMargin(0.05);
	p1->Draw();
	TPad *p2 = new TPad("p2", "p2",0,0.0,1,0.3 , kWhite, 0, 0);
	p2->SetTopMargin(0.00001);
	p2->SetBottomMargin(0.2);
	p2->Draw();

	p1->cd();
	hGeHiGain->Draw("E1");
	result->Draw("SAME");
	hGeHiGain->SetMarkerStyle(kCircle);
	hGeHiGain->GetYaxis()->SetRangeUser(1, hGeHiGain->GetMaximum() * 1.1);
	hGeHiGain->SetLineColor(kBlue);
	hGeHiGain->GetXaxis()->SetLabelSize(0);
	hGeHiGain->GetYaxis()->SetMaxDigits(3);
	hGeHiGain->GetYaxis()->SetTitleSize(.05);
	hGeHiGain->GetYaxis()->SetTitleOffset(.8);
	hGeHiGain->GetYaxis()->SetLabelFont(63);
	hGeHiGain->GetYaxis()->SetLabelSize(16);

	target[0] = toupper(target[0]);
	TLegend *legend = new TLegend(.6, .5, .87, .87, "#bf{AlCap} #it{3p #rightarrow 1s}");
	if(target.compare("Al50") == 0 ) {
		legend->AddEntry((TObject*)0, "N_{#mu} = 4.21 #times 10^{8}", "");
	} else if(target.compare("Al100") == 0 ) {
		legend->AddEntry((TObject*)0, "N_{#mu} = 2.28 #times 10^{8}", "");
	} else if(target.compare("Ti50") == 0 ) {
		legend->AddEntry((TObject*)0, "N_{#mu} = 1.88 #times 10^{8}", "");
	}
	legend->AddEntry(hGeHiGain, Form("%s#mum data", target.c_str() ), "EP");
	legend->AddEntry(fit, "Total fit", "L");
	legend->AddEntry(result, Form("N_{stop} = %.2f(%.0f) #times 10^{8}", areaGeHiGain/xrayIntensity/geEfficiency/1e8, finalErrGeHiGain/1e6), "L");
	legend->Draw("SAME");

	p2->cd();
	p2->SetGridx();
	p2->SetGridy();
	hGeHiGainRes->Draw();
	hGeHiGainRes->GetXaxis()->SetLabelFont(63);
	hGeHiGainRes->GetXaxis()->SetLabelSize(15);
	hGeHiGainRes->GetXaxis()->SetTitleSize(.1);
	hGeHiGainRes->GetXaxis()->SetTitleOffset(.4);
	hGeHiGainRes->GetXaxis()->SetTitle("[keV]");

	hGeHiGainRes->SetFillColor(38);
	hGeHiGainRes->SetLineColor(kWhite);
	hGeHiGainRes->GetYaxis()->SetNdivisions(10, 2, 0);
	hGeHiGainRes->GetYaxis()->SetLabelFont(63);
	hGeHiGainRes->GetYaxis()->SetLabelSize(16);
	hGeHiGainRes->GetYaxis()->SetTitleSize(.1);
	hGeHiGainRes->GetYaxis()->SetTitleOffset(.4);
	hGeHiGainRes->GetYaxis()->CenterTitle();
	c->cd();

	const char *FigsDir = getenv("R15b_OUT");
	c->SaveAs(Form("%s/AlCapData_%sDataset_3p1sXray_%s.pdf", FigsDir, target.c_str(), uChannel.c_str() ) );
}
