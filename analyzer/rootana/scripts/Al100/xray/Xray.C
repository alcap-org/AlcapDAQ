void Xray() {
	gStyle->SetOptStat(0);
	TFile *fData = new TFile("al100.root", "READ");
	TTree *tree = (TTree *)fData->Get("tree");
	Double_t t1, e1, timeToPrevTME, timeToNextTME;
	TString *channel = new TString("");
	tree->SetBranchAddress("timeToPrevTME", &timeToPrevTME);
	tree->SetBranchAddress("timeToNextTME", &timeToNextTME);
	tree->SetBranchAddress("e1", &e1);
	tree->SetBranchAddress("t1", &t1);
	tree->SetBranchAddress("channel", &channel);

	Int_t min = 300, max = 520;
	Int_t bins = max-min;
	TFile *fOutput = new TFile("xray_output.root", "UPDATE");
	TH1D *hGeHiGainPrompt = new TH1D("hGeHiGainPrompt", "#muAl x-rays;E [keV];Counts/1 keV", bins, min, max);

	for(Long64_t i=0; i < tree->GetEntries(); ++i) {
		tree->GetEntry(i);
		if(timeToPrevTME < 10e3 || timeToNextTME < 10e3) continue;
		if(e1 < min || e1 > max) continue;
		if(channel->Contains("GeHiGain") ) {
			hGeHiGainPrompt->Fill(e1);
		}
	}
	alcaphistogram(hGeHiGainPrompt);
	hGeHiGainPrompt->Draw();
	TLegend *legend = new TLegend(.7, .6, .87, .87, "AlCap #bf{#it{X-ray} }");
	legend->AddEntry(hGeHiGainPrompt, "X-ray", "L");
	legend->Draw("SAME");

	TLatex label;
	label.SetTextAngle(90);
	label.DrawLatex(343, 30000, "2p-1s");
	label.DrawLatex(358, 10000, "Pb");
	label.DrawLatex(417, 6000, "3p-1s");
	label.DrawLatex(435, 20000, "Pb");
	label.DrawLatex(442, 17000, "4p-1s+Pb");
	label.DrawLatex(448, 4000, "5p-1s");
	label.DrawLatex(458, 5000, "6p-1s");

	fOutput->Write();
}
