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
#include "RooUnfoldSvd.h"
#endif

void Rebin(RooUnfoldResponse* r, int n) {
	r->Htruth()   ->Rebin(n);
	r->Hmeasured()->Rebin(n);
	r->Hfakes()   ->Rebin(n);
	r->Hresponse()->Rebin2D(n, n);
}
void Process(RooUnfoldResponse *response, TH1D *hMeas, const char *arm = "SiL", std::string target = "al50", const char * particle = "proton", bool lowE = kTRUE, bool normalise = kFALSE) {
	RooUnfoldBayes unfold(response, hMeas);
	TH1D* hReco= (TH1D*) unfold.Hreco();
	const char *strLow = "lowE";
	if(!lowE) strLow = "highE";
	hReco->SetName(Form("h%s_%s_%s", particle, arm, strLow) );
	if(normalise) {
		if(target.compare("al50")==0) {
			hReco->Scale(1/(0.95*0.63*0.609*1.62E+8) ); //al50 (lifetime, capture rate, muon count, 2 sigma selection)
		} else if(target.compare("ti50")==0) {
			hReco->Scale(1/(0.95*0.2968*0.8529*8.02E+7) );
		}
	}
	Double_t error;
	Double_t integral = hReco->IntegralAndError(hReco->GetXaxis()->FindBin(9.5), hReco->GetXaxis()->FindBin(16.5), error, "");
	std::cout << arm << " " << particle << " 4000-8000keV: " << integral << " ± " << error << std::endl;
	integral = hReco->IntegralAndError(hReco->GetXaxis()->FindBin(9.5), hReco->GetXaxis()->FindBin(19.5), error, "");
	std::cout << arm << " " << particle << " 3500-17000keV: " << integral << " ± " << error << std::endl;
}
void Unfold(std::string target = "al50", std::string particle="proton", bool lowE = kTRUE, bool normalise = kFALSE)
{
	const char *dataPath = "/data/ssd3/R15bAnalysis/hists";
	const char *transferMatrixPath = "/data/R15bMC";
	TFile *fData = new TFile(Form("%s/%s-folded.root", dataPath, target.c_str() ), "READ");
	TFile *responseMatrixFile = 0;
	if(target.compare("al50") ==0) {
		if(lowE) {
			responseMatrixFile = new TFile(Form("%s/respMatrix_100M_Al50_%s_100keVBins_midLowE.root", transferMatrixPath, particle.c_str() ), "READ");
		} else {
			responseMatrixFile = new TFile(Form("%s/respMatrix_100M_Al50_%s_100keVBins_midHighE.root", transferMatrixPath, particle.c_str() ), "READ");
		}
	} else if(target.compare("ti50") ==0) {
		if(lowE) {
			responseMatrixFile = new TFile(Form("%s/respMatrix_100M_Ti50_%s_100keVBins_midLowE.root", transferMatrixPath, particle.c_str() ), "READ");
		} else {
			responseMatrixFile = new TFile(Form("%s/respMatrix_100M_Ti50_%s_100keVBins_midHighE.root", transferMatrixPath, particle.c_str() ), "READ");
		}
	}
	std::cout << "Loading data: " << fData->GetName() << std::endl;
	std::cout << "Loading MC: " << responseMatrixFile->GetName() << std::endl;

	TH1D *hMeasDataLeft = (TH1D *) fData->Get(Form("h%cSiL", particle[0] ) ); //500keV bins
	TH1D *hMeasDataRight = (TH1D *) fData->Get(Form("h%cSiR", particle[0] ) );

	RooUnfoldResponse *L_TM = (RooUnfoldResponse *)responseMatrixFile->Get("SiL_two_layer_response");
	RooUnfoldResponse *R_TM = (RooUnfoldResponse *)responseMatrixFile->Get("SiR_three_layer_response");
	Rebin(L_TM, 5); //response matrix to be 500keV per bin
	Rebin(R_TM, 5);
	

	TFile *fOutputFile = new TFile(Form("%s/unfolded.%s.alphas.systematics.root", ".", target.c_str() ), "UPDATE");
	std::cout << "Unfolding SiL..." << std::endl;
	Process(L_TM, hMeasDataLeft, "SiL", target, particle.c_str(), lowE, normalise);
	std::cout << "Unfolding SiR..." << std::endl;
	Process(R_TM, hMeasDataRight, "SiR", target, particle.c_str(), lowE, normalise);
	fOutputFile->Write();
	fOutputFile->Close();
}

#ifndef __CINT__
int Alpha() { 
	Unfold("al50", "alpha", 1); 
	Unfold("al50", "alpha", 0); 
	Unfold("ti50", "alpha", 1); 
	Unfold("ti50", "alpha", 0); 
	return 0; 
	}  // Main program when run stand-alone
#endif
