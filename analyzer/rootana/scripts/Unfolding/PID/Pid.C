//Some global variable for convenience
Bool_t debug = kTRUE;
Double_t window = .06;
Double_t ymin = .26; //y-limits chosen for protons
Double_t ymax = .87;
std::map<TString, std::vector<Double_t> > results;
std::map<TString, std::vector<Double_t> > results_err;
std::map<int, TH1D *> Slice(TH2D *h) {
	std::map<int, TH1D *> map;
	for(int i=0; i < 26; i++) { //SiR->27 and extra, SiL->26
		//store projections of different Lg #DeltaE+E segments in map, divided into equal portions of 1MeV; +1 because it also includes the last bin so we don't double count
		map[i] = h->ProjectionY(Form("%.3f_%.3f_py", (i+1)/2.+.5, (i+1)/2.+1.), h->GetXaxis()->FindBin(TMath::Log10((i+1)/2.+.5) )+1, h->GetXaxis()->FindBin(TMath::Log10((i+1)/2.+1.) ) );
	}
//	map[27] = h->ProjectionY("p27_py", h->GetXaxis()->FindBin(1.1613680)+1, h->GetXaxis()->FindBin(1.2) ); //switch on for SiR
//	map[28] = h->ProjectionY("p28_py", h->GetXaxis()->FindBin(1.2)+1, h->GetXaxis()->FindBin(1.3) ); //switch on for SiR
	return map;
}
void Fit(TH1D *h, Int_t sliceIndex) {
	////
	h->GetXaxis()->SetTitle("Lg #DeltaE / #sqrt{2}");
	h->GetXaxis()->SetTitleOffset(1.2);
	h->GetYaxis()->SetTitle(Form("Counts/%f", h->GetBinWidth(0) ) );

	//Use TSpectrum to find the peak candidates
	TSpectrum *s = new TSpectrum(3);
	Int_t nfound = s->Search(h, 2, "", 0.05);
	Double_t *xpeaks = s->GetPositionX();
	if(xpeaks[2] < xpeaks[1]) xpeaks[2] = 0.74;
	sort(xpeaks, xpeaks+3);

	//This assumes the proton band produces the highest peak in the 2D plot
	TF1 *fit = new TF1("fit", "gaus(0)+gaus(3)+gaus(6)+pol0(9)", ymin, ymax); //rough fit window chosen
	//proton, deuteron, muon or punchthrough, in that order
	fit->SetParameters(38, xpeaks[0], 0.04, 6.8, xpeaks[1], 0.031, 56, xpeaks[2], .022, .1); //Initial fit parameters

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

	fit->SetParLimits(1, .35, 0.5);
	fit->SetParLimits(2, 0.01, 0.5);
	fit->SetParLimits(3, 0, 100);
	fit->SetParLimits(4, .55, 0.65);
	fit->SetParLimits(5, 0.02, 0.035);
	fit->SetParLimits(6, 0, 100);
	fit->SetParLimits(7, .64, .8);
	fit->SetParLimits(8, 0.01, 0.025);
////////////////////////////////////////////////////////////////////////////////////////////////////

if(sliceIndex > 18) {	
	s = new TSpectrum(2);
	nfound = s->Search(h, 2, "", 0.05);
	xpeaks = s->GetPositionX();
	sort(xpeaks, xpeaks+2);

	//This assumes the proton band produces the highest peak in the 2D plot
	fit = new TF1("fit", "gaus(0)+gaus(3)+pol0(6)", ymin, ymax); //rough fit window chosen
	//proton, deuteron, muon or punchthrough, in that order
	fit->SetParameters(38, xpeaks[0], 0.04, 6.8, xpeaks[1], 0.031, .1); //Initial fit parameters

	fit->SetParName(0, "Pro. Constant");
	fit->SetParName(1, "Pro. Mean");
	fit->SetParName(2, "Pro. Sigma");
	fit->SetParLimits(2, 0.01, 0.5);

	fit->SetParName(3, "Deu. Constant");
	fit->SetParName(4, "Deu. Mean");
	fit->SetParLimits(4, .55, 0.65);
	fit->SetParName(5, "Deu. Sigma");
	fit->SetParLimits(5, 0.01, 0.035);

	fit->SetParName(6, "Pol0");
} 

if(sliceIndex > 26) {
	s = new TSpectrum(1);
	nfound = s->Search(h, 2, "", 0.1);
	xpeaks = s->GetPositionX();

	//This assumes the proton band produces the highest peak in the 2D plot
	fit = new TF1("fit", "gaus(0)+pol0(3)", ymin, ymax); //rough fit window chosen
	//proton, deuteron, muon or punchthrough, in that order
	fit->SetParameters(38, xpeaks[0], 0.04, .1); //Initial fit parameters

	fit->SetParName(0, "Pro. Constant");
	fit->SetParName(1, "Pro. Mean");
	fit->SetParLimits(1, .35, 0.5);
	fit->SetParName(2, "Pro. Sigma");
	fit->SetParLimits(2, 0.01, 0.5);
	
	fit->SetParName(3, "Pol0");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
	TFitResultPtr r = h->Fit(fit, "RSQ+");

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
if(sliceIndex <= 26) {
	results["deuteron_constant"].push_back(fit->GetParameter(3) );
	results["deuteron_mean"].push_back(fit->GetParameter(4) );
	results["deuteron_sigma"].push_back(fit->GetParameter(5) );
}
if(sliceIndex <= 16) {
	results["triton_constant"].push_back(fit->GetParameter(6) );
	results["triton_mean"].push_back(fit->GetParameter(7) );
	results["triton_sigma"].push_back(fit->GetParameter(8) );
}
//	results["area"].push_back(area);
	results_err["bin_width"].push_back(0.);
	results_err["constant"].push_back(fit->GetParError(0) );
	results_err["mean"].push_back(fit->GetParError(1) );
	results_err["sigma"].push_back(fit->GetParError(2) );
//	results_err["area"].push_back(area_error);
}
void Rotate45clockwise(Double_t x, Double_t y, Double_t *rotated) {
	rotated[0] = TMath::Power(10, 0.7071 * (x + y) );
	rotated[1] = TMath::Power(10, 0.7071 * (-x + y) );
}
void DefineCut(const char *sourceName, const char *species="proton") {
	TFile *fCut = new TFile("al50-cuts.root", "UPDATE");
	TString meanStr = Form("%s_mean", species);
	TString sigmaStr = Form("%s_sigma", species);
	Int_t mapsize = results[meanStr].size();
	const char *histoName = sourceName;

	Double_t x, y;
	Double_t *rotated;
	for(int sigma = 1; sigma < 5; ++sigma) {
		sourceName = Form("%s_%s_%dsigma", histoName, species, sigma);
		if(fCut->GetListOfKeys()->Contains(sourceName) ) {
			printf("Deleting cut with name: %s\n", sourceName);
			gDirectory->Delete(Form("%s;1", sourceName) );    
		}
		TCutG *cutg = new TCutG(sourceName, 4*mapsize+1);
		for(int i=0; i < mapsize; i++) {
			if(i<=26) x = TMath::Log10((i+1)/2.+.5);
			if(i==27) x = 1.1613680;
			if(i==28) x = 1.2;
			y = results[meanStr][i] - sigma*results[sigmaStr][i];
			Rotate45clockwise(x, y, rotated);
			//cutg->SetPoint(2*i, x, results[meanStr][i] - sigma*results[sigmaStr][i]);
			cutg->SetPoint(2*i, rotated[0], rotated[1]);
			if(i<=26) x = TMath::Log10((i+1)/2.+1.);
			if(i==27) x = 1.2;
			if(i==28) x = 1.3;
			//y=y;
			Rotate45clockwise(x, y, rotated);
			//cutg->SetPoint(2*i+1, x, results[meanStr][i] - sigma*results[sigmaStr][i]);
			cutg->SetPoint(2*i+1, rotated[0], rotated[1]);
		}
		for(int i=mapsize; i < 2*mapsize; i++) {
			int c = 2*mapsize-i;
			if(c<=26) x = TMath::Log10(c/2.+1.);
			if(c==27) x = 1.2;
			if(c==28) x = 1.3;
			y = results[meanStr][c-1] + sigma*results[sigmaStr][c-1];
			Rotate45clockwise(x, y, rotated);
			//cutg->SetPoint(2*i, x, results[meanStr][c-1] + sigma*results[sigmaStr][c-1]);
			cutg->SetPoint(2*i, rotated[0], rotated[1]);
			if(c<=26) x = TMath::Log10(c/2.+.5);
			if(c==27) x = 1.1613680;
			if(c==28) x = 1.2;
			//y=y
			Rotate45clockwise(x, y, rotated);
			//cutg->SetPoint(2*i+1, x, results[meanStr][c-1] + sigma*results[sigmaStr][c-1]);
			cutg->SetPoint(2*i+1, rotated[0], rotated[1]);
		}
		x = TMath::Log10((0+1)/2.+.5);
		y = results[meanStr][0] - sigma*results[sigmaStr][0];
		Rotate45clockwise(x, y, rotated);
		//cutg->SetPoint(4*mapsize, TMath::Log10((0+1)/2.+.5), results[meanStr][0] - sigma*results[sigmaStr][0]);
		cutg->SetPoint(4*mapsize, rotated[0], rotated[1]);
		cutg->SetLineColor(kRed);
		cutg->SetLineWidth(2);
		cutg->Write();
	}
	fCut->Close();
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
void Pid(const char *filename="al50.root", Bool_t plotLeft=kTRUE, const char *treeName="tree") {
	gStyle->SetOptFit(1);
	TFile *fData = new TFile(filename, "READ");
	TH2D *hLg_SiL_EvDeltaE = new TH2D("hLg_SiL_EvDeltaE", "SiL Ev#DeltaE;Lg E+#DeltaE / #sqrt{2} [MeV];Lg #DeltaE / #sqrt{2} [MeV]", 250, -0.1, 1.5, 100, ymin, ymax);
	TH2D *hLg_SiR_EvDeltaE = new TH2D("hLg_SiR_EvDeltaE", "SiR Ev#DeltaE;Lg E+#DeltaE / #sqrt{2} [MeV];Lg #DeltaE / #sqrt{2} [MeV]", 250, -0.1, 1.5, 100, ymin, ymax);
	Double_t a1, a2, e1, e2, e3, t1, t2, timeToPrevTME, timeToNextTME;
	TString *channel = new TString("");
	TTree *tree = (TTree *)fData->Get(treeName);
	tree->SetBranchAddress("timeToPrevTME", &timeToPrevTME);
	tree->SetBranchAddress("timeToNextTME", &timeToNextTME);
	tree->SetBranchAddress("a1", &a1);
	tree->SetBranchAddress("a2", &a2);
	tree->SetBranchAddress("t1", &t1);
	tree->SetBranchAddress("t2", &t2);
	tree->SetBranchAddress("e1", &e1);
	tree->SetBranchAddress("e2", &e2);
	tree->SetBranchAddress("e3", &e3);
	tree->SetBranchAddress("channel", &channel);
	for(Long64_t i=0; i < tree->GetEntries(); i++) {
		tree->GetEntry(i);
		if(timeToPrevTME<10e3 || timeToNextTME<10e3) continue;
if(a2 > 3980) continue;
		if(t1<0 || t1>10000) continue;
		if(t2<0 || t2>10000) continue;
		if(!TMath::IsNaN(e3) ) continue;
		if(channel->Contains("SiL1") ) {
			if(t2-t1-13.4 < -25 || t2-t1 -13.4 > 25) continue;
			hLg_SiL_EvDeltaE->Fill(0.7071 * (TMath::Log10(e1+e2) - TMath::Log10(e1) ), 0.7071 * (TMath::Log10(e1+e2) + TMath::Log10(e1) ) );
		} else if(channel->Contains("SiR1") ) {
			if(t2-t1-13<-20 || t2-t1 - 13 > 20) continue;
			hLg_SiR_EvDeltaE->Fill(0.7071 * (TMath::Log10(e1+e2) - TMath::Log10(e1) ), 0.7071 * (TMath::Log10(e1+e2) + TMath::Log10(e1) ) );
		}
	}
	std::map<int, TH1D*>::iterator it;
	TCanvas *c = new TCanvas("c", "c", 1440, 880);
	int i=0;
	if(plotLeft) {
		//Slicing via y-projection and storing in the std::map
		std::map<int, TH1D*> map = Slice(hLg_SiL_EvDeltaE);
		c->Divide(map.size()/5, 5);

		for(it=map.begin(); it != map.end(); it++) {
			c->cd(i+1);
			TH1D *hclone = (TH1D *) it->second->Clone();
			//Fitting of slices
			Fit(hclone, i);
			i++;
		}
		DefineCut(hLg_SiL_EvDeltaE->GetName(), "proton");
		DefineCut(hLg_SiL_EvDeltaE->GetName(), "deuteron");
		DefineCut(hLg_SiL_EvDeltaE->GetName(), "triton");
	} else {
		//Slicing via y-projection and storing in the std::map
		std::map<int, TH1D*> map = Slice(hLg_SiR_EvDeltaE);
		c->Divide(map.size()/5, 5);

		for(it=map.begin(); it != map.end(); it++) {
			c->cd(i+1);
			TH1D *hclone = (TH1D *) it->second->Clone();
			//Fitting of slices
			Fit(hclone, i);
			i++;
		}
		DefineCut(hLg_SiR_EvDeltaE->GetName(), "proton");
		DefineCut(hLg_SiR_EvDeltaE->GetName(), "deuteron");
		DefineCut(hLg_SiR_EvDeltaE->GetName(), "triton");
	}
}
