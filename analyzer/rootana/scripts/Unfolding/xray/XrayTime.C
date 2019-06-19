void XrayTime() {
	gStyle->SetOptFit();
	TFile *fData = new TFile("../al100.root", "READ");
	TTree *tree = (TTree *)fData->Get("tree");
	Double_t t1, e1, timeToPrevTME, timeToNextTME;
	TString *channel;
	tree->SetBranchAddress("timeToPrevTME", &timeToPrevTME);
	tree->SetBranchAddress("timeToNextTME", &timeToNextTME);
	tree->SetBranchAddress("e1", &e1);
	tree->SetBranchAddress("t1", &t1);
	tree->SetBranchAddress("channel", &channel);

	Int_t min = 0;
	Int_t max = 10000;
	Int_t bin = max-min;
	TH1D *hGeLoGain = new TH1D("hGeLoGain", "GeLoGain Al100 X-ray arrival time;Time [ns];Counts / 1 ns", bin, min, max);
	TH1D *hGeHiGain = new TH1D("hGeHiGain", "GeHiGain Al100 X-ray arrival time;Time [ns];Counts / 1 ns", bin, min, max);
	for(Long64_t i=0; i < tree->GetEntries(); ++i) {
		tree->GetEntry(i);
		if(timeToPrevTME < 10e3 || timeToNextTME < 10e3) continue;
		//if(e1 < 335 || e1 > 355) continue;
		if(channel->Contains("GeLoGain") ) {
			hGeLoGain->Fill(t1);
		} else if(channel->Contains("GeHiGain") ) {
			hGeHiGain->Fill(t1);
		}
	}
	TF1 *fit = new TF1("fit", "gaus(0)+expo(3)+expo(5)+pol0(7)", min, max);
	fit->SetParameter(0,  1.35253e+04);
	fit->SetParameter(1,  6.49693e+00);
	fit->SetParameter(2,  9.41678e+01);
	fit->SetParameter(3,  7.02852e+00);
	fit->SetParameter(4, -1.15e-3);
	fit->SetParameter(5,  1);
	fit->SetParameter(6, -1.25e-2);
	fit->SetParameter(7, 100);

	do {} while(
	hGeHiGain->Fit("fit", "SR+") );

	do {} while(
	hGeLoGain->Fit("fit", "SR+") );
}
