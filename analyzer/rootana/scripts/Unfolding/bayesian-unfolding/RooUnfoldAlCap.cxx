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
using std::cout;
using std::endl;

#include "TRandom.h"
#include "TH1D.h"

#include "RooUnfoldResponse.h"
#include "RooUnfoldBayes.h"
#include "RooUnfoldTUnfold.h"
#endif

//==============================================================================
// Global definitions
//==============================================================================
const char * responseMatrixFilename = "R15b_Si16b_full-dir-response-matrix.root";
//const char * responseMatrixFilename = "R15b_Si16b_restricted-dir-response-matrix.root";
//const char * responseMatrixFilename = "/home/mark/montecarlo/unfolding.proton.0-24.200k.target.200.leftright.root";
const char * validationTestFilename = "";///home/mark/montecarlo/unfolding.proton.5expo2.20k.target.100.leftright.root";
//const char * validationTestFilename = "/home/mark/montecarlo/unfolding.proton.uniform0-10.20k.target.100.leftright.root";
//const char * validationTestFilename = "test_data/unfolding.proton.6sigma2.20k.target.100.leftright.root";
//const char * validationTestFilename = "test_data/unfolding.proton.16sigma2.20k.target.100.leftright.root";

const char * dataFilename = "Andy_Si16b_EvdEPlot.root";

void Unfold(const char *descriptor, /*RooUnfoldResponse* response,*/ TH2D* hTransferMatrix, TH2D* hEvdE)
{       
        gStyle->SetOptStat(0);
        hEvdE->RebinY(10);
        hEvdE->RebinX(10);
        TH1D *hMeas = hEvdE->ProjectionX();
	RooUnfoldResponse response(0, 0, hTransferMatrix);
	RooUnfoldBayes unfold(&response, hMeas, 6);
	//	RooUnfoldBayes unfold(response, hMeas, 6);
        TH1D* hReco= (TH1D*) unfold.Hreco(); 
        TCanvas *c = new TCanvas(descriptor, descriptor);
        hReco->Draw();
        hReco->SetTitle("Unfolded data");
        hMeas->SetLineColor(kMagenta);
        hMeas->Draw("SAME");
        c->BuildLegend();
        hReco->SetTitle("Unfolding validation");

	double integral_min = 4000;
	double integral_max = 8000;
	int integral_bin_min = hReco->FindBin(integral_min);
	int integral_bin_max = hReco->FindBin(integral_max);
	double integral, error;
	integral = hReco->IntegralAndError(integral_bin_min, integral_bin_max, error);
	std::cout << "Integral (" << integral_min << " -- " << integral_max << " keV): " << integral << " +- " << error << std::endl;
}

void Validation(const char *descriptor, TH2D* hTransferMatrix/* transfer matrix */, TH1D* hMeas /* fake measured data with known truth distribution*/, TH1F* hInput /* truth distribution */, TH1F* hAcceptance /* truth hits detector assuming 100% efficiency */)
{
	gStyle->SetOptStat(0);
	TH1* hTrue = hInput->Clone();
//	for(int i=0; i < hInput->GetSize(); i++) {
//		hTrue->SetBinContent(i, hInput->GetBinContent(i) * hAcceptance->GetBinContent(i)/100 );
//	}
//	hTrue->Rebin(8);
	hMeas->SetTitle("Data; E [MeV]");
	RooUnfoldResponse response (0, 0, hTransferMatrix);
	cout << "==================================== UNFOLD ===================================" << endl;
	RooUnfoldBayes   unfold (&response, hMeas, 6);    // OR

	TH1D* hReco= (TH1D*) unfold.Hreco();
	for(int i=0; i < hReco->GetSize(); i++) {
		if(hAcceptance->GetBinContent(i)!=0) {
			hReco->SetBinContent(i, hReco->GetBinContent(i) / (hAcceptance->GetBinContent(i)/100) );
		} else {
			hReco->SetBinContent(i, hReco->GetBinContent(i) );
		}
	}

	unfold.PrintTable (cout, hTrue);
	TCanvas *c = new TCanvas(descriptor, descriptor);
	TPad *pad1 = new TPad("pad1","This is pad1",0.02,0.29,0.98,0.97);
	TPad *pad2 = new TPad("pad2","This is pad2",0.02,0.02,0.98,0.27);
	pad1->Draw();
	pad2->Draw();

	pad1->cd();
	hTrue->SetLineColor(kGreen);
	hTrue->Draw();
	hTrue->SetTitle("Truth");
	hMeas->SetLineColor(kMagenta);
	hMeas->Draw("SAME");
	hReco->Draw("SAME");
	hReco->SetTitle("Unfolded data");
	pad1->BuildLegend();
	hTrue->SetTitle("Unfolding validation");

	pad2->cd();
	TH1 * hReco_clone = hReco->Clone();
	hReco_clone->SetTitle(";;"); 
	hReco_clone->GetYaxis()->SetLabelSize(0.1);
	hReco_clone->GetXaxis()->SetLabelSize(0.1);
	hReco_clone->GetYaxis()->SetTitle("Difference");
	hReco_clone->GetYaxis()->SetTitleSize(0.1);
	hReco_clone->GetYaxis()->SetTitleOffset(0.25);
	TH1 * hTrue_clone = hTrue->Clone(); 
	hReco_clone->Add(hTrue_clone, -1);
	hReco_clone->Divide(hTrue_clone);
	hReco_clone->Scale(100);
	hReco_clone->Draw();
	TLine *line = new TLine(0, 0, 24, 0);
	line->Draw("SAME");
}
void RooUnfoldAlCap()
{
	TFile *fMC = new TFile(responseMatrixFilename, "READ");
	//	TFile *fFake = new TFile(validationTestFilename, "READ");
	std::cout << "Using response matrix from: " << responseMatrixFilename << std::endl;

	/*	std::cout << "=====Right stopped=====" << std::endl;
	TH2D *hTransferMatrixRight = (TH2D *)fMC->Get("SiR/hTransferRight");
	//	TH2D *hEvdE_stopped_right = (TH2D *)fFake->Get("SiR/hEvdE_stopped_right");
	TH1D *hMeasRight = hEvdE_stopped_right->ProjectionX();
	//	TH1F *hInputRight = (TH1F *)fFake->Get("SiR/hInputRight");
	//        TH1F *hAcceptanceRight = (TH1F *)fFake->Get("SiR/hAcceptanceRight");
	Validation("Right stopped", hTransferMatrixRight, hMeasRight, hInputRight, hAcceptanceRight);

	std::cout << "=====Right punch through=====" << std::endl;
	TH2D *hTransferMatrixPTRight = (TH2D *)fMC->Get("SiR/hTransferPTRight");
	//	TH2D *hEvdE_punch_right= (TH2D *)fFake->Get("SiR/hEvdE_punch_right");
	TH1D *hMeasPTRight = hEvdE_punch_right->ProjectionX();
	Validation("Right punch through", hTransferMatrixPTRight, hMeasPTRight, hInputRight, hAcceptanceRight);

	std::cout << "=====Left stopped=====" << std::endl;
	//	TH2D *hTransferMatrixLeft = (TH2D *)fFake->Get("SiL/hTransferLeft");
	//	TH2D *hEvdE_stopped_left = (TH2D *)fFake->Get("SiL/hEvdE_stopped_left");
	TH1D *hMeasLeft = hEvdE_stopped_left->ProjectionX();
	//	TH1F *hInputLeft = (TH1F *)fFake->Get("SiL/hInputLeft");
	//        TH1F *hAcceptanceLeft = (TH1F *)fFake->Get("SiL/hAcceptanceLeft");
	Validation("Left stopped", hTransferMatrixLeft, hMeasLeft, hInputLeft, hAcceptanceLeft);

	std::cout << "=====Left punch through=====" << std::endl;
	TH2D *hTransferMatrixPTLeft = (TH2D *)fMC->Get("SiL/hTransferPTLeft");
	//	TH2D *hEvdE_punch_left= (TH2D *)fFake->Get("SiL/hEvdE_punch_left");
	TH1D *hMeasPTLeft = hEvdE_punch_left->ProjectionX();
	Validation("Left punch through", hTransferMatrixPTLeft, hMeasPTLeft, hInputLeft, hAcceptanceLeft);
	*/
	
	/*        std::cout << "=====Left proton data=====" << std::endl;
        TFile *fData = new TFile(dataFilename, "READ");
        TH2D *hMeasData = (TH2D *)fData->Get("hSiL_EvdE_proton");
        Unfold("Left proton stopped", hTransferMatrixRight, hMeasData);
	*/
        std::cout << "=====Right proton data=====" << std::endl;
        TFile *fData = new TFile(dataFilename, "READ");
	RooUnfoldResponse* response = (RooUnfoldResponse*) fMC->Get("SiR_response");
	TH2D *hTransferMatrixRight = response->Hresponse();//(TH2D *)fMC->Get("SiR/hTransferRight");
        TH2D *hMeasData = (TH2D *)fData->Get("hEvdE_Si16b_Proton_Veto");
        Unfold("Right proton stopped", /*response,*/hTransferMatrixRight, hMeasData);
}

#ifndef __CINT__
int main () { RooUnfoldAlCap(); return 0; }  // Main program when run stand-alone
#endif
