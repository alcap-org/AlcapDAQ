void GetFits(std::map<Int_t, Double_t> &totAdcShift, TH2D hGe, Int_t min = 1260, Int_t max = 1310, Int_t runMin=9410, Int_t runMax=9682) { //Defaults are for GeLoGain
	gStyle->SetOptFit(1);
	TF1 *fit = new TF1("fit", "gaus(0)+pol0(3)", min, max);
	if(strcmp(hGe.GetName(), "hGeHiGain") == 0) {
		fit->SetParameters(0, 9.03304e+00);
		fit->SetParLimits(0, 10, 110);
		fit->SetParameters(1, 2.72492e+03);
		fit->SetParLimits(1, min, max);
		fit->SetParameters(2, 6.86452e+00);
		fit->SetParLimits(2, 3, 9.5);
		fit->SetParameters(3, 1.58901e+00);
	} else if(strcmp(hGe.GetName(), "hGeLoGain") == 0) {
		fit->SetParameters(0, 7.42380e+01);
		fit->SetParLimits(0, 10, 110);
		fit->SetParameters(1, 1.27369e+03);
		fit->SetParLimits(1, min, max);
		fit->SetParameters(2, 7.59546e+00);
		fit->SetParLimits(2, 3, 9.5);
		fit->SetParameters(3, 2.23232e+01);
	}
	Double_t gain = 0;
	TH1D *hProj = 0;
	TH1D *hChi2 = new TH1D(Form("h%s_Chi2", hGe.GetName() ), Form("%s #Chi^{2}", hGe.GetName() ), 100, 0, 5);
	for(Int_t i=runMin; i <= runMax; ++i) {
		hProj = hGe.ProjectionY(Form("%s%d_py", hGe.GetName(), i), hGe.GetXaxis()->FindBin(i), hGe.GetXaxis()->FindBin(i) );
		if(!hProj->GetEntries() ) continue;
		if(hProj->GetEntries() < 200)
			printf("Run %d has less than 200 entries (around 511keV): %f\n", i, hProj->GetEntries() );
		if(!hProj->Fit("fit", "QSR+") ) {
			totAdcShift[i] = fit->GetParameter(1);
			fit->SetParName(0, "A");
			fit->SetParName(1, "#mu");
			fit->SetParName(2, "#sigma");
			fit->SetParName(3, "const.");
			hChi2->Fill(fit->GetChisquare() / fit->GetNDF() );

			gPad->Update();
			TPaveStats *st = (TPaveStats *)hProj->FindObject("stats");
			st->SetX1NDC(0.137536);
			st->SetY1NDC(0.632696);
			st->SetX2NDC(0.409742);
			st->SetY2NDC(0.861996);

		} else {
			printf("run %d failed...\n", i);
		}
	}
}
void GeGainDrift(std::string target) {
	TFile *fData = new TFile(Form("%s/%s.root", getenv("R15b_DATA"), target.c_str() ), "READ");
	if(!fData) {
		printf("%s.root does not exist.", target.c_str() );
		exit;
	}
	TTree *tree = (TTree *)fData->Get("tree");
	Double_t t1, e1;
	Int_t runId, a1;
	TString *channel = new TString("");
	tree->SetBranchAddress("runId", &runId);
	tree->SetBranchAddress("a1", &a1);
	tree->SetBranchAddress("e1", &e1);
	tree->SetBranchAddress("t1", &t1);
	tree->SetBranchAddress("channel", &channel);

	std::map<std::string, std::array<int, 2> > mRun = {{"al100", {9410, 9682} }, {"al50", {9889, 10129} }, {"ti50", {10157, 10254} }, {"mt", {10273, 10317} } };

	int runMin = mRun.at(target)[0];
	int runMax = mRun.at(target)[1];
	Double_t gain = 0.0f;
	Int_t runbins = runMax - runMin;
	{
		Int_t min = 2720, max = 2790;
		Int_t bins = max-min;
		TH2D hGeHiGain("hGeHiGain", "GeHiGain;run Id;ADC", runbins, runMin, runMax, bins, min, max);

		for(Long64_t i=0; i < tree->GetEntries(); ++i) {
			tree->GetEntry(i);
			if(a1 < min || a1 > max) continue;
			if(channel->Contains("GeHiGain") ) {
				hGeHiGain.Fill(runId, a1);
			}
		}
		std::map<Int_t, Double_t> map;
		GetFits(map, hGeHiGain, min, max, runMin, runMax);
		//From earlier energy calibration work
		Double_t HiOffset = 0.1238;
		Double_t HiGain = 0.1851;
		for(std::map<int, double>::iterator it = map.begin(); it != map.end(); ++it) {
			gain = (511 - HiOffset)/it->second; //E - offset = gain*ADC
			printf("INSERT INTO FineGeCalib (run, channel, gain) VALUES(%d, '%s', %f);\n", it->first, "GeHiGain", gain);
		}
		map.clear();
	} 
	{
		Int_t min = 1230, max = 1310;
		Int_t bins = max-min;
		TH2D hGeLoGain("hGeLoGain", "GeLoGain;run Id;ADC", runbins, runMin, runMax, bins, min, max);

		for(Long64_t i=0; i < tree->GetEntries(); ++i) {
			tree->GetEntry(i);
			if(a1 < min || a1 > max) continue;
			if(channel->Contains("GeLoGain") ) {
				hGeLoGain.Fill(runId, a1);
			}
		}
		std::map<Int_t, Double_t> map;
		GetFits(map, hGeLoGain, min, max, runMin, runMax);
		//From earlier energy calibration work
		Double_t LoOffset = 0.2387;
		Double_t LoGain = 0.3991;
		for(std::map<Int_t, Double_t>::iterator it = map.begin(); it != map.end(); ++it) {
			gain = (511 - LoOffset)/it->second; //E - offset = gain*ADC
			printf("INSERT INTO FineGeCalib (run, channel, gain) VALUES(%d, '%s', %f);\n", it->first, "GeLoGain", gain);
		}
		map.clear();
	}
}
