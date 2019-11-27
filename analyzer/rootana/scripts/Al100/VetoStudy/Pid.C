//Some global variable for convenience
Bool_t debug = kTRUE;
Double_t ymin = 1.5; //y-limits chosen for protons
Double_t ymax = 2;
std::map<TString, std::vector<Double_t> > results;
std::map<TString, std::vector<Double_t> > results_err;
std::map<int, TH1D *> Slice(TH2D *h) {
	std::map<int, TH1D *> map;
	for(int i=0; i < 6; i++) {
		//store projections of different Lg #DeltaE+E segments in map, divided into equal portions of 1MeV; +1 because it also includes the last bin so we don't double count
		map[i] = h->ProjectionY(Form("%.3f_%.3f_py", i*.05, .05 + i*.05), h->GetXaxis()->FindBin(i*0.05), h->GetXaxis()->FindBin((0.05 + i*0.05) ) );
	}
	return map;
}
void Fit(TH1D *h, Int_t sliceIndex) {
	////
	h->GetXaxis()->SetTitle("Lg #DeltaE / #sqrt{2}");
	h->GetXaxis()->SetTitleOffset(1.2);
	h->GetYaxis()->SetTitle(Form("Counts/%f", h->GetBinWidth(0) ) );

	//Use TSpectrum to find the peak candidates
	TSpectrum *s = new TSpectrum(2);
	Int_t nfound = s->Search(h, 2, "", 0.05);
	Double_t *xpeaks = s->GetPositionX();
	sort(xpeaks, xpeaks+2);

	//This assumes the proton band produces the highest peak in the 2D plot
	TF1 *fit = new TF1("fit", "gaus(0)+gaus(3)+pol0(6)", ymin, ymax); //rough fit window chosen
	//proton, deuteron, muon or punchthrough, in that order
	fit->SetParameters(101.783, xpeaks[0],0.0171819, 47.7807, xpeaks[1], 0.0108407, 4.43771);

	fit->SetParName(0, "Pro. Constant");
	fit->SetParName(1, "Pro. Mean");
	fit->SetParName(2, "Pro. Sigma");
	fit->SetParName(3, "Deu. Constant");
	fit->SetParName(4, "Deu. Mean");
	fit->SetParName(5, "Deu. Sigma");
	fit->SetParName(6, "Pol0");

	fit->SetParLimits(1, 1.6, 1.7);
	fit->SetParLimits(4, 1.75, 1.85);
	fit->SetParLimits(5, 0, 2);

	TFitResultPtr r = h->Fit(fit, "RSQ+");

	Double_t area = TMath::Sqrt(2*3.142) * fit->GetParameter(0) * fit->GetParameter(2) / h->GetBinWidth(0);
	results["bin_width"].push_back(h->GetBinWidth(0) );
	results["proton_constant"].push_back(fit->GetParameter(0) );
	results["proton_mean"].push_back(fit->GetParameter(1) );
	results["proton_sigma"].push_back(fit->GetParameter(2) );
	results["deuteron_constant"].push_back(fit->GetParameter(3) );
	results["deuteron_mean"].push_back(fit->GetParameter(4) );
	results["deuteron_sigma"].push_back(fit->GetParameter(5) );
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
void DefineCut(const char *sourceName, const char *species="proton") {
	TFile *fCut = new TFile("al50-sir3-cuts.root", "UPDATE");
	TString meanStr = Form("%s_mean", species);
	TString sigmaStr = Form("%s_sigma", species);
	Int_t mapsize = results[meanStr].size();
	const char *histoName = sourceName;

	Double_t x, y;
	for(int sigma = 1; sigma < 5; ++sigma) {
		sourceName = Form("sir3_%s_%s_%dsigma", histoName, species, sigma);
		if(fCut->GetListOfKeys()->Contains(sourceName) ) {
			printf("Deleting cut with name: %s\n", sourceName);
			gDirectory->Delete(Form("%s;1", sourceName) );    
		}
		TCutG *cutg = new TCutG(sourceName, 4*mapsize+1);
		for(int i=0; i < mapsize; i++) {
			x = i*.05;
			if(i < 5) {
				cutg->SetPoint(2*i, x, results[meanStr][i] - sigma*results[sigmaStr][i]);
			} else {
				cutg->SetPoint(2*i, x, results[meanStr][4] - sigma*results[sigmaStr][4]);
			}
			x = .05 + i*.05;
			if(i < 5) {
				cutg->SetPoint(2*i+1, x, results[meanStr][i] - sigma*results[sigmaStr][i]);
			} else {
				cutg->SetPoint(2*i+1, x, results[meanStr][4] - sigma*results[sigmaStr][4]);
			}
		}
		for(int i=mapsize; i < 2*mapsize; i++) {
			int c = 2*mapsize-i;
			x = .05 + (c-1)*.05;
			if(c < 5) {
				cutg->SetPoint(2*i, x, results[meanStr][c-1] + sigma*results[sigmaStr][c-1]);
			} else {
				cutg->SetPoint(2*i, x, results[meanStr][4] + sigma*results[sigmaStr][4]);
			}
			x = (c-1)*.05;
			if(c < 5) {
				cutg->SetPoint(2*i+1, x, results[meanStr][c-1] + sigma*results[sigmaStr][c-1]);
			} else {
				cutg->SetPoint(2*i+1, x, results[meanStr][4] + sigma*results[sigmaStr][4]);
			}
		}
		x = .0;
		cutg->SetPoint(4*mapsize, x, results[meanStr][0] - sigma*results[sigmaStr][0]);
		cutg->SetLineColor(kRed);
		cutg->SetLineWidth(2);
		cutg->Write(); 
		RotateCutGPoints(cutg); //rotated cuts for report
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
void Pid(const char *filename="al50.root", Bool_t plotLeft=kFALSE, const char *treeName="tree") {
	gStyle->SetOptFit(1);
	TFile *fData = new TFile(filename, "READ");
	TFile *fOutput = new TFile("pid_veto_output_al50.root", "RECREATE");
	TH2D *hLg_SiR_EvDeltaE = new TH2D("hLg_SiR_EvDeltaE", "SiR Ev#DeltaE;Lg E+#DeltaE / #sqrt{2} [MeV];Lg #DeltaE / #sqrt{2} [MeV]", 250, 0, 0.28, 100, ymin, ymax);
	TH2D *SiREvDeltaE = new TH2D("SiREvDeltaE", "", 300, 0, 30, 200, 0, 20);
	Double_t e1, e2, e3, t1, t2, t3, timeToPrevTME, timeToNextTME;
	TString *channel = new TString("");
	TTree *tree = (TTree *)fData->Get(treeName);
	tree->SetBranchAddress("timeToPrevTME", &timeToPrevTME);
	tree->SetBranchAddress("timeToNextTME", &timeToNextTME);
	tree->SetBranchAddress("t1", &t1);
	tree->SetBranchAddress("t2", &t2);
	tree->SetBranchAddress("t3", &t3);
	tree->SetBranchAddress("e1", &e1);
	tree->SetBranchAddress("e2", &e2);
	tree->SetBranchAddress("e3", &e3);
	tree->SetBranchAddress("channel", &channel);
	for(Long64_t i=0; i < tree->GetEntries(); i++) {
		tree->GetEntry(i);
		if(timeToPrevTME<10e3 || timeToNextTME<10e3) continue;
		if(abs(t2)>10e3) continue;
		if(TMath::IsNaN(e3) ) continue;
		if(abs(t3-t1) > 200) continue;
		if(channel->Contains("SiR1") ) {
			hLg_SiR_EvDeltaE->Fill(0.7071 * (TMath::Log10(e1+e2+e3) - TMath::Log10(e1+e2) ), 0.7071 * (TMath::Log10(e1+e2+e3) + TMath::Log10(e1+e2) ) );
			SiREvDeltaE->Fill(e1+e2+e3, e1+e2);
		}
	}
	std::map<int, TH1D*>::iterator it;
	TCanvas *c = new TCanvas("c", "c", 1440, 880);
	int i=0;
	//Slicing via y-projection and storing in the std::map
	std::map<int, TH1D*> map = Slice(hLg_SiR_EvDeltaE);
	c->Divide(map.size()/2, 2);

	for(it=map.begin(); it != map.end(); it++) {
		c->cd(i+1);
		TH1D *hclone = (TH1D *) it->second->Clone();
		//Fitting of slices
		Fit(hclone, i);
		i++;
	}
	DefineCut(hLg_SiR_EvDeltaE->GetName(), "proton");
	fOutput->Write();
}
