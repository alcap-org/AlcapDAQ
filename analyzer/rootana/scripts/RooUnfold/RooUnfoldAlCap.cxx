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

void Process(RooUnfoldResponse *response, TH1D *hMeas, const char *direction = "Left", std::string target = "al50") {
	RooUnfoldBayes unfold(response, hMeas);
	TH1D* hReco= (TH1D*) unfold.Hreco();
	hReco->SetName(Form("hReco%s", direction) );

	Double_t error, integral;
	integral = hMeas->IntegralAndError(hMeas->GetXaxis()->FindBin(4000), hMeas->GetXaxis()->FindBin(7500), error); 
	std::cout << "folded " << direction << " 4000-8000keV: " << integral << " ± " << error << std::endl;
	integral = hMeas->IntegralAndError(hMeas->GetXaxis()->FindBin(3500), hMeas->GetXaxis()->FindBin(9500), error); 
	std::cout << "folded " << direction << " 3500-10000keV: " << integral << " ± " << error << std::endl;

	if(target.compare("al50")==0) {
		hReco->Scale(1/(0.56*0.609*1.71E+8) ); //al50
	} else if(target.compare("al100")==0) {
		hReco->Scale(1/(0.56*0.609*1.37E+8) ); //al100
	}
	integral = hReco->IntegralAndError(hReco->GetXaxis()->FindBin(4000), hReco->GetXaxis()->FindBin(7500), error);
	std::cout << direction << " 4000-8000keV: " << integral << " ± " << error << std::endl;
	integral = hReco->IntegralAndError(hReco->GetXaxis()->FindBin(3500), hReco->GetXaxis()->FindBin(9500), error);
	std::cout << direction << " 3500-10000keV: " << integral << " ± " << error << std::endl;

}
void RooUnfoldAlCap(std::string target = "al50")
{
	TString particle = "proton";
	TFile *fData = 0;
	if(target.compare("al50")==0) {
		fData = new TFile("/home/m-wong/data/R15b/al50.root", "READ");
	} else if(target.compare("al100") ==0 ) {
		fData = new TFile("/home/m-wong/data/R15b/al100.root", "READ");
	} else if(target.compare("ti50") ==0) {
		fData = new TFile("/home/m-wong/data/R15b/ti50.root", "READ");
	}
	TFile *responseMatrixFile = 0;
	if(target.compare("al50") ==0) {
		responseMatrixFile = new TFile(Form("/home/m-wong/data/transfer/transfer.sf1.02.al50.%s3.root", particle.Data() ), "READ");
	} else if(target.compare("al100") ==0 ) {
		responseMatrixFile = new TFile("/home/m-wong/data/transfer/transfer.sf1.035.al100.proton.root", "READ");
	} else if(target.compare("ti50") ==0) {
		responseMatrixFile = new TFile("/home/m-wong/data/transfer/transfer.sf1.03.ti50.proton.root", "READ");
	}
	std::cout << "Loading data: " << fData->GetName() << " Loading MC: " << responseMatrixFile->GetName() << std::endl;

	Double_t e1, e2, e3, t1, t2, timeToPrevTME, timeToNextTME;
	TString *channel = new TString("");
	TString  *sig3 = new TString("");;
	TString  *pt3 = new TString("");;
	TH1D *hMeasDataLeft = new TH1D("hMeasDataLeft", "hMeasDataLeft;E [keV]", 40, 0, 20000);
	TH1D *hMeasDataRight = new TH1D("hMeasDataRight", "hMeasDataRight;E [keV]", 40, 0, 20000);
	TH1D *hMeasDataRight3 = new TH1D("hMeasDataRight3", "hMeasDataRight3;E [keV]", 40, 0, 20000);
	TTree *tree = (TTree *)fData->Get("tree");
	tree->SetBranchAddress("timeToPrevTME", &timeToPrevTME);
	tree->SetBranchAddress("timeToNextTME", &timeToNextTME);
	tree->SetBranchAddress("e1", &e1);
	tree->SetBranchAddress("e2", &e2);
	tree->SetBranchAddress("e3", &e3);
	tree->SetBranchAddress("t1", &t1);
	tree->SetBranchAddress("t2", &t2);
	tree->SetBranchAddress("channel", &channel);
	tree->SetBranchAddress("sig3", &sig3);
	tree->SetBranchAddress("pt3", &pt3);
	for(Long64_t i=0; i < tree->GetEntries(); i++) {
		tree->GetEntry(i);
		e1 = e1*1e3;
		e2 = e2*1e3;
		e3 = e3*1e3;
		if(timeToPrevTME < 10e3 || timeToNextTME < 10e3) continue;
		if(t2<500) continue;
		if(abs(t2)>10e3) continue;
		if(target.compare("al50") == 0) {
			if(t2-t1 > 200 || t2-t1 < -200) continue; //Al50
		} else if(target.compare("al100") ==0 ) {
			//if(t2-t1-11 > 200 || t2-t1-11 < -200) continue; //Al100
			if(t2-t1 > 1000 || t2-t1 < -1000) continue; //Al100
		}
		if(channel->Contains("SiL") ) {
			if(sig3->Contains(particle.Data() ) ) {
				hMeasDataLeft->Fill(e1+e2);
			}
		} else if(channel->Contains("SiR") ) {
			if(e2 > 17e3) continue; //remove saturation
			if(TMath::IsNaN(e3) ) {
				if(sig3->Contains(particle.Data() ) ) {
					hMeasDataRight->Fill(e1+e2);
				}
			} else {
				if(pt3->Contains(particle.Data() ) ) {
					hMeasDataRight3->Fill(e1+e2+e3);
				}
			}
		}
	}
	//MC derived correction for SiL
	Double_t SiLMCDerivedVeto[13] = {
		1.22078,
		-22.0513,
		28.4645,
		9.97701,
		55.4582,
		64.2691,
		49.9286,
		76.8945,
		25.4483,
		48.8,
		13.5111,
		12.4286,
		-4.72
	};
	for(Int_t i=17; i < 30; ++i) {
//		hMeasDataLeft->SetBinContent(i, hMeasDataLeft->GetBinContent(i) - SiLMCDerivedVeto[i-17]);
	}

	RooUnfoldResponse *L_TM = (RooUnfoldResponse *)responseMatrixFile->Get("SiL500_TM");
	RooUnfoldResponse *R_TM = (RooUnfoldResponse *)responseMatrixFile->Get("SiR500_TM");
	TFile *fOutputFile = new TFile(Form("/home/m-wong/data/unfolded.%s.%s.root", target.c_str(), particle.Data() ), "RECREATE");
	Process(L_TM, hMeasDataLeft, "Left", target);

	hMeasDataRight3->Scale(1/0.77);
	hMeasDataRight->Add(hMeasDataRight3);
	Process(R_TM, hMeasDataRight, "Right", target);

	fOutputFile->Write();
}

#ifndef __CINT__
int main () { RooUnfoldAlCap(); return 0; }  // Main program when run stand-alone
#endif
