#if !(defined(__CINT__) || defined(__CLING__)) || defined(__ACLIC__)
#include <iostream>
#include "TRandom.h"
#include "TH1D.h"
#include "RooUnfoldResponse.h"
#include "RooUnfoldBayes.h"
#endif

void UnfoldingUncertainties(TH1D *hMeas, TString arm, TString particle, Double_t *unfoldingError) {
	TFile *responseMatrixFile = new TFile(Form("%s/transfer.sf1.03.ti50.%s.root", getenv("R15b_TM"), particle.Data() ), "READ");
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
	TFile *responseMatrixFile = new TFile(Form("%s/transfer.sf1.03.ti50.%s.root", getenv("R15b_TM"), particle.Data() ), "READ");
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
		//if(abs(t2-t1-13) > 23 * 3) continue;
		if(abs(t2-t1) > 500) continue;
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
			if(t2 > 100) hOne3->Fill(e1+e2+e3);
			if(t2 > 200) hTwo3->Fill(e1+e2+e3);
			if(t2 > 300) hThree3->Fill(e1+e2+e3);
			if(t2 > 400) hFour3->Fill(e1+e2+e3);
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

	TH1D *hOneUnf = Process(hOne, arm, particle, "100ns");
	TH1D *hTwoUnf = Process(hTwo, arm, particle, "200ns");
	TH1D *hThreeUnf = Process(hThree, arm, particle, "300ns");
	TH1D *hFourUnf = Process(hFour, arm, particle, "400ns");

	std::cout << "lifetime" << std::endl;
	std::cout << "time\t|" << "100\t|" << "200\t|" << "300\t|" << "400" << std::endl;
	for(int ii=1; ii<=nbins; ++ii) { //ignore under and overflow bins
		std::cout << (ii-1)*500 << "\t|" << hOneUnf->GetBinContent(ii) << "\t" << hTwoUnf->GetBinContent(ii) << "\t|" <<  hThreeUnf->GetBinContent(ii) << "\t|" << hFourUnf->GetBinContent(ii) << std::endl;
	}
	std::cout << std::endl;
	std::cout << "lifetime Corrected" << std::endl;
	std::cout << "time\t|" << "100\t|" << "200\t|" << "300\t|" << "400" << std::endl;
	for(int j=1; j<=nbins; ++j) { //ignore under and overflow bins
		std::cout << (j-1)*500 << "\t|" << hOneUnf->GetBinContent(j)/0.738 << "\t|" << hTwoUnf->GetBinContent(j)/0.545 << "\t|" <<  hThreeUnf->GetBinContent(j)/0.402 << "\t|" << hFourUnf->GetBinContent(j)/0.2968 << std::endl;
		//elog:294
		if(hOneUnf->GetBinContent(j) != 0) {
			lifetimeError[j] = 1- ((hFourUnf->GetBinContent(j)/0.2968) / (hOneUnf->GetBinContent(j)/0.738) );
		} else {
			lifetimeError[j] = 0;
		}
	}
}
void dtFill(Double_t t2, Double_t t1, Double_t E, TH1D *h1, TH1D *h2, TH1D *h3, TH1D *h4, TString arm, TString particle) {
	if(particle.CompareTo("proton") == 0 ) {
		if(arm.CompareTo("SiR") == 0) {
			if(E<3.000) {
				if(abs(t2-t1 - 1.65001e+01) <   2.05447e+01) h1->Fill(E);
				if(abs(t2-t1 - 1.65001e+01) < 2*2.05447e+01) h2->Fill(E);
				if(abs(t2-t1 - 1.65001e+01) < 3*2.05447e+01) h3->Fill(E);
				if(abs(t2-t1 - 1.65001e+01) < 4*2.05447e+01) h4->Fill(E);
			}
			const int c = 10;
			double energy[c] = {3.500,             4.500,       5.500,       6.500,       7.500,       8.500,       9.500,      10.500,      11.500, 12.500};
			double mean[c]   = {2.05609e+01, 2.31718e+01, 2.24811e+01, 2.26411e+01, 2.24434e+01, 2.08878e+01, 2.27497e+01, 2.38565e+01, 2.08228e+01, 2.66223e+01};
			double sigma[c]  = {1.86846e+01, 2.11462e+01, 2.24853e+01, 2.26411e+01, 2.88914e+01, 3.21257e+01, 3.21980e+01, 3.94493e+01, 4.12679e+01, 4.11926e+01};
			for(int i=0; i < c; ++i) {
				if(abs(E-energy[i])<.5) {
					if(abs(t2-t1 - mean[i]) < sigma[i]) h1->Fill(E);
					if(abs(t2-t1 - mean[i]) < 2*sigma[i]) h2->Fill(E);
					if(abs(t2-t1 - mean[i]) < 3*sigma[i]) h3->Fill(E);
					if(abs(t2-t1 - mean[i]) < 4*sigma[i]) h4->Fill(E);
				}
			}
			if(E>13.000) {
				if(abs(t2-t1 - 4.24038e+01) <   4.24625e+01) h1->Fill(E);
				if(abs(t2-t1 - 4.24038e+01) < 2*4.24625e+01) h2->Fill(E);
				if(abs(t2-t1 - 4.24038e+01) < 3*4.24625e+01) h3->Fill(E);
				if(abs(t2-t1 - 4.24038e+01) < 4*4.24625e+01) h4->Fill(E);
			}
		} else { //SiL
			if(E<3.000) {
				if(abs(t2-t1 - 2.25926e+01) <   2.67126e+01) h1->Fill(E);
				if(abs(t2-t1 - 2.25926e+01) < 2*2.67126e+01) h2->Fill(E);
				if(abs(t2-t1 - 2.25926e+01) < 3*2.67126e+01) h3->Fill(E);
				if(abs(t2-t1 - 2.25926e+01) < 4*2.67126e+01) h4->Fill(E);
			}
			const int c = 10;
			double energy[c] = {3.500,             4.500,       5.500,       6.500,       7.500,       8.500,       9.500,      10.500,      11.500, 12.500};
			double mean[c]   = {2.51086e+01, 2.54609e+01, 2.66409e+01, 2.41816e+01, 2.34213e+01, 2.39821e+01, 2.48659e+01, 2.48806e+01, 3.17553e+01, 3.11810e+01};
			double sigma[c]  = {2.52332e+01, 2.55426e+01, 2.65868e+01, 2.64862e+01, 2.63521e+01, 3.00564e+01, 3.13697e+01, 3.58416e+01, 3.47800e+01, 3.99247e+01};
			for(int i=0; i < c; ++i) {
				if(abs(E-energy[i])<.5) {
					if(abs(t2-t1 - mean[i]) < sigma[i]) h1->Fill(E);
					if(abs(t2-t1 - mean[i]) < 2*sigma[i]) h2->Fill(E);
					if(abs(t2-t1 - mean[i]) < 3*sigma[i]) h3->Fill(E);
					if(abs(t2-t1 - mean[i]) < 4*sigma[i]) h4->Fill(E);
				}
			}
			if(E>13.000) {
				if(abs(t2-t1 - 4.16573e+01) <   4.65905e+01) h1->Fill(E);
				if(abs(t2-t1 - 4.16573e+01) < 2*4.65905e+01) h2->Fill(E);
				if(abs(t2-t1 - 4.16573e+01) < 3*4.65905e+01) h3->Fill(E);
				if(abs(t2-t1 - 4.16573e+01) < 4*4.65905e+01) h4->Fill(E);
			}
		}
	} else if(particle.CompareTo("deuteron") == 0) { //deuteron
		if(arm.CompareTo("SiR") == 0) {
			const int c = 10;
			double energy[c] = {3.500,             4.500,       5.500,       6.500,       7.500,       8.500,       9.500,      10.500,      11.500, 12.500};
			double mean[c]   = {1.83581e+01, 2.15843e+01, 2.27206e+01, 2.05585e+01, 2.12236e+01, 2.02963e+01, 1.82605e+01, 1.78672e+01, 1.52147e+01, 1.22812e+01};
			double sigma[c]  = {1.77465e+01, 1.71013e+01, 1.76720e+01, 1.87732e+01, 1.91656e+01, 2.21686e+01, 2.46626e+01, 2.28983e+01, 2.77650e+01, 2.64014e+01};
			for(int i=0; i < c; ++i) {
				if(abs(E-energy[i])<.5) {
					if(abs(t2-t1 - mean[i]) < sigma[i]) h1->Fill(E);
					if(abs(t2-t1 - mean[i]) < 2*sigma[i]) h2->Fill(E);
					if(abs(t2-t1 - mean[i]) < 3*sigma[i]) h3->Fill(E);
					if(abs(t2-t1 - mean[i]) < 4*sigma[i]) h4->Fill(E);
				}
			}
			if(E>13.000) {
				if(abs(t2-t1 - 2.36543e+01) <   3.19809e+01) h1->Fill(E);
				if(abs(t2-t1 - 2.36543e+01) < 2*3.19809e+01) h2->Fill(E);
				if(abs(t2-t1 - 2.36543e+01) < 3*3.19809e+01) h3->Fill(E);
				if(abs(t2-t1 - 2.36543e+01) < 4*3.19809e+01) h4->Fill(E);
			}
		} else { //SiL
			const int c = 10;
			double energy[c] = {3.500,             4.500,       5.500,       6.500,       7.500,       8.500,       9.500,      10.500,      11.500, 12.500};
			double mean[c]   = {2.71494e+01, 2.57243e+01, 2.72705e+01, 2.38837e+01, 2.27320e+01, 2.38872e+01, 2.29913e+01, 1.97209e+01, 1.84823e+01, 2.30038e+01};
			double sigma[c]  = {2.83634e+01, 3.07353e+01, 2.57226e+01, 2.52980e+01, 2.85097e+01, 2.41038e+01, 2.41038e+01, 2.65672e+01, 2.50039e+01, 3.55600e+01};
			for(int i=0; i < c; ++i) {
				if(abs(E-energy[i])<.5) {
					if(abs(t2-t1 - mean[i]) < sigma[i]) h1->Fill(E);
					if(abs(t2-t1 - mean[i]) < 2*sigma[i]) h2->Fill(E);
					if(abs(t2-t1 - mean[i]) < 3*sigma[i]) h3->Fill(E);
					if(abs(t2-t1 - mean[i]) < 4*sigma[i]) h4->Fill(E);
				}
			}
			if(E>13.000) {
				if(abs(t2-t1 - 2.67732e+01) <   2.95573e+01) h1->Fill(E);
				if(abs(t2-t1 - 2.67732e+01) < 2*2.95573e+01) h2->Fill(E);
				if(abs(t2-t1 - 2.67732e+01) < 3*2.95573e+01) h3->Fill(E);
				if(abs(t2-t1 - 2.67732e+01) < 4*2.95573e+01) h4->Fill(E);
			}
		}
	} else if(particle.CompareTo("triton") == 0) {
		if(arm.CompareTo("SiR") == 0) {
			if(abs(t2-t1 - 11.5054) <   14.6790) h1->Fill(E);
			if(abs(t2-t1 - 11.5054) < 2*14.6790) h2->Fill(E);
			if(abs(t2-t1 - 11.5054) < 3*14.6790) h3->Fill(E);
			if(abs(t2-t1 - 11.5054) < 4*14.6790) h4->Fill(E);
		} else {
			if(abs(t2-t1 - 12.3712) <   20.4712) h1->Fill(E);
			if(abs(t2-t1 - 12.3712) < 2*20.4712) h2->Fill(E);
			if(abs(t2-t1 - 12.3712) < 3*20.4712) h3->Fill(E);
			if(abs(t2-t1 - 12.3712) < 4*20.4712) h4->Fill(E);
		}
	} else if(particle.CompareTo("alpha") == 0) {
		if(arm.CompareTo("SiR") == 0) {
			if(abs(t2-t1 - 20.0349) <   6.90189) h1->Fill(E);
			if(abs(t2-t1 - 20.0349) < 2*6.90189) h2->Fill(E);
			if(abs(t2-t1 - 20.0349) < 3*6.90189) h3->Fill(E);
			if(abs(t2-t1 - 20.0349) < 4*6.90189) h4->Fill(E);
		} else {
			if(abs(t2-t1 - 15.0798) <   18.7265) h1->Fill(E);
			if(abs(t2-t1 - 15.0798) < 2*18.7265) h2->Fill(E);
			if(abs(t2-t1 - 15.0798) < 3*18.7265) h3->Fill(E);
			if(abs(t2-t1 - 15.0798) < 4*18.7265) h4->Fill(E);
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
				if(!channel->Contains("SiR") ) continue;
			} else {
				if(a2 > 3982) continue; //remove saturation
				if(!channel->Contains("SiL") ) continue;
			}
			if(abs(t2-t1) < 300) hOne->Fill(e1+e2);
			if(abs(t2-t1) < 400) hTwo->Fill(e1+e2);
			if(abs(t2-t1) < 500) hThree->Fill(e1+e2);
			if(abs(t2-t1) < 600) hFour->Fill(e1+e2);
//			dtFill(t2, t1, e1+e2, hOne, hTwo, hThree, hFour, arm, particle);
		} else {
			if(!pt3->Contains(particle) ) continue;
			if(abs(t2-t1) < 300) hOne3->Fill(e1+e2+e3);
			if(abs(t2-t1) < 400) hTwo3->Fill(e1+e2+e3);
			if(abs(t2-t1) < 500) hThree3->Fill(e1+e2+e3);
			if(abs(t2-t1) < 600) hFour3->Fill(e1+e2+e3);
//			dtFill(t2, t1, e1+e2+e3, hOne3, hTwo3, hThree3, hFour3, arm, particle);
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
	//std::cout << "E bin\t|" << "1σ\t|" << "2σ\t|" << "3σ\t|" << "4σ" << std::endl;
	std::cout << "E bin\t|" << "300ns\t|" << "400ns\t|" << "500ns\t|" << "600ns" << std::endl;
	for(int k=1; k<=nbins; ++k) { //ignore under and overflow bins
		std::cout << (k-1)*500 << "\t|" << hOneUnf->GetBinContent(k) << "\t|" << hTwoUnf->GetBinContent(k) << "\t|" <<  hThreeUnf->GetBinContent(k) << "\t|" << hFourUnf->GetBinContent(k) << std::endl;
	}
	std::cout << std::endl;
//zero differences expected
	std::cout << "dt Corrected" << std::endl;
	//std::cout << "E bin\t|" << "1σ\t|" << "2σ\t|" << "3σ\t|" << "4σ" << std::endl;
	std::cout << "E bin\t|" << "300ns\t|" << "400ns\t|" << "500ns\t|" << "600ns" << std::endl;
	for(int j=1; j<=nbins; ++j) { //ignore under and overflow bins
		//std::cout << (j-1)*500 << "\t|" << hOneUnf->GetBinContent(j)/0.682 << "\t|" << hTwoUnf->GetBinContent(j)/0.954 << "\t|" <<  hThreeUnf->GetBinContent(j)/0.996 << "\t|" << hFourUnf->GetBinContent(j)/0.998 << std::endl;
		//.682, .954, .996
		if(hThreeUnf->GetBinContent(j) != 0) {
			//dtError[j] = 1- ((hOneUnf->GetBinContent(j)/0.682) / (hThreeUnf->GetBinContent(j)/0.996) );
			dtError[j] = 1- (hOneUnf->GetBinContent(j) / hThreeUnf->GetBinContent(j) );
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
		//if(abs(t2-t1-13) > 23 * 4 ) continue;
		if(abs(t2-t1) > 500 ) continue;
		if(TMath::IsNaN(e3) ) {
			if(arm.CompareTo("SiR") == 0) {
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
	legend->SetHeader(Form("#bf{AlCap} #it{Ti50} %s Systematics", arm.Data() ) );
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
	cFinal->SaveAs(Form("%s/AlCapData_Ti50Dataset_%s_%s-Systematics.pdf", FigsDir, arm.Data(), particle.Data() ) );
	cFinal->SaveAs(Form("%s/AlCapData_Ti50Dataset_%s_%s-Systematics.png", FigsDir, arm.Data(), particle.Data() ) );
}

void Combined(Double_t *unfoldingError, Double_t *pidError, Double_t *dtError, Double_t *lifetimeError, TString arm, TString particle) {
	TFile *fUnfolded = new TFile(Form("%s/unfolded.ti50.root", getenv("R15b_OUT") ), "READ");
        TH1D *hUncorrected = (TH1D *)fUnfolded->Get(Form("h%s_%s", particle.Data(), arm.Data() ) );
	TFile *fOutput = new TFile("ti50-systematics.root", "UPDATE");

	TH1D *hSystematics = (TH1D *) hUncorrected->Clone();
	TH1D *hUnfoldSys = (TH1D *) hUncorrected->Clone();
	hSystematics->SetName(Form("h%s_%s_ti50", particle.Data(), arm.Data() ) );
	int drawlimit = 40;
	if(arm.CompareTo("SiL") == 0) drawlimit = 32;
	for(int i=8; i<=drawlimit; ++i) {
		Double_t centralValue = hUncorrected->GetBinContent(i);
		Double_t withCombinedCutSystematics = hUncorrected->GetBinError(i) + centralValue * TMath::Sqrt(TMath::Power(pidError[i], 2) + TMath::Power(dtError[i], 2) + TMath::Power(lifetimeError[i], 2) + 2*(pidError[i]*dtError[i] + pidError[i]*lifetimeError[i]+ dtError[i]*lifetimeError[i]) );
		hSystematics->SetBinError(i, withCombinedCutSystematics); //with systematic uncertainties
		hUnfoldSys->SetBinError(i, withCombinedCutSystematics + centralValue * unfoldingError[i]);
	}

	TLegend *legend = new TLegend(.640, .598, .852, .868);
	legend->SetHeader(Form("#bf{AlCap} #it{Ti50} %s", arm.Data() ) );
	legend->AddEntry(hUnfoldSys, "Unfolding", "F");
	legend->AddEntry(hSystematics, "Systematics", "F");
	legend->AddEntry(hUncorrected, "Statistical", "F");

	TCanvas *system = new TCanvas("system", "system");
	system->SetGridx();
	system->SetGridy();
	hUnfoldSys->Draw("E3");
	hUnfoldSys->GetXaxis()->SetTitle("E[MeV]");
	hUnfoldSys->GetYaxis()->SetTitle(Form("%ss per captured muon per 0.5 MeV", particle.Data() )  );
	hUnfoldSys->GetYaxis()->SetMaxDigits(3);
	hUnfoldSys->SetFillColor(kOrange);
	hSystematics->Draw("E3 SAME");
	hSystematics->SetFillColor(kYellow);
	hUncorrected->Draw("E3 SAME");
	hUncorrected->SetFillColor(kGreen);
	legend->Draw("SAME");
	system->Draw();
	system->SaveAs(Form("%s/AlCapData_Ti50Dataset_%s_%s-Systematics-Combined.pdf", getenv("R15b_OUT"), arm.Data(), particle.Data() ) );
	system->SaveAs(Form("%s/AlCapData_Ti50Dataset_%s_%s-Systematics-Combined.png", getenv("R15b_OUT"), arm.Data(), particle.Data() ) );
	fOutput->Write();
}

void SystematicsTi(TString arm = "SiR", TString particle = "proton") {
	std::cout << "Processing ti50 " << particle << " " << arm << std::endl;
	gStyle->SetOptStat(0);
	gStyle->SetStatY(0.9);                
	gStyle->SetStatX(0.9);

	const int nbins = 50;
	Double_t dtError[nbins] = {0}; //0 -> 25MeV with 500keV bins
	Double_t lifetimeError[nbins] = {0};
	Double_t pidError[nbins] = {0};
	Double_t unfoldingError[nbins] = {0};

	TFile *fData = new TFile(Form("%s/ti50.root", getenv("R15b_DATA") ), "READ");
	TTree *tree = (TTree *)fData->Get("tree");

	lifetime(tree, lifetimeError, arm, particle);
	dt(tree, dtError, arm, particle);
	Pid(tree, pidError, unfoldingError, arm, particle);
	Finally(unfoldingError, pidError, dtError, lifetimeError, arm, particle);
	Combined(unfoldingError, pidError, dtError, lifetimeError, arm, particle);

	delete tree;
}
