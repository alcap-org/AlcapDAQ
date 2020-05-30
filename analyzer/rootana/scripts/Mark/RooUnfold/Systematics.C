#if !(defined(__CINT__) || defined(__CLING__)) || defined(__ACLIC__)
#include <iostream>
#include "TRandom.h"
#include "TH1D.h"
#include "RooUnfoldResponse.h"
#include "RooUnfoldBayes.h"
#include "RooUnfoldSvd.h"
#include "RooUnfoldTUnfold.h"
#include "RooUnfoldBinByBin.h"
#include "RooUnfoldInvert.h"
#endif

void UnfoldingUncertainties(TH1D *hMeas, TString arm, TString particle, Double_t *unfoldingError) {
	TFile *responseMatrixFile = new TFile(Form("%s/transfer.sf1.02.al50.%s.root", getenv("R15b_TM"), particle.Data() ), "READ");
	RooUnfoldResponse *L_TM = (RooUnfoldResponse *)responseMatrixFile->Get("SiL500_TM");
	RooUnfoldResponse *R_TM = (RooUnfoldResponse *)responseMatrixFile->Get("SiR500_TM");
	RooUnfoldResponse *response = L_TM;
	if(arm.CompareTo("SiR") == 0) {
		response = R_TM;
	}

	TCanvas *iterCanvas = new TCanvas("iterCanvas", "iterCanvas");
	const Int_t iterLimit = 100;
	Double_t average[iterLimit] = {0};
	Double_t it[iterLimit] = {0};
	for (int iter=1; iter < iterLimit; iter++) {
		TH1D *hReco = 0;
		TH1D *hRecoError = new TH1D(Form("hRecoError_%d", iter), Form("RecoError_%d", iter), 50, 0, 25);
		RooUnfoldBayes unfold(response, hMeas, iter);
		hReco = (TH1D*) unfold.Hreco();
		TMatrixD error = unfold.Ereco(RooUnfold::ErrorTreatment::kCovariance); //cov matrix
		TMatrixD error_inv = error.Invert();
		for(int i=8; i < 40; ++i) {
			if(!TMath::IsNaN(error(i, i) ) )
				//average[iter] += TMath::Sqrt(1 - 1/(1E-11 * error(i, i) * error_inv(i, i) ) );
				average[iter] += TMath::Sqrt(error(i, i) * error_inv(i, i) );
		}
		average[iter] = average[iter]/32;
		it[iter] = iter;
//		for(int i=0; i < error.GetNoElements(); ++i) {
//			if(hReco->GetBinContent(i) != 0) {
//				hRecoError->SetBinContent(i, error[i] / hReco->GetBinContent(i) );
//				hRecoErhRecoError->SetBinError(i, 0);
//			}
//		}
	}

	for(int i=1; i < iterLimit; ++i) {
		std::cout << i << " " << average[i] << std::endl;
	}
	TGraph *g = new TGraph(100, it, average);
	g->Draw("ALPP");
	iterCanvas->Draw();
	iterCanvas->SaveAs("itertest.root");

//	TCanvas *test = new TCanvas("test", "test");
//	for (int iter=1; iter < 10; iter++) {
//		TH1D *hReco = new TH1D(Form("hReco_%d", iter), Form("Reco_%d", iter), 50, 0, 25);
//		RooUnfoldBayes unfold(response, hMeas, iter);
//		hReco = (TH1D*) unfold.Hreco();
//		hReco->SetMarkerStyle(iter+20);
//		if(iter ==1) hReco->Draw();
//		if(iter !=1) hReco->Draw("SAME");
//	}
//	test->BuildLegend();
//	test->Draw();


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

TH1D * Process(TH1D *hMeas, TString arm, TString particle, TString cutDescription, TString algorithm = "Bayes") {
	TFile *responseMatrixFile = new TFile(Form("%s/transfer.sf1.02.al50.%s.root", getenv("R15b_TM"), particle.Data() ), "READ");
	RooUnfoldResponse *L_TM = (RooUnfoldResponse *)responseMatrixFile->Get("SiL500_TM");
	RooUnfoldResponse *R_TM = (RooUnfoldResponse *)responseMatrixFile->Get("SiR500_TM");
	RooUnfoldResponse *response = L_TM;
	if(arm.CompareTo("SiR") == 0) {
		response = R_TM;
	}
	TH1D *hReco = 0;
	if(algorithm.CompareTo("Bayes") == 0) {
		RooUnfoldBayes unfold(response, hMeas, 1); //unbiased but incomplete regularisation
		hReco = (TH1D*) unfold.Hreco();
	} else if(algorithm.CompareTo("SVD") == 0 ) {
		RooUnfoldSvd unfold(response, hMeas); //regularized decomposition
		hReco = (TH1D*) unfold.Hreco();
	} else if (algorithm.CompareTo("TUnfold") == 0) {
		RooUnfoldTUnfold unfold(response, hMeas); //regularised unfolding
		hReco = (TH1D*) unfold.Hreco();
	} else if(algorithm.CompareTo("BinByBin") == 0 ) {
		RooUnfoldBinByBin unfold(response, hMeas); //bin by bin
		hReco = (TH1D*) unfold.Hreco();
	} else if(algorithm.CompareTo("Invert") == 0 ) {
		RooUnfoldInvert unfold(response, hMeas); //rmatrix inversion
		hReco = (TH1D*) unfold.Hreco();
	}
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
		if(abs(t2-t1-13) > 23 * 3) continue;
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

	TH1D *hOneUnf = Process(hOne, arm, particle, "400ns");
	TH1D *hTwoUnf = Process(hTwo, arm, particle, "500ns");
	TH1D *hThreeUnf = Process(hThree, arm, particle, "600ns");
	TH1D *hFourUnf = Process(hFour, arm, particle, "700ns");

	std::cout << "lifetime" << std::endl;
	std::cout << "time\t|" << "400\t|" << "500\t|" << "600\t|" << "700" << std::endl;
	for(int ii=1; ii<=nbins; ++ii) { //ignore under and overflow bins
		std::cout << (ii-1)*500 << "\t|" << hOneUnf->GetBinContent(ii) << "\t" << hTwoUnf->GetBinContent(ii) << "\t|" <<  hThreeUnf->GetBinContent(ii) << "\t|" << hFourUnf->GetBinContent(ii) << std::endl;
	}
	std::cout << std::endl;
	std::cout << "lifetime Corrected" << std::endl;
	std::cout << "time\t|" << "400\t|" << "500\t|" << "600\t|" << "700" << std::endl;
	for(int j=1; j<=nbins; ++j) { //ignore under and overflow bins
		std::cout << (j-1)*500 << "\t|" << hOneUnf->GetBinContent(j)/0.63 << "\t|" << hTwoUnf->GetBinContent(j)/0.56 << "\t|" <<  hThreeUnf->GetBinContent(j)/0.50 << "\t|" << hFourUnf->GetBinContent(j)/0.45 << std::endl;
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
				if(abs(t2-t1 - 8.19758) <   15.1644) h1->Fill(E);
				if(abs(t2-t1 - 8.19758) < 2*15.1644) h2->Fill(E);
				if(abs(t2-t1 - 8.19758) < 3*15.1644) h3->Fill(E);
				if(abs(t2-t1 - 8.19758) < 4*15.1644) h4->Fill(E);
			}
			const int c = 10;
			double energy[c] = {  3.500,   4.500,   5.500,   6.500,   7.500,   8.500,   9.500, 10.500,  11.500, 12.500};
			double mean[c]   = {12.0452, 13.9437, 14.8337, 15.4478, 15.5178, 14.9985, 11.9798, 12.544, 21.9398, 21.0489};
			double sigma[c]  = {14.6326, 17.2193, 19.9066, 23.1122, 25.6022, 28.4722, 30.5675, 34.3171, 51.4912, 50.2579};
			for(int i=0; i < c; ++i) {
				if(abs(E-energy[i])<.5) {
					if(abs(t2-t1 - mean[i]) < sigma[i]) h1->Fill(E);
					if(abs(t2-t1 - mean[i]) < 2*sigma[i]) h2->Fill(E);
					if(abs(t2-t1 - mean[i]) < 3*sigma[i]) h3->Fill(E);
					if(abs(t2-t1 - mean[i]) < 4*sigma[i]) h4->Fill(E);
				}
			}
			if(E>13.000) {
				if(abs(t2-t1 - 34.3982) <   42.8872) h1->Fill(E);
				if(abs(t2-t1 - 34.3982) < 2*42.8872) h2->Fill(E);
				if(abs(t2-t1 - 34.3982) < 3*42.8872) h3->Fill(E);
				if(abs(t2-t1 - 34.3982) < 4*42.8872) h4->Fill(E);
			}
		} else { //SiL
			if(E<3.000) {
				if(abs(t2-t1 - 8.77) < 20.62) h1->Fill(E);
				if(abs(t2-t1 - 8.77) < 2*20.62) h2->Fill(E);
				if(abs(t2-t1 - 8.77) < 3*20.62) h3->Fill(E);
				if(abs(t2-t1 - 8.77) < 4*20.62) h4->Fill(E);
			}
			const int c = 10;
			double energy[c] = {3.500, 4.500, 5.500, 6.500, 7.500,       8.500,       9.500,      10.500,      11.500,      12.500};
			double mean[c]   = {11.13, 12.78, 13.85, 12.31, 10.51, 9.71093e+00, 1.32628e+01, 1.60727e+01, 1.82259e+01, 2.03972e+01};
			double sigma[c]  = {20.78, 19.71, 21.61, 22.63, 24.99, 2.38256e+01, 2.62970e+01, 2.68189e+01, 2.83577e+01, 3.00732e+01};
			for(int i=0; i < c; ++i) {
				if(abs(E-energy[i])<.5) {
					if(abs(t2-t1 - mean[i]) < sigma[i]) h1->Fill(E);
					if(abs(t2-t1 - mean[i]) < 2*sigma[i]) h2->Fill(E);
					if(abs(t2-t1 - mean[i]) < 3*sigma[i]) h3->Fill(E);
					if(abs(t2-t1 - mean[i]) < 4*sigma[i]) h4->Fill(E);
				}
			}
			if(E>13.000) {
				if(abs(t2-t1 - 2.78784e+01) <   3.05465e+01) h1->Fill(E);
				if(abs(t2-t1 - 2.78784e+01) < 2*3.05465e+01) h2->Fill(E);
				if(abs(t2-t1 - 2.78784e+01) < 3*3.05465e+01) h3->Fill(E);
				if(abs(t2-t1 - 2.78784e+01) < 4*3.05465e+01) h4->Fill(E);
			}
		}
	} else if(particle.CompareTo("deuteron") == 0) { //deuteron
		if(arm.CompareTo("SiR") == 0) {
			const int c = 10;
			double energy[c] = {3.500      ,       4.500,       5.500,       6.500,       7.500,       8.500,       9.500,      10.500,      11.500, 12.500};
			double mean[c]   = {1.08344e+01, 1.42647e+01, 1.35716e+01, 1.37404e+01, 1.30509e+01, 1.31261e+01, 1.04033e+01, 9.61924e+00, 8.11634e+00, 7.23814e+00};
			double sigma[c]  = {1.16602e+01, 1.14147e+01, 1.24299e+01, 1.44599e+01, 1.60661e+01, 1.64692e+01, 1.83440e+01, 1.99810e+01, 1.93172e+01, 2.36401e+01};
			if(E<3.0) {
				if(abs(t2-t1 - 8.14451e+00) <   1.82157e+01) h1->Fill(E);
				if(abs(t2-t1 - 8.14451e+00) < 2*1.82157e+01) h2->Fill(E);
				if(abs(t2-t1 - 8.14451e+00) < 3*1.82157e+01) h3->Fill(E);
				if(abs(t2-t1 - 8.14451e+00) < 4*1.82157e+01) h4->Fill(E);
			}
			for(int i=0; i < c; ++i) {
				if(abs(E-energy[i])<.5) {
					if(abs(t2-t1 - mean[i]) <   sigma[i]) h1->Fill(E);
					if(abs(t2-t1 - mean[i]) < 2*sigma[i]) h2->Fill(E);
					if(abs(t2-t1 - mean[i]) < 3*sigma[i]) h3->Fill(E);
					if(abs(t2-t1 - mean[i]) < 4*sigma[i]) h4->Fill(E);
				}
			}
			if(E>13.000) {
				if(abs(t2-t1 - 1.64564e+01) <   2.96719e+01) h1->Fill(E);
				if(abs(t2-t1 - 1.64564e+01) < 2*2.96719e+01) h2->Fill(E);
				if(abs(t2-t1 - 1.64564e+01) < 3*2.96719e+01) h3->Fill(E);
				if(abs(t2-t1 - 1.64564e+01) < 4*2.96719e+01) h4->Fill(E);
			}
		} else { //SiL
			const int c = 10;
			double energy[c] = {3.500      ,       4.500,       5.500,       6.500,       7.500,       8.500,       9.500,      10.500,      11.500, 12.500};
			double mean[c]   = {1.15352e+01, 1.32744e+01, 1.49181e+01, 1.37824e+01, 1.44089e+01, 1.30099e+01, 1.08938e+01, 8.62374e+00, 7.78350e+00, 8.27474e+00};
			double sigma[c]  = {1.89506e+01, 1.82267e+01, 1.92237e+01, 2.05172e+01, 1.97364e+01, 2.10444e+01, 2.23440e+01, 2.00658e+01, 2.18556e+01, 2.42436e+01};
			if(E<3.0) {
				if(abs(t2-t1 - 5.72919e+00 ) <   2.47974e+01) h1->Fill(E);
				if(abs(t2-t1 - 5.72919e+00 ) < 2*2.47974e+01) h2->Fill(E);
				if(abs(t2-t1 - 5.72919e+00 ) < 3*2.47974e+01) h3->Fill(E);
				if(abs(t2-t1 - 5.72919e+00 ) < 4*2.47974e+01) h4->Fill(E);
			}
			for(int i=0; i < c; ++i) {
				if(abs(E-energy[i])<.5) {
					if(abs(t2-t1 - mean[i]) <   sigma[i]) h1->Fill(E);
					if(abs(t2-t1 - mean[i]) < 2*sigma[i]) h2->Fill(E);
					if(abs(t2-t1 - mean[i]) < 3*sigma[i]) h3->Fill(E);
					if(abs(t2-t1 - mean[i]) < 4*sigma[i]) h4->Fill(E);
				}
			}
			if(E>13.000) {
				if(abs(t2-t1 - 1.53973e+01) <   2.63256e+01) h1->Fill(E);
				if(abs(t2-t1 - 1.53973e+01) < 2*2.63256e+01) h2->Fill(E);
				if(abs(t2-t1 - 1.53973e+01) < 3*2.63256e+01) h3->Fill(E);
				if(abs(t2-t1 - 1.53973e+01) < 4*2.63256e+01) h4->Fill(E);
			}
		}
	} else if(particle.CompareTo("triton") == 0) {
		if(arm.CompareTo("SiR") == 0) {
			const int c = 7;
			double energy[c] = {5.         , 7.         , 9.         , 11.        , 13.        , 15.        , 17.        };
			double mean[c]   = {1.43615e+01, 1.41943e+01, 1.12490e+01, 9.18983e+00, 8.85546e+00, 4.39997e+00, 4.81629e+00};
			double sigma[c]  = {1.03597e+01, 1.11212e+01, 1.38507e+01, 1.65536e+01, 1.93905e+01, 2.58679e+01, 2.34997e+01};
			if(E<4.0) {
				if(abs(t2-t1 - 9.47768e+00 ) <   1.44954e+01) h1->Fill(E);
				if(abs(t2-t1 - 9.47768e+00 ) < 2*1.44954e+01) h2->Fill(E);
				if(abs(t2-t1 - 9.47768e+00 ) < 3*1.44954e+01) h3->Fill(E);
				if(abs(t2-t1 - 9.47768e+00 ) < 4*1.44954e+01) h4->Fill(E);
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
				if(abs(t2-t1 + 2.85782e+00 ) <   4.20486e+01) h1->Fill(E);
				if(abs(t2-t1 + 2.85782e+00 ) < 2*4.20486e+01) h2->Fill(E);
				if(abs(t2-t1 + 2.85782e+00 ) < 3*4.20486e+01) h3->Fill(E);
				if(abs(t2-t1 + 2.85782e+00 ) < 4*4.20486e+01) h4->Fill(E);
			}
		} else {
			const int c = 7;
			double energy[c] = {5.         , 7.         , 9.         , 11.        , 13.        , 15.        , 17.        };
			double mean[c]   = {1.34600e+01, 1.48010e+01, 1.52866e+01, 7.37621e+00, 5.18502e+00, 4.36898e+00, 1.42361e+01};
			double sigma[c]  = {1.89357e+01, 1.80846e+01, 1.92937e+01, 1.99088e+01, 2.21445e+01, 2.44698e+01, 3.29257e+01};
			if(E<4.0) {
				if(abs(t2-t1 - 1.01630e+01 ) <   2.07438e+01) h1->Fill(E);
				if(abs(t2-t1 - 1.01630e+01 ) < 2*2.07438e+01) h2->Fill(E);
				if(abs(t2-t1 - 1.01630e+01 ) < 3*2.07438e+01) h3->Fill(E);
				if(abs(t2-t1 - 1.01630e+01 ) < 4*2.07438e+01) h4->Fill(E);
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
				if(abs(t2-t1 - 1.08323e+01 ) <   3.10974e+01) h1->Fill(E);
				if(abs(t2-t1 - 1.08323e+01 ) < 2*3.10974e+01) h2->Fill(E);
				if(abs(t2-t1 - 1.08323e+01 ) < 3*3.10974e+01) h3->Fill(E);
				if(abs(t2-t1 - 1.08323e+01 ) < 4*3.10974e+01) h4->Fill(E);
			}
		}
	} else if(particle.CompareTo("alpha") == 0) {
		if(arm.CompareTo("SiR") == 0) {
			const int c = 4;
			double energy[c] = {11.        , 13.        , 15.        , 17.        };
			double mean[c]   = {2.33144e+01, 2.20304e+01, 1.63956e+01, 1.23506e+01};
			double sigma[c]  = {4.58768e+00, 5.85964e+00, 3.89140e+00, 6.19020e+00};
			if(E<10.0) {
				if(abs(t2-t1 - 2.37704e+01 ) <   6.34680e+00) h1->Fill(E);
				if(abs(t2-t1 - 2.37704e+01 ) < 2*6.34680e+00) h2->Fill(E);
				if(abs(t2-t1 - 2.37704e+01 ) < 3*6.34680e+00) h3->Fill(E);
				if(abs(t2-t1 - 2.37704e+01 ) < 4*6.34680e+00) h4->Fill(E);
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
				if(abs(t2-t1 - 5.85122e+00 ) <   1.59851e+01) h1->Fill(E);
				if(abs(t2-t1 - 5.85122e+00 ) < 2*1.59851e+01) h2->Fill(E);
				if(abs(t2-t1 - 5.85122e+00 ) < 3*1.59851e+01) h3->Fill(E);
				if(abs(t2-t1 - 5.85122e+00 ) < 4*1.59851e+01) h4->Fill(E);
			}
		} else {
			const int c = 4;
			double energy[c] = {11.        , 13.        , 15.        , 17.        };
			double mean[c]   = {1.72773e+01, 9.35924e+00, 4.71938e+00, 1.11704e+01};
			double sigma[c]  = {1.98657e+01, 2.58570e+01, 3.03777e+01, 2.99396e+01};
			if(E<10.0) {
				if(abs(t2-t1 - 2.01136e+01) <   1.98342e+01) h1->Fill(E);
				if(abs(t2-t1 - 2.01136e+01) < 2*1.98342e+01) h2->Fill(E);
				if(abs(t2-t1 - 2.01136e+01) < 3*1.98342e+01) h3->Fill(E);
				if(abs(t2-t1 - 2.01136e+01) < 4*1.98342e+01) h4->Fill(E);
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
				if(abs(t2-t1 - 1.62286e+01 ) <   1.87028e+01) h1->Fill(E);
				if(abs(t2-t1 - 1.62286e+01 ) < 2*1.87028e+01) h2->Fill(E);
				if(abs(t2-t1 - 1.62286e+01 ) < 3*1.87028e+01) h3->Fill(E);
				if(abs(t2-t1 - 1.62286e+01 ) < 4*1.87028e+01) h4->Fill(E);
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
		if(t2<500) continue;
		if(abs(t2)>10e3) continue;
		if(TMath::IsNaN(e3) )  {
			if(!sig3->Contains(particle) ) continue;
			if(arm.CompareTo("SiR") == 0 ) {
				if(a2 > 3986) continue; //remove saturation
				if(!channel->Contains("SiR") ) continue;
			} else {
				if(a2 > 3982) continue; //remove saturation
				if(!channel->Contains("SiL") ) continue;
			}
			dtFill(t2, t1, e1+e2, hOne, hTwo, hThree, hFour, arm, particle);
		} else {
			if(!pt3->Contains(particle) ) continue;
			dtFill(t2, t1, e1+e2+e3, hOne3, hTwo3, hThree3, hFour3, arm, particle);
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
		if(timeToPrevTME < 10e3 || timeToNextTME < 10e3) continue;
		if(t2<400) continue;
		if(abs(t2)>10e3) continue;
		if(abs(t2-t1-13) > 23 * 4 ) continue;
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
			pidError[k] = 1- ((hOneUnf->GetBinContent(k)/0.682) / (hThreeUnf->GetBinContent(k)/0.996) );
		} else {
			pidError[k] = 0;
		}
	}
}

void Finally(Double_t *unfoldingError, Double_t *pidError, Double_t *dtError, Double_t *lifetimeError, TString arm, TString particle) {
	TString target = "al50";
	Int_t nbins = 50;
	//where does this error come from?
	TH1D *hPid = new TH1D("hPid", "PID;E [keV]", nbins, 0, 25); hPid->SetFillColor(kRed);
	TH1D *hDt = new TH1D("hDt", "t_{2}-t_{1};E [keV]", nbins, 0, 25); hDt->SetFillColor(kGreen);
	TH1D *hLifetime = new TH1D("hLifetime", "#tau;E [keV]", nbins, 0, 25); hLifetime->SetFillColor(kBlue);
//	TH1D *hUnfolding = new TH1D("hUnfolding", "Unfolding;E [keV]", nbins, 0, 25); hUnfolding->SetFillColor(kOrange);
	int drawlimit = 40;
	if(arm.CompareTo("SiL") == 0) drawlimit = 32;
	for(int i=8; i<=drawlimit; ++i) {
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
	legend->SetHeader(Form("#bf{AlCap} #it{Al50} %s Systematics", arm.Data() ) );
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
	cFinal->SaveAs(Form("%s/AlCapData_Al50Dataset_%s_%s-Systematics.pdf", FigsDir, arm.Data(), particle.Data() ) );
	cFinal->SaveAs(Form("%s/AlCapData_Al50Dataset_%s_%s-Systematics.png", FigsDir, arm.Data(), particle.Data() ) );
}

void Combined(Double_t *unfoldingError, Double_t *pidError, Double_t *dtError, Double_t *lifetimeError, TString arm, TString particle) {
	TFile *fUnfolded = new TFile(Form("%s/unfolded.al50.root", getenv("R15b_OUT") ), "READ");
        TH1D *hUncorrected = (TH1D *)fUnfolded->Get(Form("h%s_%s", particle.Data(), arm.Data() ) );
	TFile *fOutput = new TFile("al50-systematics.root", "UPDATE");

	TH1D *hSystematics = (TH1D *) hUncorrected->Clone();
//	TH1D *hUnfoldSys = (TH1D *) hUncorrected->Clone();
	hSystematics->SetName(Form("h%s_%s_al50", particle.Data(), arm.Data() ) );
	int drawlimit = 40;
	if(arm.CompareTo("SiL") == 0) drawlimit = 32;
	for(int i=8; i<=drawlimit; ++i) {
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
	system->SaveAs(Form("%s/AlCapData_Al50Dataset_%s_%s-Systematics-Combined.pdf", getenv("R15b_OUT"), arm.Data(), particle.Data() ) );
	system->SaveAs(Form("%s/AlCapData_Al50Dataset_%s_%s-Systematics-Combined.png", getenv("R15b_OUT"), arm.Data(), particle.Data() ) );
	fOutput->Write();
}

void Systematics(TString arm = "SiR", TString particle = "proton") {
	std::cout << "Processing al50 " << particle << " " << arm << std::endl;
	gStyle->SetOptStat(0);
	gStyle->SetStatY(0.9);                
	gStyle->SetStatX(0.9);

	const int nbins = 50;
	Double_t dtError[nbins] = {0}; //0 -> 25MeV with 500keV bins
	Double_t lifetimeError[nbins] = {0};
	Double_t pidError[nbins] = {0};
	Double_t unfoldingError[nbins] = {0};

	TFile *fData = new TFile(Form("%s/al50.root", getenv("R15b_DATA") ), "READ");
	TTree *tree = (TTree *)fData->Get("tree");

	lifetime(tree, lifetimeError, arm, particle);
	dt(tree, dtError, arm, particle);
	Pid(tree, pidError, unfoldingError, arm, particle);
	Finally(unfoldingError, pidError, dtError, lifetimeError, arm, particle);
	Combined(unfoldingError, pidError, dtError, lifetimeError, arm, particle);

	delete tree;
}
