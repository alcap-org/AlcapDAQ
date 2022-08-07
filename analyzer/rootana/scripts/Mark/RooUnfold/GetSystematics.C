void PopulateSystHist(TH1D &h1, TH1D &h2, TH1D &hSyst) {
	for(Int_t i=0; i < hSyst.GetNbinsX(); ++i) {
		if(h1.GetBinContent(i) > h2.GetBinContent(i) ) {
			Float_t diff = (h1.GetBinContent(i) - h1.GetBinError(i) ) - (h2.GetBinContent(i) + h2.GetBinError(i) );
			if(diff<0) continue;
			hSyst.SetBinContent(i, diff);
		}
		if(h1.GetBinContent(i) < h2.GetBinContent(i) ) {
			Float_t diff = (h2.GetBinContent(i) - h2.GetBinError(i) ) - (h1.GetBinContent(i) + h1.GetBinError(i) );
			if(diff<0) continue;
			hSyst.SetBinContent(i, diff);
		}
	}
}
void MisCalib(std::string target = "al50", std::string particle="proton") {
	TFile *fDown = new TFile(Form("./unfolded_misCalibDown.%s.root", target.c_str()), "READ");
	TH1D *hSiL_Down = (TH1D *)fDown->Get(Form("h%s_SiL", particle.c_str() ) );
	TH1D *hSiR_Down = (TH1D *)fDown->Get(Form("h%s_SiR", particle.c_str() ) );
	TFile *fUp = new TFile(Form("./unfolded_misCalibUp.%s.root", target.c_str()), "READ");
	TH1D *hSiL_Up = (TH1D *)fUp->Get(Form("h%s_SiL", particle.c_str() ) );
	TH1D *hSiR_Up = (TH1D *)fUp->Get(Form("h%s_SiR", particle.c_str() ) );

	TFile *fOutput = new TFile(Form("%s-unfolded-systematics.root", target.c_str() ), "UPDATE");
	TH1D *hMisCalibErrSiL = new TH1D(Form("h%s_MisCalibErrSiL", particle.c_str() ), Form("h%s_MisCalibErrSiL;E[MeV]", particle.c_str() ), 50, 0, 25);
	TH1D *hMisCalibErrSiR = new TH1D(Form("h%s_MisCalibErrSiR", particle.c_str() ), Form("h%s_MisCalibErrSiR;E[MeV]", particle.c_str() ), 50, 0, 25); 
	for(Int_t i=0; i < hMisCalibErrSiL->GetNbinsX(); ++i) {
//		hMisCalibErrSiL->SetBinContent(i, TMath::Abs(hSiL_Down->GetBinContent(i) - hSiL_Up->GetBinContent(i) ) );
//		hMisCalibErrSiR->SetBinContent(i, TMath::Abs(hSiR_Down->GetBinContent(i) - hSiR_Up->GetBinContent(i) ) );
		PopulateSystHist(*hSiL_Down, *hSiL_Up, *hMisCalibErrSiL);
		PopulateSystHist(*hSiR_Down, *hSiR_Up, *hMisCalibErrSiR);
	}
	fOutput->Write();
	fOutput->Close();
}

void NumIter(std::string target = "al50", std::string particle="proton") {
	const char *data = "/data/ssd3/R15bAnalysis/hists";
	TFile *f4 = new TFile(Form("%s/%s-unfolded.root", data, target.c_str() ), "READ");
	TH1D *hSiL_4 = (TH1D *)f4->Get(Form("h%s_SiL", particle.c_str() ) );
	TH1D *hSiR_4 = (TH1D *)f4->Get(Form("h%s_SiR", particle.c_str() ) );
	TFile *f20 = new TFile(Form("./unfolded.%s.iter20.root", target.c_str() ), "READ");
	TH1D *hSiL_20 = (TH1D *)f20->Get(Form("h%s_SiL", particle.c_str() ) );
	TH1D *hSiR_20 = (TH1D *)f20->Get(Form("h%s_SiR", particle.c_str() ) );
	TFile *fOutput = new TFile(Form("%s-unfolded-systematics.root", target.c_str() ), "UPDATE");
	TH1D *hNumIterErrSiL = new TH1D(Form("h%s_NumIterErrSiL", particle.c_str() ), Form("h%s_NumIterErrSiL;E[MeV]", particle.c_str() ), 50, 0, 25);
	TH1D *hNumIterErrSiR = new TH1D(Form("h%s_NumIterErrSiR", particle.c_str() ), Form("h%s_NumIterErrSiR;E[MeV]", particle.c_str() ), 50, 0, 25); 
	for(Int_t i=0; i < hNumIterErrSiL->GetNbinsX(); ++i) {
		hNumIterErrSiL->SetBinContent(i, TMath::Abs(hSiL_4->GetBinContent(i) - hSiL_20->GetBinContent(i) ) );
		hNumIterErrSiR->SetBinContent(i, TMath::Abs(hSiR_4->GetBinContent(i) - hSiR_20->GetBinContent(i) ) );
//		PopulateSystHist(*hSiL_4, *hSiL_20, *hNumIterErrSiL);
//		PopulateSystHist(*hSiR_4, *hSiR_20, *hNumIterErrSiR);
	}
	fOutput->Write();
	fOutput->Close();
}

void BinSize(std::string target = "al50", std::string particle="proton") {
	const char *data = "/data/ssd3/R15bAnalysis/hists";
	TFile *f500 = new TFile(Form("%s/%s-unfolded.root", data, target.c_str() ), "READ");
	TH1D *hSiL_500 = (TH1D *)f500->Get(Form("h%s_SiL", particle.c_str() ) );
	TH1D *hSiR_500 = (TH1D *)f500->Get(Form("h%s_SiR", particle.c_str() ) );
	TFile *f100 = new TFile(Form("./unfolded.%s.100keVBins.root", target.c_str() ), "READ");
	TH1D *hSiL_100 = (TH1D *)f100->Get(Form("h%s_SiL", particle.c_str() ) ); 
	TH1D *hSiR_100 = (TH1D *)f100->Get(Form("h%s_SiR", particle.c_str() ) ); 
	hSiL_100->Rebin(5);
	hSiR_100->Rebin(5);
	TFile *fOutput = new TFile(Form("%s-unfolded-systematics.root", target.c_str() ), "UPDATE");
	TH1D *hBinSizeErrSiL = new TH1D(Form("h%s_BinSizeErrSiL", particle.c_str() ), Form("h%s_BinSizeErrSiL;E[MeV]", particle.c_str() ), 50, 0, 25);
	TH1D *hBinSizeErrSiR = new TH1D(Form("h%s_BinSizeErrSiR", particle.c_str() ), Form("h%s_BinSizeErrSiR;E[MeV]", particle.c_str() ), 50, 0, 25); 
	for(Int_t i=0; i < hBinSizeErrSiL->GetNbinsX(); ++i) {
		hBinSizeErrSiL->SetBinContent(i, TMath::Abs(hSiL_100->GetBinContent(i) - hSiL_500->GetBinContent(i) ) );
		hBinSizeErrSiR->SetBinContent(i, TMath::Abs(hSiR_100->GetBinContent(i) - hSiR_500->GetBinContent(i) ) );
//		PopulateSystHist(*hSiL_100, *hSiL_500, *hBinSizeErrSiL);
//		PopulateSystHist(*hSiR_100, *hSiR_500, *hBinSizeErrSiR);
	}
	fOutput->Write();
	fOutput->Close();
}
/*
 * alpha beam energy uncertainty with midLowE" is for muon beam positions with 100 keV lower beam energy (closer to front of target),
 * and "midHighE" is the opposited (higher energy, closer to the back).
 * only need to process right detector, left one is invalid due to whatever reason
 */
void AlphaBeamEnergy(std::string target = "al50") {
	TFile *fDown = new TFile(Form("./unfolded.%s.alphas.systematics.root", target.c_str()), "READ");
	TH1D *hSiR_lowE = (TH1D *)fDown->Get("halpha_SiR_lowE");
	TH1D *hSiR_highE = (TH1D *)fDown->Get("halpha_SiR_highE");

	TFile *fOutput = new TFile(Form("%s-unfolded-systematics.root", target.c_str() ), "UPDATE");
	TH1D *hAlphaBeamEnergySiR = new TH1D("halpha_BeamEnergyErrSiR", "halpha_BeamEnergyErrSiR;E[MeV]", 50, 0, 25); 
	for(Int_t i=0; i < hSiR_lowE->GetNbinsX(); ++i) {
		PopulateSystHist(*hSiR_lowE, *hSiR_highE, *hAlphaBeamEnergySiR);
		hAlphaBeamEnergySiR->SetBinContent(i, TMath::Abs(hSiR_lowE->GetBinContent(i) - hSiR_highE->GetBinContent(i) ) );
	}
	fOutput->Write();
	fOutput->Close();

}

/*
 * I would delete the old output files before running again
 */
void GetSystematics() {
//	std::cout << "options: MisCalib, NumIter, BinSize" << std::endl;
//	if(strcmp(option, "MisCalib") == 0) {
		MisCalib("al50", "proton");
		MisCalib("al50", "deuteron");
		MisCalib("al50", "triton");
		MisCalib("al50", "alpha");
		MisCalib("al100", "proton");
		MisCalib("al100", "deuteron");
		MisCalib("al100", "triton");
		MisCalib("al100", "alpha");
		MisCalib("ti50", "proton");
		MisCalib("ti50", "deuteron");
		MisCalib("ti50", "triton");
		MisCalib("ti50", "alpha");
//	} else if(strcmp(option, "NumIter") == 0) {
		NumIter("al50", "proton");
		NumIter("al50", "deuteron");
		NumIter("al50", "triton");
		NumIter("al50", "alpha");
		NumIter("al100", "proton");
		NumIter("al100", "deuteron");
		NumIter("al100", "triton");
		NumIter("al100", "alpha");
		NumIter("ti50", "proton");
		NumIter("ti50", "deuteron");
		NumIter("ti50", "triton");
		NumIter("ti50", "alpha");
//	} else if(strcmp(option, "BinSize") == 0) {
		BinSize("al50", "proton");
		BinSize("al50", "deuteron");
		BinSize("al50", "triton");
		BinSize("al50", "alpha");
		BinSize("al100", "proton");
		BinSize("al100", "deuteron");
		BinSize("al100", "triton");
		BinSize("al100", "alpha");
		BinSize("ti50", "proton");
		BinSize("ti50", "deuteron");
		BinSize("ti50", "triton");
		BinSize("ti50", "alpha");
//	}
		AlphaBeamEnergy("al50");
		AlphaBeamEnergy("ti50");
}
