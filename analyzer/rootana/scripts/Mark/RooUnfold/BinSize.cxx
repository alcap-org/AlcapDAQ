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
void Process(RooUnfoldResponse *response, TH1D *hMeas, const char *arm = "SiL", std::string target = "al50", const char * particle = "proton", bool normalise = kFALSE) {
	RooUnfoldBayes unfold(response, hMeas);
	TH1D* hReco= (TH1D*) unfold.Hreco();
	hReco->SetName(Form("h%s_%s", particle, arm) );
	if(normalise) {
		if(target.compare("al50")==0) {
			hReco->Scale(1/(0.95*0.63*0.609*1.62E+8) ); //al50 (lifetime, capture rate, muon count, 2 sigma selection)
		} else if(target.compare("al100")==0) {
			hReco->Scale(1/(0.95*0.63*0.609*1.31E+8) ); //al100
		} else if(target.compare("ti50")==0) {
			hReco->Scale(1/(0.95*0.2968*0.8529*8.02E+7) );
		}
	}
}
void Unfold(std::string target = "al50", std::string particle="proton", bool normalise = kFALSE)
{
	const char *dataPath = getenv("R15b_OUT");
	const char *transferMatrixPath = "/data/R15bMC";
	TFile *fData = new TFile(Form("%s/%s-folded-100keVBins.root", dataPath, target.c_str() ), "READ");
	TFile *responseMatrixFile = 0;
	if(target.compare("al50") ==0) {
		responseMatrixFile = new TFile(Form("%s/respMatrix_100M_Al50_%s_100keVBins.root", transferMatrixPath, particle.c_str() ), "READ");
	} else if(target.compare("al100") ==0 ) {
		responseMatrixFile = new TFile(Form("%s/respMatrix_100M_Al100_%s_100keVBins.root", transferMatrixPath, particle.c_str() ), "READ");
	} else if(target.compare("ti50") ==0) {
		responseMatrixFile = new TFile(Form("%s/respMatrix_100M_Ti50_%s_100keVBins.root", transferMatrixPath, particle.c_str() ), "READ");
	}
	std::cout << "Loading data: " << fData->GetName() << std::endl;
	std::cout << "Loading MC: " << responseMatrixFile->GetName() << std::endl;

	TH1D *hMeasDataLeft = (TH1D *) fData->Get(Form("h%cSiL", particle[0] ) );
	TH1D *hMeasDataRight = (TH1D *) fData->Get(Form("h%cSiR", particle[0] ) );

	RooUnfoldResponse *L_TM = (RooUnfoldResponse *)responseMatrixFile->Get("SiL_two_layer_response");
	RooUnfoldResponse *R_TM = 0;
	if(target.compare("al100") == 0) {
		R_TM = (RooUnfoldResponse *)responseMatrixFile->Get("SiR_two_layer_response");
	} else {
		R_TM = (RooUnfoldResponse *)responseMatrixFile->Get("SiR_three_layer_response");
	}
//	Rebin(L_TM, 5); //response matrix to be 500keV per bin
//	Rebin(R_TM, 5);
	

	//TFile *fOutputFile = new TFile(Form("%s/unfolded.%s.root", getenv("R15b_OUT"), target.c_str() ), "UPDATE");
	TFile *fOutputFile = new TFile(Form("%s/unfolded.%s.100keVBins.root", ".", target.c_str() ), "UPDATE");
	std::cout << "Unfolding SiL..." << std::endl;
	Process(L_TM, hMeasDataLeft, "SiL", target, particle.c_str(), normalise);
	std::cout << "Unfolding SiR..." << std::endl;
	Process(R_TM, hMeasDataRight, "SiR", target, particle.c_str(), normalise);
	fOutputFile->Write();
	fOutputFile->Close();
}

#ifndef __CINT__
int BinSize() { 
	Unfold("al50", "proton"); 
	Unfold("al50", "deuteron"); 
	Unfold("al50", "triton"); 
	Unfold("al50", "alpha"); 
	Unfold("al100", "proton"); 
	Unfold("al100", "deuteron"); 
	Unfold("al100", "triton"); 
	Unfold("al100", "alpha"); 
	Unfold("ti50", "proton"); 
	Unfold("ti50", "deuteron"); 
	Unfold("ti50", "triton"); 
	Unfold("ti50", "alpha"); 
	return 0; 
	}  // Main program when run stand-alone
#endif
