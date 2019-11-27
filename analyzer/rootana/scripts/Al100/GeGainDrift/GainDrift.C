std::map<Int_t, Double_t> GetFits(TH2D *hGe, Int_t min = 1240, Int_t max = 1300) { //Defaults are for GeLoGain
	std::map<Int_t, Double_t> totAdcShift;
	gStyle->SetOptFit(1);
	TF1 *fit = new TF1("fit", "gaus(0)+pol0(3)", min, max);
//GeHiGain
fit->SetParameters(0, 5.04400e+01);
fit->SetParLimits(0, 0, 100);
fit->SetParameters(1, 2.76019e+03);
fit->SetParLimits(1, 2680, 2840);
fit->SetParameters(2, 7.75720e+00);
fit->SetParLimits(2, 0, 10);
fit->SetParameters(3, 6.14190e+00);


//GeLoGain
//	fit->SetParameters(0, 4.42380e+01);
//	fit->SetParLimits(0, 0, 100);
//	fit->SetParameters(1, 1.26369e+03);
//	fit->SetParLimits(1, 1240, 1300);
//	fit->SetParameters(2, 7.59546e+00);
//	fit->SetParLimits(2, 0, 10);
//	fit->SetParameters(3, 2.23232e+01);
	Double_t offset = 0.2387;
	Double_t gain = 0;
	for(Int_t i=9890; i < 10128; ++i) {
		TH1D *hProj = hGe->ProjectionY(Form("%d_py", i), hGe->GetXaxis()->FindBin(i), hGe->GetXaxis()->FindBin(i) );
		if(!hProj->Fit("fit", "QSR+") ) {
			totAdcShift[i] = fit->GetParameter(1);
		}
	}
	return totAdcShift;
}
void GainDrift() {
	TFile *fData = new TFile("../al50.plain.root", "READ");
	TTree *tree = (TTree *)fData->Get("tree");
	Double_t t1, e1;
	Int_t a1, runId;
	TString *channel;
	tree->SetBranchAddress("runId", &runId);
	tree->SetBranchAddress("a1", &a1);
	tree->SetBranchAddress("e1", &e1);
	tree->SetBranchAddress("t1", &t1);
	tree->SetBranchAddress("channel", &channel);

	if(1) {
		Int_t min = 2680, max = 2840;
		Int_t bins = max-min;
		TH2D *hGeHiGain = new TH2D("hGeHiGain", "GeHiGain;run Id;ADC", 238, 9890, 10128, bins, min, max);

		for(Long64_t i=0; i < tree->GetEntries(); ++i) {
			tree->GetEntry(i);
			if(a1 < min || a1 > max) continue;
			if(channel->Contains("GeLoGain") ) {
			} else if(channel->Contains("GeHiGain") ) {
				hGeHiGain->Fill(runId, a1);
			}
		}
		std::map<Int_t, Double_t> HiAdcShift = GetFits(hGeHiGain, 2720, 2800);
		//From earlier energy calibration work
		Double_t HiOffset = 0.1238;
		Double_t HiGain = 0.1851;
		Double_t gain = 0.0f;
		for(std::map<Int_t, Double_t>::iterator it = HiAdcShift.begin(); it != HiAdcShift.end(); ++it) {
			gain = (511 - HiOffset)/it->second; //E - offset = gain*ADC
			printf("INSERT INTO FineGeCalib (run, channel, gain) VALUES(%d, '%s', %f);\n", it->first, "GeHiGain", gain);
		}
	} else {
		Int_t min = 1180, max = 1340;
		Int_t bins = max-min;
		TH2D *hGeLoGain = new TH2D("hGeLoGain", "GeLoGain;run Id;ADC", 238, 9890, 10128, bins, min, max);

		for(Long64_t i=0; i < tree->GetEntries(); ++i) {
			tree->GetEntry(i);
			if(a1 < min || a1 > max) continue;
			if(channel->Contains("GeLoGain") ) {
				hGeLoGain->Fill(runId, a1);
			} else if(channel->Contains("GeHiGain") ) {
			}
		}
		std::map<Int_t, Double_t> LoAdcShift = GetFits(hGeLoGain, 1240, 1300);
		//From earlier energy calibration work
		Double_t LoOffset = 0.2387;
		Double_t LoGain = 0.3991;
		Double_t gain = 0.0f;
		for(std::map<Int_t, Double_t>::iterator it = LoAdcShift.begin(); it != LoAdcShift.end(); ++it) {
			gain = (511 - LoOffset)/it->second; //E - offset = gain*ADC
			printf("INSERT INTO FineGeCalib (run, channel, gain) VALUES(%d, '%s', %f);\n", it->first, "GeLoGain", gain);
		}
	}
}
