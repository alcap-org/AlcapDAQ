TH1D * Plot(const char *particle = "proton") {
	//statistical errors
	TFile *fUnfoldedAl50_stat = new TFile(Form("%s/unfolded.al50.root", getenv("R15b_OUT") ) );
	TH1D *hSiR_al50_stat = (TH1D *) fUnfoldedAl50_stat->Get(Form("h%s_SiR", particle) );
	TH1D *hSiL_al50_stat = (TH1D *) fUnfoldedAl50_stat->Get(Form("h%s_SiL", particle) );
	TFile *fUnfoldedAl100_stat = new TFile(Form("%s/unfolded.al100.root", getenv("R15b_OUT") ) );
	TH1D *hSiR_al100_stat = (TH1D *) fUnfoldedAl100_stat->Get(Form("h%s_SiR", particle) );
	TH1D *hSiL_al100_stat = (TH1D *) fUnfoldedAl100_stat->Get(Form("h%s_SiL", particle) );

	//systematic errors
	TFile *fUnfoldedAl50 = new TFile(Form("%s/al50-systematics.root", getenv("R15b_OUT") ) );
	TH1D *hSiR_al50 = (TH1D *) fUnfoldedAl50->Get(Form("h%s_SiR_al50", particle) );
	TH1D *hSiL_al50 = (TH1D *) fUnfoldedAl50->Get(Form("h%s_SiL_al50", particle) );
	TFile *fUnfoldedAl100= new TFile(Form("%s/al100-systematics.root", getenv("R15b_OUT") ) );
	TH1D *hSiR_al100 = (TH1D *) fUnfoldedAl100->Get(Form("h%s_SiR_al100", particle) );
	TH1D *hSiL_al100 = (TH1D *) fUnfoldedAl100->Get(Form("h%s_SiL_al100", particle) );

	std::string capitalized = std::string(particle);
	capitalized[0] = toupper(capitalized[0]);
	TH1D *hWeightedAverage= new TH1D("hWeightedAverage", ";E[MeV];Particles / muon capture / MeV", 40, 0, 20);
	hWeightedAverage->GetYaxis()->SetMaxDigits(3);
	TH1D *hStats= new TH1D("hStats", ";E[MeV];Particles / muon capture / MeV", 40, 0, 20);
	hStats->GetYaxis()->SetMaxDigits(3);
	hStats->SetLineWidth(2);

	for(int i = 9; i <= 40; ++i) { //lower than the 9th bin, the uncertainty are great due to many reasons
	
		//syst errors from unfolded.{al50, al100}.root
		Double_t stat_a = hSiR_al50_stat->GetBinError(i);
		Double_t stat_b = hSiL_al50_stat->GetBinError(i);
		Double_t stat_c = hSiR_al100_stat->GetBinError(i);
		Double_t stat_d = hSiL_al100_stat->GetBinError(i);

		Double_t stat_reciprocal_sum_of_squares = 1/TMath::Power(stat_a, 2) + 1/TMath::Power(stat_b, 2) + 1/TMath::Power(stat_c, 2) + 1/TMath::Power(stat_d, 2);
		Double_t stat_variance = 1/stat_reciprocal_sum_of_squares;

		//stat and syst combined errors from Systematics.C/Systematics100.C
		Double_t err_a = hSiR_al50->GetBinError(i);
		Double_t err_b = hSiL_al50->GetBinError(i);
		Double_t err_c = hSiR_al100->GetBinError(i);
		Double_t err_d = hSiL_al100->GetBinError(i);

		Double_t reciprocal_sum_of_squares = 1/TMath::Power(err_a, 2) + 1/TMath::Power(err_b, 2) + 1/TMath::Power(err_c, 2) + 1/TMath::Power(err_d, 2);

		Double_t a = hSiR_al50->GetBinContent(i);
		Double_t b = hSiL_al50->GetBinContent(i);
		Double_t c = hSiR_al100->GetBinContent(i);
		Double_t d = hSiL_al100->GetBinContent(i);

		Double_t mean = (a*(1/TMath::Power(err_a, 2) ) + b*(1/TMath::Power(err_b, 2) ) + c*(1/TMath::Power(err_c, 2) ) + d*(1/TMath::Power(err_d, 2) ) ) / reciprocal_sum_of_squares;
		Double_t variance = 1 / reciprocal_sum_of_squares;	

		if(TMath::IsNaN(mean) ) {
			mean = 0;
			variance = 0;
			stat_variance = 0;
		}
		if(i > 32 && i <= 40) {
			mean = a;
			variance = err_a*err_a;
			stat_variance = stat_a*stat_a;
		}
		hWeightedAverage->SetBinContent(i, mean);
		hWeightedAverage->SetBinError(i, TMath::Sqrt(variance) );
		hStats->SetBinContent(i, mean);
		hStats->SetBinError(i, TMath::Sqrt(stat_variance) );
	}
	hWeightedAverage->Rebin(2);
	hStats->Rebin(2);

        //currently no physical motivation for this function
	TF1 *fit = 0;
	if(strcmp(particle, "proton") == 0) {
        	fit = new TF1("fit", "[0]*exp(-(x)*[2]) + [3]*exp(-(x-[4])*[5])", 4, 20);
		fit->SetParameter(0, 5.49525e-03);
		fit->SetParameter(2, 5.91451e-01);
		fit->SetParameter(3, 1.21979e-03);
		fit->SetParameter(4, 7.54185e+00);
		fit->SetParameter(5, 1.79100e-01);
		fit->SetParLimits(0, 0, 1000);
	} else if(strcmp(particle, "deuteron") == 0) {
		fit = new TF1("fit", "[0]*exp(-(x)*[2]) + [3]*exp(-(x-[4])*[5])", 5, 18);
		fit->SetParameter(0, 5.49525e-03);
		fit->SetParameter(2, 5.91451e-01);
		fit->SetParameter(3, 1.21979e-03);
		fit->SetParameter(4, 7.54185e+00);
		fit->SetParameter(5, 1.79100e-01);
		fit->SetParLimits(0, 0, 1000);
	} else if(strcmp(particle, "triton") == 0) {
		fit = new TF1("fit", "[0]*exp(-(x)*[2]) + [3]*exp(-(x-[4])*[5])", 6, 18);
		fit->SetParameter(0, 5.49525e-03);
		fit->SetParameter(2, 5.91451e-01);
		fit->SetParameter(3, 1.21979e-03);
		fit->SetParameter(4, 7.54185e+00);
		fit->SetParameter(5, 1.79100e-01);
		fit->SetParLimits(0, 0, 1000);
	} else {
        	fit = new TF1("fit", "[0]*exp(-(x)*[2])", 15, 19);
		fit->SetParameter(0, 5.49525e-03);
		fit->SetParameter(2, 5.91451e-01);
	}

	hWeightedAverage->GetXaxis()->SetRangeUser(2, 20);
//	if(strcmp(particle, "proton") == 0) {
//		hWeightedAverage->Draw("E2");
//	} else {
		hWeightedAverage->Draw("E2 SAME");
//	}
	if(strcmp(particle, "proton") == 0) {
		hWeightedAverage->GetYaxis()->SetRangeUser(1e-5, 8e-3);
		hStats->GetXaxis()->SetRangeUser(4, 20);
		hWeightedAverage->SetFillColor(kRed);
	} else if(strcmp(particle, "deuteron") == 0) {
		hWeightedAverage->GetXaxis()->SetRangeUser(5, 20);
		hStats->GetXaxis()->SetRangeUser(5, 20);
		hWeightedAverage->SetFillColor(kBlue);
	} else if(strcmp(particle, "triton") == 0) {
		hWeightedAverage->GetXaxis()->SetRangeUser(6, 20);
		hStats->GetXaxis()->SetRangeUser(6, 20);
		hWeightedAverage->SetFillColor(kGreen);
	} else {
		hWeightedAverage->GetXaxis()->SetRangeUser(15, 20);
		hStats->GetXaxis()->SetRangeUser(15, 20);
		hWeightedAverage->SetFillColor(kMagenta);
	}
	hStats->Draw("SAME");

	if(strcmp(particle, "proton") == 0) {
		Double_t stats_error, syst_error;
		hStats->IntegralAndError(hStats->FindBin(4), hStats->FindBin(18), stats_error);
		std::cout << "4 - 18MeV: " << hWeightedAverage->IntegralAndError(hWeightedAverage->FindBin(4), hWeightedAverage->FindBin(18.), syst_error ) << " ± " << stats_error *1e3<< " " << (syst_error-stats_error )*1e3<< std::endl;
		std::cout << "Fit 4 - 40MeV: " << fit->Integral(4, 40) << std::endl;
	} else if(strcmp(particle, "deuteron") == 0) {
		Double_t stats_error, syst_error;
		hStats->IntegralAndError(hStats->FindBin(5), hStats->FindBin(18), stats_error);
		std::cout << "5 - 18MeV: " << hWeightedAverage->IntegralAndError(hWeightedAverage->FindBin(5), hWeightedAverage->FindBin(18.), syst_error ) << " ± " << stats_error *1e3<< " " << (syst_error-stats_error )*1e3<<  std::endl;
		std::cout << "Fit 5 - 40MeV: " << fit->Integral(5, 40) << std::endl;
	} else if(strcmp(particle, "triton") == 0) {
		Double_t stats_error, syst_error;
		hStats->IntegralAndError(hStats->FindBin(6), hStats->FindBin(18), stats_error);
		std::cout << "6 - 18MeV: " << hWeightedAverage->IntegralAndError(hWeightedAverage->FindBin(6), hWeightedAverage->FindBin(18.), syst_error ) << " ±"  << stats_error *1e3<< " " << (syst_error-stats_error )*1e3<<  std::endl;
		std::cout << "Fit 6 - 40MeV: " << fit->Integral(6, 40) << std::endl;
	} else {
		Double_t stats_error, syst_error;
		hStats->IntegralAndError(hStats->FindBin(15), hStats->FindBin(19), stats_error);
		std::cout << "15 - 18MeV: " << hWeightedAverage->IntegralAndError(hWeightedAverage->FindBin(15), hWeightedAverage->FindBin(19.), syst_error ) << " ± " << stats_error *1e3<< " " << (syst_error-stats_error )*1e3<<  std::endl;
		std::cout << "Fit 15 - 40MeV: " << fit->Integral(15, 40) << std::endl;
	}
	return hWeightedAverage;
}

void DrawComparisonWithTwist() {
	Double_t twist_protons_x[26] = {80.7538    ,82.593     ,    87.3773,   92.5298,     97.6831,    102.469,    107.255,    112.409,    117.195,    122.349,    127.503,    132.656,    137.81 ,   142.595,     147.748,    152.532,    157.684,    162.467,    167.25 ,    172.769,    177.551,    182.333,    187.483, 192.515, 197.301, 202.086};
	Double_t twist_protons_y[26] = {0.000528275,0.000524582,0.000504242,0.000482051,0.000456157,0.000430261,0.000404365,0.000374767,0.000345167,0.000319273,0.000289675,0.000263781,0.000236035,0.000211991,0.0001898  ,0.00016946 ,0.000150973,0.000134336,0.000119551,0.00010477 ,9.18365e-05,8.26068e-05,7.15275e-05,6.27966e-05,5.54088e-5,4.80209e-5};
	Double_t twist_protons_erry[26] = {0.000179629,0.000164814,0.000125925,7.59256e-05,5.18516e-05,2.22221e-05,1.48148e-05,1.11111e-05,1.29629e-05,1.29629e-05,1.11111e-05,1.11111e-05,1.29629e-05,7.40738e-06,9.25922e-06,9.25922e-06,7.40738e-06,7.40738e-06,7.40738e-06,5.55553e-06,9.25922e-06,7.40738e-06,7.40738e-06, 7.38784e-06, 5.54088e-06, 7.38784e-06};

	Double_t twist_protons_E[26] = {3.476,3.636,4.069,4.563,5.086,5.596,6.132,6.735,7.321,7.979,8.665,9.380,10.12,10.83,11.63,12.40,13.25,14.07,14.91,15.91,16.80,17.72,18.73, 19.75, 20.74, 21.76};
	Double_t scale_y[26] = {0};
	Double_t scale_erry[26] = {0};
	for(int j=0; j < 26; ++j) {
		scale_y[j] = twist_protons_y[j] * (938.272/twist_protons_x[j]);
		scale_erry[j] = twist_protons_erry[j] * (938.272/twist_protons_x[j]);
	}
	TGraphErrors *gTProtons = new TGraphErrors(26, twist_protons_E, scale_y, 0, scale_erry);
	gTProtons->SetFillColor(kGreen);

	Double_t twist_deuterons_x[32] = {132.773, 137.255, 142.297, 147.339, 152.941, 157.983, 162.465, 167.507, 172.549, 177.591, 182.633, 187.675, 192.717, 197.199, 202.801, 207.843, 212.885, 217.367, 222.409, 227.451, 232.493, 237.535, 242.577, 247.619, 252.661, 257.143, 262.185, 267.787, 272.269, 277.311, 282.353, 286.835}; 
	Double_t twist_deuterons_y[32] = {0.000106953, 0.000111834, 0.000114941, 0.000114053, 0.000110947, 0.000106065, 0.000101627, 9.71893e-05, 9.23077e-05, 8.87574e-05, 8.47633e-05, 8.1213e-05 , 7.76627e-05, 7.5e-05 , 7.14497e-05, 6.78994e-05, 6.52367e-05, 6.12426e-05, 5.90237e-05, 5.54734e-05, 5.28107e-05, 4.92604e-05, 4.65976e-05, 4.34911e-05, 4.03846e-05, 3.81657e-05, 3.5503e-05, 3.28402e-05, 3.06213e-05, 2.84024e-05, 2.66272e-05, 2.44083e-05}; 
	Double_t twist_deuterons_erry[32] = {4.83728e-05, 2.84024e-05, 2.04142e-05, 1.77515e-05, 1.37574e-05, 1.02071e-05, 6.21302e-06, 4.88166e-06, 4.43787e-06, 4.88166e-06, 6.21302e-06, 7.10059e-06, 6.6568e-06 , 7.54438e-06, 7.10059e-06, 7.54438e-06, 7.98817e-06, 6.6568e-06 , 7.54438e-06, 7.54438e-06, 7.54438e-06, 7.98817e-06, 8.43195e-06, 7.98817e-06, 7.54438e-06, 7.98817e-06, 7.98817e-06, 7.54438e-06, 7.98817e-06, 7.10059e-06, 7.54438e-06, 7.10059e-06}; 
	Double_t twist_deuterons_E[32] = {0};
	Double_t scale_deuteron_y[32] = {0};
	Double_t scale_deuteron_erry[32] = {0};
	for(int j=0; j<32; ++j) {
		twist_deuterons_E[j] = twist_deuterons_x[j]*twist_deuterons_x[j]/(2*1875.6);
		scale_deuteron_y[j] = twist_deuterons_y[j] * (1875.6/twist_deuterons_x[j]);
		scale_deuteron_erry[j] = twist_deuterons_erry[j] * (1875.6/twist_deuterons_x[j]);
	}
	TGraphErrors *gTDeuterons = new TGraphErrors(32, twist_deuterons_E, scale_deuteron_y, 0, scale_deuteron_erry);
	gTDeuterons->SetFillColor(kGreen-6);

	TCanvas *c = new TCanvas("c", "c");
	c->SetLogy();
	TMultiGraph *mg = new TMultiGraph();
	mg->Add(gTProtons);
	mg->Add(gTDeuterons);
	mg->Draw("A4");

	TH1D * hp = Plot("proton");
	TH1D * hd = Plot("deuteron");

	mg->GetXaxis()->SetRangeUser(4, 20);
	mg->GetYaxis()->SetTitle("Particles / muon capture / MeV");
	mg->GetXaxis()->SetTitle("E [MeV]");

//	TH1D * ht = Plot("triton");
//	TH1D * ha = Plot("alpha");


	TLegend *legend = new TLegend(0.571633, 0.636943, 0.861032, 0.872611);

	legend->AddEntry(hp, "AlCap Protons", "f");
	legend->AddEntry(hd, "AlCap Deuterons", "f");
	legend->AddEntry(gTProtons, "TWIST Protons", "f");
	legend->AddEntry(gTDeuterons, "TWIST Deuterons", "f");
	legend->Draw("SAME");
	
	c->Draw();
}
