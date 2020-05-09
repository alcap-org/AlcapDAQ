void WeightedAverage() {
	TFile *fUnfoldedAl50 = new TFile(Form("%s/al50-systematics.root", getenv("R15b_OUT") ) );
	TH1D *hproton_SiR_al50 = (TH1D *) fUnfoldedAl50->Get("hproton_SiR_al50");
	TH1D *hproton_SiL_al50 = (TH1D *) fUnfoldedAl50->Get("hproton_SiL_al50");
	TFile *fUnfoldedAl100= new TFile(Form("%s/al100-systematics.root", getenv("R15b_OUT") ) );
	TH1D *hproton_SiR_al100 = (TH1D *) fUnfoldedAl100->Get("hproton_SiR_al100");
	TH1D *hproton_SiL_al100 = (TH1D *) fUnfoldedAl100->Get("hproton_SiL_al100");

	TH1D *hWeightedAverage = new TH1D("hWeightedAverage", ";E[MeV];protons per captured muons / 0.5 MeV", 50, 0, 25);
	hWeightedAverage->GetYaxis()->SetMaxDigits(3);
	for(int i = 1; i <= 50; ++i) {
		Double_t err_a = hproton_SiR_al50->GetBinError(i);
		Double_t err_b = hproton_SiL_al50->GetBinError(i);
		Double_t err_c = hproton_SiR_al100->GetBinError(i);
		Double_t err_d = hproton_SiL_al100->GetBinError(i);

		Double_t reciprocal_sum_of_squares = 1/TMath::Power(err_a, 2) + 1/TMath::Power(err_b, 2) + 1/TMath::Power(err_c, 2) + 1/TMath::Power(err_d, 2);

		Double_t a = hproton_SiR_al50->GetBinContent(i);
		Double_t b = hproton_SiL_al50->GetBinContent(i);
		Double_t c = hproton_SiR_al100->GetBinContent(i);
		Double_t d = hproton_SiL_al100->GetBinContent(i);

		Double_t mean = (a*(1/TMath::Power(err_a, 2) ) + b*(1/TMath::Power(err_b, 2) ) + c*(1/TMath::Power(err_c, 2) ) + d*(1/TMath::Power(err_d, 2) ) ) / reciprocal_sum_of_squares;
		Double_t variance = 1 / reciprocal_sum_of_squares;	

		if(TMath::IsNaN(mean) ) {
			mean = 0;
			variance = 0;
		}
		if(i > 32 && i <= 40) {
			mean = a;
			variance = err_a*err_a;
		}
		hWeightedAverage->SetBinContent(i, mean);
		hWeightedAverage->SetBinError(i, TMath::Sqrt(variance) );
	}
        //currently no physical motivation for this function
        TF1 *fit = new TF1("fit", "[0]*exp(-(x-[1])*[2]) + [3]*exp(-(x-[4])*[5])", 4, 20);
        fit->SetParameter(0, 5.49525e-03);
        fit->SetParameter(1, 3.44865e+00);
//fit->FixParameter(1, 4);
        fit->SetParameter(2, 5.91451e-01);
        fit->SetParameter(3, 1.21979e-03);
        fit->SetParameter(4, 7.54185e+00);
//fit->FixParameter(4, 8);
        fit->SetParameter(5, 1.79100e-01);
        fit->SetParLimits(0, 0, 1000);

	hWeightedAverage->Fit("fit", "RS");

	TF1 *fit2 = new TF1("fit2", "[0]*exp(-x*[1])", 4, 20);
	fit2->SetParameter(0, 4.96668e-03);
	fit2->SetParameter(1, 1.79567e-01);

	hWeightedAverage->Fit("fit2", "RS");

	hWeightedAverage->Draw();
	fit->Draw("SAME"); fit->SetLineColor(kRed);
	fit2->Draw("SAME"); fit2->SetLineColor(kBlue);
	
	TLegend *legend = new TLegend(0.429799, 0.505308, 0.855301, 0.85138);
	legend->SetHeader("#splitline{AlCap Al #rightarrow p}{inverse variance weighted average}");
	legend->AddEntry(fit, "ae^{-(x-b)c} + de^{-(x-f)g}", "l");
	legend->AddEntry("0", "#Chi^{2}/NDF = 907/26", "");
	legend->AddEntry(fit2, "ae^{-xb}", "l");
	legend->AddEntry("0", " #Chi^{2}/NDF = 3785/30", "");
	legend->Draw("SAME");

	std::cout << "4 - 8MeV: " << hWeightedAverage->Integral(hWeightedAverage->FindBin(4), hWeightedAverage->FindBin(7.5) ) << std::endl;
	std::cout << "3.5 - 10MeV: " << hWeightedAverage->Integral(hWeightedAverage->FindBin(3.5), hWeightedAverage->FindBin(9.5) ) << std::endl;
	std::cout << "Fit 4 - 20MeV: " << fit->Integral(3.5, 20)/.5 << std::endl;
}
