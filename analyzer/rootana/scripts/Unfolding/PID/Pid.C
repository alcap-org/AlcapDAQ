//Some global variable for convenience
Bool_t debug = kTRUE;
Double_t window = .06;
Double_t ymin = .26; //y-limits chosen for protons
Double_t ymax = .92;
std::map<TString, std::vector<Double_t> > results;
std::map<TString, std::vector<Double_t> > results_err;
std::map<int, TH1D *> Slice(TH2D *h) {
	std::map<int, TH1D *> map;
	for(int i=0; i < 30; i++) { //up to 10MeV
		//store projections of different Lg #DeltaE+E segments in map, divided into equal portions of 1MeV
		map[i] = h->ProjectionY(Form("%.3f_%.3f_py", (i+1)/2.+.5, (i+1)/2.+1.), h->GetXaxis()->FindBin(TMath::Log10((i+1)/2.+.5) )+1, h->GetXaxis()->FindBin(TMath::Log10((i+1)/2.+1.) ) );
	}
	return map;
}
void Fit(TH1D *h, Int_t sliceIndex) {
	////
	h->GetXaxis()->SetTitle("Lg #DeltaE / #sqrt{2}");
	h->GetXaxis()->SetTitleOffset(1.2);
	h->GetYaxis()->SetTitle(Form("Counts/%f", h->GetBinWidth(0) ) );
	h->Rebin(2);

	//Use TSpectrum to find the peak candidates
	TSpectrum *s = new TSpectrum(3);
	Int_t nfound = s->Search(h, 3.3, "new", 0.25);
	Double_t *xpeaks = s->GetPositionX();
	sort(xpeaks, xpeaks+3);

	//This assumes the proton band produces the highest peak in the 2D plot
	TF1 *fit = new TF1("fit", "gaus(0)+gaus(3)+gaus(6)+pol0(9)", ymin, ymax); //rough fit window chosen
	//proton, deuteron, muon or punchthrough, in that order
	fit->SetParameters(38, xpeaks[0], 0.04, 6.8, xpeaks[1], 0.031, 56, xpeaks[2], 0.026, 1); //Initial fit parameters

	fit->SetParName(0, "Pro. Constant");
	fit->SetParName(1, "Pro. Mean");
	fit->SetParName(2, "Pro. Sigma");

	fit->SetParName(3, "Deu. Constant");
	fit->SetParName(4, "Deu. Mean");
	fit->SetParName(5, "Deu. Sigma");

	fit->SetParName(6, "Tri. Constant");
	fit->SetParName(7, "Tri. Mean");
	fit->SetParName(8, "Tri. Sigma");

	fit->SetParName(9, "Pol0");
	TFitResultPtr r = h->Fit(fit, "RS");
	TMatrixDSym cov = r->GetCovarianceMatrix();
	//Get the covariance between sigma and constant parameters after fitting
	Double_t sigmaAndConstantCovariance = cov.GetMatrixArray()[2];
	Double_t area = TMath::Sqrt(2*3.142) * fit->GetParameter(0) * fit->GetParameter(2) / h->GetBinWidth(0);
//	Double_t area_error = area * TMath::Sqrt(TMath::Power(fit->GetParError(0) / fit->GetParameter(0), 2) + TMath::Power(fit->GetParError(2) / fit->GetParameter(2), 2) + 2*(fit->GetParError(0) / fit->GetParameter(0) )*(fit->GetParError(2) / fit->GetParameter(2) )*sigmaAndConstantCovariance);
//	TLatex latex;
//	latex.DrawLatex(.35, 20, Form("Area: %.2f#pm%.2f", area, area_error) );
//	std::cout << "========" << h->GetName() << "========" << std::endl;
//	if(debug) {
//		std::cout << "Bin width:\t" << h->GetBinWidth(0)  << std::endl;
//		std::cout << "Constant:\t" << fit->GetParameter(0) << "\t±" << fit->GetParError(0) << std::endl;
//		std::cout << "Mean:\t\t" << fit->GetParameter(1) << "\t±" << fit->GetParError(1) << std::endl;
//		std::cout << "Sigma:\t\t" << fit->GetParameter(2) << "\t±" << fit->GetParError(2) << std::endl;
//		Double_t int_err;
//		std::cout << "Entries:\t" << h->IntegralAndError(h->FindBin(hmax-window), h->FindBin(hmax+window), int_err ) << "\t\t±" << int_err << "\t(plain sum over bins)" << std::endl;
//		std::cout << "Integral:\t " << area << "\t±" << area_error << "\t(c*s*sqrt(2pi) )" << std::endl;
//	}
//	std::cout << "Background:\t" << fit->GetParameter(3) * (2*window) / h->GetBinWidth(0) << "\t\t±" << fit->GetParError(3) << "\t(as reference)" << std::endl;
	results["bin_width"].push_back(h->GetBinWidth(0) );
	results["proton_constant"].push_back(fit->GetParameter(0) );
	results["proton_mean"].push_back(fit->GetParameter(1) );
	results["proton_sigma"].push_back(fit->GetParameter(2) );
	results["deuteron_constant"].push_back(fit->GetParameter(3) );
	results["deuteron_mean"].push_back(fit->GetParameter(4) );
	results["deuteron_sigma"].push_back(fit->GetParameter(5) );
	results["triton_constant"].push_back(fit->GetParameter(6) );
	results["triton_mean"].push_back(fit->GetParameter(7) );
	results["triton_sigma"].push_back(fit->GetParameter(8) );
//	results["area"].push_back(area);
	results_err["bin_width"].push_back(0.);
	results_err["constant"].push_back(fit->GetParError(0) );
	results_err["mean"].push_back(fit->GetParError(1) );
	results_err["sigma"].push_back(fit->GetParError(2) );
//	results_err["area"].push_back(area_error);
}
TCutG * DefineCut(const char *sourceName, const char *species="proton") {
	TString meanStr = Form("%s_mean", species);
	TString sigmaStr = Form("%s_sigma", species);
	Int_t mapsize = results[meanStr].size();
	Int_t sigma = 3;
	sourceName = Form("%s_%s_%dsigma", sourceName, species, sigma);
	TFile *fCut = new TFile("al50-cuts.root", "UPDATE");
	if(fCut->GetListOfKeys()->Contains(sourceName) ) {
		printf("Deleting cut with name: %s\n", sourceName);
		gDirectory->Delete(Form("%s;1", sourceName) );    
	}
	TCutG *cutg = new TCutG(sourceName, 4*mapsize+1);
	for(int i=0; i < mapsize; i++) {
		cutg->SetPoint(2*i, TMath::Log10((i+1)/2.+.5), results[meanStr][i] - sigma*results[sigmaStr][i]);
		cutg->SetPoint(2*i+1, TMath::Log10((i+1)/2.+1.), results[meanStr][i] - sigma*results[sigmaStr][i]);
	}
	for(int i=mapsize; i < 2*mapsize; i++) {
		int c = 2*mapsize-i;
		cutg->SetPoint(2*i, TMath::Log10(c/2.+1.), results[meanStr][c-1] + sigma*results[sigmaStr][c-1]);
		cutg->SetPoint(2*i+1, TMath::Log10(c/2.+.5), results[meanStr][c-1] + sigma*results[sigmaStr][c-1]);
	}
	cutg->SetPoint(4*mapsize, TMath::Log10((0+1)/2.+.5), results[meanStr][0] - sigma*results[sigmaStr][0]);
	cutg->SetLineColor(kRed);
	cutg->Write();
	fCut->Close();
	return cutg;
}
/**
  Get those Lg10 EvDeltaE plots from the TMETree processed ROOT file
  TCutg definition fine tuned and used for selecting protons for energy and time spectrum
  Plots of the Lg10 EvDeltaE plot and its projection is for checks
  The other TCanvas contains slices from 0-10 MeV (rotate 45 counter clockwise)
  The slices are put into a std::map and a gaus+pol0 fit is performed
  A rough integral or plain sum over bins are done as another check on the gaussian integral
  The difference between the Entries var and Integral var is(should be) the Background var (pol0 * fit window size and corrected with the bin size)
  */
void Pid(const char *filename="al50.root", Bool_t plotLeft=kTRUE, const char *treeName="oTree") {
	gStyle->SetOptFit(1);
	TFile *fData = new TFile(filename, "READ");
	TH2D *hLg_SiL_EvDeltaE = new TH2D("hLg_SiL_EvDeltaE", "SiL Ev#DeltaE;Lg E+#DeltaE / #sqrt{2} [MeV];Lg #DeltaE / #sqrt{2} [MeV]", 500, -0.1, 1.5, 500, -0.9, 1.5);
	TH2D *hLg_SiR_EvDeltaE = new TH2D("hLg_SiR_EvDeltaE", "SiR Ev#DeltaE;Lg E+#DeltaE / #sqrt{2} [MeV];Lg #DeltaE / #sqrt{2} [MeV]", 500, -0.1, 1.5, 500, -0.9, 1.5);
	Double_t e1, e2, e3, t1, t2;
	TString *channel = new TString("");
	TString *pid = new TString("");
	TTree *tree = (TTree *)fData->Get(treeName);
//	tree->SetBranchAddress("t1", &t1);
//	tree->SetBranchAddress("t2", &t2);
	tree->SetBranchAddress("e1", &e1);
	tree->SetBranchAddress("e2", &e2);
	tree->SetBranchAddress("e3", &e3);
	tree->SetBranchAddress("channel", &channel);
	tree->SetBranchAddress("pid", &pid);
	for(Long64_t i=0; i < tree->GetEntries(); i++) {
		tree->GetEntry(i);
//		if(t2-t1 > 1000 || t2-t1 < -1000) continue;
//		if(t2 < 400) continue;
		e1 = e1 * 0.001;
		e2 = e2 * 0.001;
		e3 = e3 * 0.001;
		if(channel->Contains("SiL1") ) {
			hLg_SiL_EvDeltaE->Fill(0.7071 * (TMath::Log10(e1+e2+e3) - TMath::Log10(e1) ), 0.7071 * (TMath::Log10(e1+e2+e3) + TMath::Log10(e1) ) );
		} else if(channel->Contains("SiR1") ) {
			hLg_SiR_EvDeltaE->Fill(0.7071 * (TMath::Log10(e1+e2+e3) - TMath::Log10(e1) ), 0.7071 * (TMath::Log10(e1+e2+e3) + TMath::Log10(e1) ) );
		}
	}
	std::map<int, TH1D*>::iterator it;
	TCanvas *c = new TCanvas("c", "c", 1440, 880);
	int i=0;
	if(plotLeft) {
		//Slicing via y-projection and storing in the std::map
		std::map<int, TH1D*> map = Slice(hLg_SiL_EvDeltaE);
		c->Divide(map.size()/4, 4);

		for(it=map.begin(); it != map.end(); it++) {
			c->cd(i+1);
			TH1D *hclone = (TH1D *) it->second->Clone();
			//Fitting of slices
			Fit(hclone, i);
			i++;
		}
		TCutG *cutg_p = DefineCut(hLg_SiL_EvDeltaE->GetName(), "proton");
		TCutG *cutg_d = DefineCut(hLg_SiL_EvDeltaE->GetName(), "deuteron");
		TCutG *cutg_t = DefineCut(hLg_SiL_EvDeltaE->GetName(), "triton");
	} else {
		//Slicing via y-projection and storing in the std::map
		std::map<int, TH1D*> map = Slice(hLg_SiR_EvDeltaE);
		c->Divide(map.size()/4, 4);

		for(it=map.begin(); it != map.end(); it++) {
			c->cd(i+1);
			TH1D *hclone = (TH1D *) it->second->Clone();
			//Fitting of slices
			Fit(hclone, i);
			i++;
		}
		TCutG *cutg_p = DefineCut(hLg_SiR_EvDeltaE->GetName(), "proton");
		TCutG *cutg_d = DefineCut(hLg_SiR_EvDeltaE->GetName(), "deuteron");
		TCutG *cutg_t = DefineCut(hLg_SiR_EvDeltaE->GetName(), "triton");
	}
}
