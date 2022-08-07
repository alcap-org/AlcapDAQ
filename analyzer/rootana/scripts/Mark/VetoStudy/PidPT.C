//Some global variable for convenience
Bool_t debug = kTRUE;
Double_t xmin = 0.1;
Double_t xmax = 0.28;
Double_t ymin = 1.0;
Double_t ymax = 1.6;
std::map<TString, std::vector<Double_t> > results;
std::map<TString, std::vector<Double_t> > results_err;
std::map<int, TH1D *> Slice(TH2D *h) {
	std::map<int, TH1D *> map;
	for(int i=0; i < 3; i++) {
		//store projections of different Lg #DeltaE+E segments in map, divided into equal portions of 1MeV; +1 because it also includes the last bin so we don't double count
		map[i] = h->ProjectionX(Form("%.3f_%.3f_py", ymin + i*.2, .2 + ymin + i*.2), h->GetYaxis()->FindBin(ymin + i*0.2), h->GetYaxis()->FindBin(.2 + ymin + i*.2) );
	}
	return map;
}
void Fit(TH1D *h, Int_t sliceIndex) {
	////
	h->GetXaxis()->SetTitle("Lg #DeltaE / #sqrt{2}");
	h->GetXaxis()->SetTitleOffset(1.2);
	h->GetYaxis()->SetTitle(Form("Counts/%f", h->GetBinWidth(0) ) );


	//Use TSpectrum to find the peak candidates
	TSpectrum *s = new TSpectrum(1);
	Int_t nfound = s->Search(h, 2, "", 0.05);
	Double_t *xpeaks = s->GetPositionX();

	//This assumes the proton band produces the highest peak in the 2D plot
	TF1 *fit = new TF1("fit", "gaus(0)+pol0(3)", xmin, xmax); //rough fit window chosen
	//proton, deuteron, muon or punchthrough, in that order
	fit->SetParameters(4, xpeaks[0], 0.02, 1);

	fit->SetParName(0, "Pro. Constant");
	fit->SetParName(1, "Pro. Mean");
	fit->SetParName(2, "Pro. Sigma");
	fit->SetParName(3, "Pol0");

	fit->SetParLimits(1, xpeaks[0]-0.1, xpeaks[0]+0.1);
	fit->SetParLimits(2, 0, 1);

	TFitResultPtr r = h->Fit(fit, "RSQ+");

	results["proton_constant"].push_back(fit->GetParameter(0) );
	results["proton_mean"].push_back(fit->GetParameter(1) );
	results["proton_sigma"].push_back(fit->GetParameter(2) );
}
void DefineCut(const char *sourceName, const char *species="proton") {
	TFile *fCut = new TFile("al50-sir3-cuts.root", "UPDATE");
	TString meanStr = Form("%s_mean", species);
	TString sigmaStr = Form("%s_sigma", species);
	Int_t mapsize = results[meanStr].size();
	const char *histoName = sourceName;

	Double_t x, y;
	for(int sigma = 1; sigma < 5; ++sigma) {
		sourceName = Form("sir3_pt_%s_%s_%dsigma", histoName, species, sigma);
		if(fCut->GetListOfKeys()->Contains(sourceName) ) {
			printf("Deleting cut with name: %s\n", sourceName);
			gDirectory->Delete(Form("%s;1", sourceName) );    
		}
		TCutG *cutg = new TCutG(sourceName, 4*mapsize+1);
		for(int i=0; i < mapsize; i++) {
			y = ymin + i*.2;
			cutg->SetPoint(2*i, results[meanStr][i] - sigma*results[sigmaStr][i], y);
			y = .2 + ymin + i*.2;
			cutg->SetPoint(2*i+1, results[meanStr][i] - sigma*results[sigmaStr][i], y);
		}
		for(int i=mapsize; i < 2*mapsize; i++) {
			int c = 2*mapsize-i;
			y = .2 + ymin + (c-1)*.2;
			cutg->SetPoint(2*i, results[meanStr][c-1] + sigma*results[sigmaStr][c-1], y);
			y = ymin + (c-1)*.2;
			cutg->SetPoint(2*i+1, results[meanStr][c-1] + sigma*results[sigmaStr][c-1], y);
		}
		y = ymin;
		cutg->SetPoint(4*mapsize, results[meanStr][0] - sigma*results[sigmaStr][0], y);
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
void PidPT(const char *filename="al50.root", Bool_t plotLeft=kFALSE, const char *treeName="tree") {
	gStyle->SetOptFit(1);
	TFile *fData = new TFile(filename, "READ");
	TFile *fOutput = new TFile("pid_veto_output_al50.root", "RECREATE");
	TH2D *hLg_SiR_EvDeltaE = new TH2D("hLg_SiR_EvDeltaE", "SiR Ev#DeltaE;Lg E+#DeltaE / #sqrt{2} [MeV];Lg #DeltaE / #sqrt{2} [MeV]", 100, xmin, xmax, 100, ymin, ymax);
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
		if(abs(t3-t2) > 200) continue;
		if(channel->Contains("SiR1") ) {
			hLg_SiR_EvDeltaE->Fill(0.7071 * (TMath::Log10(e2+e3) - TMath::Log10(e2) ), 0.7071 * (TMath::Log10(e2+e3) + TMath::Log10(e2) ) );
			SiREvDeltaE->Fill(e2+e3, e2);
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
