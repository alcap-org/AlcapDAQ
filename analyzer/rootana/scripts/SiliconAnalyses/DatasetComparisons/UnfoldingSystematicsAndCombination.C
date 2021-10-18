void HistToGraph(TH1D &h, TGraphErrors &g, Int_t llimit, Int_t ulimit) {
	Float_t xerr = 0.25;
	Int_t j =0;
	for(Int_t i = 1; i <= h.GetNbinsX(); i++ ) {     
		if(i >= llimit && i <= ulimit) {
			g.SetPoint(j, h.GetBinCenter(i), h.GetBinContent(i) );
			g.SetPointError(j, xerr, h.GetBinError(i) );
			++j;
		}
	}
}

void Process(const char * target = "al50", const char * particle = "proton") {
	Float_t xerr = 0.25;
	const char * path = "/data/ssd3/R15bAnalysis/hists";
	TFile *fErr = new TFile(Form("%s/%s-unfolded-systematics.root", path, target), "READ");
	TFile *fNominal = new TFile(Form("%s/%s-unfolded.root", path, target), "READ");
	//right
	TH1D *hr = (TH1D *)fNominal->Get(Form("h%s_%s", particle, "SiR") );
	TH1D *hr_MisCalibErr = (TH1D *)fErr->Get(Form("h%s_MisCalibErr%s", particle, "SiR") );
	hr_MisCalibErr->SetLineColor(kRed);
	TH1D *hr_NumIterErr = (TH1D *)fErr->Get(Form("h%s_NumIterErr%s", particle, "SiR") );
	hr_NumIterErr->SetLineColor(kBlue);
	TH1D *hr_BinSizeErr = (TH1D *)fErr->Get(Form("h%s_BinSizeErr%s", particle, "SiR") );
	hr_BinSizeErr->SetLineColor(kGreen+3);

	TH1D *hr_BeamEnergyErr = (TH1D *)fErr->Get(Form("h%s_BeamEnergyErr%s", particle, "SiR") );

	TH1D *hrWithErr = (TH1D *)hr->Clone();
	hrWithErr->SetTitle(Form("%s %s %s", target, "SiR", particle) );
	for(Int_t i=0; i < hrWithErr->GetNbinsX(); ++i) {
		Double_t quadrature = TMath::Sqrt(
						  TMath::Power(hr_MisCalibErr->GetBinContent(i)/hr->GetBinContent(i), 2) +
						  TMath::Power(hr_NumIterErr->GetBinContent(i)/hr->GetBinContent(i), 2) +
						  TMath::Power(hr_BinSizeErr->GetBinContent(i)/hr->GetBinContent(i), 2) );
		if((strcmp(target, "al50")==0 || strcmp(target, "ti50")==0 ) && strcmp(particle, "alpha") == 0) {
			quadrature = TMath::Sqrt(
						 //						 TMath::Power(hr_MisCalibErr->GetBinContent(i)/hr->GetBinContent(i), 2) +
						 //						 TMath::Power(hr_NumIterErr->GetBinContent(i)/hr->GetBinContent(i), 2) +
						 //						 TMath::Power(hr_BinSizeErr->GetBinContent(i)/hr->GetBinContent(i), 2)) +
						 TMath::Power(hr_BeamEnergyErr->GetBinContent(i)/hr->GetBinContent(i), 2) );
		}
		//		hrWithErr->SetBinError(i, hrWithErr->GetBinError(i) + quadrature*hr->GetBinContent(i) );
		hrWithErr->SetBinError(i, std::sqrt(hrWithErr->GetBinError(i)*hrWithErr->GetBinError(i)
						    + quadrature*hr->GetBinContent(i)*quadrature*hr->GetBinContent(i)) );
	}

	//left
	TH1D *hl = (TH1D *)fNominal->Get(Form("h%s_%s", particle, "SiL") );
	TH1D *hl_MisCalibErr = (TH1D *)fErr->Get(Form("h%s_MisCalibErr%s", particle, "SiL") );
	hl_MisCalibErr->SetLineColor(kRed);
	TH1D *hl_NumIterErr = (TH1D *)fErr->Get(Form("h%s_NumIterErr%s", particle, "SiL") );
	hl_NumIterErr->SetLineColor(kBlue);
	TH1D *hl_BinSizeErr = (TH1D *)fErr->Get(Form("h%s_BinSizeErr%s", particle, "SiL") );
	hl_BinSizeErr->SetLineColor(kGreen+3);

	TH1D *hlWithErr = (TH1D *)hl->Clone();
	hlWithErr->SetTitle(Form("%s %s %s", target, "SiL", particle) );
	for(Int_t i=0; i < hlWithErr->GetNbinsX(); ++i) {
		Double_t quadrature = TMath::Sqrt(
						  TMath::Power(hl_MisCalibErr->GetBinContent(i)/hl->GetBinContent(i), 2) +
						  TMath::Power(hl_NumIterErr->GetBinContent(i)/hl->GetBinContent(i), 2) +
						  TMath::Power(hl_BinSizeErr->GetBinContent(i)/hl->GetBinContent(i), 2) );
		//		hlWithErr->SetBinError(i, hlWithErr->GetBinError(i) + quadrature*hl->GetBinContent(i));
		hlWithErr->SetBinError(i, std::sqrt(hlWithErr->GetBinError(i)*hlWithErr->GetBinError(i)
						    + quadrature*hl->GetBinContent(i)*quadrature*hl->GetBinContent(i)));
	}

	TGraphErrors gr, gl, gr_err, gl_err;
	gr_err.SetTitle(";E[MeV];Counts/bin");
	gr_err.SetLineWidth(2);
	gr.SetLineWidth(2);
	gl_err.SetLineWidth(2);
	gl.SetLineWidth(2);

	//elog:493, 494, determining min and max bins to be shown for the paper
	//rmin, rmax, lmin, lmax
	Int_t limits[4] = {8, 40, 7, 30}; 
	if(strcmp(particle, "deuteron") == 0 ) {
		limits[0] = 11;
		limits[1] = 36; 
		limits[2] = 10;
		limits[3] = 36;
	} else if(strcmp(particle, "triton") == 0 ) {
		limits[0] = 12;
		limits[1] = 36; 
		limits[2] = 11;
		limits[3] = 36;
	} else if(strcmp(particle, "alpha") == 0 ) {
		limits[0] = 31;
		limits[1] = 40; 
		limits[2] = 29;
		limits[3] = 40;
	}
	//scaling with lifetime, sig2, capture rate, num of muons
	Double_t scale = 1/(0.95*0.63*0.609*1.62E+8);
	if(strcmp(target, "al100") == 0) scale = 1/(0.95*0.63*0.609*1.34E+8);
	if(strcmp(target, "ti50") == 0 ) scale = 1/(0.95*0.2968*0.8529*9.60E+7);
	hr->Scale(scale);
	hl->Scale(scale);
	hrWithErr->Scale(scale);
	hlWithErr->Scale(scale);

	std::string lvalid = "SiL 95% threshold from " + std::to_string(limits[2]/2.-.5) + " to " + std::to_string(limits[3]/2.-.5);
	std::string rvalid = "SiR 95% threshold from " + std::to_string(limits[0]/2.-.5) + " to " + std::to_string(limits[1]/2.-.5);

	//just for TWIST comparison, need to rebin to 1MeV per bin
	//hr->Rebin(2);
	//hl->Rebin(2);
	//hrWithErr->Rebin(2);
	//hlWithErr->Rebin(2);

	HistToGraph(*hr, gr, limits[0], limits[1]);
	HistToGraph(*hl, gl, limits[2], limits[3]);
	HistToGraph(*hrWithErr, gr_err, limits[0], limits[1]);
	HistToGraph(*hlWithErr, gl_err, limits[2], limits[3]);

	//draw combined plots, combined with weighted inverse error
	TGraphErrors gCombined;
	gCombined.SetName(Form("gCombined-%s-%s", target, particle) );
	Int_t j =0;
	int minr = limits[0];
	int maxr = limits[1];
	int minl = limits[2];
	int maxl = limits[3];
	bool use_left_arm = false;
	bool use_right_arm = false;
	for(Int_t i = 1; i <= hrWithErr->GetNbinsX(); i++ ) {
		if(hrWithErr->GetBinContent(i) == 0 || hlWithErr->GetBinContent(i)==0 ) continue;

		if (i < minr || i > maxr) { use_right_arm = false; }
		else { use_right_arm = true; }
		if (i < minl || i > maxl) { use_left_arm = false; }
		else { use_left_arm = true; }

		//		std::cout << "#" << i << ": " << use_left_arm << " " << use_right_arm << std::endl;
		//		if(i > limits[1] || i < limits[2]) continue;
		if (!use_right_arm && !use_left_arm) { continue; }
		//		if(i == limits[2]) {
		else if (!use_right_arm && use_left_arm) {
			//only left detector
		  if(!(strcmp(particle, "alpha") == 0 )) {
			gCombined.SetPoint(j, hlWithErr->GetBinCenter(i), hlWithErr->GetBinContent(i) );
			gCombined.SetPointError(j, xerr, hlWithErr->GetBinError(i) );
		  }
		}
		//		} else if(i > limits[2] && i <= limits[3]) {
		else if (use_right_arm && use_left_arm) {
			Float_t invRErr = TMath::Power(1./hrWithErr->GetBinError(i), 2);
			Float_t invLErr = TMath::Power(1./hlWithErr->GetBinError(i), 2);
			Float_t weightedAvg = (hrWithErr->GetBinContent(i)*invRErr + hlWithErr->GetBinContent(i)*invLErr)/(invRErr+invLErr);        
			Float_t errors = TMath::Sqrt(TMath::Power(hrWithErr->GetBinError(i)*invRErr, 2) + TMath::Power(hlWithErr->GetBinError(i)*invLErr, 2) )/(invRErr+invLErr);
			//			std::cout << "L Error = " << hlWithErr->GetBinError(i) << ", R Error = " << hrWithErr->GetBinError(i) << ", Combined Err = " << errors << std::endl;
			if(strcmp(particle, "alpha") == 0 ) {
				weightedAvg = hrWithErr->GetBinContent(i);
				errors = hrWithErr->GetBinError(i);
			}
			gCombined.SetPoint(j, hrWithErr->GetBinCenter(i), weightedAvg);
			gCombined.SetPointError(j, xerr, errors);
		  }
		  //else if(i > limits[3]) {
		  else if (use_right_arm && !use_left_arm) {
			//only right detector 
			gCombined.SetPoint(j, hrWithErr->GetBinCenter(i), hrWithErr->GetBinContent(i) );
			gCombined.SetPointError(j, xerr, hrWithErr->GetBinError(i) );
		  }
		++j;
	}
	//statistical only
	TGraphErrors gStats;
	gStats.SetName(Form("gStats-%s-%s", target, particle) );
	Int_t k =0;
	for(Int_t i = 1; i <= hr->GetNbinsX(); i++ ) {
		if(hr->GetBinContent(i) == 0 || hl->GetBinContent(i)==0 ) continue;

		if (i < minr || i > maxr) { use_right_arm = false; }
		else { use_right_arm = true; }
		if (i < minl || i > maxl) { use_left_arm = false; }
		else { use_left_arm = true; }

		//if(i > limits[1] || i < limits[2]) continue;
		if (!use_right_arm && !use_left_arm) { continue; }
		//if(i == limits[2]) {
		else if (!use_right_arm && use_left_arm) {
		  if(!(strcmp(particle, "alpha") == 0 )) {
			gStats.SetPoint(k, hl->GetBinCenter(i), hl->GetBinContent(i) );
			gStats.SetPointError(k, xerr, hl->GetBinError(i) );
		  }
		  //		} else if(i > limits[2] && i <= limits[3]) {
		} else if (use_right_arm && use_left_arm) {
			//take the central value from gCombined, errors only from statistical and other corrections
			Float_t invRErr = TMath::Power(1./hr->GetBinError(i), 2);
			Float_t invLErr = TMath::Power(1./hl->GetBinError(i), 2);
			Float_t weightedAvg = (hr->GetBinContent(i)*invRErr + hl->GetBinContent(i)*invLErr)/(invRErr+invLErr);
			Float_t errors = TMath::Sqrt(TMath::Power(hr->GetBinError(i)*invRErr, 2) + TMath::Power(hl->GetBinError(i)*invLErr, 2) )/(invRErr+invLErr);
			if(strcmp(particle, "alpha") == 0) {
				weightedAvg = hr->GetBinContent(i);
				errors = hr->GetBinError(i);
			}
			gStats.SetPoint(k, hr->GetBinCenter(i), gCombined.GetY()[k] );
			gStats.SetPointError(k, xerr, errors);
			//		} else if(i > limits[3]) {
		} else if (use_right_arm && !use_left_arm) {
			gStats.SetPoint(k, hr->GetBinCenter(i), hr->GetBinContent(i) );
			gStats.SetPointError(k, xerr, hr->GetBinError(i) );
		}
		++k;
	}

	//TFile fFinal(Form("%s/final-%s-plots.root", path, target), "UPDATE");
	//TFile fFinal(Form("%s/final-%s-plots.root", "/home/m-wong/Analysis/Plotters", target), "UPDATE");
	TFile fFinal(Form("%s/final-%s-plots.root", ".", target), "UPDATE");
	gCombined.Write();
	gStats.Write();
	fFinal.Write();
	fFinal.Close();
}
void UnfoldingSystematicsAndCombination() {
	Process("al50", "proton");
	Process("al50", "deuteron");
	Process("al50", "triton");
	Process("al50", "alpha");
	Process("al100", "proton");
	Process("al100", "deuteron");
	Process("al100", "triton");
	Process("al100", "alpha");
	Process("ti50", "proton");
	Process("ti50", "deuteron");
	Process("ti50", "triton");
	Process("ti50", "alpha");
}
