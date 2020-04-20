#if !(defined(__CINT__) || defined(__CLING__)) || defined(__ACLIC__)
#include <iostream>
#include "TRandom.h"
#include "TH1D.h"
#include "RooUnfoldResponse.h"
#include "RooUnfoldBayes.h"
#endif

TH1D * Process(TH1D *hMeas, TString arm, TString particle, TString cutDescription) {
	TFile *responseMatrixFile = new TFile(Form("%s/transfer.sf1.02.al50.%s.root", getenv("R15b_TM"), particle.Data() ), "READ");
	if(particle.CompareTo("proton") == 0) {
		responseMatrixFile = new TFile(Form("%s/transfer.sf1.02.al50.%s3.root", getenv("R15b_TM"), particle.Data() ), "READ");
	}

	RooUnfoldResponse *L_TM = (RooUnfoldResponse *)responseMatrixFile->Get("SiL500_TM");
	RooUnfoldResponse *R_TM = (RooUnfoldResponse *)responseMatrixFile->Get("SiR500_TM");
	RooUnfoldResponse *response = L_TM;
	if(arm.CompareTo("SiR") == 0) {
		response = R_TM;
	}
	RooUnfoldBayes unfold(response, hMeas);
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
		if(abs(t2-t1-13) > 23 * 4) continue;
		if(TMath::IsNaN(e3) ) {
			if(!sig3->Contains(particle) ) continue;
			if(arm.CompareTo("SiR") == 0 ) {
				if(a2 > 3986) continue; //remove saturation
				if(!channel->Contains("SiR") ) continue;
			} else {
				if(a2 > 3982) continue; //remove saturation
				if(!channel->Contains("SiL") ) continue;
			}
			
			if(t2 > 400) hOne->Fill(e1+e2);
			if(t2 > 500) hTwo->Fill(e1+e2);
			if(t2 > 600) hThree->Fill(e1+e2);
			if(t2 > 700) hFour->Fill(e1+e2);
		} else {
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
				if(abs(t2-t1 - 8.19758) <   15.1644) h1->Fill(E);
				if(abs(t2-t1 - 8.19758) < 2*15.1644) h2->Fill(E);
				if(abs(t2-t1 - 8.19758) < 3*15.1644) h3->Fill(E);
				if(abs(t2-t1 - 8.19758) < 4*15.1644) h4->Fill(E);
			}
			const int c = 10;
			double energy[c] =   {3.500,  4.500,  5.500,  6.500, 7.500,  8.500, 9.500, 10.500, 11.500, 12.500};
			double mean[c] =  {12.0452, 13.9437, 14.8337, 15.4478, 15.5178, 14.9985, 11.9798, 12.544, 21.9398, 21.0489};
			double sigma[c] = {14.6326, 17.2193, 19.9066, 23.1122, 25.6022, 28.4722, 30.5675, 34.3171, 51.4912, 50.2579};
			for(int i=0; i < c; ++i) {
				if(abs(E-energy[i])<500) {
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
			const int c = 8;
			double energy[c] =   {3.500, 4.500, 5.500,  6.500, 7.500,  8.500, 9.500, 10.500};
			double mean[c] = {11.13, 12.78, 13.85, 12.31, 10.51, 8.78, 9.72, 12.70};
			double sigma[c] = {20.78, 19.71, 21.61, 22.63, 24.99, 26.30, 32.32, 38.22};
			for(int i=0; i < c; ++i) {
				if(abs(E-energy[i])<500) {
					if(abs(t2-t1 - mean[i]) < sigma[i]) h1->Fill(E);
					if(abs(t2-t1 - mean[i]) < 2*sigma[i]) h2->Fill(E);
					if(abs(t2-t1 - mean[i]) < 3*sigma[i]) h3->Fill(E);
					if(abs(t2-t1 - mean[i]) < 4*sigma[i]) h4->Fill(E);
				}
			}
			if(E>11.000) {
				if(abs(t2-t1 - 18.88) < 29.82) h1->Fill(E);
				if(abs(t2-t1 - 18.88) < 2*29.82) h2->Fill(E);
				if(abs(t2-t1 - 18.88) < 3*29.82) h3->Fill(E);
				if(abs(t2-t1 - 18.88) < 4*29.82) h4->Fill(E);
			}
		}
	} else if(particle.CompareTo("deuteron") == 0) { //deuteron
		if(arm.CompareTo("SiR") == 0) {
			const int c = 5;
			double energy[c] =   {4.000, 6.000, 8.000, 10.000, 12.000};
			double mean[c] = {12.66, 12.08, 9.79, 8.58, 7.36};
			double sigma[c] = {10.81, 13.10, 17.22, 18.57, 24.30};
			for(int i=0; i < c; ++i) {
				if(abs(E-energy[i])<1000) {
					if(abs(t2-t1 - mean[i]) <   sigma[i]) h1->Fill(E);
					if(abs(t2-t1 - mean[i]) < 2*sigma[i]) h2->Fill(E);
					if(abs(t2-t1 - mean[i]) < 3*sigma[i]) h3->Fill(E);
					if(abs(t2-t1 - mean[i]) < 4*sigma[i]) h4->Fill(E);
				}
			}
			if(E>13.000) {
				if(abs(t2-t1 - 4.73) < 35.53) h1->Fill(E);
				if(abs(t2-t1 - 4.73) < 2*35.53) h2->Fill(E);
				if(abs(t2-t1 - 4.73) < 3*35.53) h3->Fill(E);
				if(abs(t2-t1 - 4.73) < 4*35.53) h4->Fill(E);
			}
		} else { //SiL
			if(abs(t2-t1 - 12.7294) <   22.0637) h1->Fill(E);
			if(abs(t2-t1 - 12.7294) < 2*22.0637) h2->Fill(E);
			if(abs(t2-t1 - 12.7294) < 3*22.0637) h3->Fill(E);
			if(abs(t2-t1 - 12.7294) < 4*22.0637) h4->Fill(E);
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
			if(!pt2->Contains(particle) ) continue;
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
			dtError[j] = 1- ((hOneUnf->GetBinContent(j)/0.682) / (hTwoUnf->GetBinContent(j)/0.954) );
		} else {
			dtError[j] = 0;
		}
	}
}
void Pid(TTree *tree, Double_t *pidError, TString arm, TString particle) {
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
		if(t2<500) continue;
		if(abs(t2)>10e3) continue;
		if(abs(t2-t1-13) > 23 * 4 ) continue;
		if(TMath::IsNaN(e3) ) {
			if(arm.CompareTo("SiR") ) {
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

void Finally(Double_t *pidError, Double_t *dtError, Double_t *lifetimeError, TString arm, TString particle) {
	TString target = "al50";
	Int_t nbins = 50;
	TH1D *hPid = new TH1D("hPid", "PID;E [keV]", nbins, 0, 25); hPid->SetFillColor(kRed);
	TH1D *hDt = new TH1D("hDt", "t_{2}-t_{1};E [keV]", nbins, 0, 25); hDt->SetFillColor(kGreen);
	TH1D *hLifetime = new TH1D("hLifetime", "#tau;E [keV]", nbins, 0, 25); hLifetime->SetFillColor(kBlue);
	for(int i=1; i<=nbins; ++i) {
		hPid->SetBinContent(i, abs(pidError[i]) );
		hDt->SetBinContent(i, abs(dtError[i]) );
		hLifetime->SetBinContent(i, abs(lifetimeError[i]) );
	}
	THStack *hSystematics = new THStack("hSystematics", "Systematic errors");
	hSystematics->Add(hPid);
	hSystematics->Add(hDt);
	hSystematics->Add(hLifetime);

TLegend *legend = new TLegend(.240, .598, .652, .868);
//TLegend *legend = new TLegend(0.266476, 0.598739, 0.593123, 0.869748); //deuteron, triton
//TLegend *legend = new TLegend(0.146132, 0.598739, 0.434097, 0.869748); //alpha


	legend->SetHeader(Form("#bf{AlCap} #it{Al50} %s Systematics", arm.Data() ) );
	legend->AddEntry(hPid, "Pid", "F");
	legend->AddEntry(hDt, "t_{2}-t_{1}", "F");
	legend->AddEntry(hLifetime, "#tau", "F");

	TCanvas *cFinal = new TCanvas("c", "c");
	hSystematics->Draw();
	hSystematics->GetXaxis()->SetTitle("E[keV]");
	hSystematics->GetYaxis()->SetTitle("Fractional");
	legend->Draw("SAME");
	cFinal->Draw();
	const char *FigsDir = getenv("R15b_OUT");
	cFinal->SaveAs(Form("%s/AlCapData_Al50Dataset_%s_%s-Systematics.pdf", FigsDir, arm.Data(), particle.Data() ) );
}

void Systematics(TString arm = "SiR", TString particle = "proton") {
	std::cout << "Processing al50 " << particle << " " << arm << std::endl;
	gStyle->SetOptStat(0);
	gStyle->SetStatY(0.9);                
	gStyle->SetStatX(0.9);

	const int nbins = 40;
	Double_t dtError[nbins] = {}; //0 -> 20000keV with 500keV bins
	Double_t lifetimeError[nbins] = {};
	Double_t pidError[nbins] = {};

	TFile *fData = new TFile(Form("%s/al50.root", getenv("R15b_DATA") ), "READ");
	TTree *tree = (TTree *)fData->Get("tree");

	lifetime(tree, lifetimeError, arm, particle);
	dt(tree, dtError, arm, particle);
	Pid(tree, pidError, arm, particle);
	Finally(pidError, dtError, lifetimeError, arm, particle);

	delete tree;
}
