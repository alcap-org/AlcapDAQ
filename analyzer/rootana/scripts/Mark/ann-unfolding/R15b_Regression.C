double M = 938.27;
void LoadTraining(TTree * tTraining, const char * particle) {
	std::cout << "==========Training neural network==========" << std::endl;
	TChain *g4sim = new TChain("tree");
	g4sim->AddFile("/data/hdd4/R15bMC/protons/sf1.02.al50.proton.root");

	std::vector<std::string>* particleName = 0;
	std::vector<std::string>* volName = 0;
	std::vector<std::string>* ovolName = 0;
	std::vector<std::string>* oprocess = 0;
	std::vector<int> *stopped = 0;
	std::vector<double>* edep = 0;
	std::vector<double>* Ox = 0;
	std::vector<double>* Oy = 0;
	std::vector<double>* Oz = 0;
	std::vector<double>* Ot = 0;
	double px=0, py=0, pz=0;

	g4sim->SetBranchAddress("M_edep"	, &edep);
	g4sim->SetBranchAddress("M_ovolName"	, &ovolName);
	g4sim->SetBranchAddress("M_volName"	, &volName);
	g4sim->SetBranchAddress("M_oprocess"	, &oprocess);
	g4sim->SetBranchAddress("M_stopped"	, &stopped);
	g4sim->SetBranchAddress("M_particleName", &particleName);
	g4sim->SetBranchAddress("i_px"          , &px);
	g4sim->SetBranchAddress("i_py"          , &py);
	g4sim->SetBranchAddress("i_pz"          , &pz);

	Double_t SiR1_E, SiR2_E, SiR3_E, SiL1_E, SiL3_E, truth_E;
	tTraining->Branch("SiR1_E", &SiR1_E, "SiR1_E/D");
	tTraining->Branch("SiR2_E", &SiR2_E, "SiR2_E/D");
	tTraining->Branch("SiR3_E", &SiR3_E, "SiR3_E/D");
	tTraining->Branch("SiL1_E", &SiL1_E, "SiL1_E/D");
	tTraining->Branch("SiL3_E", &SiL3_E, "SiL3_E/D");
	tTraining->Branch("truth_E", &truth_E, "truth_E/D");

	bool stop = false;
	std::cout << "Loading training trees..." << std::endl;
	for(int i=0; i<g4sim->GetEntries(); i++) {
if(i>1e6) continue;
		if(i % 1000000 == 0) std::cout << i << "/" << g4sim->GetEntries() << std::endl;
		g4sim->GetEvent(i);
		if(volName->size() == 0) continue;
		for (unsigned iElement = 0; iElement < particleName->size(); ++iElement) {
			if(oprocess->at(iElement)!="NULL" && ovolName->at(iElement)!="Target") continue;
			double i_edep = edep->at(iElement)*1e3;
			if(volName->at(iElement)=="SiL3"){
				SiL3_E += i_edep;
				if(stopped->at(iElement) ) stop=true;
			} else if(volName->at(iElement)=="SiL1"){
				SiL1_E += i_edep;
			} else if(volName->at(iElement)=="SiR1"){
				SiR1_E += i_edep;
			} else if(volName->at(iElement)=="SiR2"){
				SiR2_E += i_edep;
			} else if(volName->at(iElement)=="SiR3"){
				SiR3_E += i_edep;
			}
		}
		if(stop) { 
			truth_E = sqrt(pz*pz + py*py + px*px + M*M) - M;
			if(SiL1_E>0.2 && SiL3_E>0.2) {
				tTraining->Fill();
			}
		}
		stop = false;
		truth_E=0;
		SiL1_E=0; SiL3_E=0;
		SiR1_E=0; SiR2_E=0; SiR3_E=0;
	}
}
void LoadTest(TMultiLayerPerceptron *mlp, const char * particle, TTree *tTest) {
	gStyle->SetOptStat(0);
	std::cout << "==========Generating test data==========" << std::endl;
	TFile *fTest = new TFile("/data/hdd4/R15bMC/protons/sf1.02.al50.proton.root", "READ");
	TTree *g4sim = (TTree *)fTest->Get("tree");
	std::vector<std::string>* particleName = 0;
	std::vector<std::string>* volName = 0;
	std::vector<std::string>* ovolName = 0;
	std::vector<std::string>* oprocess = 0;
	std::vector<int> *stopped = 0;
	std::vector<double>* edep = 0;
	std::vector<double>* Ox = 0;
	std::vector<double>* Oy = 0;
	std::vector<double>* Oz = 0;
	std::vector<double>* Ot = 0;
	double px=0, py=0, pz=0;
	g4sim->SetBranchAddress("M_edep"	, &edep);
	g4sim->SetBranchAddress("M_ovolName"	, &ovolName);
	g4sim->SetBranchAddress("M_volName"	, &volName);
	g4sim->SetBranchAddress("M_oprocess"	, &oprocess);
	g4sim->SetBranchAddress("M_stopped"	, &stopped);
	g4sim->SetBranchAddress("M_particleName", &particleName);
	g4sim->SetBranchAddress("i_px"          , &px);
	g4sim->SetBranchAddress("i_py"          , &py);
	g4sim->SetBranchAddress("i_pz"          , &pz);

	Double_t SiL1_E=0;
	Double_t SiL3_E=0;
	Double_t SiR1_E=0;
	Double_t SiR2_E=0;
	Double_t SiR3_E=0; 
	Double_t truth_E=0;
	tTest->Branch("SiR1_E", &SiR1_E, "SiR1_E/D");
	tTest->Branch("SiR2_E", &SiR2_E, "SiR2_E/D");
	tTest->Branch("SiR3_E", &SiR3_E, "SiR3_E/D");
	tTest->Branch("SiL1_E", &SiL1_E, "SiL1_E/D");
	tTest->Branch("SiL3_E", &SiL3_E, "SiL3_E/D");
	tTest->Branch("truth_E", &truth_E, "truth_E/D");

	bool stop = false;
	for(int i=0; i<g4sim->GetEntries(); i++) {
		if(i % 100000 == 0) std::cout << i << "/" << g4sim->GetEntries() << std::endl;
		g4sim->GetEvent(i);
		if(volName->size() == 0) continue;
		for (unsigned iElement = 0; iElement < particleName->size(); ++iElement) {
			if(oprocess->at(iElement)!="NULL" && ovolName->at(iElement)!="Target") continue;
			double i_edep = edep->at(iElement)*1e3;
			if(volName->at(iElement)=="SiL3"){
				SiL3_E += i_edep;
				if(stopped->at(iElement) ) stop=true;
			} else if(volName->at(iElement)=="SiL1"){
				SiL1_E += i_edep;
			} else if(volName->at(iElement)=="SiR1"){
				SiR1_E += i_edep;
			} else if(volName->at(iElement)=="SiR2"){
				SiR2_E += i_edep;
			} else if(volName->at(iElement)=="SiR3"){
				SiR3_E += i_edep;
			}
		}
		if(stop) { 
			truth_E = sqrt(pz*pz + py*py + px*px + M*M) - M;
		}
		if(SiL1_E!=0 && SiL3_E!=0) {
			tTest->Fill();
		}
		stop=false;
		truth_E=0;
		SiL1_E=0; SiL3_E=0;
		SiR1_E=0; SiR2_E=0; SiR3_E=0;
	}
	TH1F *hUnfold = new TH1F("hUnfold", "Unfolded data; Energy [MeV]", 25, 0, 25);
	TH1F *hEval = new TH1F("hEval", "Fake data; Energy [MeV]", 25, 0, 25);
	TH1F *hTrue = new TH1F("htTrue", "Truth; Energy [MeV]", 25, 0, 25);
	Double_t params[2];
	for(int i=0; i < tTest->GetEntries(); i++) {
		tTest->GetEntry(i);
		params[0] = SiL1_E;
		params[1] = SiL3_E;
		hUnfold->Fill(mlp->Evaluate(0, params) );
		hTrue->Fill(truth_E);
		hEval->Fill(SiL1_E+SiL3_E);
	}
//	TCanvas *cTest = new TCanvas("Test set", "Test set");
//	TPad *pad1 = new TPad("pad1","This is pad1",0.02,0.29,0.98,0.97);
//	TPad *pad2 = new TPad("pad2","This is pad2",0.02,0.02,0.98,0.27);
//	pad1->Draw();
//	pad2->Draw();
//
//	pad1->cd();
//	hUnfold->Draw("E");
//	hEval->Draw("SAME");
//	hEval->SetLineColor(kMagenta);
//	hTrue->Draw("SAME");
//	hTrue->SetLineColor(kGreen);
//	hEval->SetTitle("Neural network unfolding validation");
//
//	pad2->cd();
//	TH1 *hUnfold_clone = (TH1*) hUnfold->Clone();
//	hUnfold_clone->SetTitle(";;"); 
//	hUnfold_clone->GetYaxis()->SetLabelSize(0.1);
//	hUnfold_clone->GetXaxis()->SetLabelSize(0.1);
//	hUnfold_clone->GetYaxis()->SetTitle("Difference");
//	hUnfold_clone->GetYaxis()->SetTitleSize(0.1);
//	hUnfold_clone->GetYaxis()->SetTitleOffset(0.25);
//	TH1 *hTrue_clone = (TH1 *)hTrue->Clone();
//	hUnfold_clone->Add(hTrue_clone, -1);
//	hUnfold_clone->Divide(hTrue_clone);
//	hUnfold_clone->Scale(100);
//	hUnfold_clone->Draw("E");
//	TLine *line = new TLine(0, 0, 24, 0);
//	line->Draw("SAME");
	
}
void LoadData(TMultiLayerPerceptron *mlp, const char * particle) {
	std::cout << "==========Unfolding data==========" << std::endl;
	TFile *fData= new TFile("/home/m-wong/data/R15b/al50.root", "READ");
	TTree *tData = (TTree *)fData->Get("tree");
	Double_t e1, e2, e3, t1, t2, timeToPrevTME, timeToNextTME;
	Int_t a2;
	TString *channel = new TString("");
	TString  *sig = new TString("");
	TString  *pt = new TString("");
	TH1D *hMeasDataLeft = new TH1D("hMeasDataLeft", "hMeasDataLeft;E [keV]", 50, 0, 25);
	TH1D *hMeasDataRight = new TH1D("hMeasDataRight", "hMeasDataRight;E [keV]", 50, 0, 25);
	TH1D *hMeasDataRight3 = new TH1D("hMeasDataRight3", "hMeasDataRight3;E [keV]", 50, 0, 25);
Double_t params[2];
TH1D *hUnfold = new TH1D("hUnfold", "Unfolded data; Energy [MeV]", 50, 0, 25);
	TTree *tree = (TTree *)fData->Get("tree");
	tree->SetBranchAddress("timeToPrevTME", &timeToPrevTME);
	tree->SetBranchAddress("timeToNextTME", &timeToNextTME);
	tree->SetBranchAddress("e1", &e1);
	tree->SetBranchAddress("e2", &e2);
	tree->SetBranchAddress("a2", &a2);
	tree->SetBranchAddress("e3", &e3);
	tree->SetBranchAddress("t1", &t1);
	tree->SetBranchAddress("t2", &t2);
	tree->SetBranchAddress("channel", &channel);
	tree->SetBranchAddress("sig3", &sig);
	tree->SetBranchAddress("pt3", &pt);
	for(Long64_t i=0; i < tree->GetEntries(); i++) {
		tree->GetEntry(i);
		if(a2 > 3980) continue; //remove saturation
//		e1 = e1*1e3;
//		e2 = e2*1e3;
//		e3 = e3*1e3;
		if(timeToPrevTME < 10e3 || timeToNextTME < 10e3) continue;
		if(t2<400) continue;
		if(t2>10e3) continue;
		if(abs(t2-t1-12) > 20 * 5) continue; //Al50
		if(channel->Contains("SiL") ) {
			if(sig->Contains(particle) ) {
				hMeasDataLeft->Fill(e1+e2);
params[0] = e1;
params[1] = e2;
hUnfold->Fill(mlp->Evaluate(0, params) );				
			}
		} else if(channel->Contains("SiR") ) {
			if(TMath::IsNaN(e3) ) {
				if(sig->Contains(particle ) ) {
					hMeasDataRight->Fill(e1+e2);
				}
			} else {
				if(pt->Contains(particle ) ) {
					hMeasDataRight3->Fill(e1+e2+e3);
				}
			}
		}
	}
	hUnfold->Draw();
	hMeasDataLeft->Draw("SAME");
}
void R15b_Regression(const char * particle = "proton", Int_t training_epoch=10) {
	if (strcmp(particle, "proton") == 0) M = 938.27;
	if( !gROOT->GetClass("TMultiLayerPerceptron") ) {
		gSystem->Load("libMLP");
	}

	TFile *fOutputFile= new TFile("NNUnfolding-output.root", "RECREATE");
	TTree *tTraining= new TTree("Training", "Training");
	LoadTraining(tTraining, particle);

	TMultiLayerPerceptron *mlp = new TMultiLayerPerceptron("SiL1_E,SiL3_E:2:truth_E", tTraining);
	mlp->Train(100);
//	mlp->LoadWeights("unfolding-proton-SiL-weights");

	// analyze it
	//TCanvas* cIO=new TCanvas("TruthDeviation", "TruthDeviation");
	//cIO->Divide(2,2);
	//TMLPAnalyzer* mlpa=new TMLPAnalyzer(mlp);
	//mlpa->GatherInformations();
	//mlpa->CheckNetwork();

	// draw statistics shows the quality of the ANN's approximation
	//cIO->cd(1);
	// draw the difference between the ANN's output for (x,y) and 
	// the true value f(x,y), vs. f(x,y), as TProfiles
	//mlpa->DrawTruthDeviations();

	//cIO->cd(2);
	// draw the difference between the ANN's output for (x,y) and 
	// the true value f(x,y), vs. x, and vs. y, as TProfiles
	//mlpa->DrawTruthDeviationInsOut();

	//cIO->cd(3);
        //draw a box plot of the ANN's output for (x,y) vs f(x,y)
	//mlpa->GetIOTree()->Draw("Out.Out0-True.True0:True.True0>>hDelta","","goff");
	//TH2F* hDelta=(TH2F*)gDirectory->Get("hDelta");
	//hDelta->SetTitle("Difference between ANN output and truth vs. truth");
	//hDelta->Draw("E");

	Bool_t run_test = false;
	if(run_test) {
		TTree *tTest = new TTree("Test", "Test");
		LoadTest(mlp, particle, tTest);
	}
	//////Data//////
	LoadData(mlp, particle);
	fOutputFile->Write();
	fOutputFile->Close();
}
