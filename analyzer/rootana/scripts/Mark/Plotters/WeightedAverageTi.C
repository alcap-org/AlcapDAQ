TH1D * Plot(const char *particle = "proton", Bool_t drawFit = kFALSE) {
	//statistical errors
	TFile *fUnfoldedTi50_stat = new TFile(Form("%s/unfolded.ti50.root", getenv("R15b_OUT") ) );
	TH1D *hSiR_ti50_stat = (TH1D *) fUnfoldedTi50_stat->Get(Form("h%s_SiR", particle) );
	TH1D *hSiL_ti50_stat = (TH1D *) fUnfoldedTi50_stat->Get(Form("h%s_SiL", particle) );

	//systematic errors
	TFile *fUnfoldedTi50 = new TFile(Form("%s/ti50-systematics.root", getenv("R15b_OUT") ) );
	TH1D *hSiR_ti50 = (TH1D *) fUnfoldedTi50->Get(Form("h%s_SiR_ti50", particle) );
	TH1D *hSiL_ti50 = (TH1D *) fUnfoldedTi50->Get(Form("h%s_SiL_ti50", particle) );

	std::string capitalized = std::string(particle);
	capitalized[0] = toupper(capitalized[0]);
	TH1D *hWeightedAverage= new TH1D("hWeightedAverage", ";E[MeV];Particles / muon capture / MeV", 40, 0, 20);
	hWeightedAverage->GetYaxis()->SetMaxDigits(3);
	TH1D *hStats= new TH1D("hStats", ";E[MeV];Particles / muon capture / MeV", 40, 0, 20);
	hStats->GetYaxis()->SetMaxDigits(3);
	hStats->SetLineWidth(2);

	for(int i = 9; i <= 40; ++i) { //lower than the 9th bin, the uncertainty are great due to many reasons
	
		//syst errors from unfolded.{ti50, al100}.root
		Double_t stat_a = hSiR_ti50_stat->GetBinError(i);
		Double_t stat_b = hSiL_ti50_stat->GetBinError(i);

		Double_t stat_reciprocal_sum_of_squares = 1/TMath::Power(stat_a, 2) + 1/TMath::Power(stat_b, 2);
		Double_t stat_variance = 1/stat_reciprocal_sum_of_squares;

		//stat and syst combined errors from Systematics.C/Systematics100.C
		Double_t err_a = hSiR_ti50->GetBinError(i);
		Double_t err_b = hSiL_ti50->GetBinError(i);

		Double_t reciprocal_sum_of_squares = 1/TMath::Power(err_a, 2) + 1/TMath::Power(err_b, 2);

		Double_t a = hSiR_ti50->GetBinContent(i);
		Double_t b = hSiL_ti50->GetBinContent(i);

		Double_t mean = (a*(1/TMath::Power(err_a, 2) ) + b*(1/TMath::Power(err_b, 2) ) ) / reciprocal_sum_of_squares;
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
	if(strcmp(particle, "proton") == 0) {
		hWeightedAverage->Draw("E2");
	} else {
		hWeightedAverage->Draw("E2 SAME");
	}
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
	if(drawFit) TFitResultPtr fitResult = hWeightedAverage->Fit("fit", "RS");
	printf("%s : %.2fe^{-(x)%.2f} + %.2fe^{-(x-%.2f)%.2f\n", particle, 1e3*fit->GetParameter(0), 1e3*fit->GetParameter(2), 1e3*fit->GetParameter(3), 1e3*fit->GetParameter(4), 1e3*fit->GetParameter(5) );
//	fit->Draw("SAME"); fit->SetLineColor(kBlack); fit->SetLineStyle(kDashed);
//	fit2->Draw("SAME"); fit2->SetLineColor(kBlue);
	
//	TLegend *legend = new TLegend(0.429799, 0.505308, 0.855301, 0.85138);
//	legend->SetHeader("#splitline{AlCap Al #rightarrow p}{inverse variance weighted average}");
//	legend->AddEntry(fit, "ae^{-(x-b)c} + de^{-(x-f)g}", "l");
//	legend->AddEntry("0", "#Chi^{2}/NDF = 907/26", "");
//	legend->AddEntry(fit2, "ae^{-xb}", "l");
//	legend->AddEntry("0", " #Chi^{2}/NDF = 3785/30", "");
//	legend->Draw("SAME");

	if(drawFit) {
		if(strcmp(particle, "proton") == 0) {
			Double_t stats_error, syst_error;
			hStats->IntegralAndError(hStats->FindBin(4), hStats->FindBin(18), stats_error);
			std::cout << "4 - 18MeV: " << hWeightedAverage->IntegralAndError(hWeightedAverage->FindBin(4), hWeightedAverage->FindBin(18.), syst_error ) << " ± " << stats_error << " ± " << (syst_error-stats_error )<< std::endl;
			std::cout << "Fit 4 - 40MeV: " << fit->Integral(4, 40) << " ± ";

	//		auto covMatrix = fitResult->GetCovarianceMatrix();
	//		Double_t sigma_integral = fit->IntegralError(4, 40, fitResult->GetParams(), covMatrix.GetMatrixArray() );
	//		std::cout << sigma_integral << std::endl;
		} else if(strcmp(particle, "deuteron") == 0) {
			Double_t stats_error, syst_error;
			hStats->IntegralAndError(hStats->FindBin(5), hStats->FindBin(18), stats_error);
			std::cout << "5 - 18MeV: " << hWeightedAverage->IntegralAndError(hWeightedAverage->FindBin(5), hWeightedAverage->FindBin(18.), syst_error ) << " ± " << stats_error << " ± " << (syst_error-stats_error )<<  std::endl;
			std::cout << "Fit 5 - 40MeV: " << fit->Integral(5, 40) << std::endl;
		} else if(strcmp(particle, "triton") == 0) {
			Double_t stats_error, syst_error;
			hStats->IntegralAndError(hStats->FindBin(6), hStats->FindBin(18), stats_error);
			std::cout << "6 - 18MeV: " << hWeightedAverage->IntegralAndError(hWeightedAverage->FindBin(6), hWeightedAverage->FindBin(18.), syst_error ) << " ±"  << stats_error << " ± " << (syst_error-stats_error )<<  std::endl;
			std::cout << "Fit 6 - 40MeV: " << fit->Integral(6, 40) << std::endl;
		} else {
			Double_t stats_error, syst_error;
			hStats->IntegralAndError(hStats->FindBin(15), hStats->FindBin(19), stats_error);
			std::cout << "15 - 18MeV: " << hWeightedAverage->IntegralAndError(hWeightedAverage->FindBin(15), hWeightedAverage->FindBin(19.), syst_error ) << " ± " << stats_error << " ± " << (syst_error-stats_error )<<  std::endl;
			std::cout << "Fit 15 - 40MeV: " << fit->Integral(15, 40) << std::endl;
		}
	}
	return hWeightedAverage;
}

void WeightedAverageTi() {

	TCanvas *c = new TCanvas("c", "c");
	c->SetLogy();
	TH1D * hp = Plot("proton");
	TH1D * hd = Plot("deuteron");
	TH1D * ht = Plot("triton");
	TH1D * ha = Plot("alpha");

	TLegend *legend = new TLegend(0.133238, 0.144374, 0.406877, 0.394904);
	legend->AddEntry(hp, "protons", "f");
	legend->AddEntry(hd, "deuterons", "f");
	legend->AddEntry(ht, "tritons", "f");
	legend->AddEntry(ha, "alphas", "f");
	legend->Draw("SAME");
	
	c->Draw();
	c->SaveAs(Form("%s/AlCapData_Ti_weighted_average.png", getenv("R15b_OUT") ) );
}
