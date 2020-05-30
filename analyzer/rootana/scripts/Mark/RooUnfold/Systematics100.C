#if !(defined(__CINT__) || defined(__CLING__)) || defined(__ACLIC__)
#include <iostream>
#include "TRandom.h"
#include "TH1D.h"
#include "RooUnfoldResponse.h"
#include "RooUnfoldBayes.h"
#endif

void UnfoldingUncertainties(TH1D *hMeas, TString arm, TString particle, Double_t *unfoldingError) {
	TFile *responseMatrixFile = new TFile(Form("%s/transfer.sf1.035.al100.%s.root", getenv("R15b_TM"), particle.Data() ), "READ");
	RooUnfoldResponse *L_TM = (RooUnfoldResponse *)responseMatrixFile->Get("SiL500_TM");
	RooUnfoldResponse *R_TM = (RooUnfoldResponse *)responseMatrixFile->Get("SiR500_TM");
	RooUnfoldResponse *response = L_TM;
	if(arm.CompareTo("SiR") == 0) {
		response = R_TM;
	}

	TH1D *hReco = 0;
	RooUnfoldBayes unfold(response, hMeas, 1);
	hReco = (TH1D*) unfold.Hreco();
	TVectorD error = unfold.ErecoV(RooUnfold::ErrorTreatment::kCovariance); //diagonals of the cov matrix
	std::cout << "unfolding errors" << std::endl;
	std::cout << "E bin\t|" << "error" << std::endl;
	for(int i=0; i <= error.GetNoElements(); ++i) {
		if(hReco->GetBinContent(i) != 0) {
			unfoldingError[i] = error[i] / hReco->GetBinContent(i);
		} else {
			unfoldingError[i] = 0;
		}
	}
}

TH1D * Process(TH1D *hMeas, TString arm, TString particle, TString cutDescription) {
	TFile *responseMatrixFile = new TFile(Form("%s/transfer.sf1.035.al100.%s.root", getenv("R15b_TM"), particle.Data() ), "READ");
	RooUnfoldResponse *L_TM = (RooUnfoldResponse *)responseMatrixFile->Get("SiL500_TM");
	RooUnfoldResponse *R_TM = (RooUnfoldResponse *)responseMatrixFile->Get("SiR500_TM");
	RooUnfoldResponse *response = L_TM;
	if(arm.CompareTo("SiR") == 0) {
		response = R_TM;
	}
	RooUnfoldBayes unfold(response, hMeas, 1);
	TH1D* hReco= (TH1D*) unfold.Hreco();
	hReco->SetTitle(Form("Unfolded %s %s %s", arm.Data(), particle.Data(), cutDescription.Data() ) );
	std::cout << arm << " " << particle << std::endl;
	Double_t error;
	Double_t integral = hMeas->IntegralAndError(hMeas->GetXaxis()->FindBin(2.000), hMeas->GetXaxis()->FindBin(16.500), error); 
	std::cout << "folded 2.000-17.000MeV: " << integral << " ± " << error << std::endl;
	integral = hReco->IntegralAndError(hReco->GetXaxis()->FindBin(4.000), hReco->GetXaxis()->FindBin(7.500), error);
	std::cout << " 4.000-8.000MeV: " << integral << " ± " << error << std::endl;
	integral = hReco->IntegralAndError(hReco->GetXaxis()->FindBin(0.), hReco->GetXaxis()->FindBin(9.500), error);
	std::cout << " 0-10.000MeV: " << integral << " ± " << error << std::endl;
	integral = hReco->IntegralAndError(hReco->GetXaxis()->FindBin(3.500), hReco->GetXaxis()->FindBin(9.500), error);
	std::cout << " 3.500-10.000MeV: " << integral << " ± " << error << std::endl;
	std::cout << "==========================================" << std::endl;

	return hReco;
}
void lifetime(TTree *tree, Double_t *lifetimeError, TString arm, TString particle) {
	Double_t scale = 1/0.77;
	Double_t t1, t2, t3, e1, e2, e3, timeToPrevTME, timeToNextTME;
	Int_t a2;
	TString *channel = new TString();
	TString *sig1 = new TString();
	TString *sig2 = new TString();
	TString *sig3 = new TString();
	TString *sig4 = new TString();
	TString *pt1 = new TString();
	TString *pt2 = new TString();
	TString *pt3 = new TString();
	TString *pt4 = new TString();
	tree->SetBranchAddress("timeToPrevTME", &timeToPrevTME);
	tree->SetBranchAddress("timeToNextTME", &timeToNextTME);
	tree->SetBranchAddress("e1", &e1);
	tree->SetBranchAddress("t1", &t1);
	tree->SetBranchAddress("e2", &e2);
	tree->SetBranchAddress("a2", &a2);
	tree->SetBranchAddress("t2", &t2);
	tree->SetBranchAddress("e3", &e3);
	tree->SetBranchAddress("t3", &t3);
	tree->SetBranchAddress("channel", &channel);
	tree->SetBranchAddress("sig1", &sig1);
	tree->SetBranchAddress("sig2", &sig2);
	tree->SetBranchAddress("sig3", &sig3);
	tree->SetBranchAddress("sig4", &sig4);
	tree->SetBranchAddress("pt1", &pt1);
	tree->SetBranchAddress("pt2", &pt2);
	tree->SetBranchAddress("pt3", &pt3);
	tree->SetBranchAddress("pt4", &pt4);

	Int_t nbins = 50;
	TH1D *hOne = new TH1D(Form("h300_%s_%s", arm.Data(), particle.Data() ), "300ns;E [keV];Counts / 500keV", nbins, 0, 25); 
	TH1D *hTwo = new TH1D(Form("h400_%s_%s", arm.Data(), particle.Data() ), "400ns;E [keV];Counts / 500keV", nbins, 0, 25); 
	TH1D *hThree = new TH1D(Form("h500_%s_%s", arm.Data(), particle.Data() ), "500ns;E [keV];Counts / 500keV", nbins, 0, 25); 
	TH1D *hFour = new TH1D(Form("h600_%s_%s", arm.Data(), particle.Data() ), "600ns;E [keV];Counts / 500keV", nbins, 0, 25); 
	TH1D *hOne3 = new TH1D(Form("h300_%s_%s3", arm.Data(), particle.Data() ), "300ns;E [keV];Counts / 500keV", nbins, 0, 25); 
	TH1D *hTwo3 = new TH1D(Form("h400_%s_%s3", arm.Data(), particle.Data() ), "400ns;E [keV];Counts / 500keV", nbins, 0, 25); 
	TH1D *hThree3 = new TH1D(Form("h500_%s_%s3", arm.Data(), particle.Data() ), "500ns;E [keV];Counts / 500keV", nbins, 0, 25); 
	TH1D *hFour3 = new TH1D(Form("h600_%s_%s3", arm.Data(), particle.Data() ), "600ns;E [keV];Counts / 500keV", nbins, 0, 25); 
	for(Long64_t i=0; i < tree->GetEntries(); ++i) {
		tree->GetEntry(i);
//		e1 *= 1e3;
//		e2 *= 1e3;
//		e3 *= 1e3;
		if(abs(t2)>10e3) continue;
		if(timeToPrevTME < 10e3 || timeToNextTME < 10e3) continue;
		if(channel->Contains("SiL") ) {if(abs(t2-t1 + 567) > 24 *3) continue; }
		if(channel->Contains("SiR") ) {if(abs(t2-t1 - 211) > 32 *3) continue; }
		if(TMath::IsNaN(e3) ) {
			if(!sig3->Contains(particle) ) continue;
			if(arm.CompareTo("SiR") == 0 ) {
				if(a2 > 3986) continue; //remove saturation
			} else {
				if(a2 > 3982) continue; //remove saturation
			}
			
			if(t2 > 400) hOne->Fill(e1+e2);
			if(t2 > 500) hTwo->Fill(e1+e2);
			if(t2 > 600) hThree->Fill(e1+e2);
			if(t2 > 700) hFour->Fill(e1+e2);
		} else {
			if(abs(t3-t2) > 200) continue;
			if(!pt3->Contains(particle) ) continue;
			if(t2 > 400) hOne3->Fill(e1+e2+e3);
			if(t2 > 500) hTwo3->Fill(e1+e2+e3);
			if(t2 > 600) hThree3->Fill(e1+e2+e3);
			if(t2 > 700) hFour3->Fill(e1+e2+e3);
		}
	}
	hOne3->Scale(scale);
	hOne->Add(hOne3);
	hTwo3->Scale(scale);
	hTwo->Add(hTwo3);
	hThree3->Scale(scale);
	hThree->Add(hThree3);
	hFour3->Scale(scale);
	hFour->Add(hFour3);

	TH1D *hOneUnf = Process(hOne, arm, particle, "300ns");
	TH1D *hTwoUnf = Process(hTwo, arm, particle, "400ns");
	TH1D *hThreeUnf = Process(hThree, arm, particle, "500ns");
	TH1D *hFourUnf = Process(hFour, arm, particle, "600ns");

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
			lifetimeError[j] = 1- ((hFourUnf->GetBinContent(j)/0.50) / (hOneUnf->GetBinContent(j)/0.71) );
		} else {
			lifetimeError[j] = 0;
		}
	}
}
void dtFill(Double_t t2, Double_t t1, Double_t E, TH1D *h1, TH1D *h2, TH1D *h3, TH1D *h4, TString arm, TString particle) {
	if(particle.CompareTo("proton") == 0 ) {
		if(arm.CompareTo("SiR") == 0) {
			if(E<3.000) {
				if(abs(t2-t1 - 2.00309e+02) <   2.40715e+01) h1->Fill(E);
				if(abs(t2-t1 - 2.00309e+02) < 2*2.40715e+01) h2->Fill(E);
				if(abs(t2-t1 - 2.00309e+02) < 3*2.40715e+01) h3->Fill(E);
				if(abs(t2-t1 - 2.00309e+02) < 4*2.40715e+01) h4->Fill(E);
			}
			const int c = 12;
			double energy[c] =   {3.500,             4.500,       5.500,       6.500,       7.500,       8.500,       9.500,      10.500,      11.500,      12.500, 13.500, 14.500};
			double mean[c]   =   {2.05407e+02, 2.08025e+02, 2.10359e+02, 2.10960e+02, 2.13304e+02, 2.14460e+02, 2.11396e+02, 2.15547e+02, 2.21333e+02, 2.25367e+02, 2.31142e+02, 2.36037e+02};
			double sigma[c]  =   {2.26178e+01, 2.53379e+01, 2.74922e+01, 3.07790e+01, 3.18520e+01, 3.61717e+01, 3.78173e+01, 4.14607e+01, 4.52926e+01, 4.55567e+01, 4.91536e+01, 4.98836e+01};
			for(int i=0; i < c; ++i) {
				if(abs(E-energy[i])<.5) {
					if(abs(t2-t1 - mean[i]) < sigma[i]) h1->Fill(E);
					if(abs(t2-t1 - mean[i]) < 2*sigma[i]) h2->Fill(E);
					if(abs(t2-t1 - mean[i]) < 3*sigma[i]) h3->Fill(E);
					if(abs(t2-t1 - mean[i]) < 4*sigma[i]) h4->Fill(E);
				}
			}
			if(E>15.000) {
				if(abs(t2-t1 - 2.38365e+02) <   4.73257e+01) h1->Fill(E);
				if(abs(t2-t1 - 2.38365e+02) < 2*4.73257e+01) h2->Fill(E);
				if(abs(t2-t1 - 2.38365e+02) < 3*4.73257e+01) h3->Fill(E);
				if(abs(t2-t1 - 2.38365e+02) < 4*4.73257e+01) h4->Fill(E);
			}
		} else { //SiL
			if(E<3.000) {
				if(abs(t2-t1 + 5.71674e+02) <   2.23134e+01) h1->Fill(E);
				if(abs(t2-t1 + 5.71674e+02) < 2*2.23134e+01) h2->Fill(E);
				if(abs(t2-t1 + 5.71674e+02) < 3*2.23134e+01) h3->Fill(E);
				if(abs(t2-t1 + 5.71674e+02) < 4*2.23134e+01) h4->Fill(E);
			}
			const int c = 12;
			double energy[c] =   {3.500, 4.500, 5.500,  6.500, 7.500,  8.500, 9.500, 10.500, 11.500, 12.500, 13.500, 14.500};
			double mean[c]   =   {-5.68465e+02, -5.66751e+02, -5.67283e+02, -5.67391e+02, -5.67668e+02, -5.68393e+02, -5.67110e+02, -5.65988e+02, -5.62315e+02, -5.60232e+02, -5.52439e+02, -5.44861e+02};
			double sigma[c]  =   { 1.99205e+01, 2.05535e+01, 2.13026e+01, 2.30468e+01, 2.37264e+01, 2.52898e+01, 2.74964e+01, 2.85066e+01, 3.12856e+01, 3.25803e+01, 3.11072e+01, 3.69043e+01};
			for(int i=0; i < c; ++i) {
				if(abs(E-energy[i])<.5) {
					if(abs(t2-t1 - mean[i]) < sigma[i]) h1->Fill(E);
					if(abs(t2-t1 - mean[i]) < 2*sigma[i]) h2->Fill(E);
					if(abs(t2-t1 - mean[i]) < 3*sigma[i]) h3->Fill(E);
					if(abs(t2-t1 - mean[i]) < 4*sigma[i]) h4->Fill(E);
				}
			}
			if(E>15.000) {
				if(abs(t2-t1 + 5.59418e+02) <   8.39850e+01) h1->Fill(E);
				if(abs(t2-t1 + 5.59418e+02) < 2*8.39850e+01) h2->Fill(E);
				if(abs(t2-t1 + 5.59418e+02) < 3*8.39850e+01) h3->Fill(E);
				if(abs(t2-t1 + 5.59418e+02) < 4*8.39850e+01) h4->Fill(E);
			}
		}
	} else if(particle.CompareTo("deuteron") == 0) { //deuteron
		if(arm.CompareTo("SiR") == 0) {
			const int c = 12;
			double energy[c] =   {3.500,             4.500,       5.500,       6.500,       7.500,       8.500,       9.500,      10.500,      11.500,      12.500,      13.500,      14.500};
			double mean[c]   =   {2.02048e+02, 2.04298e+02, 2.06706e+02, 2.05925e+02, 2.08258e+02, 2.04351e+02, 2.05291e+02, 2.02974e+02, 2.03582e+02, 2.03325e+02, 2.01155e+02, 2.05418e+02};
			double sigma[c]  =   {2.07768e+01, 1.85590e+01, 2.11972e+01, 2.12487e+01, 2.23483e+01, 2.61471e+01, 2.42041e+01, 2.59959e+01, 2.96122e+01, 3.31580e+01, 3.90417e+01, 3.51477e+01};
			for(int i=0; i < c; ++i) {
				if(abs(E-energy[i])<.5) {
					if(abs(t2-t1 - mean[i]) < sigma[i]) h1->Fill(E);
					if(abs(t2-t1 - mean[i]) < 2*sigma[i]) h2->Fill(E);
					if(abs(t2-t1 - mean[i]) < 3*sigma[i]) h3->Fill(E);
					if(abs(t2-t1 - mean[i]) < 4*sigma[i]) h4->Fill(E);
				}
			}
			if(E>15.000) {
				if(abs(t2-t1 - 2.17410e+02) <   3.74541e+01) h1->Fill(E);
				if(abs(t2-t1 - 2.17410e+02) < 2*3.74541e+01) h2->Fill(E);
				if(abs(t2-t1 - 2.17410e+02) < 3*3.74541e+01) h3->Fill(E);
				if(abs(t2-t1 - 2.17410e+02) < 4*3.74541e+01) h4->Fill(E);
			}
		} else { //SiL
			const int c = 12;
			double energy[c] =   {3.500       ,        4.500,        5.500,        6.500,        7.500,        8.500,        9.500,       10.500,       11.500,       12.500,       13.500,       14.500};
			double mean[c]   =   {-5.70697e+02, -5.66322e+02, -5.65257e+02, -5.66662e+02, -5.65310e+02, -5.67597e+02, -5.69034e+02, -5.72863e+02, -5.74804e+02, -5.71815e+02, -5.73689e+02, -5.74401e+02};
			double sigma[c]  =   { 2.04009e+01,  1.91148e+01,  1.83256e+01,  1.88325e+01,  2.07188e+01,  1.95376e+01,  2.27581e+01,  2.26845e+01,  2.32077e+01,  2.24143e+01,  2.28877e+01,  3.13415e+01};
			for(int i=0; i < c; ++i) {
				if(abs(E-energy[i])<.5) {
					if(abs(t2-t1 - mean[i]) < sigma[i]) h1->Fill(E);
					if(abs(t2-t1 - mean[i]) < 2*sigma[i]) h2->Fill(E);
					if(abs(t2-t1 - mean[i]) < 3*sigma[i]) h3->Fill(E);
					if(abs(t2-t1 - mean[i]) < 4*sigma[i]) h4->Fill(E);
				}
			}
			if(E>15.000) {
				if(abs(t2-t1 + 5.61400e+02) <   2.50616e+01) h1->Fill(E);
				if(abs(t2-t1 + 5.61400e+02) < 2*2.50616e+01) h2->Fill(E);
				if(abs(t2-t1 + 5.61400e+02) < 3*2.50616e+01) h3->Fill(E);
				if(abs(t2-t1 + 5.61400e+02) < 4*2.50616e+01) h4->Fill(E);
			}
		}
	} else if(particle.CompareTo("triton") == 0) {
		if(arm.CompareTo("SiR") == 0) {
			const int c = 7;
			double energy[c] = {5.         , 7.         , 9.         , 11.        , 13.        , 15.        , 17.        };
			double mean[c]   = {2.04373e+02, 2.06286e+02, 2.00388e+02, 2.01213e+02, 1.97370e+02, 2.00305e+02, 1.99123e+02};
			double sigma[c]  = {1.87975e+01, 2.12020e+01, 2.54504e+01, 2.58702e+01, 2.44349e+01, 2.72377e+01, 3.22884e+01};
			if(E<4.0) {
				if(abs(t2-t1 - 2.02190e+02 ) <   2.32842e+01) h1->Fill(E);
				if(abs(t2-t1 - 2.02190e+02 ) < 2*2.32842e+01) h2->Fill(E);
				if(abs(t2-t1 - 2.02190e+02 ) < 3*2.32842e+01) h3->Fill(E);
				if(abs(t2-t1 - 2.02190e+02 ) < 4*2.32842e+01) h4->Fill(E);
			}
			for(int i=0; i < c; ++i) {
				if(abs(E-energy[i])<1.) {
					if(abs(t2-t1 - mean[i]) <   sigma[i]) h1->Fill(E);
					if(abs(t2-t1 - mean[i]) < 2*sigma[i]) h2->Fill(E);
					if(abs(t2-t1 - mean[i]) < 3*sigma[i]) h3->Fill(E);
					if(abs(t2-t1 - mean[i]) < 4*sigma[i]) h4->Fill(E);
				}
			}
		} else {
			const int c = 7;
			double energy[c] = {5.          , 7.         , 9.         , 11.        , 13.        , 15.        , 17.        };
			double mean[c]   = {-5.64237e+02,-5.63769e+02,-5.70446e+02,-5.71345e+02,-5.74695e+02,-5.77142e+02,-5.78904e+02};
			double sigma[c]  = { 1.89892e+01, 1.83317e+01, 1.92184e+01, 2.08616e+01, 2.13967e+01, 2.11965e+01, 2.77259e+01};
			if(E<4.0) {
				if(abs(t2-t1 +5.71339e+02  ) <   2.82428e+01) h1->Fill(E);
				if(abs(t2-t1 +5.71339e+02  ) < 2*2.82428e+01) h2->Fill(E);
				if(abs(t2-t1 +5.71339e+02  ) < 3*2.82428e+01) h3->Fill(E);
				if(abs(t2-t1 +5.71339e+02  ) < 4*2.82428e+01) h4->Fill(E);
			}
			for(int i=0; i < c; ++i) {
				if(abs(E-energy[i])<1.) {
					if(abs(t2-t1 - mean[i]) <   sigma[i]) h1->Fill(E);
					if(abs(t2-t1 - mean[i]) < 2*sigma[i]) h2->Fill(E);
					if(abs(t2-t1 - mean[i]) < 3*sigma[i]) h3->Fill(E);
					if(abs(t2-t1 - mean[i]) < 4*sigma[i]) h4->Fill(E);
				}
			}
			if(E>18.000) {
				if(abs(t2-t1 + 5.72768e+02 ) <   3.00878e+01) h1->Fill(E);
				if(abs(t2-t1 + 5.72768e+02 ) < 2*3.00878e+01) h2->Fill(E);
				if(abs(t2-t1 + 5.72768e+02 ) < 3*3.00878e+01) h3->Fill(E);
				if(abs(t2-t1 + 5.72768e+02 ) < 4*3.00878e+01) h4->Fill(E);
			}
		}

	} else if(particle.CompareTo("alpha") == 0) {
		if(arm.CompareTo("SiR") == 0) {
			const int c = 4;
			double energy[c] = {9.         , 11.         , 13.         , 15.      };
			double mean[c]   = {2.13200e+02, 2.08957e+02, 2.09367e+02, 2.00138e+02};
			double sigma[c]  = {2.55589e+01, 2.06339e+01, 2.16175e+01, 2.07128e+01};
			for(int i=0; i < c; ++i) {
				if(abs(E-energy[i])<1.) {
					if(abs(t2-t1 - mean[i]) <   sigma[i]) h1->Fill(E);
					if(abs(t2-t1 - mean[i]) < 2*sigma[i]) h2->Fill(E);
					if(abs(t2-t1 - mean[i]) < 3*sigma[i]) h3->Fill(E);
					if(abs(t2-t1 - mean[i]) < 4*sigma[i]) h4->Fill(E);
				}
			}
			if(E>16.000) {
				if(abs(t2-t1 - 1.95191e+02  ) <   1.84623e+01) h1->Fill(E);
				if(abs(t2-t1 - 1.95191e+02  ) < 2*1.84623e+01) h2->Fill(E);
				if(abs(t2-t1 - 1.95191e+02  ) < 3*1.84623e+01) h3->Fill(E);
				if(abs(t2-t1 - 1.95191e+02  ) < 4*1.84623e+01) h4->Fill(E);
			}
		} else {
			const int c = 4;
			double energy[c] = {11.         , 13.         , 15.      };
			double mean[c]   = {-5.54839e+02,-5.62131e+02, -5.66994e+02};
			double sigma[c]  = { 1.49946e+01, 1.86325e+01, 2.42610e+01};
			if(E<10.0) {
				if(abs(t2-t1 + 5.63455e+02 ) <   2.01489e+01) h1->Fill(E);
				if(abs(t2-t1 + 5.63455e+02 ) < 2*2.01489e+01) h2->Fill(E);
				if(abs(t2-t1 + 5.63455e+02 ) < 3*2.01489e+01) h3->Fill(E);
				if(abs(t2-t1 + 5.63455e+02 ) < 4*2.01489e+01) h4->Fill(E);
			}
			for(int i=0; i < c; ++i) {
				if(abs(E-energy[i])<1.) {
					if(abs(t2-t1 - mean[i]) <   sigma[i]) h1->Fill(E);
					if(abs(t2-t1 - mean[i]) < 2*sigma[i]) h2->Fill(E);
					if(abs(t2-t1 - mean[i]) < 3*sigma[i]) h3->Fill(E);
					if(abs(t2-t1 - mean[i]) < 4*sigma[i]) h4->Fill(E);
				}
			}
			if(E>16.000) {
				if(abs(t2-t1 + 5.73947e+02  ) <   2.03846e+01) h1->Fill(E);
				if(abs(t2-t1 + 5.73947e+02  ) < 2*2.03846e+01) h2->Fill(E);
				if(abs(t2-t1 + 5.73947e+02  ) < 3*2.03846e+01) h3->Fill(E);
				if(abs(t2-t1 + 5.73947e+02  ) < 4*2.03846e+01) h4->Fill(E);
			}

		}
	}
}

void dt(TTree *tree, Double_t *dtError, TString arm, TString particle) {
	Double_t scale = 1/0.77;
	Double_t t1, t2, t3, e1, e2, e3, timeToPrevTME, timeToNextTME;
	Int_t a2;
	TString *channel = new TString();
	TString *sig1 = new TString();
	TString *sig2 = new TString();
	TString *sig3 = new TString();
	TString *sig4 = new TString();
	TString *pt1 = new TString();
	TString *pt2 = new TString();
	TString *pt3 = new TString();
	TString *pt4 = new TString();
	tree->SetBranchAddress("timeToPrevTME", &timeToPrevTME);
	tree->SetBranchAddress("timeToNextTME", &timeToNextTME);
	tree->SetBranchAddress("e1", &e1);
	tree->SetBranchAddress("t1", &t1);
	tree->SetBranchAddress("e2", &e2);
	tree->SetBranchAddress("t2", &t2);
	tree->SetBranchAddress("a2", &a2);
	tree->SetBranchAddress("e3", &e3);
	tree->SetBranchAddress("t3", &t3);
	tree->SetBranchAddress("channel", &channel);
	tree->SetBranchAddress("sig1", &sig1);
	tree->SetBranchAddress("sig2", &sig2);
	tree->SetBranchAddress("sig3", &sig3);
	tree->SetBranchAddress("sig4", &sig4);
	tree->SetBranchAddress("pt1", &pt1);
	tree->SetBranchAddress("pt2", &pt2);
	tree->SetBranchAddress("pt3", &pt3);
	tree->SetBranchAddress("pt4", &pt4);

	Int_t nbins = 50;
	TH1D *hOne = new TH1D(Form("hdt_1_%s_%s", arm.Data(), particle.Data() ), "1#sigma;E [keV];Counts / 500keV", nbins, 0, 25); 
	TH1D *hTwo = new TH1D(Form("hdt_2_%s_%s", arm.Data(), particle.Data() ), "2#sigma;E [keV];Counts / 500keV", nbins, 0, 25); 
	TH1D *hThree = new TH1D(Form("hdt_3_%s_%s", arm.Data(), particle.Data() ), "3#sigma;E [keV];Counts / 500keV", nbins, 0, 25); 
	TH1D *hFour = new TH1D(Form("hdt_4_%s_%s", arm.Data(), particle.Data() ), "4#sigma;E [keV];Counts / 500keV", nbins, 0, 25); 
	TH1D *hOne3 = new TH1D(Form("hdt_1_%s_%s3", arm.Data(), particle.Data() ), "1#sigma;E [keV];Counts / 500keV", nbins, 0, 25); 
	TH1D *hTwo3 = new TH1D(Form("hdt_2_%s_%s3", arm.Data(), particle.Data() ), "2#sigma;E [keV];Counts / 500keV", nbins, 0, 25); 
	TH1D *hThree3 = new TH1D(Form("hdt_3_%s_%s3", arm.Data(), particle.Data() ), "3#sigma;E [keV];Counts / 500keV", nbins, 0, 25); 
	TH1D *hFour3 = new TH1D(Form("hdt_4_%s_%s3", arm.Data(), particle.Data() ), "4#sigma;E [keV];Counts / 500keV", nbins, 0, 25); 
	for(Long64_t i=0; i < tree->GetEntries(); ++i) {
		tree->GetEntry(i);
//		e1 *= 1e3;
//		e2 *= 1e3;
//		e3 *= 1e3;
		if(timeToPrevTME < 10e3 || timeToNextTME < 10e3) continue;
		if(t2<400) continue;
		if(abs(t2)>10e3) continue;
		if(TMath::IsNaN(e3) )  {
			if(!sig3->Contains(particle) ) continue;
			if(arm.CompareTo("SiR") == 0 ) {
				if(a2 > 3986) continue; //remove saturation
			} else {
				if(a2 > 3982) continue; //remove saturation
			}
			dtFill(t2, t1, e1+e2, hOne, hTwo, hThree, hFour, arm, particle);
		}
	}
	TH1D *hOneUnf = Process(hOne, arm, particle, "1#sigma");
	TH1D *hTwoUnf = Process(hTwo, arm, particle, "2#sigma");
	TH1D *hThreeUnf = Process(hThree, arm, particle, "3#sigma");
	TH1D *hFourUnf = Process(hFour, arm, particle, "4#sigma");
	
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
void Pid(TTree *tree, Double_t *pidError, Double_t *unfoldingError, TString arm, TString particle) {
	Double_t scale = 1/0.77;
	Double_t t1, t2, t3, e1, e2, e3, timeToPrevTME, timeToNextTME;
	Int_t a2;
	TString *channel = new TString();
	TString *sig1 = new TString();
	TString *sig2 = new TString();
	TString *sig3 = new TString();
	TString *sig4 = new TString();
	TString *pt1 = new TString();
	TString *pt2 = new TString();
	TString *pt3 = new TString();
	TString *pt4 = new TString();
	tree->SetBranchAddress("timeToPrevTME", &timeToPrevTME);
	tree->SetBranchAddress("timeToNextTME", &timeToNextTME);
	tree->SetBranchAddress("e1", &e1);
	tree->SetBranchAddress("t1", &t1);
	tree->SetBranchAddress("a2", &a2);
	tree->SetBranchAddress("e2", &e2);
	tree->SetBranchAddress("t2", &t2);
	tree->SetBranchAddress("e3", &e3);
	tree->SetBranchAddress("t3", &t3);
	tree->SetBranchAddress("channel", &channel);
	tree->SetBranchAddress("sig1", &sig1);
	tree->SetBranchAddress("sig2", &sig2);
	tree->SetBranchAddress("sig3", &sig3);
	tree->SetBranchAddress("sig4", &sig4);
	tree->SetBranchAddress("pt1", &pt1);
	tree->SetBranchAddress("pt2", &pt2);
	tree->SetBranchAddress("pt3", &pt3);
	tree->SetBranchAddress("pt4", &pt4);

	Int_t nbins = 50;
	TH1D *hOne = new TH1D(Form("hPid_1_%s_%s", arm.Data(), particle.Data() ), "1#sigma;E [keV];Counts / 500keV", nbins, 0, 25); 
	TH1D *hTwo = new TH1D(Form("hPid_2_%s_%s", arm.Data(), particle.Data() ), "2#sigma;E [keV];Counts / 500keV", nbins, 0, 25); 
	TH1D *hThree = new TH1D(Form("hPid_3_%s_%s", arm.Data(), particle.Data() ), "3#sigma;E [keV];Counts / 500keV", nbins, 0, 25); 
	TH1D *hFour = new TH1D(Form("hPid_4_%s_%s", arm.Data(), particle.Data() ), "4#sigma;E [keV];Counts / 500keV", nbins, 0, 25); 
	TH1D *hOne3 = new TH1D(Form("hPid_1_%s_%s3", arm.Data(), particle.Data() ), "1#sigma;E [keV];Counts / 500keV", nbins, 0, 25); 
	TH1D *hTwo3 = new TH1D(Form("hPid_2_%s_%s3", arm.Data(), particle.Data() ), "2#sigma;E [keV];Counts / 500keV", nbins, 0, 25); 
	TH1D *hThree3 = new TH1D(Form("hPid_3_%s_%s3", arm.Data(), particle.Data() ), "3#sigma;E [keV];Counts / 500keV", nbins, 0, 25); 
	TH1D *hFour3 = new TH1D(Form("hPid_4_%s_%s3", arm.Data(), particle.Data() ), "4#sigma;E [keV];Counts / 500keV", nbins, 0, 25); 
	for(Long64_t i=0; i < tree->GetEntries(); ++i) {
		tree->GetEntry(i);
//		e1 *= 1e3;
//		e2 *= 1e3;
//		e3 *= 1e3;
		if(timeToPrevTME < 10e3 || timeToNextTME < 10e3) continue;
		if(t2<400) continue;
		if(abs(t2)>10e3) continue;
		if(channel->Contains("SiL") ) {if(abs(t2-t1 + 567) > 24 *3) continue; }
		if(channel->Contains("SiR") ) {if(abs(t2-t1 - 211) > 32 *3) continue; }
		if(TMath::IsNaN(e3) ) {
			if(arm.CompareTo("SiR") == 0 ) {
				if(a2 > 3986) continue; //remove saturation
				if(!channel->Contains("SiR") ) continue;
			} else {
				if(a2 > 3982) continue; //remove saturation
				if(!channel->Contains("SiL") ) continue;
			}
			if(sig1->Contains(particle) ) hOne->Fill(e1+e2);
			if(sig2->Contains(particle) ) hTwo->Fill(e1+e2);
			if(sig3->Contains(particle) ) hThree->Fill(e1+e2);
			if(sig4->Contains(particle) ) hFour->Fill(e1+e2);
		} else {
			if(pt1->Contains(particle) ) hOne3->Fill(e1+e2+e3);
			if(pt2->Contains(particle) ) hTwo3->Fill(e1+e2+e3);
			if(pt3->Contains(particle) ) hThree3->Fill(e1+e2+e3);
			if(pt4->Contains(particle) ) hFour3->Fill(e1+e2+e3);
		}
	}
	hOne3->Scale(scale);
	hOne->Add(hOne3);
	hTwo3->Scale(scale);
	hTwo->Add(hTwo3);
	hThree3->Scale(scale);
	hThree->Add(hThree3);
	hFour3->Scale(scale);
	hFour->Add(hFour3);

	TH1D *hOneUnf = Process(hOne, arm, particle, "1#sigma");
	TH1D *hTwoUnf = Process(hTwo, arm, particle, "2#sigma");
	TH1D *hThreeUnf = Process(hThree, arm, particle, "3#sigma");
	TH1D *hFourUnf = Process(hFour, arm, particle, "4#sigma");

	UnfoldingUncertainties(hThree, arm, particle, unfoldingError);

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
			pidError[k] = 1- ((hOneUnf->GetBinContent(k)/0.682) / (hTwoUnf->GetBinContent(k)/0.954) );
		} else {
			pidError[k] = 0;
		}
	}
}

void Finally(Double_t *unfoldingError, Double_t *pidError, Double_t *dtError, Double_t *lifetimeError, TString arm, TString particle) {
	Int_t nbins = 32;
	//where does this error come from?
	TH1D *hPid = new TH1D("hPid", "PID;E [keV]", nbins, 0, 25); hPid->SetFillColor(kRed);
	TH1D *hDt = new TH1D("hDt", "t_{2}-t_{1};E [keV]", nbins, 0, 25); hDt->SetFillColor(kGreen);
	TH1D *hLifetime = new TH1D("hLifetime", "#tau;E [keV]", nbins, 0, 25); hLifetime->SetFillColor(kBlue);
//	TH1D *hUnfolding = new TH1D("hUnfolding", "Unfolding;E [keV]", nbins, 0, 25); hUnfolding->SetFillColor(kOrange);
//	for(int i=1; i<=nbins; ++i) {
	for(int i=8; i<=32; ++i) {
		hPid->SetBinContent(i, abs(pidError[i]) );
		hDt->SetBinContent(i, abs(dtError[i]) );
		hLifetime->SetBinContent(i, abs(lifetimeError[i]) );
//		hUnfolding->SetBinContent(i, abs(unfoldingError[i] ));
	}
	THStack *hSystematics = new THStack("hSystematics", "Systematic errors");
	hSystematics->Add(hPid);
	hSystematics->Add(hDt);
	hSystematics->Add(hLifetime);
//	hSystematics->Add(hUnfolding);

	TLegend *legend = new TLegend(.240, .598, .512, .868);
	legend->SetHeader(Form("#bf{AlCap} #it{Al100} %s Systematics", arm.Data() ) );
	legend->AddEntry(hPid, "PID", "F");
	legend->AddEntry(hDt, "#Delta t", "F");
	legend->AddEntry(hLifetime, "#tau", "F");
//	legend->AddEntry(hUnfolding, "Unfolding", "F");

	TCanvas *cFinal = new TCanvas("c", "c");
	cFinal->SetGridx();
	cFinal->SetGridy();
	hSystematics->Draw();
	hSystematics->GetXaxis()->SetTitle("E[MeV]");
	hSystematics->GetYaxis()->SetTitle("Fractional");
	legend->Draw("SAME");
	cFinal->Draw();
	const char *FigsDir = getenv("R15b_OUT");
	cFinal->SaveAs(Form("%s/AlCapData_Al100Dataset_%s_%s-Systematics.pdf", FigsDir, arm.Data(), particle.Data() ) );
	cFinal->SaveAs(Form("%s/AlCapData_Al100Dataset_%s_%s-Systematics.png", FigsDir, arm.Data(), particle.Data() ) );
}

void Combined(Double_t *unfoldingError, Double_t *pidError, Double_t *dtError, Double_t *lifetimeError, TString arm, TString particle) {
	TFile *fUnfolded = new TFile(Form("%s/unfolded.al100.root", getenv("R15b_OUT") ), "READ");
        TH1D *hUncorrected = (TH1D *)fUnfolded->Get(Form("h%s_%s", particle.Data(), arm.Data() ) );
	TFile *fOutput = new TFile("al100-systematics.root", "UPDATE");

	TH1D *hSystematics = (TH1D *) hUncorrected->Clone();
//	TH1D *hUnfoldSys = (TH1D *) hUncorrected->Clone();
	hSystematics->SetName(Form("h%s_%s_al100", particle.Data(), arm.Data() ) );
	for(int i=8; i<=32; ++i) { //instead of 8, 50
		Double_t centralValue = hUncorrected->GetBinContent(i);
		Double_t withCombinedCutSystematics = hUncorrected->GetBinError(i) + centralValue * TMath::Sqrt(TMath::Power(pidError[i], 2) + TMath::Power(dtError[i], 2) + TMath::Power(lifetimeError[i], 2) + 2*(pidError[i]*dtError[i] + pidError[i]*lifetimeError[i]+ dtError[i]*lifetimeError[i]) );
		hSystematics->SetBinError(i, withCombinedCutSystematics); //with systematic uncertainties
//		hUnfoldSys->SetBinError(i, withCombinedCutSystematics + centralValue * unfoldingError[i]);
	}

	TLegend *legend = new TLegend(.640, .598, .852, .868);
	legend->SetHeader(Form("#bf{AlCap} #it{Al50} %s", arm.Data() ) );
//	legend->AddEntry(hUnfoldSys, "Unfolding", "F");
	legend->AddEntry(hSystematics, "Systematics", "F");
	legend->AddEntry(hUncorrected, "Statistical", "F");

	TCanvas *system = new TCanvas("system", "system");
	system->SetGridx();
	system->SetGridy();
//	hUnfoldSys->Draw("E3");
//	hUnfoldSys->GetXaxis()->SetTitle("E[MeV]");
//	hUnfoldSys->GetYaxis()->SetTitle(Form("%ss per captured muon per 0.5 MeV", particle.Data() )  );
//	hUnfoldSys->GetYaxis()->SetMaxDigits(3);
//	hUnfoldSys->SetFillColor(kOrange);
	hSystematics->Draw("E3");
	hSystematics->SetFillColor(kYellow);
	hUncorrected->Draw("E3 SAME");
	hUncorrected->SetFillColor(kGreen);
	legend->Draw("SAME");
	system->Draw();
	system->SaveAs(Form("%s/AlCapData_Al100Dataset_%s_%s-Systematics-Combined.pdf", getenv("R15b_OUT"), arm.Data(), particle.Data() ) );
	system->SaveAs(Form("%s/AlCapData_Al100Dataset_%s_%s-Systematics-Combined.png", getenv("R15b_OUT"), arm.Data(), particle.Data() ) );
	fOutput->Write();
}

void Systematics100(TString arm = "SiR", TString particle = "proton") {
	std::cout << "Processing al100 " << particle << " " << arm << std::endl;
	gStyle->SetOptStat(0);
	gStyle->SetStatY(0.9);                
	gStyle->SetStatX(0.9);

	const int nbins = 50;
	Double_t dtError[nbins] = {0}; //0 -> 25MeV with 500keV bins
	Double_t lifetimeError[nbins] = {0};
	Double_t pidError[nbins] = {0};
	Double_t unfoldingError[nbins] = {0};

	TFile *fData = new TFile(Form("%s/al100.root", getenv("R15b_DATA") ), "READ");
	TTree *tree = (TTree *)fData->Get("tree");

	lifetime(tree, lifetimeError, arm, particle);
	dt(tree, dtError, arm, particle);
	Pid(tree, pidError, unfoldingError, arm, particle);
	Finally(unfoldingError, pidError, dtError, lifetimeError, arm, particle);
	Combined(unfoldingError, pidError, dtError, lifetimeError, arm, particle);

	delete tree;
}
