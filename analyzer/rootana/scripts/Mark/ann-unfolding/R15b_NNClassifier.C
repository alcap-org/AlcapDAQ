double M = 938.27;
void Training(const char * particle, TTree *tTraining) {
	std::cout << "==========Training neural network==========" << std::endl;
	TChain *g4sim = new TChain("tree");
//	g4sim->AddFile("/data/R15bMC/electron.0-18.200k.target.100.leftright.root");
//	g4sim->AddFile("/data/R15bMC/muon.1-30.200k.target.100.leftright.root");
//	g4sim->AddFile("/data/R15bMC/pion.1-30.200k.target.100.leftright.root");
	g4sim->AddFile("/data/hdd4/R15bMC/protons/sf1.02.al50.proton.root");
	g4sim->AddFile("/data/hdd4/R15bMC/deuterons/sf1.02.al50.deuteron.root");
	g4sim->AddFile("/data/hdd4/R15bMC/tritons/sf1.02.al50.triton.root");
	g4sim->AddFile("/data/hdd4/R15bMC/alphas/sf1.02.al50.alpha.root");

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

	Int_t type = 0;
	Double_t SiL1_E = 0;
	Double_t SiL3_E = 0;
	Double_t SiR1_E = 0; 
	Double_t SiR2_E = 0; 
        Double_t SiR3_E = 0;
	//Tree to store neural network training data
	tTraining->Branch("SiR1_E", &SiR1_E, "SiR1_E/D");
	tTraining->Branch("SiR2_E", &SiR2_E, "SiR2_E/D");
	tTraining->Branch("SiR3_E", &SiR2_E, "SiR3_E/D");
	tTraining->Branch("SiL1_E", &SiL1_E, "SiL1_E/D");
	tTraining->Branch("SiL3_E", &SiL3_E, "SiL3_E/D");
	tTraining->Branch("type", &type, "type/I");
	for(int i=0; i<g4sim->GetEntries(); i++) {
		SiL1_E=0; SiL3_E=0; type=0;
		SiR1_E=0; SiR2_E=0;
		if(i % 1000000 == 0) std::cout << i << "/" << g4sim->GetEntries() << std::endl;
		g4sim->GetEvent(i);
		if(volName->size() == 0) continue;
		for (unsigned iElement = 0; iElement < particleName->size(); ++iElement) {
			if(oprocess->at(iElement)!="NULL" && ovolName->at(iElement)!="Target") continue;
			double i_edep = edep->at(iElement)*1e3;
			if(volName->at(iElement)=="SiL3"){
				SiL3_E += i_edep;
				if(particleName->at(iElement)==particle && stopped->at(iElement) ) type=1;
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
		if(SiL1_E!=0 && SiL3_E!=0) {
			tTraining->Fill();
		}
	}
}
void Test(TMultiLayerPerceptron *mlp, TTree *tTraining, TTree *tTest) {
	std::cout << "==========Generating test data==========" << std::endl;
	Double_t SiL1_E = 0;
	Double_t SiL3_E = 0;
	Double_t SiR1_E = 0; 
	Double_t SiR2_E = 0; 
        Double_t SiR3_E = 0;
	tTraining->SetBranchAddress("SiR1_E", &SiR1_E);
	tTraining->SetBranchAddress("SiR2_E", &SiR2_E);
	tTraining->SetBranchAddress("SiR3_E", &SiR3_E);
	tTraining->SetBranchAddress("SiL1_E", &SiL1_E);
	tTraining->SetBranchAddress("SiL3_E", &SiL3_E);

	//Tree to store neural network test data
	Double_t tSiL1_E = 0;
	Double_t tSiL3_E = 0;
	Double_t tSiR1_E = 0; 
	Double_t tSiR2_E = 0; 
        Double_t tSiR3_E = 0;
	tTest->Branch("SiR1_E", &tSiR1_E, "SiR1_E/D");
	tTest->Branch("SiR2_E", &tSiR2_E, "SiR2_E/D");
	tTest->Branch("SiR3_E", &tSiR2_E, "SiR3_E/D");
	tTest->Branch("SiL1_E", &tSiL1_E, "SiL1_E/D");
	tTest->Branch("SiL3_E", &tSiL3_E, "SiL3_E/D");

	TRandom *rand = new TRandom3();
	for(int i=0; i<tTraining->GetEntries(); i++) {
		tSiR1_E = rand->Gaus(SiR1_E, 0.01);
		tSiR2_E = rand->Gaus(SiR2_E, 0.01);
		tSiR3_E = rand->Gaus(SiR3_E, 0.01);
		tSiL1_E = rand->Gaus(SiL1_E, 0.01);
		tSiL3_E = rand->Gaus(SiL3_E, 0.01);
		tTest->Fill();
	}

	TH1F *htEval = new TH1F("htEval", "Neural network evaluation function", 50, -.1, 1.1);
	TH2D *htRaw = new TH2D("htRaw", "SiL protons without NN cut; E [MeV]; dE [MeV]", 128, 0, 24, 128, 0, 8);
	TH2D *htNNCut = new TH2D("htNNCut", "SiL protons with NN cut; E [MeV]; dE [MeV]", 128, 0, 24, 128, 0, 8);
	htNNCut->SetDirectory(0);
	Double_t params[2];
	for (int i = 0; i < tTest->GetEntries(); i++) {
		if(i % 10000 == 0) std::cout << i << "/" << tTest->GetEntries() << std::endl;
		tTest->GetEntry(i);
		if(tSiL1_E==0 || tSiL3_E==0) continue;
		params[0] = tSiL1_E;
		params[1] = tSiL3_E;
//		params[0] = dSiR1_E;
//		params[1] = dSiR2_E;
		htEval->Fill(mlp->Evaluate(0, params) );
		if(mlp->Evaluate(0, params) > 0.5) {
			htNNCut->Fill(tSiL1_E+tSiL3_E, tSiL1_E);
		}
		if(mlp->Evaluate(0, params) <= 0.5) {
			htRaw->Fill(tSiL1_E+tSiL3_E, tSiL1_E);
		}
//		if(mlp->Evaluate(0, params) > 0.5) hNNCut->Fill(dSiR1_E+dSiR2_E, dSiR1_E);
//		if(mlp->Evaluate(0, params) <= 0.5) hRaw->Fill(dSiR1_E+dSiR2_E, dSiR1_E);
	}
	TH1D *htNNCut_px = htNNCut->ProjectionX();

	TCanvas *cTest = new TCanvas("Test set", "Test set");
	htEval->SetLineColor(kBlue);
	htEval->SetFillStyle(3008);
	htEval->SetFillColor(kBlue);
	htEval->SetStats(0);
	htEval->Draw();

	TCanvas *cTest2 = new TCanvas("Test set result", "Test set result");
	cTest2->Divide(1, 2);
	cTest2->cd(1);
	htRaw->SetMarkerColor(kRed);
	htRaw->Draw();
	htNNCut->SetMarkerColor(kBlue);
	htNNCut->Draw("SAME");
	cTest2->cd(2);
	htNNCut_px->Draw();
}
void Data(TMultiLayerPerceptron *mlp) {
	std::cout << "==========Classifying data==========" << std::endl;
	TFile *fData = new TFile("tproof-output.root", "READ");
	TTree *tData = (TTree *)fData->Get("tNNUnfolding");
	Double_t dSiR1_E, dSiR2_E, dSiR3_E,dSiL1_E, dSiL3_E;
	tData->SetBranchAddress("SiR1_E", &dSiR1_E);
	tData->SetBranchAddress("SiR2_E", &dSiR2_E);
	tData->SetBranchAddress("SiR3_E", &dSiR3_E);
	tData->SetBranchAddress("SiL1_E", &dSiL1_E);
	tData->SetBranchAddress("SiL3_E", &dSiL3_E);

	//Tree to store particle classification data
	Int_t Pid = 0;
	Double_t SiL1_E, SiL3_E;
	Double_t SiR1_E, SiR2_E, SiR3_E;
	TFile *fNNOutput = new TFile("NNclassifier-output.root", "RECREATE");
	TTree *tNNClassifierOutput = new TTree("tNNClassifierOutput", "NNClassifierOutput");
	tNNClassifierOutput->Branch("SiR1_E", &SiR1_E, "SiR1_E/D");
	tNNClassifierOutput->Branch("SiR2_E", &SiR2_E, "SiR2_E/D");
	tNNClassifierOutput->Branch("SiR3_E", &SiR3_E, "SiR3_E/D");
	tNNClassifierOutput->Branch("SiL1_E", &SiL1_E, "SiL1_E/D");
	tNNClassifierOutput->Branch("SiL3_E", &SiL3_E, "SiL3_E/D");
	tNNClassifierOutput->Branch("Pid", &Pid, "Pid/I");

	TH1F *hEval = new TH1F("hEval", "Neural network evaluation function", 120, -.1, 1.1);
	TH2D *hRaw = new TH2D("hRaw", "SiL protons without NN cut; E [MeV]; dE [MeV]", 128, 0, 24, 128, 0, 8);
	TH2D *hNNCut = new TH2D("hNNCut", "SiL protons with NN cut; E [MEV]; dE [MeV]", 128, 0, 24, 128, 0, 8);
	Double_t params[2];
	for (int i = 0; i < tData->GetEntries(); i++) {
		tData->GetEntry(i);
		SiL1_E = 0;
		SiL3_E = 0;
		SiR1_E = 0;
		SiR2_E = 0;
		SiR3_E = 0;
		if(dSiL1_E==0 && dSiL3_E==0) continue;
		params[0] = dSiL1_E;
		params[1] = dSiL3_E;
//		params[0] = dSiR1_E;
//		params[1] = dSiR2_E;
		hEval->Fill(mlp->Evaluate(0, params) );
		if(mlp->Evaluate(0, params) > 0.5) {
			hNNCut->Fill(dSiL1_E+dSiL3_E, dSiL1_E);
			SiL1_E = dSiL1_E;
			SiL3_E = dSiL3_E;
			tNNClassifierOutput->Fill();
			
		}
		if(mlp->Evaluate(0, params) <= 0.5) {
			hRaw->Fill(dSiL1_E+dSiL3_E, dSiL1_E);
		}
//		if(mlp->Evaluate(0, params) > 0.5) hNNCut->Fill(dSiR1_E+dSiR2_E, dSiR1_E);
//		if(mlp->Evaluate(0, params) <= 0.5) hRaw->Fill(dSiR1_E+dSiR2_E, dSiR1_E);
	}
	TH1D *hNNCut_px = hNNCut->ProjectionX();

	TCanvas *e = new TCanvas("Data set", "Data set");
	hEval->SetLineColor(kBlue);
	hEval->SetFillStyle(3008);
	hEval->SetFillColor(kBlue);
	hEval->SetStats(0);
	hEval->Draw();

	TCanvas *c1 = new TCanvas("Data set result", "Data set result");
	c1->Divide(1, 2);
	c1->cd(1);
	hRaw->SetMarkerColor(kRed);
	hRaw->Draw();
	hNNCut->SetMarkerColor(kBlue);
	hNNCut->Draw("colz SAME");
	c1->cd(2);
	hNNCut_px->Draw();
	fNNOutput->Write();
}
void R15b_NNClassifier(const char * particle = "proton", Int_t training_epoch=10) {
	if (strcmp(particle, "proton")==0) M = 938.27;
	if( !gROOT->GetClass("TMultiLayerPerceptron") ) {
		gSystem->Load("libMLP");
	}

	TTree *tTraining = new TTree("tTraining", "Training");
	Training(particle, tTraining);

	TMultiLayerPerceptron *mlp = new TMultiLayerPerceptron("SiL1_E,SiL3_E:20:type!", tTraining);
//	mlp->LoadWeights("proton-SiL-weights");

	TCanvas *mlpa_canvas = new TCanvas("mlpa_canvas", "Network analysis");
	mlpa_canvas->Divide(2,2);
	TMLPAnalyzer ana(mlp);
	// Initialisation
	ana.GatherInformations();
	// output to the console
	ana.CheckNetwork();
	mlpa_canvas->cd(1);
	// shows how each variable influences the network
	ana.DrawDInputs();
	mlpa_canvas->cd(2);
	// shows the network structure
	mlp->Draw();
	mlpa_canvas->cd(3);
	// draws the resulting network
	ana.DrawNetwork(0,"type==1","type==0");

	///////////////Fake Data//////////////////////////////////////////////////
	TTree *tTest = new TTree("tTest", "Test");
	Test(mlp, tTraining, tTest);
	///////////////Real Data//////////////////////////////////////////////////
	Data(mlp);
}
