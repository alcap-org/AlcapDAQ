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

void PiecewiseFit(TH1D *hReco, Double_t low, Double_t high) {
        //currently no physical motivation for this function
        TF1 *fit = new TF1("fit", "[0]*exp(-(x-[1])*[2]) + [3]*exp(-(x-[4])*[5])", low, high);
        fit->SetParameter(0, 5.49525e-03);
//        fit->SetParameter(1, 3.44865e+00);
fit->FixParameter(1, 4);
        fit->SetParameter(2, 5.91451e-01);
        fit->SetParameter(3, 1.21979e-03);
//        fit->SetParameter(4, 7.54185e+00);
fit->FixParameter(4, 8);
        fit->SetParameter(5, 1.79100e-01);
        fit->SetParLimits(0, 0, 1000);
        hReco->Fit("fit", "SR");
        std::cout << "chi2/ndf: " << fit->GetChisquare() << "/" << fit->GetNDF() << std::endl;
}
void Process(RooUnfoldResponse *response, TH1D *hMeas, const char *arm = "SiL", std::string target = "al50", const char * particle = "proton", bool normalise = kFALSE) {
	RooUnfoldBayes unfold(response, hMeas);
	TH1D* hReco= (TH1D*) unfold.Hreco();
	hReco->SetName(Form("h%s_%s", particle, arm) );
	std::cout << std::fixed;
	Double_t error, integral;
	integral = hMeas->IntegralAndError(hMeas->GetXaxis()->FindBin(4), hMeas->GetXaxis()->FindBin(7.5), error); 
	std::cout << "folded " << arm << " 4000-8000keV: " << integral << " ± " << error << std::endl;
	integral = hMeas->IntegralAndError(hMeas->GetXaxis()->FindBin(3.5), hMeas->GetXaxis()->FindBin(9.5), error); 
	std::cout << "folded " << arm << " 3500-10000keV: " << integral << " ± " << error << std::endl;

	integral = hReco->IntegralAndError(hReco->GetXaxis()->FindBin(4), hReco->GetXaxis()->FindBin(7.5), error);
	std::cout << "\033[0;32m" << "No correction " << arm << " 4-8MeV: " << integral << " ± " << error << "\033[0m" << std::endl;
	integral = hReco->IntegralAndError(hReco->GetXaxis()->FindBin(3.5), hReco->GetXaxis()->FindBin(9.5), error);
	std::cout << "\033[0;32m" << "No correction " << arm << " 3.5-10MeV: " << integral << " ± " << error << "\033[0m" << std::endl;
	if(normalise) {
		if(target.compare("al50")==0) {
			hReco->Scale(1/(0.63*0.609*1.62E+8) ); //al50 (lifetime, capture rate, muon count, 3 sigma selection)
		} else if(target.compare("al100")==0) {
			hReco->Scale(1/(0.63*0.609*1.31E+8) ); //al100
		} else if(target.compare("ti50")==0) {
			hReco->Scale(1/(0.2968*0.8529*8.02E+7) );
		}
	}
	integral = hReco->IntegralAndError(hReco->GetXaxis()->FindBin(4), hReco->GetXaxis()->FindBin(7.5), error);
	std::cout << arm << " 4000-8000keV: " << integral << " ± " << error << std::endl;
	integral = hReco->IntegralAndError(hReco->GetXaxis()->FindBin(3.5), hReco->GetXaxis()->FindBin(9.5), error);
	std::cout << arm << " 3500-10000keV: " << integral << " ± " << error << std::endl;

	if(strcmp(particle, "proton") == 0) {
		if(strcmp(arm, "SiR")==0 && (target.compare("al50")==0 || target.compare("ti50")==0 ) ) {
			PiecewiseFit(hReco, 4., 20.);
		} else {
			PiecewiseFit(hReco, 4., 16.);
		}
	} else {
		PiecewiseFit(hReco, 6., 18.);
	}
}
void RooUnfoldAlCap(std::string target = "al50", std::string particle="proton", bool normalise = kTRUE)
{
	const char *dataPath = getenv("R15b_DATA");
	const char *transferMatrixPath = getenv("R15b_TM");
	TFile *fData = new TFile(Form("%s/%s.root", dataPath, target.c_str() ), "READ");
	TFile *responseMatrixFile = 0;
	if(target.compare("al50") ==0) {
		responseMatrixFile = new TFile(Form("%s/transfer.sf1.02.al50.%s.root", transferMatrixPath, particle.c_str() ), "READ");
	} else if(target.compare("al100") ==0 ) {
		responseMatrixFile = new TFile(Form("%s/transfer.sf1.035.al100.%s.root", transferMatrixPath, particle.c_str() ), "READ");
	} else if(target.compare("ti50") ==0) {
		responseMatrixFile = new TFile(Form("%s/transfer.sf1.03.ti50.%s.root", transferMatrixPath, particle.c_str() ), "READ");
	}
	std::cout << "Loading data: " << fData->GetName() << std::endl;
	std::cout << "Loading MC: " << responseMatrixFile->GetName() << std::endl;

	Double_t e1, e2, e3, t1, t2, timeToPrevTME, timeToNextTME;
	Int_t a2;
	TString *channel = new TString("");
	TString  *sig = new TString("");
	TString  *pt = new TString("");
	TH1D *hMeasDataLeft = new TH1D("hMeasDataLeft", "hMeasDataLeft;E [keV]", 50, 0, 25);
	TH1D *hMeasDataRight = new TH1D("hMeasDataRight", "hMeasDataRight;E [keV]", 50, 0, 25);
	TH1D *hMeasDataRight3 = new TH1D("hMeasDataRight3", "hMeasDataRight3;E [keV]", 50, 0, 25);
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
		if(timeToPrevTME < 10e3 || timeToNextTME < 10e3) continue;
		if(t2<400) continue;
		if(t2>10e3) continue;
		if(target.compare("al50") == 0 || target.compare("ti50") == 0 ) {
			//if(abs(t2-t1-12) > 20 * 5) continue; //Al50
			if(abs(t2-t1)> 200) continue; //Al50
		}
		if(target.compare("al100") == 0) {
			if(channel->Contains("SiL") ) {if(abs(t2-t1 + 567) > 24 *3) continue; }
			if(channel->Contains("SiR") ) {if(abs(t2-t1 - 211) > 32 *3) continue; }
		}
		if(channel->Contains("SiL") ) {
			if(sig->Contains(particle) ) {
				hMeasDataLeft->Fill(e1+e2);
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

	const char *sourceName = Form("h%s", particle.c_str() );
	if(fOutputFile->GetListOfKeys()->Contains(sourceName) ) {
		printf("Deleting histogram with name: %s\n", sourceName);
		gDirectory->Delete(Form("%s", sourceName) );
	}
	Process(L_TM, hMeasDataLeft, "SiL", target, particle.c_str(), normalise);

	hMeasDataRight3->Scale(1/0.77);
	hMeasDataRight->Add(hMeasDataRight3);
	Process(R_TM, hMeasDataRight, "SiR", target, particle.c_str(), normalise);

	fOutputFile->Write();
}

#ifndef __CINT__
int main () { RooUnfoldAlCap(); return 0; }  // Main program when run stand-alone
#endif
