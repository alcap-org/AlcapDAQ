#if !(defined(__CINT__) || defined(__CLING__)) || defined(__ACLIC__)
#include <iostream>
#include "TRandom.h"
#include "TH1D.h"
#include "RooUnfoldResponse.h"
#include "RooUnfoldBayes.h"
#endif

TH1D * Process(TH1D *hMeas, TString arm, TString particle, TString cutDescription) {
	TFile *responseMatrixFile = new TFile(Form("/home/m-wong/data/transfer/transfer.sf1.02.al50.%s.root", particle.Data() ), "READ");
	RooUnfoldResponse *L_TM = (RooUnfoldResponse *)responseMatrixFile->Get("SiL500_TM");
	RooUnfoldResponse *R_TM = (RooUnfoldResponse *)responseMatrixFile->Get("SiR500_TM");
	RooUnfoldResponse *response = 0;
	if(arm.CompareTo("SiR") == 0) {
		response = R_TM;
	} else {
		response = L_TM;
	}
	RooUnfoldBayes unfold(response, hMeas);
	TH1D* hReco= (TH1D*) unfold.Hreco();
	hReco->SetTitle(Form("Unfolded %s %s %s", arm.Data(), particle.Data(), cutDescription.Data() ) );
	std::cout << arm << " " << particle << std::endl;
	Double_t error;
	Double_t integral = hMeas->IntegralAndError(hMeas->GetXaxis()->FindBin(2000), hMeas->GetXaxis()->FindBin(16500), error); 
	std::cout << "folded 2000-17000keV: " << integral << " ± " << error << std::endl;
	integral = hReco->IntegralAndError(hReco->GetXaxis()->FindBin(4000), hReco->GetXaxis()->FindBin(7500), error);
	std::cout << " 4000-8000keV: " << integral << " ± " << error << std::endl;
	integral = hReco->IntegralAndError(hReco->GetXaxis()->FindBin(0.), hReco->GetXaxis()->FindBin(9500), error);
	std::cout << " 0-10000keV: " << integral << " ± " << error << std::endl;
	integral = hReco->IntegralAndError(hReco->GetXaxis()->FindBin(3500), hReco->GetXaxis()->FindBin(9500), error);
	std::cout << " 3500-10000keV: " << integral << " ± " << error << std::endl;
	std::cout << "==========================================" << std::endl;

	return hReco;
}
void lifetime(TTree *tree, Double_t *lifetimeError, TString arm, TString particle) {
	Double_t t1, t2, t3, a1, a2, a3, e1, e2, e3, timeToPrevTME, timeToNextTME;
	TString *channel = new TString();
	TString *sig1 = new TString();
	TString *sig2 = new TString();
	TString *sig3 = new TString();
	TString *sig4 = new TString();
	tree->SetBranchAddress("timeToPrevTME", &timeToPrevTME);
	tree->SetBranchAddress("timeToNextTME", &timeToNextTME);
	tree->SetBranchAddress("e1", &e1);
	tree->SetBranchAddress("t1", &t1);
	tree->SetBranchAddress("e2", &e2);
	tree->SetBranchAddress("t2", &t2);
	tree->SetBranchAddress("e3", &e3);
	tree->SetBranchAddress("t3", &t3);
	tree->SetBranchAddress("channel", &channel);
	tree->SetBranchAddress("sig1", &sig1);
	tree->SetBranchAddress("sig2", &sig2);
	tree->SetBranchAddress("sig3", &sig3);
	tree->SetBranchAddress("sig4", &sig4);

	Int_t nbins = 40;
	TH1D *hOne = new TH1D(Form("h300_%s_%s", arm.Data(), particle.Data() ), "300ns;E [keV];Counts / 500keV", nbins, 0, 20000); 
	TH1D *hTwo = new TH1D(Form("h400_%s_%s", arm.Data(), particle.Data() ), "400ns;E [keV];Counts / 500keV", nbins, 0, 20000); 
	TH1D *hThree = new TH1D(Form("h500_%s_%s", arm.Data(), particle.Data() ), "500ns;E [keV];Counts / 500keV", nbins, 0, 20000); 
	TH1D *hFour = new TH1D(Form("h600_%s_%s", arm.Data(), particle.Data() ), "600ns;E [keV];Counts / 500keV", nbins, 0, 20000); 
	for(Long64_t i=0; i < tree->GetEntries(); ++i) {
		tree->GetEntry(i);
                if(timeToPrevTME < 10e3 || timeToNextTME < 10e3) continue;
                if(abs(t2)>10e3) continue;
		if(t2-t1-12 > 60 || t2-t1-12 < -60) continue;
		if(!TMath::IsNaN(e3) ) continue;
		if(!sig3->Contains(particle) ) continue;
		if(arm.CompareTo("SiR") == 0 ) {
			if(channel->Contains("SiL") ) continue;
			//remove saturated pulses
		} else {
			if(channel->Contains("SiR") ) continue;
		}
		e1 *= 1000;
		e2 *= 1000;
		if(t2 > 400) hOne->Fill(e1+e2);
		if(t2 > 500) hTwo->Fill(e1+e2);
		if(t2 > 600) hThree->Fill(e1+e2);
		if(t2 > 700) hFour->Fill(e1+e2);
	}
	TH1D *hOneUnf = Process(hOne, arm, particle, "300ns");
	TH1D *hTwoUnf = Process(hTwo, arm, particle, "400ns");
	TH1D *hThreeUnf = Process(hThree, arm, particle, "500ns");
	TH1D *hFourUnf = Process(hFour, arm, particle, "600ns");

	TCanvas *dlifetime = new TCanvas(Form("lifetime_%s_%s", arm.Data(), particle.Data() ), Form("lifetime %s %s", arm.Data(), particle.Data() ) );
	hFourUnf->Draw("E PLC");
	hThreeUnf->Draw("ESAME PLC");
	hTwoUnf->Draw("ESAME PLC");
	hOneUnf->Draw("ESAME PLC");
	dlifetime->BuildLegend();
	dlifetime->Write();

	std::cout << "lifetime" << std::endl;
	std::cout << "time\t|" << "300\t|" << "400\t|" << "500\t|" << "600" << std::endl;
	for(int ii=1; ii<=nbins; ++ii) { //ignore under and overflow bins
		std::cout << (ii-1)*500 << "\t|" << hOneUnf->GetBinContent(ii) << "\t" << hTwoUnf->GetBinContent(ii) << "\t|" <<  hThreeUnf->GetBinContent(ii) << "\t|" << hFourUnf->GetBinContent(ii) << std::endl;
	}
	std::cout << std::endl;
	std::cout << "lifetime Corrected" << std::endl;
	std::cout << "time\t|" << "300\t|" << "400\t|" << "500\t|" << "600" << std::endl;
	for(int j=1; j<=nbins; ++j) { //ignore under and overflow bins
		std::cout << (j-1)*500 << "\t|" << hOneUnf->GetBinContent(j)/0.71 << "\t|" << hTwoUnf->GetBinContent(j)/0.63 << "\t|" <<  hThreeUnf->GetBinContent(j)/0.56 << "\t|" << hFourUnf->GetBinContent(j)/0.50 << std::endl;
		//elog:294
		if(hOneUnf->GetBinContent(j) != 0) {
			lifetimeError[j] = 1- ((hFourUnf->GetBinContent(j)/0.45) / (hOneUnf->GetBinContent(j)/0.63) );
		} else {
			lifetimeError[j] = 0;
		}
	}
}
void dtFill(Double_t t2, Double_t t1, Double_t e2, Double_t e1, TH1D *h1, TH1D *h2, TH1D *h3, TH1D *h4, TString arm, TString particle) {
	if(particle.CompareTo("proton") == 0 ) {
	if(arm.CompareTo("SiR") == 0) {
		if(e1+e2<3000) {
			if(abs(t2-t1 - 8.39) < 14.78) h1->Fill(e1+e2);
			if(abs(t2-t1 - 8.39) < 2*14.78) h2->Fill(e1+e2);
			if(abs(t2-t1 - 8.39) < 3*14.78) h3->Fill(e1+e2);
			if(abs(t2-t1 - 8.39) < 4*14.78) h4->Fill(e1+e2);
		}
		if(abs(e1+e2-3500)<500) {
			if(abs(t2-t1 - 11.73) < 14.81) h1->Fill(e1+e2);
			if(abs(t2-t1 - 11.73) < 2*14.81) h2->Fill(e1+e2);
			if(abs(t2-t1 - 11.73) < 3*14.81) h3->Fill(e1+e2);
			if(abs(t2-t1 - 11.73) < 4*14.81) h4->Fill(e1+e2);
		}
		if(abs(e1+e2-4500)<500) {
			if(abs(t2-t1 - 13.27) < 16.88) h1->Fill(e1+e2);
			if(abs(t2-t1 - 13.27) < 2*16.88) h2->Fill(e1+e2);
			if(abs(t2-t1 - 13.27) < 3*16.88) h3->Fill(e1+e2);
			if(abs(t2-t1 - 13.27) < 4*16.88) h4->Fill(e1+e2);
		}
		if(abs(e1+e2-5500)<500) {
			if(abs(t2-t1 - 12.76) < 19.06) h1->Fill(e1+e2);
			if(abs(t2-t1 - 12.76) < 2*19.06) h2->Fill(e1+e2);
			if(abs(t2-t1 - 12.76) < 3*19.06) h3->Fill(e1+e2);
			if(abs(t2-t1 - 12.76) < 4*19.06) h4->Fill(e1+e2);
		}
		if(abs(e1+e2-6500)<500) {
			if(abs(t2-t1 - 14.8) < 22.96) h1->Fill(e1+e2);
			if(abs(t2-t1 - 14.8) < 2*22.96) h2->Fill(e1+e2);
			if(abs(t2-t1 - 14.8) < 3*22.96) h3->Fill(e1+e2);
			if(abs(t2-t1 - 14.8) < 4*22.96) h4->Fill(e1+e2);
		}
		if(abs(e1+e2-7500)<500) {
			if(abs(t2-t1 - 15.26) < 25.96) h1->Fill(e1+e2);
			if(abs(t2-t1 - 15.26) < 2*25.96) h2->Fill(e1+e2);
			if(abs(t2-t1 - 15.26) < 3*25.96) h3->Fill(e1+e2);
			if(abs(t2-t1 - 15.26) < 4*25.96) h4->Fill(e1+e2);
		}
		if(abs(e1+e2-8500)<500) {
			if(abs(t2-t1 - 11.98) < 26.55) h1->Fill(e1+e2);
			if(abs(t2-t1 - 11.98) < 2*26.55) h2->Fill(e1+e2);
			if(abs(t2-t1 - 11.98) < 3*26.55) h3->Fill(e1+e2);
			if(abs(t2-t1 - 11.98) < 4*26.55) h4->Fill(e1+e2);
		}
		if(abs(e1+e2-9500)<500) {
			if(abs(t2-t1 - 14.43) < 30.69) h1->Fill(e1+e2);
			if(abs(t2-t1 - 14.43) < 2*30.69) h2->Fill(e1+e2);
			if(abs(t2-t1 - 14.43) < 3*30.69) h3->Fill(e1+e2);
			if(abs(t2-t1 - 14.43) < 4*30.69) h4->Fill(e1+e2);
		}
		if(abs(e1+e2-10500)<500) {
			if(abs(t2-t1 - 18.88) < 40.66) h1->Fill(e1+e2);
			if(abs(t2-t1 - 18.88) < 2*40.66) h2->Fill(e1+e2);
			if(abs(t2-t1 - 18.88) < 3*40.66) h3->Fill(e1+e2);
			if(abs(t2-t1 - 18.88) < 4*40.66) h4->Fill(e1+e2);
		}
		if(e1+e2>11000) {
			if(abs(t2-t1 - 14.5) < 33.47) h1->Fill(e1+e2);
			if(abs(t2-t1 - 14.5) < 2*33.47) h2->Fill(e1+e2);
			if(abs(t2-t1 - 14.5) < 3*33.47) h3->Fill(e1+e2);
			if(abs(t2-t1 - 14.5) < 4*33.47) h4->Fill(e1+e2);
		}
	} else {
		if(e1+e2<3000) {
			if(abs(t2-t1 - 8.77) < 20.62) h1->Fill(e1+e2);
			if(abs(t2-t1 - 8.77) < 2*20.62) h2->Fill(e1+e2);
			if(abs(t2-t1 - 8.77) < 3*20.62) h3->Fill(e1+e2);
			if(abs(t2-t1 - 8.77) < 4*20.62) h4->Fill(e1+e2);
		}
		if(abs(e1+e2-3500)<500) {
			if(abs(t2-t1 - 11.13) < 20.78) h1->Fill(e1+e2);
			if(abs(t2-t1 - 11.13) < 2*20.78) h2->Fill(e1+e2);
			if(abs(t2-t1 - 11.13) < 3*20.78) h3->Fill(e1+e2);
			if(abs(t2-t1 - 11.13) < 4*20.78) h4->Fill(e1+e2);
		}
		if(abs(e1+e2-4500)<500) {
			if(abs(t2-t1 - 12.78) < 19.71) h1->Fill(e1+e2);
			if(abs(t2-t1 - 12.78) < 2*19.71) h2->Fill(e1+e2);
			if(abs(t2-t1 - 12.78) < 3*19.71) h3->Fill(e1+e2);
			if(abs(t2-t1 - 12.78) < 4*19.71) h4->Fill(e1+e2);
		}
		if(abs(e1+e2-5500)<500) {
			if(abs(t2-t1 - 13.85) < 21.61) h1->Fill(e1+e2);
			if(abs(t2-t1 - 13.85) < 2*21.61) h2->Fill(e1+e2);
			if(abs(t2-t1 - 13.85) < 3*21.61) h3->Fill(e1+e2);
			if(abs(t2-t1 - 13.85) < 4*21.61) h4->Fill(e1+e2);
		}
		if(abs(e1+e2-6500)<500) {
			if(abs(t2-t1 - 12.31) < 22.63) h1->Fill(e1+e2);
			if(abs(t2-t1 - 12.31) < 2*22.63) h2->Fill(e1+e2);
			if(abs(t2-t1 - 12.31) < 3*22.63) h3->Fill(e1+e2);
			if(abs(t2-t1 - 12.31) < 4*22.63) h4->Fill(e1+e2);
		}
		if(abs(e1+e2-7500)<500) {
			if(abs(t2-t1 - 10.51) < 24.99) h1->Fill(e1+e2);
			if(abs(t2-t1 - 10.51) < 2*24.99) h2->Fill(e1+e2);
			if(abs(t2-t1 - 10.51) < 3*24.99) h3->Fill(e1+e2);
			if(abs(t2-t1 - 10.51) < 4*24.99) h4->Fill(e1+e2);
		}
		if(abs(e1+e2-8500)<500) {
			if(abs(t2-t1 - 8.78) < 26.30) h1->Fill(e1+e2);
			if(abs(t2-t1 - 8.78) < 2*26.30) h2->Fill(e1+e2);
			if(abs(t2-t1 - 8.78) < 3*26.30) h3->Fill(e1+e2);
			if(abs(t2-t1 - 8.78) < 4*26.30) h4->Fill(e1+e2);
		}
		if(abs(e1+e2-9500)<500) {
			if(abs(t2-t1 - 9.72) < 32.32) h1->Fill(e1+e2);
			if(abs(t2-t1 - 9.72) < 2*32.32) h2->Fill(e1+e2);
			if(abs(t2-t1 - 9.72) < 3*32.32) h3->Fill(e1+e2);
			if(abs(t2-t1 - 9.72) < 4*32.32) h4->Fill(e1+e2);
		}
		if(abs(e1+e2-10500)<500) {
			if(abs(t2-t1 - 12.70) < 38.22) h1->Fill(e1+e2);
			if(abs(t2-t1 - 12.70) < 2*38.22) h2->Fill(e1+e2);
			if(abs(t2-t1 - 12.70) < 3*38.22) h3->Fill(e1+e2);
			if(abs(t2-t1 - 12.70) < 4*38.22) h4->Fill(e1+e2);
		}
		if(e1+e2>11000) {
			if(abs(t2-t1 - 18.88) < 29.82) h1->Fill(e1+e2);
			if(abs(t2-t1 - 18.88) < 2*29.82) h2->Fill(e1+e2);
			if(abs(t2-t1 - 18.88) < 3*29.82) h3->Fill(e1+e2);
			if(abs(t2-t1 - 18.88) < 4*29.82) h4->Fill(e1+e2);
		}
	}
	} else { //deuteron
		if(arm.CompareTo("SiR") == 0) {
		if(abs(e1+e2-4000)<1000) {
			if(abs(t2-t1 - 12.66) < 10.81) h1->Fill(e1+e2);
			if(abs(t2-t1 - 12.66) < 2*10.81) h2->Fill(e1+e2);
			if(abs(t2-t1 - 12.66) < 3*10.81) h3->Fill(e1+e2);
			if(abs(t2-t1 - 12.66) < 4*10.81) h4->Fill(e1+e2);
		}
		if(abs(e1+e2-6000)<1000) {
			if(abs(t2-t1 - 12.08) < 13.10) h1->Fill(e1+e2);
			if(abs(t2-t1 - 12.08) < 2*13.10) h2->Fill(e1+e2);
			if(abs(t2-t1 - 12.08) < 3*13.10) h3->Fill(e1+e2);
			if(abs(t2-t1 - 12.08) < 4*13.10) h4->Fill(e1+e2);
		}
		if(abs(e1+e2-8000)<1000) {
			if(abs(t2-t1 - 9.79) < 17.22) h1->Fill(e1+e2);
			if(abs(t2-t1 - 9.79) < 2*17.22) h2->Fill(e1+e2);
			if(abs(t2-t1 - 9.79) < 3*17.22) h3->Fill(e1+e2);
			if(abs(t2-t1 - 9.79) < 4*17.22) h4->Fill(e1+e2);
		}
//rebin 4
		if(abs(e1+e2-10000)<1000) {
			if(abs(t2-t1 - 8.58) < 18.57) h1->Fill(e1+e2);
			if(abs(t2-t1 - 8.58) < 2*18.57) h2->Fill(e1+e2);
			if(abs(t2-t1 - 8.58) < 3*18.57) h3->Fill(e1+e2);
			if(abs(t2-t1 - 8.58) < 4*18.57) h4->Fill(e1+e2);
		}
//rebin 4
		if(abs(e1+e2-12000)<1000) {
			if(abs(t2-t1 - 7.36) < 24.30) h1->Fill(e1+e2);
			if(abs(t2-t1 - 7.36) < 2*24.30) h2->Fill(e1+e2);
			if(abs(t2-t1 - 7.36) < 3*24.30) h3->Fill(e1+e2);
			if(abs(t2-t1 - 7.36) < 4*24.30) h4->Fill(e1+e2);
		}
//rebin 4
		if(e1+e2>13000) {
			if(abs(t2-t1 - 4.73) < 35.53) h1->Fill(e1+e2);
			if(abs(t2-t1 - 4.73) < 2*35.53) h2->Fill(e1+e2);
			if(abs(t2-t1 - 4.73) < 3*35.53) h3->Fill(e1+e2);
			if(abs(t2-t1 - 4.73) < 4*35.53) h4->Fill(e1+e2);
		}
		} else {
		if(abs(e1+e2-4000)<1000) {
			if(abs(t2-t1 - 12.68) < 22.33) h1->Fill(e1+e2);
			if(abs(t2-t1 - 12.68) < 2*22.33) h2->Fill(e1+e2);
			if(abs(t2-t1 - 12.68) < 3*22.33) h3->Fill(e1+e2);
			if(abs(t2-t1 - 12.68) < 4*22.33) h4->Fill(e1+e2);
		}
		if(abs(e1+e2-6000)<1000) {
			if(abs(t2-t1 - 14.38) < 30.94) h1->Fill(e1+e2);
			if(abs(t2-t1 - 14.38) < 2*30.94) h2->Fill(e1+e2);
			if(abs(t2-t1 - 14.38) < 3*30.94) h3->Fill(e1+e2);
			if(abs(t2-t1 - 14.38) < 4*30.94) h4->Fill(e1+e2);
		}
		if(abs(e1+e2-8000)<1000) {
			if(abs(t2-t1 - 15.25) < 21.09) h1->Fill(e1+e2);
			if(abs(t2-t1 - 15.25) < 2*21.09) h2->Fill(e1+e2);
			if(abs(t2-t1 - 15.25) < 3*21.09) h3->Fill(e1+e2);
			if(abs(t2-t1 - 15.25) < 4*21.09) h4->Fill(e1+e2);
		}
		if(abs(e1+e2-10000)<1000) {
			if(abs(t2-t1 - 9.37) < 19.79) h1->Fill(e1+e2);
			if(abs(t2-t1 - 9.37) < 2*19.79) h2->Fill(e1+e2);
			if(abs(t2-t1 - 9.37) < 3*19.79) h3->Fill(e1+e2);
			if(abs(t2-t1 - 9.37) < 4*19.79) h4->Fill(e1+e2);
		}
		if(abs(e1+e2-12000)<1000) {
			if(abs(t2-t1 - 9.78) < 24.85) h1->Fill(e1+e2);
			if(abs(t2-t1 - 9.78) < 2*24.85) h2->Fill(e1+e2);
			if(abs(t2-t1 - 9.78) < 3*24.85) h3->Fill(e1+e2);
			if(abs(t2-t1 - 9.78) < 4*24.85) h4->Fill(e1+e2);
		}
		if(e1+e2>13000) {
			if(abs(t2-t1 - 6.15) < 24.21) h1->Fill(e1+e2);
			if(abs(t2-t1 - 6.15) < 2*24.21) h2->Fill(e1+e2);
			if(abs(t2-t1 - 6.15) < 3*24.21) h3->Fill(e1+e2);
			if(abs(t2-t1 - 6.15) < 4*24.21) h4->Fill(e1+e2);
		}

		}
	}
}

void dt(TTree *tree, Double_t *dtError, TString arm, TString particle) {
	Double_t t1, t2, t3, a1, a2, a3, e1, e2, e3, timeToPrevTME, timeToNextTME;
	TString *channel = new TString();
	TString *sig1 = new TString();
	TString *sig2 = new TString();
	TString *sig3 = new TString();
	TString *sig4 = new TString();
	tree->SetBranchAddress("timeToPrevTME", &timeToPrevTME);
	tree->SetBranchAddress("timeToNextTME", &timeToNextTME);
	tree->SetBranchAddress("e1", &e1);
	tree->SetBranchAddress("t1", &t1);
	tree->SetBranchAddress("e2", &e2);
	tree->SetBranchAddress("t2", &t2);
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
		if(abs(t2)>10e3) continue;
		if(!TMath::IsNaN(e3) ) continue;
		if(!sig3->Contains(particle) ) continue;
		if(arm.CompareTo("SiR") == 0 ) {
			if(channel->Contains("SiL") ) continue;
		} else {
			if(channel->Contains("SiR") ) continue;
		}
		e1 *= 1000;
		e2 *= 1000;
		dtFill(t2, t1, e2, e1, hOne, hTwo, hThree, hFour, arm, particle);
	}
	TH1D *hOneUnf = Process(hOne, arm, particle, "1#sigma");
	TH1D *hTwoUnf = Process(hTwo, arm, particle, "2#sigma");
	TH1D *hThreeUnf = Process(hThree, arm, particle, "3#sigma");
	TH1D *hFourUnf = Process(hFour, arm, particle, "4#sigma");
	
	TCanvas *dt = new TCanvas(Form("dt_%s_%s", arm.Data(), particle.Data() ), Form("dt %s %s", arm.Data(), particle.Data() ) );
	hFourUnf->Draw("E PLC");
	hThreeUnf->Draw("ESAME PLC");
	hTwoUnf->Draw("ESAME PLC");
	hOneUnf->Draw("ESAME PLC");
	dt->BuildLegend();
	dt->Write();

	std::cout << "dt" << std::endl;
	std::cout << "E bin\t|" << "1σ\t|" << "2σ\t|" << "3σ\t|" << "4σ" << std::endl;
	for(int k=1; k<=nbins; ++k) { //ignore under and overflow bins
		std::cout << (k-1)*500 << "\t|" << hOneUnf->GetBinContent(k) << "\t" << hTwoUnf->GetBinContent(k) << "\t|" <<  hThreeUnf->GetBinContent(k) << "\t|" << hFourUnf->GetBinContent(k) << std::endl;
	}
	std::cout << std::endl;
	std::cout << "dt Corrected" << std::endl;
	std::cout << "E bin\t|" << "1σ\t|" << "2σ\t|" << "3σ\t|" << "4σ" << std::endl;
	for(int j=1; j<=nbins; ++j) { //ignore under and overflow bins
		std::cout << (j-1)*500 << "\t|" << hOneUnf->GetBinContent(j)/0.682 << "\t|" << hTwoUnf->GetBinContent(j)/0.954 << "\t|" <<  hThreeUnf->GetBinContent(j)/0.996 << "\t|" << hFourUnf->GetBinContent(j)/0.998 << std::endl;
		//.682, .954, .996
		if(hThreeUnf->GetBinContent(j) != 0) {
			dtError[j] = 1- ((hOneUnf->GetBinContent(j)/0.682) / (hThreeUnf->GetBinContent(j)/0.996) );
		} else {
			dtError[j] = 0;
		}
	}
}
void Pid(TTree *tree, Double_t *pidError, TString arm, TString particle) {
	Double_t t1, t2, t3, a1, a2, a3, e1, e2, e3, timeToPrevTME, timeToNextTME;
	TString *channel = new TString();
	TString *sig1 = new TString();
	TString *sig2 = new TString();
	TString *sig3 = new TString();
	TString *sig4 = new TString();
	tree->SetBranchAddress("timeToPrevTME", &timeToPrevTME);
	tree->SetBranchAddress("timeToNextTME", &timeToNextTME);
	tree->SetBranchAddress("e1", &e1);
	tree->SetBranchAddress("t1", &t1);
	tree->SetBranchAddress("e2", &e2);
	tree->SetBranchAddress("t2", &t2);
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
		if(abs(t2)>10e3) continue;
		if(t2-t1-12 > 60 || t2-t1-12 < -60) continue;
		if(!TMath::IsNaN(e3) ) continue;
		if(!arm.CompareTo("SiR") ) {
			if(channel->Contains("SiL") ) continue;
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
	TH1D *hOneUnf = Process(hOne, arm, particle, "1#sigma");
	TH1D *hTwoUnf = Process(hTwo, arm, particle, "2#sigma");
	TH1D *hThreeUnf = Process(hThree, arm, particle, "3#sigma");
	TH1D *hFourUnf = Process(hFour, arm, particle, "4#sigma");

	TCanvas *d = new TCanvas(Form("pid_%s_%s", arm.Data(), particle.Data() ), Form("pid %s %s", arm.Data(), particle.Data() ) );
	hFourUnf->Draw("E PLC");
	hThreeUnf->Draw("ESAME PLC");
	hTwoUnf->Draw("ESAME PLC");
	hOneUnf->Draw("ESAME PLC");
	d->BuildLegend();
	d->Write();

	std::cout << "PID" << std::endl;
	std::cout << "E bin\t|" << "1σ\t|" << "2σ\t|" << "3σ\t|" << "4σ" << std::endl;
	for(int j=1; j<=nbins; ++j) { //ignore under and overflow bins
		std::cout << (j-1)*500 << "\t|" << hOneUnf->GetBinContent(j) << "\t" << hTwoUnf->GetBinContent(j) << "\t|" <<  hThreeUnf->GetBinContent(j) << "\t|" << hFourUnf->GetBinContent(j) << std::endl;
	}
	std::cout << std::endl;
	std::cout << "PID Corrected" << std::endl;
	std::cout << "E bin\t|" << "1σ\t|" << "2σ\t|" << "3σ\t|" << "4σ" << std::endl;
	for(int k=1; k<=nbins; ++k) { //ignore under and overflow bins
		std::cout << (k-1)*500 << "\t|" << hOneUnf->GetBinContent(k)/0.682 << "\t|" << hTwoUnf->GetBinContent(k)/0.954 << "\t|" <<  hThreeUnf->GetBinContent(k)/0.996 << "\t|" << hFourUnf->GetBinContent(k)/0.998 << std::endl;
		if(hThreeUnf->GetBinContent(k) != 0) {
			pidError[k] = 1- ((hOneUnf->GetBinContent(k)/0.682) / (hThreeUnf->GetBinContent(k)/0.996) );
		} else {
			pidError[k] = 0;
		}
	}
}

void Finally(TTree *tree, Double_t *pidError, Double_t *dtError, Double_t *lifetimeError, TString arm, TString particle) {
	TString target = "al50";
	Int_t nbins = 40;
	//TODO: rebuild this so we choose one unfolded result as the central value
	TFile *fUnfolded = new TFile(Form("/home/m-wong/data/unfolded.%s.%s.root", target.Data(), particle.Data() ), "READ");
	TH1D *hUncorrected = 0;
	if(arm.CompareTo("SiL") == 0) {
		hUncorrected = (TH1D *)fUnfolded->Get("hRecoLeft");
	} else {
		hUncorrected = (TH1D *)fUnfolded->Get("hRecoRight");
	}

	hUncorrected->GetYaxis()->SetMaxDigits(3);
	hUncorrected->GetXaxis()->SetRangeUser(5000, 16000);
	if(target.CompareTo("al50")==0) {
		hUncorrected->Scale(1/(0.56*0.609*1.61E+8) );
	} else if(target.CompareTo("al100")==0) {
		hUncorrected->Scale(1/(0.56*0.609*1.34E+8) );
	}

	TH1D *hSystematics = (TH1D *) hUncorrected->Clone();
	TH1D *hStatistics = (TH1D *) hUncorrected->Clone();
Double_t staterror;
Double_t integral = hStatistics->IntegralAndError(hStatistics->GetXaxis()->FindBin(3500), hStatistics->GetXaxis()->FindBin(9500), staterror, "");
std::cout << "Statistical errors --- Integral " << arm << " " << particle << ": " << integral << " pm " << staterror << std::endl;
	hSystematics->SetTitle("Syst. unc.; E[keV];Particles per captured #mu^{-}/ 500keV"); 
	hStatistics->SetTitle("Stat. unc.; E[keV];Particles per captured #mu^{-}/ 500keV");
	hSystematics->SetName(Form("hsyst_%s_%s", arm.Data(), particle.Data() ) );
	hStatistics->SetName(Form("hstat_%s_%s", arm.Data(), particle.Data() ) );

	for(int i=1; i<=nbins; ++i) {
		Double_t withCorrectionFactors = hUncorrected->GetBinContent(i);
		Double_t withCombinedUncertainties = hUncorrected->GetBinError(i) + withCorrectionFactors * TMath::Sqrt(TMath::Power(pidError[i], 2) + TMath::Power(dtError[i], 2) + TMath::Power(lifetimeError[i], 2) );
		hSystematics->SetBinError(i, withCombinedUncertainties); //with systematic uncertainties
	}
Double_t syserror;
integral = hSystematics->IntegralAndError(hSystematics->GetXaxis()->FindBin(3500), hSystematics->GetXaxis()->FindBin(9500), syserror, "");
std::cout << "Systematic errors --- Integral " << arm << " " << particle << ": " << integral << " pm " << syserror-staterror << std::endl;

	//Unfolding systematics
//	TFile *fCutOff = new TFile("corranderr/cutoff.root", "READ");
//	TFile *fBinWidth = new TFile("corranderr/binwidth.root", "READ");
//	TFile *fIterations = new TFile("corranderr/iterations.root", "READ");
//	TH1D *hCutOffLeft = (TH1D *)fCutOff->Get("hl");
//	TH1D *hCutOffRight = (TH1D *)fCutOff->Get("hr");
//	TH1D *hBinWidthLeft = (TH1D *)fBinWidth->Get("hl");
//	TH1D *hBinWidthRight = (TH1D *)fBinWidth->Get("hr");
//	TH1D *hIterationsLeft = (TH1D *)fIterations->Get("hl");
//	TH1D *hIterationsRight = (TH1D *)fIterations->Get("hr");
//	TH1D *hUnfoldingUncertainties = (TH1D *) hUncorrected->Clone();
//	hUnfoldingUncertainties->SetTitle("Unfold. unc.; E[keV]; Counts / 500keV");
//	hUnfoldingUncertainties->SetName(Form("hunf_%s_%s", arm.Data(), particle.Data() ) );
//	for(int j=1; j<=nbins; ++j) {
//		Double_t withCorrectionFactors = hUncorrected->GetBinContent(j+5);
//		Double_t withUnfoldingUncertainties = 0;
//		if(arm.CompareTo("SiR") == 0) {
//			withUnfoldingUncertainties = hSystematics->GetBinError(j+5) + withCorrectionFactors * TMath::Sqrt(TMath::Power(hCutOffRight->GetBinError(j), 2) + TMath::Power(hBinWidthRight->GetBinError(j), 2) + TMath::Power(hIterationsRight->GetBinError(j), 2) );
//		} else {
//			withUnfoldingUncertainties = hSystematics->GetBinError(j+5) + withCorrectionFactors * TMath::Sqrt(TMath::Power(hCutOffLeft->GetBinError(j), 2) + TMath::Power(hBinWidthLeft->GetBinError(j), 2) + TMath::Power(hIterationsLeft->GetBinError(j), 2) );
//		}
//		hUnfoldingUncertainties->SetBinError(i+5, withUnfoldingUncertainties); //with unfolding uncertainties
//	}
	
	//combine everything together
	TCanvas *e = new TCanvas(Form("%s%s", arm.Data(), particle.Data() ), Form("%s%s", arm.Data(), particle.Data() ));
//	hUnfoldingUncertainties->SetFillColor(kOrange);
//	hUnfoldingUncertainties->Draw("E3");
	hSystematics->SetFillColor(kYellow);
	hSystematics->Draw("E3");
	hStatistics->SetFillColor(kGreen);
	hStatistics->Draw("E3 SAME");
	hUncorrected->Draw("SAME"); hUncorrected->SetTitle("Unfolded data");

	e->BuildLegend(.7, .7, .88, .88);
	hSystematics->SetTitle(Form("%s unfolded %s", arm.Data(), particle.Data() ) );
	e->Write(); //canvases do not get written automatically since they are memory objects?
}

void Unfolding(TString arm = "SiR", TString particle = "deuteron") {
	std::string target = "al50";
	gStyle->SetOptStat(0);
	gStyle->SetStatY(0.9);                
	gStyle->SetStatX(0.9);

	TFile *fData = new TFile(Form("/home/m-wong/data/R15b/%s.root", target.c_str() ), "READ");

	Double_t dtError[40] = {}; //0 -> 20000keV with 500keV bins
	Double_t lifetimeError[40] = {};
	Double_t pidError[40] = {};

	TTree *tree = (TTree *)fData->Get("tree");
	TFile *fOutput = new TFile(Form("%s-systematics.root", particle.Data() ), "RECREATE");
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
