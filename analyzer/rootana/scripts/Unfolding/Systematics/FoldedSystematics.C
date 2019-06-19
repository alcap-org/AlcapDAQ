void lifetime(TTree *tree, Double_t *lifetimeError, TString arm = "SiR", TString particle = "proton") {
	Double_t t1, t2, t3, a1, a2, a3, e1, e2, e3, timeToPrevTME, timeToNextTME;
	TString *channel = new TString();
	TString *sig1 = new TString();
	TString *sig2 = new TString();
	TString *sig3 = new TString();
	TString *sig4 = new TString();
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
	tree->SetBranchAddress("sig1", &sig1);
	tree->SetBranchAddress("sig2", &sig2);
	tree->SetBranchAddress("sig3", &sig3);
	tree->SetBranchAddress("sig4", &sig4);

	Int_t nbins = 40;
	TH1D *hOne = new TH1D(Form("h300_%s_%s", arm.Data(), particle.Data() ), "300ns;E [keV];Counts / 500keV", nbins, 0, 20000); 
	TH1D *hTwo = new TH1D(Form("h400_%s_%s", arm.Data(), particle.Data() ), "nbins0ns;E [keV];Counts / 500keV", nbins, 0, 20000); 
	TH1D *hThree = new TH1D(Form("h500_%s_%s", arm.Data(), particle.Data() ), "500ns;E [keV];Counts / 500keV", nbins, 0, 20000); 
	TH1D *hFour = new TH1D(Form("h600_%s_%s", arm.Data(), particle.Data() ), "600ns;E [keV];Counts / 500keV", nbins, 0, 20000); 
	for(Long64_t i=0; i < tree->GetEntries(); ++i) {
		tree->GetEntry(i);
                if(timeToPrevTME < 10e3 || timeToNextTME < 10e3) continue;
                if(t2>10e3) continue;
		if(t2-t1-13 > 60 || t2-t1-13 < -60) continue;
		if(!TMath::IsNaN(e3) ) continue;
		if(!sig3->Contains(particle) ) continue;
		if(!arm.CompareTo("SiR") ) {
			if(channel->Contains("SiL") ) continue;
			//remove saturated pulses
			if(channel->Contains("SiR1_1") && a1>3850. ) continue;
			if(channel->Contains("SiR1_2") && a1>3972. ) continue;
			if(channel->Contains("SiR1_3") && a1>3956. ) continue;
			if(channel->Contains("SiR1_4") && a1>3986. ) continue;
		} else {
			if(channel->Contains("SiR") ) continue;
		}
		e1 *= 1000;
		e2 *= 1000;
		if(t2 > 300) hOne->Fill(e1+e2);
		if(t2 > 400) hTwo->Fill(e1+e2);
		if(t2 > 500) hThree->Fill(e1+e2);
		if(t2 > 600) hFour->Fill(e1+e2);
	}

	TCanvas *dlifetime = new TCanvas(Form("lifetime_%s_%s", arm.Data(), particle.Data() ), Form("lifetime %s %s", arm.Data(), particle.Data() ) );
	hFour->Draw("E PLC");
	hThree->Draw("ESAME PLC");
	hTwo->Draw("ESAME PLC");
	hOne->Draw("ESAME PLC");
	dlifetime->BuildLegend();
	dlifetime->Write();

	std::cout << "lifetime" << std::endl;
	std::cout << "time\t|" << "300\t|" << "400\t|" << "500\t|" << "600" << std::endl;
	for(int i=1; i<=nbins; ++i) { //ignore under and overflow bins
		std::cout << (i-1)*500 << "\t|" << hOne->GetBinContent(i) << "\t" << hTwo->GetBinContent(i) << "\t|" <<  hThree->GetBinContent(i) << "\t|" << hFour->GetBinContent(i) << std::endl;
	}
	std::cout << std::endl;
	std::cout << "lifetime Corrected" << std::endl;
	std::cout << "time\t|" << "300\t|" << "400\t|" << "500\t|" << "600" << std::endl;
	for(int i=1; i<=nbins; ++i) { //ignore under and overflow bins
		std::cout << (i-1)*500 << "\t|" << hOne->GetBinContent(i)/0.71 << "\t|" << hTwo->GetBinContent(i)/0.63 << "\t|" <<  hThree->GetBinContent(i)/0.56 << "\t|" << hFour->GetBinContent(i)/0.50 << std::endl;
		lifetimeError[i] = abs(1- ((hTwo->GetBinContent(i)/0.63) / (hThree->GetBinContent(i)/0.56) ) );
	}
}


void dt(TTree *tree, Double_t *dtError, TString arm = "SiR", TString particle = "proton") {
	Double_t t1, t2, t3, a1, a2, a3, e1, e2, e3, timeToPrevTME, timeToNextTME;
	TString *channel = new TString();
	TString *sig1 = new TString();
	TString *sig2 = new TString();
	TString *sig3 = new TString();
	TString *sig4 = new TString();
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
	tree->SetBranchAddress("sig1", &sig1);
	tree->SetBranchAddress("sig2", &sig2);
	tree->SetBranchAddress("sig3", &sig3);
	tree->SetBranchAddress("sig4", &sig4);

	Int_t nbins = 40;
	TH1D *hOne = new TH1D(Form("hdt_1_%s_%s", arm.Data(), particle.Data() ), "1#sigma;E [keV];Counts / 500keV", nbins, 0, 20000); 
	TH1D *hTwo = new TH1D(Form("hdt_2_%s_%s", arm.Data(), particle.Data() ), "2#sigma;E [keV];Counts / 500keV", nbins, 0, 20000); 
	TH1D *hThree = new TH1D(Form("hdt_3_%s_%s", arm.Data(), particle.Data() ), "3#sigma;E [keV];Counts / 500keV", nbins, 0, 20000); 
	TH1D *hFour = new TH1D(Form("hdt_4_%s_%s", arm.Data(), particle.Data() ), "4#sigma;E [keV];Counts / 500keV", nbins, 0, 20000); 
	for(Long64_t i=0; i < tree->GetEntries(); ++i) {
		tree->GetEntry(i);
		if(timeToPrevTME < 10e3 || timeToNextTME < 10e3) continue;
		if(t2<500) continue;
		if(t2>10e3) continue;
		if(!TMath::IsNaN(e3) ) continue;
		if(!sig3->Contains(particle) ) continue;
		if(!arm.CompareTo("SiR") ) {
			if(channel->Contains("SiL") ) continue;
			if(channel->Contains("SiR1_1") && a1>3850. ) continue;
			if(channel->Contains("SiR1_2") && a1>3972. ) continue;
			if(channel->Contains("SiR1_3") && a1>3956. ) continue;
			if(channel->Contains("SiR1_4") && a1>3986. ) continue;
		} else {
			if(channel->Contains("SiR") ) continue;
		}
		e1 *= 1000;
		e2 *= 1000;
		if(abs(t2-t1-13) < 20) hOne->Fill(e1+e2);
		if(abs(t2-t1-13) < 40) hTwo->Fill(e1+e2);
		if(abs(t2-t1-13) < 60) hThree->Fill(e1+e2);
		if(abs(t2-t1-13) < 80) hFour->Fill(e1+e2);
	}
	
	TCanvas *dt = new TCanvas(Form("dt_%s_%s", arm.Data(), particle.Data() ), Form("dt %s %s", arm.Data(), particle.Data() ) );
	hFour->Draw("E PLC");
	hThree->Draw("ESAME PLC");
	hTwo->Draw("ESAME PLC");
	hOne->Draw("ESAME PLC");
	dt->BuildLegend();
	dt->Write();

	std::cout << "dt" << std::endl;
	std::cout << "E bin\t|" << "1σ\t|" << "2σ\t|" << "3σ\t|" << "4σ" << std::endl;
	for(int i=1; i<=nbins; ++i) { //ignore under and overflow bins
		std::cout << (i-1)*500 << "\t|" << hOne->GetBinContent(i) << "\t" << hTwo->GetBinContent(i) << "\t|" <<  hThree->GetBinContent(i) << "\t|" << hFour->GetBinContent(i) << std::endl;
	}
	std::cout << std::endl;
	std::cout << "dt Corrected" << std::endl;
	std::cout << "E bin\t|" << "1σ\t|" << "2σ\t|" << "3σ\t|" << "4σ" << std::endl;
	for(int i=1; i<=nbins; ++i) { //ignore under and overflow bins
		std::cout << (i-1)*500 << "\t|" << hOne->GetBinContent(i)/0.682 << "\t|" << hTwo->GetBinContent(i)/0.954 << "\t|" <<  hThree->GetBinContent(i)/0.996 << "\t|" << hFour->GetBinContent(i)/0.998 << std::endl;
		dtError[i] = abs(1- ((hTwo->GetBinContent(i)/0.954) / (hThree->GetBinContent(i)/0.996) ) );
	}
}
void Pid(TTree *tree, Double_t *pidError, TString arm = "SiR", TString particle = "proton") {
	Double_t t1, t2, t3, a1, a2, a3, e1, e2, e3, timeToPrevTME, timeToNextTME;
	TString *channel = new TString();
	TString *sig1 = new TString();
	TString *sig2 = new TString();
	TString *sig3 = new TString();
	TString *sig4 = new TString();
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
	tree->SetBranchAddress("sig1", &sig1);
	tree->SetBranchAddress("sig2", &sig2);
	tree->SetBranchAddress("sig3", &sig3);
	tree->SetBranchAddress("sig4", &sig4);

	Int_t nbins = 40;
	TH1D *hOne = new TH1D(Form("hPid_1_%s_%s", arm.Data(), particle.Data() ), "1#sigma;E [keV];Counts / 500keV", nbins, 0, 20000); 
	TH1D *hTwo = new TH1D(Form("hPid_2_%s_%s", arm.Data(), particle.Data() ), "2#sigma;E [keV];Counts / 500keV", nbins, 0, 20000); 
	TH1D *hThree = new TH1D(Form("hPid_3_%s_%s", arm.Data(), particle.Data() ), "3#sigma;E [keV];Counts / 500keV", nbins, 0, 20000); 
	TH1D *hFour = new TH1D(Form("hPid_4_%s_%s", arm.Data(), particle.Data() ), "4#sigma;E [keV];Counts / 500keV", nbins, 0, 20000); 
	for(Long64_t i=0; i < tree->GetEntries(); ++i) {
		tree->GetEntry(i);
		if(timeToPrevTME < 10e3 || timeToNextTME < 10e3) continue;
		if(t2<500) continue;
		if(t2>10e3) continue;
		if(t2-t1-13 > 60 || t2-t1-13 < -60) continue;
		if(!TMath::IsNaN(e3) ) continue;
		if(!arm.CompareTo("SiR") ) {
			if(channel->Contains("SiL") ) continue;
			if(channel->Contains("SiR1_1") && a1>3850. ) continue;
			if(channel->Contains("SiR1_2") && a1>3972. ) continue;
			if(channel->Contains("SiR1_3") && a1>3956. ) continue;
			if(channel->Contains("SiR1_4") && a1>3986. ) continue;
		} else {
			if(channel->Contains("SiR") ) continue;
		}
		e1 *= 1000;
		e2 *= 1000;
		if(sig1->Contains(particle) ) hOne->Fill(e1+e2);
		if(sig2->Contains(particle) ) hTwo->Fill(e1+e2);
		if(sig3->Contains(particle) ) hThree->Fill(e1+e2);
		if(sig4->Contains(particle) ) hFour->Fill(e1+e2);
	}

	TCanvas *d = new TCanvas(Form("pid_%s_%s", arm.Data(), particle.Data() ), Form("pid %s %s", arm.Data(), particle.Data() ) );
	hFour->Draw("E PLC");
	hThree->Draw("ESAME PLC");
	hTwo->Draw("ESAME PLC");
	hOne->Draw("ESAME PLC");
	d->BuildLegend();
	d->Write();

	std::cout << "PID" << std::endl;
	std::cout << "E bin\t|" << "1σ\t|" << "2σ\t|" << "3σ\t|" << "4σ" << std::endl;
	for(int i=1; i<=nbins; ++i) { //ignore under and overflow bins
		std::cout << (i-1)*500 << "\t|" << hOne->GetBinContent(i) << "\t" << hTwo->GetBinContent(i) << "\t|" <<  hThree->GetBinContent(i) << "\t|" << hFour->GetBinContent(i) << std::endl;
	}
	std::cout << std::endl;
	std::cout << "PID Corrected" << std::endl;
	std::cout << "E bin\t|" << "1σ\t|" << "2σ\t|" << "3σ\t|" << "4σ" << std::endl;
	for(int i=1; i<=nbins; ++i) { //ignore under and overflow bins
		std::cout << (i-1)*500 << "\t|" << hOne->GetBinContent(i)/0.682 << "\t|" << hTwo->GetBinContent(i)/0.954 << "\t|" <<  hThree->GetBinContent(i)/0.996 << "\t|" << hFour->GetBinContent(i)/0.998 << std::endl;
		pidError[i] = abs(1- ((hTwo->GetBinContent(i)/0.954) / (hThree->GetBinContent(i)/0.996) ) );
	}
}

void Finally(TTree *tree, Double_t *pidError, Double_t *dtError, Double_t *lifetimeError, TString arm = "SiR", TString particle = "proton") {
	Double_t t1, t2, t3, a1, a2, a3, e1, e2, e3, timeToPrevTME, timeToNextTME;
	TString *channel = new TString();
	TString *sig1 = new TString();
	TString *sig2 = new TString();
	TString *sig3 = new TString();
	TString *sig4 = new TString();
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
	tree->SetBranchAddress("sig1", &sig1);
	tree->SetBranchAddress("sig2", &sig2);
	tree->SetBranchAddress("sig3", &sig3);
	tree->SetBranchAddress("sig4", &sig4);

	Int_t nbins = 40;
	TH1D *hUncorrected = new TH1D(Form("hUncorrected_%s_%s", arm.Data(), particle.Data() ), "Uncorrected; E[keV];Counts / 500keV", nbins, 0, 20000);
	for(Long64_t i=0; i < tree->GetEntries(); ++i) {
		tree->GetEntry(i);
		if(timeToPrevTME < 10e3 || timeToNextTME < 10e3) continue;
		if(t2<500) continue;
		if(t2>10e3) continue;
		if(t2-t1-13 > 60 || t2-t1-13 < -60) continue;
		if(!TMath::IsNaN(e3) ) continue;
		if(!sig3->Contains(particle) ) continue;
		if(!arm.CompareTo("SiR") ) {
			if(channel->Contains("SiL") ) continue;
			if(channel->Contains("SiR1_1") && a1>3850. ) continue;
			if(channel->Contains("SiR1_2") && a1>3972. ) continue;
			if(channel->Contains("SiR1_3") && a1>3956. ) continue;
			if(channel->Contains("SiR1_4") && a1>3986. ) continue;
		} else {
			if(channel->Contains("SiR") ) continue;
		}
		e1 *= 1000;
		e2 *= 1000;
		hUncorrected->Fill(e1+e2);
	}

	TH1D *hSystematics = new TH1D(Form("hsyst_%s_%s", arm.Data(), particle.Data() ), "Syst. unc.; E[keV];Counts / 500keV", nbins, 0, 20000);
	TH1D *hStatistics = new TH1D(Form("hstat_%s_%s", arm.Data(), particle.Data() ), "Stat. unc.; E[keV];Counts / 500keV", nbins, 0, 20000);
	Double_t pidCorr = 0.996; //3sig
	Double_t dtCorr = 0.996; //3sig
	Double_t lifetimeCorr = 0.56; //500ns
	for(int i=1; i<=nbins; ++i) {
		Double_t withCorrectionFactors = hUncorrected->GetBinContent(i)/pidCorr/dtCorr/lifetimeCorr; //apply correction factors
		hSystematics->SetBinContent(i, withCorrectionFactors);
		hStatistics->SetBinContent(i, withCorrectionFactors);
		Double_t withCombinedUncertainties = hUncorrected->GetBinError(i) + withCorrectionFactors * TMath::Sqrt(TMath::Power(pidError[i], 2) + TMath::Power(dtError[i], 2) + TMath::Power(lifetimeError[i], 2) );
		if(isnan(withCombinedUncertainties) ) withCombinedUncertainties = 0.f;
		hSystematics->SetBinError(i, withCombinedUncertainties); //with systematic uncertainties
	}
	TCanvas *e = new TCanvas(Form("%s%s", arm.Data(), particle.Data() ), Form("%s%s", arm.Data(), particle.Data() ));
	hSystematics->SetFillColor(kYellow);
	hSystematics->Draw("E3");
	hStatistics->SetFillColor(kGreen);
	hStatistics->Draw("E3 SAME");

	e->BuildLegend(.7, .7, .88, .88);
	hSystematics->SetTitle(Form("%s folded %s", arm.Data(), particle.Data() ) );
	e->Write(); //canvases do not get written automatically since they are memory objects?
}

void FoldedSystematics(TString arm = "SiR", TString particle = "proton") {
	gStyle->SetOptStat(0);
	gStyle->SetStatY(0.9);                
	gStyle->SetStatX(0.9);

	TFile *fData = new TFile("../al50.root", "READ");

	Double_t dtError[40] = {}; //0 -> 20000keV with 500keV bins
	Double_t lifetimeError[40] = {};
	Double_t pidError[40] = {};

	TTree *tree = (TTree *)fData->Get("tree");
	TFile *fOutput = new TFile("folded.root", "UPDATE");
	if(!fOutput->GetDirectory(particle) ) {
		TDirectory *d = fOutput->mkdir(particle);
		d->cd();
	} else {
		gDirectory->cd(particle);
	}
	lifetime(tree, lifetimeError, arm, particle);

	tree = (TTree *)fData->Get("tree");
	dt(tree, dtError, arm, particle);

	tree = (TTree *)fData->Get("tree");
	Pid(tree, pidError, arm, particle);

	gDirectory->cd("/");
	tree = (TTree *)fData->Get("tree");
	Finally(tree, pidError, dtError, lifetimeError, arm , particle);

	fOutput->Write();
}
