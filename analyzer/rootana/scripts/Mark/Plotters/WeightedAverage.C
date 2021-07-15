TH1D * Plot(TH1D *hStats, const char *particle = "proton", Bool_t drawFit = kTRUE) {
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
	TH1D *hWeightedAverage= new TH1D("hWeightedAverage", ";Energy [MeV];Particles / muon capture / MeV", 40, 0, 20);
	hWeightedAverage->GetYaxis()->SetMaxDigits(3);
	//TH1D *hStats= new TH1D("hStats", ";Energy [MeV];Particles / muon capture / MeV", 40, 0, 20);
	hStats->GetYaxis()->SetMaxDigits(3);
	hStats->SetLineWidth(2);

	for(int i = 1; i <= 40; ++i) { //lower than the 9th bin, the uncertainty are great due to many reasons
	
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

		//weighted mean
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
		fit = new TF1("fit", "[0]*exp(-(x)/[1])", 3, 17);
		fit->SetParameter(1, 1);
        //	fit = new TF1("fit", "[0]*exp(-(x)*[2]) + [3]*exp(-(x-[4])*[5])", 4, 20);
	//	fit->SetParameter(0, 5.49525e-03);
	//	fit->SetParameter(2, 5.91451e-01);
	//	fit->SetParameter(3, 1.21979e-03);
	//	fit->SetParameter(4, 7.54185e+00);
	//	fit->SetParameter(5, 1.79100e-01);
	//	fit->SetParLimits(0, 0, 1000);
	} else if(strcmp(particle, "deuteron") == 0) {
		fit = new TF1("fit", "[0]*exp(-(x)/[1])", 5, 17);
		fit->SetParameter(1, 1);
	//	fit = new TF1("fit", "[0]*exp(-(x)*[2]) + [3]*exp(-(x-[4])*[5])", 5, 18);
	//	fit->SetParameter(0, 5.49525e-03);
	//	fit->SetParameter(2, 5.91451e-01);
	//	fit->SetParameter(3, 1.21979e-03);
	//	fit->SetParameter(4, 7.54185e+00);
	//	fit->SetParameter(5, 1.79100e-01);
	//	fit->SetParLimits(0, 0, 1000);
	} else if(strcmp(particle, "triton") == 0) {
		fit = new TF1("fit", "[0]*exp(-(x)/[1])", 5, 17);
		fit->SetParameter(1, 1);
	//	fit = new TF1("fit", "[0]*exp(-(x)*[2]) + [3]*exp(-(x-[4])*[5])", 6, 18);
	//	fit->SetParameter(0, 5.49525e-03);
	//	fit->SetParameter(2, 5.91451e-01);
	//	fit->SetParameter(3, 1.21979e-03);
	//	fit->SetParameter(4, 7.54185e+00);
	//	fit->SetParameter(5, 1.79100e-01);
	//	fit->SetParLimits(0, 0, 1000);
	} else {
        	fit = new TF1("fit", "[0]*exp(-(x)/[1])", 15, 20);
		fit->SetParameter(1, 1);
	}

//	hWeightedAverage->GetXaxis()->SetRangeUser(2, 20);
//	if(strcmp(particle, "proton") == 0) {
//		hWeightedAverage->Draw("LE2");
//	} else {
//		hWeightedAverage->Draw("LE2 SAME");
//	}
	if(strcmp(particle, "proton") == 0) {
//		hWeightedAverage->GetYaxis()->SetRangeUser(1e-5, 1e-1);
//		hStats->GetXaxis()->SetRangeUser(2, 20);
//		hWeightedAverage->SetLineColor(kRed);
//		hWeightedAverage->SetMarkerColor(kRed);
//		hWeightedAverage->SetMarkerStyle(kFullCircle);
	} else if(strcmp(particle, "deuteron") == 0) {
//		hWeightedAverage->GetXaxis()->SetRangeUser(5, 20);
//		hStats->GetXaxis()->SetRangeUser(3, 20);
//		hWeightedAverage->SetLineColor(kBlue);
//		hWeightedAverage->SetMarkerColor(kBlue);
//		hWeightedAverage->SetMarkerStyle(kStar);
	} else if(strcmp(particle, "triton") == 0) {
//		hWeightedAverage->GetXaxis()->SetRangeUser(6, 20);
//		hStats->GetXaxis()->SetRangeUser(3, 20);
//		hWeightedAverage->SetLineColor(kGreen);
//		hWeightedAverage->SetMarkerColor(kGreen);
//		hWeightedAverage->SetMarkerStyle(kFullSquare);
	} else {
//		hWeightedAverage->GetXaxis()->SetRangeUser(15, 20);
//		hStats->GetXaxis()->SetRangeUser(10, 20);
//		hWeightedAverage->SetLineColor(kMagenta);
//		hWeightedAverage->SetMarkerColor(kMagenta);
//		hWeightedAverage->SetMarkerStyle(kFullTriangleUp);
	}
//	hStats->Draw("LE1 SAME");
	if(drawFit) {
		TFitResultPtr fitResult = hWeightedAverage->Fit("fit", "RS");
		auto covMatrix = fitResult->GetCovarianceMatrix();
		Double_t sigma_integral = fit->IntegralError(3, 80, fitResult->GetParams(), covMatrix.GetMatrixArray() );
		std::cout << sigma_integral << std::endl;
	}
	//printf("%s : %.2fe^{-(x)%.2f}\n", particle, fit->GetParameter(0), fit->GetParameter(1) );
	//printf("%s : %.2fe^{-(x)%.2f} + %.2fe^{-(x-%.2f)%.2f\n", particle, 1e3*fit->GetParameter(0), 1e3*fit->GetParameter(2), 1e3*fit->GetParameter(3), 1e3*fit->GetParameter(4), 1e3*fit->GetParameter(5) );
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
			hStats->IntegralAndError(hStats->FindBin(3), hStats->FindBin(16), stats_error);
			std::cout << "3 - 17MeV: " << hWeightedAverage->IntegralAndError(hWeightedAverage->FindBin(3), hWeightedAverage->FindBin(16.), syst_error ) *1e3 << " ± " << stats_error*1e3 << " ± " << (syst_error-stats_error )*1e3 << std::endl;
			std::cout << "Fit 3 - 80MeV: " << fit->Integral(3, 80) << std::endl;

		} else if(strcmp(particle, "deuteron") == 0) {
			Double_t stats_error, syst_error;
			hStats->IntegralAndError(hStats->FindBin(5), hStats->FindBin(16), stats_error);
			std::cout << "5 - 17MeV: " << hWeightedAverage->IntegralAndError(hWeightedAverage->FindBin(3), hWeightedAverage->FindBin(16.), syst_error ) *1e3 << " ± " << stats_error*1e3 << " ± " << (syst_error-stats_error )*1e3 << std::endl;
			std::cout << "Fit 5 - 40MeV: " << fit->Integral(5, 40) << std::endl;
		} else if(strcmp(particle, "triton") == 0) {
			Double_t stats_error, syst_error;
			hStats->IntegralAndError(hStats->FindBin(5), hStats->FindBin(16), stats_error);
			std::cout << "5 - 17MeV: " << hWeightedAverage->IntegralAndError(hWeightedAverage->FindBin(3), hWeightedAverage->FindBin(16.), syst_error ) *1e3 << " ± " << stats_error*1e3 << " ± " << (syst_error-stats_error )*1e3 << std::endl;
			std::cout << "Fit 6 - 40MeV: " << fit->Integral(6, 40) << std::endl;
		} else {
			Double_t stats_error, syst_error;
			hStats->IntegralAndError(hStats->FindBin(15), hStats->FindBin(19), stats_error);
			std::cout << "15 - 20MeV: " << hWeightedAverage->IntegralAndError(hWeightedAverage->FindBin(15), hWeightedAverage->FindBin(19.), syst_error )*1e3 << " ± " << stats_error*1e3 << " ± " << (syst_error-stats_error )*1e3<<  std::endl;
			std::cout << "Fit 15 - 40MeV: " << fit->Integral(15, 40) << std::endl;
		}
	}
	return hWeightedAverage;
}

void WeightedAverage() {
	TH1D *hStats= new TH1D("hStats", ";Energy [MeV];Particles / muon capture / MeV", 40, 0, 20);
	TH1D *hp = Plot(hStats, "proton");
	TGraphErrors *gProton = new TGraphErrors(hp);
	TGraphErrors *gPStat = new TGraphErrors(hStats);
	for(int i=0; i<3; ++i) {
		gProton->RemovePoint(0);
		gPStat->RemovePoint(0);
	}
	gProton->SetLineColor(kRed);
	gPStat->SetMarkerStyle(kFullCircle);
	gPStat->SetMarkerColor(kRed);
	
	hStats= new TH1D("hStats", ";Energy [MeV];Particles / muon capture / MeV", 40, 0, 20);
	TH1D *hd = Plot(hStats, "deuteron");
	TGraphErrors *gDeuteron = new TGraphErrors(hd);
	TGraphErrors *gDStat = new TGraphErrors(hStats);
	for(int i=0; i<4; ++i) {
		gDeuteron->RemovePoint(0);
		gDStat->RemovePoint(0);
	}
	gDeuteron->RemovePoint(gDeuteron->GetN()-1);
	gDStat->RemovePoint(gDStat->GetN()-1);
	gDeuteron->RemovePoint(gDeuteron->GetN()-1);
	gDStat->RemovePoint(gDStat->GetN()-1);
	gDeuteron->SetLineColor(kBlue);
	gDStat->SetMarkerStyle(kStar);
	gDStat->SetMarkerColor(kBlue);

	hStats= new TH1D("hStats", ";Energy [MeV];Particles / muon capture / MeV", 40, 0, 20);
	TH1D *ht = Plot(hStats, "triton");
	TGraphErrors *gTriton = new TGraphErrors(ht);
	TGraphErrors *gTStat = new TGraphErrors(hStats);
	for(int i=0; i<5; ++i) {
		gTriton->RemovePoint(0);
		gTStat->RemovePoint(0);
	}
	gTriton->RemovePoint(gTriton->GetN()-1);
	gTStat->RemovePoint(gTStat->GetN()-1);
	gTriton->SetLineColor(kGreen);
	gTStat->SetMarkerStyle(kFullSquare);
	gTStat->SetMarkerColor(kGreen);

	hStats= new TH1D("hStats", ";Energy [MeV];Particles / muon capture / MeV", 40, 0, 20);
	TH1D *ha = Plot(hStats, "alpha");
	TGraphErrors *gAlpha = new TGraphErrors(ha);
	TGraphErrors *gAStat = new TGraphErrors(hStats);
	for(int i=0; i<13; ++i) {
		gAlpha->RemovePoint(0);
		gAStat->RemovePoint(0);
	}
	gAlpha->SetLineColor(kMagenta);
	gAStat->SetMarkerStyle(kFullTriangleUp);
	gAStat->SetMarkerColor(kMagenta);

	TMultiGraph *g = new TMultiGraph();
	g->SetTitle(";Energy [MeV];Particles / muon capture / MeV;");
	g->Add(gProton);
	g->Add(gDeuteron);
	g->Add(gTriton);
	g->Add(gAlpha);

	TMultiGraph *gs = new TMultiGraph();
	gs->SetTitle(";Energy [MeV];Particles / muon capture / MeV;");
	gs->Add(gPStat);
	gs->Add(gDStat);
	gs->Add(gTStat);
	gs->Add(gAStat);

	TCanvas *c = new TCanvas("c", "c");
	c->SetLogy();
	g->Draw("APEL");
	gs->Draw("PE");
	g->GetYaxis()->SetRangeUser(1e-5, 1e-1);
	g->GetXaxis()->SetRangeUser(2, 20);

	TLegend *legend = new TLegend(0.58596 , 0.592437, 0.859599, 0.842437);
	legend->SetHeader("AlCap Al");
	legend->AddEntry(gPStat, "protons", "pl");
	legend->AddEntry(gDStat, "deuterons", "pl");
	legend->AddEntry(gTStat, "tritons", "pl");
	legend->AddEntry(gAStat, "alphas", "pl");
	legend->Draw("SAME");
	c->Draw();
	c->SaveAs(Form("%s/AlCapData_Al_weighted_average.pdf", getenv("R15b_OUT") ) );
}
//void WeightedAverage() {
//
//	TCanvas *c = new TCanvas("c", "c");
//	c->SetLogy();
//	TH1D * hp = Plot("proton");
//	hp->GetXaxis()->SetRangeUser(2, 20);
//	TH1D * hd = Plot("deuteron");
//	TH1D * ht = Plot("triton");
//	TH1D * ha = Plot("alpha");
//
//	TLegend *legend = new TLegend(0.58596 , 0.592437, 0.859599, 0.842437);
//	legend->SetHeader("AlCap Al");
//	legend->AddEntry(hp, "protons", "pl");
//	legend->AddEntry(hd, "deuterons", "pl");
//	legend->AddEntry(ht, "tritons", "pl");
//	legend->AddEntry(ha, "alphas", "pl");
//	legend->Draw("SAME");
//	
//	c->Draw();
//	c->SaveAs(Form("%s/AlCapData_Al_weighted_average.pdf", getenv("R15b_OUT") ) );
//}
