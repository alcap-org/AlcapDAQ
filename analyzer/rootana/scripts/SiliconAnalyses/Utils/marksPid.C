//Some global variable for convenience
Bool_t debug = kTRUE;
Double_t window = .06;
Double_t ymin = .26; //y-limits chosen for protons
Double_t ymax = 1.4;
std::map<TString, std::vector<Double_t> > results;
std::map<TString, std::vector<Double_t> > results_err;
std::map<int, TH1D *> Slice(TH2D *h) {
	std::map<int, TH1D *> map;
	for(int i=0; i < 45; i++) { //sliceIndexes Al50 SiR 35, SiL 30 //Al100 SiL 27 SiR 35
		//store projections of different Lg #DeltaE+E segments in map, divided into equal portions of 1MeV
	  float slice_width = 0.5; // MeV
	  float min_slice = 1.0+(i*slice_width);//(i+1)/2+(slice_width);
	  float max_slice = min_slice+slice_width;//(i+1)/2.+(2*slice_width);
	  std::cout << "sliceIndex " << i << ": " << min_slice << " -- " << max_slice << " MeV" << std::endl;
	  map[i] = h->ProjectionY(Form("%.3f_%.3f_py", min_slice, max_slice), h->GetXaxis()->FindBin(TMath::Log10(min_slice) )+1, h->GetXaxis()->FindBin(TMath::Log10(max_slice) ) );
	  //	  map[i] = h->ProjectionY(Form("%.3f_%.3f_py", (i+1)/2.+.5, (i+1)/2.+1.), h->GetXaxis()->FindBin(TMath::Log10((i+1)/2.+.5) )+1, h->GetXaxis()->FindBin(TMath::Log10((i+1)/2.+1.) ) );
	}
	return map;
}
void Fit(TH1D *h, Int_t sliceIndex, bool plotLeft = kFALSE) {
	std::cout << "Processing sliceIndex: " << sliceIndex << std::endl;
	std::cout << "==========================" << std::endl;
	h->GetXaxis()->SetTitle("Lg #DeltaE / #sqrt{2}");
	h->GetXaxis()->SetTitleOffset(1.2);
	h->GetYaxis()->SetTitle(Form("Counts/%f", h->GetBinWidth(0) ) );

	//Use TSpectrum to find the peak candidates
	TSpectrum *s = new TSpectrum(4);
	Int_t nfound = s->Search(h, 2, "", 0.05);
	std::cout << "Found " << nfound << " peaks" << std::endl;
	Double_t *xpeaks = s->GetPositionX();
	//	if(xpeaks[2] < xpeaks[1]) xpeaks[2] = 0.74;
	sort(xpeaks, xpeaks+nfound);

	// add triton peak if we saw three peaks and one was alpha
	if (nfound==3 && xpeaks[2]>1.0) {
	  xpeaks[3] = xpeaks[2];
	  xpeaks[2] = 0.7;
	  nfound = 4;
	}
	
	TF1 *fit = new TF1("fit", "gaus(0)+gaus(3)+gaus(6)+pol0(9)", ymin, ymax);
	if(nfound == 4) {
	  fit = new TF1("fit", "gaus(0)+gaus(3)+gaus(6)+gaus(9)+pol0(12)", ymin, ymax);
	}
	//proton, deuteron, tritons, alphas
	fit->SetParameters(38, xpeaks[0], 0.04, 6.8, xpeaks[1], 0.031, 30, xpeaks[2], .022);
	if(nfound == 4) {
		fit->SetParName(9, "Alp. Constant");
		fit->SetParName(10, "Alp. Mean");
		fit->SetParName(11, "Alp. Sigma");
		fit->SetParName(12, "Pol0");

		fit->SetParLimits(6, 0, 50);
		fit->SetParLimits(7, 0.6, 0.8);
		fit->SetParLimits(8, 0.015, 0.03);
		fit->SetParameter(9, 10);
		fit->SetParameter(10, xpeaks[3]);
		fit->SetParameter(11, .022);
		fit->SetParameter(12, 1);

		fit->SetParLimits(10, 1., 1.4);
		fit->SetParLimits(11, 0., 0.1);
	} else {
		fit->SetParName(9, "Pol0");

		fit->SetParLimits(6, 0, 50);
		fit->SetParLimits(7, 0.6, 0.8);
		fit->SetParLimits(8, 0.015, 0.03);

		fit->SetParameter(9, 1);
		fit->SetParLimits(6, 0, 50);
		fit->SetParameter(8, 0.01);
		fit->SetParLimits(8, 0.015, 0.03);
	}

	fit->SetParName(0, "Pro. Constant");
	fit->SetParName(1, "Pro. Mean");
	fit->SetParName(2, "Pro. Sigma");

	fit->SetParName(3, "Deu. Constant");
	fit->SetParName(4, "Deu. Mean");
	fit->SetParName(5, "Deu. Sigma");

	fit->SetParName(6, "Tri. Constant");
	fit->SetParName(7, "Tri. Mean");
	fit->SetParName(8, "Tri. Sigma");

////////////////////////////////////////////////////////////////////////////////////////////////////
	Int_t tritonLimit = 14;//17;
	if(plotLeft) {
		tritonLimit = 17;
	}
	if(sliceIndex > tritonLimit) { //SiR 17 SiL 12
	  delete s;
		s = new TSpectrum(2);
		nfound = s->Search(h, 3, "", 0.05);
		xpeaks = s->GetPositionX();
		sort(xpeaks, xpeaks+2);

		delete fit;
		TF1 *fit = new TF1("fit", "gaus(0)+gaus(3)+pol0(6)", ymin, ymax); //rough fit window chosen
		fit->SetParameters(38, xpeaks[0], 0.04, 6.8, xpeaks[1], 0.031, .1); //Initial fit parameters

		fit->SetParName(0, "Pro. Constant");
		fit->SetParName(1, "Pro. Mean");
		fit->SetParName(2, "Pro. Sigma");
		fit->SetParName(3, "Deu. Constant");
		fit->SetParName(4, "Deu. Mean");
		fit->SetParName(5, "Deu. Sigma");
		fit->SetParName(6, "Pol0");
		fit->SetParLimits(3, 0, 100);
		fit->SetParLimits(5, 0.02, 0.035);
		delete s;
	}

	TFitResultPtr r = h->Fit(fit, "RSL");
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
	results["alpha_constant"].push_back(fit->GetParameter(9) );
	results["alpha_mean"].push_back(fit->GetParameter(10) );
	results["alpha_sigma"].push_back(fit->GetParameter(11) );
	results_err["bin_width"].push_back(0.);
	results_err["constant"].push_back(fit->GetParError(0) );
	results_err["mean"].push_back(fit->GetParError(1) );
	results_err["sigma"].push_back(fit->GetParError(2) );
}
void Rotate45clockwise(Double_t x, Double_t y, Double_t *rotated) {
	rotated[0] = TMath::Power(10, 0.7071 * (x + y) );
	rotated[1] = TMath::Power(10, 0.7071 * (-x + y) );
}
void RotateCutGPoints(TCutG *cut) {
        Double_t * cx = cut->GetX();
        Double_t * cy = cut->GetY();
        Int_t size = cut->GetN();
        TCutG *rotatedCut = new TCutG(Form("r_%s", cut->GetName() ), size);
        for(Int_t i = 0; i < size; ++i) {
                rotatedCut->SetPoint(i, TMath::Power(10, 0.7071 * (cx[i] + cy[i]) ), TMath::Power(10, 0.7071 * (-cx[i] + cy[i]) ) );
        }
        rotatedCut->SetLineColor(kRed);
        rotatedCut->Write();
}

void DefineCut(const char *sourceName, const char *species="proton", bool plotLeft=kFALSE) {
	TFile *fCut = new TFile("si16b-cuts.root", "UPDATE");
	TString meanStr = Form("%s_mean", species);
	TString sigmaStr = Form("%s_sigma", species);
	Int_t mapsize = results[meanStr].size();
	const char *histoName = sourceName;
	//SiR 22, SiL 16
	int mod;
	if(!plotLeft) {
		mod = 22;
		if(std::strcmp(species, "deuteron")==0) {
		  mod = 15;//18;
		}
		if(std::strcmp(species, "triton")==0) {
		  mod = 14;//17;
		}
		if(std::strcmp(species, "alpha")==0) {
		  mod = 2;//5;
		}
	} else {
		mod = 16;
		if(std::strcmp(species, "deuteron")==0) {
			mod = 18;
		}
		if(std::strcmp(species, "triton")==0) {
			mod = 12;
		}
		if(std::strcmp(species, "alpha")==0) {
			mod = 5;
		}
	}
	for(int sigma = 1; sigma < 5; ++sigma) {
		sourceName = Form("%s_%s_%dsigma", histoName, species, sigma);
		if(fCut->GetListOfKeys()->Contains(sourceName) ) {
			printf("Deleting cut with name: %s\n", sourceName);
			gDirectory->Delete(Form("%s;1", sourceName) );    
		}
		sourceName = Form("r_%s_%s_%dsigma", histoName, species, sigma);
		if(fCut->GetListOfKeys()->Contains(sourceName) ) {
			printf("Deleting cut with name: %s\n", sourceName);
			gDirectory->Delete(Form("%s;1", sourceName) );    
		}
		sourceName = Form("%s_%s_%dsigma", histoName, species, sigma);
		TCutG *cutg = new TCutG(sourceName, 4*mapsize+1);
		for(int i=0; i < mapsize; i++) {
			if(i < mod) {
				cutg->SetPoint(2*i, TMath::Log10((i+1)/2.+.5), results[meanStr][i] - sigma*results[sigmaStr][i]);
				cutg->SetPoint(2*i+1, TMath::Log10((i+1)/2.+1.), results[meanStr][i] - sigma*results[sigmaStr][i]);
			} else {
				cutg->SetPoint(2*i, TMath::Log10((i+1)/2.+.5), results[meanStr][mod-1] - sigma*results[sigmaStr][mod-1]);
				cutg->SetPoint(2*i+1, TMath::Log10((i+1)/2.+1.), results[meanStr][mod-1] - sigma*results[sigmaStr][mod-1]);
			}
		}
		for(int i=mapsize; i < 2*mapsize; i++) {
			int c = 2*mapsize-i;
			if(c < mod) {
				cutg->SetPoint(2*i, TMath::Log10(c/2.+1.), results[meanStr][c-1] + sigma*results[sigmaStr][c-1]);
				cutg->SetPoint(2*i+1, TMath::Log10(c/2.+.5), results[meanStr][c-1] + sigma*results[sigmaStr][c-1]);
			} else {
				cutg->SetPoint(2*i, TMath::Log10(c/2.+1.), results[meanStr][mod-1-1] + sigma*results[sigmaStr][mod-1-1]);
				cutg->SetPoint(2*i+1, TMath::Log10(c/2.+.5), results[meanStr][mod-1-1] + sigma*results[sigmaStr][mod-1-1]);
			}
		}
		cutg->SetPoint(4*mapsize, TMath::Log10((0+1)/2.+.5), results[meanStr][0] - sigma*results[sigmaStr][0]);
		cutg->SetLineColor(kRed);
		cutg->Write();
		RotateCutGPoints(cutg);
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
void Pid(const char *target="~/data/results/Si16b/subtrees_newPP_geq1TgtPulse_1.root", Bool_t plotLeft=kFALSE, const char *treeName="siBlockTree_SiR") {
	gStyle->SetOptFit(1);
	//	TFile *fData = new TFile(Form("%s/%s.root", getenv("R15b_DATA"), target), "READ");
	TFile *fData = new TFile(target, "READ");
	TH2D *hLg_SiL_EvDeltaE = new TH2D("hLg_SiL_EvDeltaE", "SiL Ev#DeltaE;Lg E+#DeltaE / #sqrt{2} [MeV];Lg #DeltaE / #sqrt{2} [MeV]", 250, -0.1, 1.5, 100, ymin, ymax);
	TH2D *hLg_SiR_EvDeltaE = new TH2D("hLg_SiR_EvDeltaE", "SiR Ev#DeltaE;Lg E+#DeltaE / #sqrt{2} [MeV];Lg #DeltaE / #sqrt{2} [MeV]", 250, -0.1, 1.5, 100, ymin, ymax);
	Double_t e1, e2, e3, t1, t2, t3, timeToPrevTME, timeToNextTME;
	Int_t a1, a2;
	TString *channel = new TString("SiR");
	TTree *tree = (TTree *)fData->Get(treeName);
	//	tree->SetBranchAddress("timeToPrevTME", &timeToPrevTME);
	//	tree->SetBranchAddress("timeToNextTME", &timeToNextTME);
	tree->SetBranchAddress("thin_time", &t1);
	tree->SetBranchAddress("thick_time", &t2);	
	tree->SetBranchAddress("third_time", &t3);
	tree->SetBranchAddress("thin_energy", &e1);
	tree->SetBranchAddress("thick_energy", &e2);
	tree->SetBranchAddress("third_energy", &e3);
	//	tree->SetBranchAddress("a1", &a1);
	//	tree->SetBranchAddress("a2", &a2);
	//	tree->SetBranchAddress("channel", &channel);
	for(Long64_t i=0; i < tree->GetEntries(); i++) {
		tree->GetEntry(i);
		if(channel->Contains("SiR") && e1>7000) continue; //3850*1.77056 - 23.15 ) continue;
		//		if(channel->Contains("SiR1_2") && a1>3972. ) continue;
		//		if(channel->Contains("SiR1_3") && a1>3956. ) continue;
		//		if(channel->Contains("SiR1_4") && a1>3986. ) continue;
		if(e2 > 17000) continue;//3980*4.2416 + 107.35) continue;
		//		if(timeToPrevTME<10e3 || timeToNextTME<10e3) continue;
		if(abs(t2)>10000) continue;
		//		if(!TMath::IsNaN(e3) ) continue;
		if (e3 > 0 || abs(t3-t2)<500) continue;
//		if(t2 < 400) continue;
		e1 = e1 * 0.001;
		e2 = e2 * 0.001;
		e3 = e3 * 0.001;
		if(channel->Contains("SiL") ) {
			if(abs(t2-t1) > 500) continue;
			hLg_SiL_EvDeltaE->Fill(0.7071 * (TMath::Log10(e1+e2) - TMath::Log10(e1) ), 0.7071 * (TMath::Log10(e1+e2) + TMath::Log10(e1) ) );
		} else if(channel->Contains("SiR") ) {
			if(abs(t2-t1) > 500) continue;
			hLg_SiR_EvDeltaE->Fill(0.7071 * (TMath::Log10(e1+e2) - TMath::Log10(e1) ), 0.7071 * (TMath::Log10(e1+e2) + TMath::Log10(e1) ) );
		}
	}
	hLg_SiR_EvDeltaE->Draw("");
	std::map<int, TH1D*>::iterator it;
	TCanvas *c = new TCanvas("c", "c", 1440, 880);
	int i=0;
	if(plotLeft) {
		//Slicing via y-projection and storing in the std::map
		std::map<int, TH1D*> map = Slice(hLg_SiL_EvDeltaE);
		c->Divide(map.size()/6, 6);

		for(it=map.begin(); it != map.end(); it++) {
			c->cd(i+1);
			TH1D *hclone = (TH1D *) it->second->Clone();
			//Fitting of slices
			Fit(hclone, i, plotLeft);
			i++;
		}
		DefineCut(hLg_SiL_EvDeltaE->GetName(), "proton", plotLeft);
		DefineCut(hLg_SiL_EvDeltaE->GetName(), "deuteron", plotLeft);
		DefineCut(hLg_SiL_EvDeltaE->GetName(), "triton", plotLeft);
		DefineCut(hLg_SiL_EvDeltaE->GetName(), "alpha", plotLeft);
	} else {
		//Slicing via y-projection and storing in the std::map
		std::map<int, TH1D*> map = Slice(hLg_SiR_EvDeltaE);
		c->Divide(map.size()/6, 6);

		for(it=map.begin(); it != map.end(); it++) {
			c->cd(i+1);
			TH1D *hclone = (TH1D *) it->second->Clone();
			//Fitting of slices
			Fit(hclone, i, plotLeft);
			i++;
			TFile *fCut = new TFile("si16b-cuts.root", "UPDATE");
			hclone->Write();
			fCut->Close();
		}
		std::cout << "Here" << std::endl;
		DefineCut(hLg_SiR_EvDeltaE->GetName(), "proton", plotLeft);
		DefineCut(hLg_SiR_EvDeltaE->GetName(), "deuteron", plotLeft);
		DefineCut(hLg_SiR_EvDeltaE->GetName(), "triton", plotLeft);
		DefineCut(hLg_SiR_EvDeltaE->GetName(), "alpha", plotLeft);
		TFile *fCut = new TFile("si16b-cuts.root", "UPDATE");
		hLg_SiR_EvDeltaE->Write();
		c->Write();
		fCut->Close();
	}
}
