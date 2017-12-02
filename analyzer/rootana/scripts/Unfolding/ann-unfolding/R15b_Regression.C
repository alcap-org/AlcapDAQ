Bool_t debug = false;
double M = 938.27;
void LoadTraining(TTree * tTraining, const char * particle) {
	std::cout << "==========Training neural network==========" << std::endl;
	TChain *g4sim = new TChain("tree");
	//	g4sim->AddFile("data/electrons-100k.root");
	//	g4sim->AddFile("data/muons-100k.root");
	//	g4sim->AddFile("data/pions-100k.root");
	g4sim->AddFile("/home/mark/montecarlo/proton.0-24.200k.target.200.leftright.root");
	//	g4sim->AddFile("data/deuterons-100k.root");
	//	g4sim->AddFile("data/tritons-100k.root");
	//	g4sim->AddFile("data/alphas-100k.root");
	//	TFile *s = new TFile("data/training-100k.root", "READ");
	//
	//	TTree *g4sim = (TTree *)s->Get("tree");

	const std::vector<double> *Ot, *edep;
	double px=0, py=0, pz=0;
	std::vector<int> *stopped;
	std::vector<std::string> *volName, *ovolName, *oprocess, *particleName;
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
	Long64_t numOfEntries = (debug) ? 1000 : g4sim->GetEntries();
	for(int i=0; i<numOfEntries; i++) {
		if(i % 100000 == 0) std::cout << i << "/" << g4sim->GetEntries() << std::endl;
		g4sim->GetEvent(i);
		if(volName->size() == 0) continue;
		for (unsigned iElement = 0; iElement < particleName->size(); ++iElement) {
			if(oprocess->at(iElement)!="NULL" && ovolName->at(iElement)!="Target") continue;
			//			if(particleName->at(iElement) == particle && volName->at(iElement)=="SiR2" && stopped->at(iElement) ) { } else continue;
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
			if(SiL1_E!=0 && SiL3_E!=0) {
				tTraining->Fill();
			}
		}
		stop = false;
		truth_E=0;
		SiL1_E=0; SiL3_E=0;
		SiR1_E=0; SiR2_E=0; SiR3_E=0;
	}
}
void LoadTest(TMultiLayerPerceptron *mlp, TTree * tTest, const char * particle) {
	std::cout << "==========Generating test data==========" << std::endl;
	//TFile *test_file = new TFile("data/old-data-exponential-depth/regression-test-proton-uniform_0-10MeV.root", "READ");
	TFile *fTest = new TFile("/home/mark/montecarlo/proton.5expo2.20k.target.100.leftright.root", "READ");
	//TFile *test_file = new TFile("data/regression-test-proton-6MeV-sigma2.root", "READ");
	TTree *g4sim = (TTree *)fTest->Get("tree");
	const std::vector<double> *Ot, *edep;
	double px=0, py=0, pz=0;
	std::vector<int> *stopped;
	std::vector<std::string> *volName, *ovolName, *oprocess, *particleName;
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
	tTest->Branch("SiR1_E", &SiR1_E, "SiR1_E/D");
	tTest->Branch("SiR2_E", &SiR2_E, "SiR2_E/D");
	tTest->Branch("SiR3_E", &SiR3_E, "SiR3_E/D");
	tTest->Branch("SiL1_E", &SiL1_E, "SiL1_E/D");
	tTest->Branch("SiL3_E", &SiL3_E, "SiL3_E/D");
	tTest->Branch("truth_E", &truth_E, "truth_E/D");

	bool stop = false;
	for(int i=0; i<g4sim->GetEntries(); i++) {
		if(i % 1000 == 0) std::cout << i << "/" << g4sim->GetEntries() << std::endl;
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
	fTest->Close();	
	TH1F *hUnfold = new TH1F("hUnfold", "Unfolded data; Energy [MeV]", 24, 0, 24);
	TH1F *hEval = new TH1F("hEval", "Fake data; Energy [MeV]", 24, 0, 24);
	TH1F *hTrue = new TH1F("htTrue", "Truth; Energy [MeV]", 24, 0, 24);
	Double_t params[2];
	for(int i=0; i < tTest->GetEntries(); i++) {
		tTest->GetEntry(i);
		params[0] = SiL1_E;
		params[1] = SiL3_E;
		hUnfold->Fill(mlp->Evaluate(0, params) );
		hTrue->Fill(truth_E);
		hEval->Fill(SiL1_E+SiL3_E);
	}
	TCanvas *cTest = new TCanvas("Test set", "Test set");
	hEval->Draw("E"); hEval->SetLineColor(kMagenta);
	hTrue->Draw("SAME"); hTrue->SetLineColor(kGreen);
	hEval->SetTitle("Neural network unfolding validation");
}
void LoadData(TMultiLayerPerceptron *mlp, const char * particle) {
	std::cout << "==========Unfolding data==========" << std::endl;
	TFile *fData= new TFile("NNclassifier-output.root", "READ");
	TTree *tData = (TTree *)fData->Get("tNNClassifierOutput");

	Double_t SiL1_E, SiL3_E, SiR1_E, SiR2_E, SiR3_E; 
	tData->SetBranchAddress("SiL1_E", &SiL1_E);
	tData->SetBranchAddress("SiL3_E", &SiL3_E);
	tData->SetBranchAddress("SiR1_E", &SiR1_E);
	tData->SetBranchAddress("SiR2_E", &SiR2_E);
	tData->SetBranchAddress("SiR3_E", &SiR3_E);

	TH1D *hUnfold = new TH1D("hEval", "Unfolded data; Energy [MeV]", 24, 0, 24);
	TH1D *hMeas = new TH1D("hMeas", "Data; Energy [MeV]", 24, 0, 24);
	TH1D *hTrue = new TH1D("hTrue", "Truth; Energy [MeV]", 24, 0, 24);
	TH2D *hEvdE = new TH2D("hEvdE", "Data EvdE; E [MeV]; dE [MeV]", 128, 0, 24, 128, 0, 8);
	Double_t params[2];
	for(int i=0; i < tData->GetEntries(); i++) {
		tData->GetEntry(i);
		params[0] = SiL1_E;
		params[1] = SiL3_E;
		hUnfold->Fill(mlp->Evaluate(0, params) );
		hMeas->Fill(SiL1_E+SiL3_E);
		hEvdE->Fill(SiL1_E+SiL3_E, SiL1_E);
	}
	TCanvas *cData = new TCanvas("Data set", "Data set");
	hUnfold->Draw("E");
	hMeas->Draw("SAME");
	hMeas->SetLineColor(kMagenta);
	cData->BuildLegend();
	TCanvas *debug = new TCanvas("Debug", "Debug");
	hEvdE->Draw("colz");
}
void R15b_Regression(const char * particle = "proton", Int_t training_epoch=10) {
	if (particle == "proton") M = 938.27;
	if( !gROOT->GetClass("TMultiLayerPerceptron") ) {
		gSystem->Load("libMLP");
	}

	TFile *nn_output = new TFile("nn_output.root", "RECREATE");
	TTree *tTraining= new TTree("Training", "Training");
	LoadTraining(tTraining, particle);

//	TMultiLayerPerceptron *mlp = new TMultiLayerPerceptron("SiL1_E,SiL3_E:2:truth_E", training, "Entry$%2", "(Entry$%2)==0", TNeuron::kSigmoid); 
	TMultiLayerPerceptron *mlp = new TMultiLayerPerceptron("SiL1_E,SiL3_E:2:truth_E", tTraining);
	mlp->LoadWeights("unfolding-proton-SiL-weights");
	mlp->SetLearningMethod(TMultiLayerPerceptron::kBFGS);
	mlp->Train(training_epoch, "text,graph update=10"); //text, graph
	mlp->DumpWeights();

	// analyze it
	TMLPAnalyzer* mlpa=new TMLPAnalyzer(mlp);
	mlpa->GatherInformations();
	mlpa->CheckNetwork();
//	mlpa->DrawDInputs();

	// draw statistics shows the quality of the ANN's approximation
	TCanvas* cIO=new TCanvas("TruthDeviation", "TruthDeviation");
	cIO->Divide(2,2);
	cIO->cd(1);
	TCanvas *d = new TCanvas("d", "d");
	// draw the difference between the ANN's output for (x,y) and 
	// the true value f(x,y), vs. f(x,y), as TProfiles
	mlpa->DrawTruthDeviations();

	cIO->cd(2);
	// draw the difference between the ANN's output for (x,y) and 
	// the true value f(x,y), vs. x, and vs. y, as TProfiles
	mlpa->DrawTruthDeviationInsOut();

	cIO->cd(3);
      // draw a box plot of the ANN's output for (x,y) vs f(x,y)
	mlpa->GetIOTree()->Draw("Out.Out0-True.True0:True.True0>>hDelta","","goff");
	TH2F* hDelta=(TH2F*)gDirectory->Get("hDelta");
	hDelta->SetTitle("Difference between ANN output and truth vs. truth");
	hDelta->Draw("E");

	Bool_t run_test = false;
	if(run_test) {
		TTree *tTest = new TTree("Test", "Test");
		LoadTest(mlp, tTest, particle);
	}
	//////Data//////
	LoadData(mlp, particle);
	nn_output->Write();
}
