//=====================================================================-*-C++-*-
// File and Version Information:
//      $Id$
//
// Description:
//      AlCap data unfolding and validation
//
// Authors: Tim Adye <T.J.Adye@rl.ac.uk> and Fergus Wilson <fwilson@slac.stanford.edu>
//
//==============================================================================

#if !(defined(__CINT__) || defined(__CLING__)) || defined(__ACLIC__)
#include <iostream>
#include "TRandom.h"
#include "TH1D.h"
#include "RooUnfoldResponse.h"
#include "RooUnfoldBayes.h"
#endif

void Process(RooUnfoldResponse *response, TH1D *hMeas, const char *arm = "SiL", std::string target = "al50", const char * particle = "proton", bool normalise = kFALSE) {
	RooUnfoldBayes unfold(response, hMeas);
	TH1D* hReco= (TH1D*) unfold.Hreco();
	hReco->SetName(Form("h%s_%s", particle, arm) );

	Double_t error, integral;
	integral = hMeas->IntegralAndError(hMeas->GetXaxis()->FindBin(4000), hMeas->GetXaxis()->FindBin(7500), error); 
	std::cout << "folded " << arm << " 4000-8000keV: " << integral << " ± " << error << std::endl;
	integral = hMeas->IntegralAndError(hMeas->GetXaxis()->FindBin(3500), hMeas->GetXaxis()->FindBin(9500), error); 
	std::cout << "folded " << arm << " 3500-10000keV: " << integral << " ± " << error << std::endl;
	if(normalise) {
		if(target.compare("al50")==0) {
			hReco->Scale(1/(0.56*0.609*1.71E+8 * 1.0482180) ); //al50 (lifetime, capture rate, muon count, 2 sigma selection)
		} else if(target.compare("al100")==0) {
			hReco->Scale(1/(0.56*0.609*1.37E+8 * 1.0482180) ); //al100
		}
	}
	integral = hReco->IntegralAndError(hReco->GetXaxis()->FindBin(4000), hReco->GetXaxis()->FindBin(7500), error);
	std::cout << arm << " 4000-8000keV: " << integral << " ± " << error << std::endl;
	integral = hReco->IntegralAndError(hReco->GetXaxis()->FindBin(3500), hReco->GetXaxis()->FindBin(9500), error);
	std::cout << arm << " 3500-10000keV: " << integral << " ± " << error << std::endl;

}
void RooUnfoldAlCap(std::string target = "al50", std::string particle="proton", bool normalise = kFALSE)
{
	TFile *fData = new TFile(Form("%s/%s.root", dataPath, target.c_str() ), "READ");
	const char *dataPath = getenv("R15b_DATA");
	const char *transferMatrixPath = getenv("R15b_TM");
	TFile *responseMatrixFile = 0;
	if(target.compare("al50") ==0) {
		responseMatrixFile = new TFile(Form("%s/transfer.sf1.02.al50.%s.root", transferMatrixPath, particle.c_str() ), "READ");
		if(particle.compare("proton") == 0) {
			responseMatrixFile = new TFile(Form("%s/transfer.sf1.02.al50.%s3.root", transferMatrixPath, particle.c_str() ), "READ");
		}
	} else if(target.compare("al100") ==0 ) {
		responseMatrixFile = new TFile(Form("%s/transfer.sf1.035.al100.%s.root", transferMatrixPath, particle.c_str() ), "READ");
	} else if(target.compare("ti50") ==0) {
		responseMatrixFile = new TFile(Form("%s/transfer.sf1.03.ti50.proton.root", transferMatrixPath), "READ");
	}
	std::cout << "Loading data: " << fData->GetName() << " Loading MC: " << responseMatrixFile->GetName() << std::endl;

	Double_t e1, e2, e3, t1, t2, timeToPrevTME, timeToNextTME;
	Int_t a2;
	TString *channel = new TString("");
	TString  *sig2 = new TString("");
	TString  *pt2 = new TString("");
	TString  *pt3 = new TString("");
	TH1D *hMeasDataLeft = new TH1D("hMeasDataLeft", "hMeasDataLeft;E [keV]", 40, 0, 20000);
	TH1D *hMeasDataRight = new TH1D("hMeasDataRight", "hMeasDataRight;E [keV]", 40, 0, 20000);
	TH1D *hMeasDataRight3 = new TH1D("hMeasDataRight3", "hMeasDataRight3;E [keV]", 40, 0, 20000);
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
	tree->SetBranchAddress("sig2", &sig2);
	tree->SetBranchAddress("pt2", &pt2);
	for(Long64_t i=0; i < tree->GetEntries(); i++) {
		tree->GetEntry(i);
		if(a2 > 3980) continue; //remove saturation
		e1 = e1*1e3;
		e2 = e2*1e3;
		e3 = e3*1e3;
		if(timeToPrevTME < 10e3 || timeToNextTME < 10e3) continue;
		if(t2<500) continue;
		if(t2>10e3) continue;
		if(target.compare("al50") == 0) {
			if(abs(t2-t1-12)> 60) continue; //Al50
		}
		if(channel->Contains("SiL") ) {
			if(target.compare("al100") == 0) {
				if(abs(t2-t1+567) > 75) continue;
			}
			if(sig2->Contains(particle) ) {
				hMeasDataLeft->Fill(e1+e2);
			}
		} else if(channel->Contains("SiR") ) {
			if(target.compare("al100") == 0) {
				if(abs(t2-t1-210) > 94) continue;
			}
			if(TMath::IsNaN(e3) ) {
				if(sig2->Contains(particle ) ) {
					hMeasDataRight->Fill(e1+e2);
				}
			} else {
				if(pt2->Contains(particle ) ) {
					hMeasDataRight3->Fill(e1+e2+e3);
				}
			}
		}
	}
	//MC derived correction for SiL
//	Double_t SiLMCDerivedVeto[13] = {
//		1.22078,
//		-22.0513,
//		28.4645,
//		9.97701,
//		55.4582,
//		64.2691,
//		49.9286,
//		76.8945,
//		25.4483,
//		48.8,
//		13.5111,
//		12.4286,
//		-4.72
//	};
//	for(Int_t i=17; i < 30; ++i) {
//		hMeasDataLeft->SetBinContent(i, hMeasDataLeft->GetBinContent(i) - SiLMCDerivedVeto[i-17]);
//	}

	RooUnfoldResponse *L_TM = (RooUnfoldResponse *)responseMatrixFile->Get("SiL500_TM");
	RooUnfoldResponse *R_TM = (RooUnfoldResponse *)responseMatrixFile->Get("SiR500_TM");
	TFile *fOutputFile = new TFile(Form("%s/unfolded.%s.root", getenv("R15b_OUT"), target.c_str() ), "UPDATE");

	const char *sourceName = Form("h%s_SiL", particle.c_str() );
	if(fOutputFile->GetListOfKeys()->Contains(sourceName) ) {
		printf("Deleting histogram with name: %s\n", sourceName);
		gDirectory->Delete(Form("%s", sourceName) );
	}
	Process(L_TM, hMeasDataLeft, "SiL", target, particle.c_str(), normalise);

	hMeasDataRight3->Scale(1/0.77);
	hMeasDataRight->Add(hMeasDataRight3);
	sourceName = Form("h%s_SiR", particle.c_str() );
	if(fOutputFile->GetListOfKeys()->Contains(sourceName) ) {
		printf("Deleting histogram with name: %s\n", sourceName);
		gDirectory->Delete(Form("%s", sourceName) );
	}
	Process(R_TM, hMeasDataRight, "SiR", target, particle.c_str(), normalise);

	fOutputFile->Write();
}

#ifndef __CINT__
int main () { RooUnfoldAlCap(); return 0; }  // Main program when run stand-alone
#endif
