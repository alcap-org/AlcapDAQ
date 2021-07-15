void VetoCorrectionAndErrors(std::string target) {
	const Int_t bins = 50;
	/*
 	 * al50 sil correction and error
 	 */
	if(target.compare("al50") == 0) {
		TH1D * hAl50SiLCorr = new TH1D("hAl50SiLCorr", "Al50SiLCorr;E[MeV]", bins, 0, 25);
		hAl50SiLCorr->SetBinContent(21, 3.78527);
		hAl50SiLCorr->SetBinContent(22, 10.0763);
		hAl50SiLCorr->SetBinContent(23, 15.7674);
		hAl50SiLCorr->SetBinContent(24, 33.1916);
		hAl50SiLCorr->SetBinContent(25, 38.3497);
		hAl50SiLCorr->SetBinContent(26, 38.4791);
		hAl50SiLCorr->SetBinContent(27, 30.3713);
		hAl50SiLCorr->SetBinContent(28, 20.9807);
		hAl50SiLCorr->SetBinContent(29, 4.86694);
		hAl50SiLCorr->SetBinError(21, 2.2995);
		hAl50SiLCorr->SetBinError(22, 3.77506);
		hAl50SiLCorr->SetBinError(23, 6.47173);
		hAl50SiLCorr->SetBinError(24, 1.81641);
		hAl50SiLCorr->SetBinError(25, 0.896904);
		hAl50SiLCorr->SetBinError(26, 13.1802);
		hAl50SiLCorr->SetBinError(27, 4.52338);
		hAl50SiLCorr->SetBinError(28, 9.93375);
		hAl50SiLCorr->SetBinError(29, 2.55789);
	}
	/*
 	 * al100 sil and sir correction and error
 	 */
	if(target.compare("al100") == 0) {
		TH1D *hAl100SiLCorr = new TH1D("hAl100SiLCorr", "Al100SiLCorr;E[MeV]", bins, 0, 25);
		hAl100SiLCorr->SetBinContent(21, 4.04104);
		hAl100SiLCorr->SetBinContent(22, 9.57486);
		hAl100SiLCorr->SetBinContent(23, 15.9   );
		hAl100SiLCorr->SetBinContent(24, 26.663);
		hAl100SiLCorr->SetBinContent(25, 33.1143);
		hAl100SiLCorr->SetBinContent(26, 33.2113);
		hAl100SiLCorr->SetBinContent(27, 30.9755);
		hAl100SiLCorr->SetBinContent(28, 20.8635);
		hAl100SiLCorr->SetBinContent(29, 4.0724);
		hAl100SiLCorr->SetBinError(21, 2.45488);
		hAl100SiLCorr->SetBinError(22, 3.5872);
		hAl100SiLCorr->SetBinError(23, 6.52617);
		hAl100SiLCorr->SetBinError(24, 1.45913);
		hAl100SiLCorr->SetBinError(25, 0.774461);
		hAl100SiLCorr->SetBinError(26, 11.3759);
		hAl100SiLCorr->SetBinError(27, 4.61336);
		hAl100SiLCorr->SetBinError(28, 9.87828);
		hAl100SiLCorr->SetBinError(29, 2.14031);

		TH1D *hAl100SiRCorr = new TH1D("hAl100SiRCorr", "Al100SiRCorr;E[MeV]", bins, 0, 25);
		hAl100SiRCorr->SetBinContent(21, 12.8);
		hAl100SiRCorr->SetBinContent(22, 26.0633);
		hAl100SiRCorr->SetBinContent(23, 36.087);
		hAl100SiRCorr->SetBinContent(24, 44.5641);
		hAl100SiRCorr->SetBinContent(25, 45.1183);
		hAl100SiRCorr->SetBinContent(26, 51.4228);
		hAl100SiRCorr->SetBinContent(27, 38.8915);
		hAl100SiRCorr->SetBinContent(28, 53.9929);
		hAl100SiRCorr->SetBinContent(29, 35.5169);
		hAl100SiRCorr->SetBinContent(30, 27.5392);
		hAl100SiRCorr->SetBinContent(31, 15.5727);
		hAl100SiRCorr->SetBinContent(32, 0.243902);
		hAl100SiRCorr->SetBinError(21, 7.77583);
		hAl100SiRCorr->SetBinError(22, 9.76458);
		hAl100SiRCorr->SetBinError(23, 14.812);
		hAl100SiRCorr->SetBinError(24, 2.43877);
		hAl100SiRCorr->SetBinError(25, 1.0552);
		hAl100SiRCorr->SetBinError(26, 17.6138);
		hAl100SiRCorr->SetBinError(27, 5.79235);
		hAl100SiRCorr->SetBinError(28, 25.5641);
		hAl100SiRCorr->SetBinError(29, 18.6664);
		hAl100SiRCorr->SetBinError(30, 0.0182321);
		hAl100SiRCorr->SetBinError(31, 7.21045);
		hAl100SiRCorr->SetBinError(32, 0.646068);

	}
	/*
	 * ti50 sil correction and error
	 */
	if(target.compare("ti50") == 0) {
		TH1D *hTi50SiLCorr = new TH1D("hTi50SiLCorr", "Ti50SiLCorr;E[MeV]", bins, 0, 25);
		hTi50SiLCorr->SetBinContent(21, 0.748503);
		hTi50SiLCorr->SetBinContent(22, 2.12389);
		hTi50SiLCorr->SetBinContent(24, 5.83567);
		hTi50SiLCorr->SetBinContent(24, 8.87138);
		hTi50SiLCorr->SetBinContent(25, 12.2621);
		hTi50SiLCorr->SetBinContent(26, 13.4877);
		hTi50SiLCorr->SetBinContent(27, 11.4146);
		hTi50SiLCorr->SetBinContent(28, 8.37323);
		hTi50SiLCorr->SetBinContent(29, 2.63496);
		hTi50SiLCorr->SetBinError(21, 0.175182);
		hTi50SiLCorr->SetBinError(22, 1.14867);
		hTi50SiLCorr->SetBinError(23, 2.04429);
		hTi50SiLCorr->SetBinError(24, 4.4422);
		hTi50SiLCorr->SetBinError(25, 1.03724);
		hTi50SiLCorr->SetBinError(26, 1.88063);
		hTi50SiLCorr->SetBinError(27, 3.92495);
		hTi50SiLCorr->SetBinError(28, 0.611184);
		hTi50SiLCorr->SetBinError(29, 0.767324);
	}
}

void DrawFolded(std::string target) {
	TFile *fData = new TFile(Form("%s/%s.root", getenv("R15b_DATA"), target.c_str() ), "READ");
//	target[0] = toupper(target[0]);
	TTree *tree = (TTree *)fData->Get("tree");
	Double_t t1, t2, t3, e1, e2, e3, timeToPrevTME, timeToNextTME;
	Int_t a1, a2, a3;
	TString *channel = new TString();
	TString *sig = new TString();
	TString *pt = new TString();
	tree->SetBranchAddress("timeToPrevTME", &timeToPrevTME);
	tree->SetBranchAddress("timeToNextTME", &timeToNextTME);
	tree->SetBranchAddress("a1", &a1);
	tree->SetBranchAddress("e1", &e1);
	tree->SetBranchAddress("t1", &t1);
	tree->SetBranchAddress("a2", &a2);
	tree->SetBranchAddress("e2", &e2);
	tree->SetBranchAddress("t2", &t2);
	tree->SetBranchAddress("a3", &a3);
	tree->SetBranchAddress("e3", &e3);
	tree->SetBranchAddress("t3", &t3);
	tree->SetBranchAddress("channel", &channel);
	tree->SetBranchAddress("sig2", &sig);
	tree->SetBranchAddress("pt2", &pt);
	
	const Int_t bins = 50;
	const char * filename = Form("%s/%s-folded.root", getenv("R15b_OUT"), target.c_str() );
	TFile *fOutput = new TFile(filename, "RECREATE");
	std::cout << "Output TFile to: " << filename << std::endl;
	TH1D *hpSiR = new TH1D("hpSiR", "Proton;E[MeV];Counts / 0.5 MeV", bins, 0, 25);
	TH1D *hpSiR3 = new TH1D("hpSiR3", "Proton;E[MeV];Counts / 0.5 MeV", bins, 0, 25);
	TH1D *hpSiRsub = new TH1D("hpSiRsub", "Proton;E[MeV];Counts / 0.5 MeV", bins, 0, 25);
	
	TH1D *hdSiR = new TH1D("hdSiR", "Deuteron;E[MeV];Counts / 0.5 MeV", bins, 0, 25);
	TH1D *htSiR = new TH1D("htSiR", "Triton;E[MeV];Counts / 0.5 MeV", bins, 0, 25);
	TH1D *haSiR = new TH1D("haSiR", "Alpha;E[MeV];Counts / 0.5 MeV", bins, 0, 25);
	TH1D *hpSiL = new TH1D("hpSiL", "Proton;E[MeV];Counts / 0.5 MeV", bins, 0, 25);
	TH1D *hdSiL = new TH1D("hdSiL", "Deuteron;E[MeV];Counts / 0.5 MeV", bins, 0, 25);
	TH1D *htSiL = new TH1D("htSiL", "Triton;E[MeV];Counts / 0.5 MeV", bins, 0, 25);
	TH1D *haSiL = new TH1D("haSiL", "Alpha;E[MeV];Counts / 0.5 MeV", bins, 0, 25);
	for(Int_t i=0; i < tree->GetEntries(); ++i) {
		tree->GetEntry(i);
		if(timeToPrevTME < 10e3 || timeToNextTME < 10e3) continue;
		if(t2<400) continue;
		if(t2>10e3) continue;
		if(target.compare("al50") == 0 || target.compare("ti50") == 0 ) {
			if(abs(t2-t1)> 500) continue; //Al50
		}
		if(target.compare("al100") == 0) {
			if(channel->Contains("SiL") ) {if(abs(t2-t1 + 567) > 500) continue; }
			if(channel->Contains("SiR") ) {if(abs(t2-t1 - 211) > 500) continue; }
		}
		if(a2>3980) continue;
		if(TMath::IsNaN(e3) ) {
			if(channel->Contains("SiR") ) {
				if(channel->Contains("SiR1_1") && a1>3850. ) continue;
				if(channel->Contains("SiR1_2") && a1>3972. ) continue;
				if(channel->Contains("SiR1_3") && a1>3956. ) continue;
				if(channel->Contains("SiR1_4") && a1>3986. ) continue;
				if(sig->Contains("proton") ) {
					hpSiR->Fill(e1+e2);
				}
				if(sig->Contains("deuteron") ) {
					hdSiR->Fill(e1+e2);
				}
				if(sig->Contains("triton") ) {
					htSiR->Fill(e1+e2);
				}
				if(sig->Contains("alpha") ) {
					haSiR->Fill(e1+e2);
				}
			} else if(channel->Contains("SiL") ) {
				if(sig->Contains("proton") ) {
					hpSiL->Fill(e1+e2);
				}
				if(sig->Contains("deuteron") ) {
					hdSiL->Fill(e1+e2);
				}
				if(sig->Contains("triton") ) {
					htSiL->Fill(e1+e2);
				}
				if(sig->Contains("alpha") ) {
					haSiL->Fill(e1+e2);
				}
			}
		} else {
			if(channel->Contains("SiR") ) {
				if(abs(t3-t2) > 500) continue;
				if(pt->Contains("proton") ) {
					hpSiR3->Fill(e1+e2+e3);
					hpSiRsub->Fill(e1+e2);
				}
			}
		}
	}
	Float_t veto = 0.74;
	hpSiR3->Scale(1/veto);
	hpSiRsub->Scale((1-veto)/veto);
	hpSiR->Add(hpSiR3);
	hpSiR->Add(hpSiRsub, -1);
	//set errors
	VetoCorrectionAndErrors(target);
	fOutput->Write();
	fOutput->Close();
}
