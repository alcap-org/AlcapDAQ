#include <sstream>
#include <iostream>

#include "TFile.h"
#include "TH1.h"
#include "TMath.h"
#include "TFitResult.h"

static const int n_quadrants = 4;

struct RangeCurveRun {
  double scale_factor;
  std::vector<int> run_numbers;
  double n_entering_muons;
  double n_SiR1_silicon_muons[n_quadrants];
  double n_SiR2_peak_pos;
  double n_xrays;
};

void range_curve_2015_11_11() {
  
  const int SiR1_MuonThreshold[n_quadrants] = {100, 100, 100, 100};

  std::vector<RangeCurveRun*> scale_factors;

  RangeCurveRun* sf_1_6 = new RangeCurveRun;
  sf_1_6->scale_factor = 1.6;
  sf_1_6->run_numbers.push_back(8432); sf_1_6->run_numbers.push_back(8433);  sf_1_6->run_numbers.push_back(8434);
  sf_1_6->run_numbers.push_back(8435); sf_1_6->run_numbers.push_back(8436);
  scale_factors.push_back(sf_1_6);

  RangeCurveRun* sf_1_55 = new RangeCurveRun;
  sf_1_55->scale_factor = 1.55;
  sf_1_55->run_numbers.push_back(8437); sf_1_55->run_numbers.push_back(8438);  sf_1_55->run_numbers.push_back(8439);
  sf_1_55->run_numbers.push_back(8440); sf_1_55->run_numbers.push_back(8441);
  scale_factors.push_back(sf_1_55);

  RangeCurveRun* sf_1_50 = new RangeCurveRun;
  sf_1_50->scale_factor = 1.50;
  sf_1_50->run_numbers.push_back(8442); sf_1_50->run_numbers.push_back(8443);  sf_1_50->run_numbers.push_back(8444);
  sf_1_50->run_numbers.push_back(8445); sf_1_50->run_numbers.push_back(8446);
  scale_factors.push_back(sf_1_50);

  RangeCurveRun* sf_1_45 = new RangeCurveRun;
  sf_1_45->scale_factor = 1.45;
  sf_1_45->run_numbers.push_back(8447); sf_1_45->run_numbers.push_back(8448);  sf_1_45->run_numbers.push_back(8449);
  sf_1_45->run_numbers.push_back(8450); sf_1_45->run_numbers.push_back(8451);
  scale_factors.push_back(sf_1_45);

  RangeCurveRun* sf_1_4 = new RangeCurveRun;
  sf_1_4->scale_factor = 1.4;
  sf_1_4->run_numbers.push_back(8426); sf_1_4->run_numbers.push_back(8427);  sf_1_4->run_numbers.push_back(8428);
  sf_1_4->run_numbers.push_back(8429); sf_1_4->run_numbers.push_back(8430);
  sf_1_4->run_numbers.push_back(8452); sf_1_4->run_numbers.push_back(8453);  sf_1_4->run_numbers.push_back(8454);
  sf_1_4->run_numbers.push_back(8455); sf_1_4->run_numbers.push_back(8456);
  scale_factors.push_back(sf_1_4);

  RangeCurveRun* sf_1_35 = new RangeCurveRun;
  sf_1_35->scale_factor = 1.35;
  sf_1_35->run_numbers.push_back(8457); sf_1_35->run_numbers.push_back(8458);  sf_1_35->run_numbers.push_back(8459);
  sf_1_35->run_numbers.push_back(8460); sf_1_35->run_numbers.push_back(8461);
  scale_factors.push_back(sf_1_35);

  RangeCurveRun* sf_1_30 = new RangeCurveRun;
  sf_1_30->scale_factor = 1.30;
  sf_1_30->run_numbers.push_back(8462); sf_1_30->run_numbers.push_back(8463);  sf_1_30->run_numbers.push_back(8464);
  sf_1_30->run_numbers.push_back(8465); sf_1_30->run_numbers.push_back(8467); // NB not 8466
  scale_factors.push_back(sf_1_30);

  RangeCurveRun* sf_1_25 = new RangeCurveRun;
  sf_1_25->scale_factor = 1.25;
  sf_1_25->run_numbers.push_back(8468); sf_1_25->run_numbers.push_back(8469);  sf_1_25->run_numbers.push_back(8470);
  sf_1_25->run_numbers.push_back(8471); sf_1_25->run_numbers.push_back(8472); // NB not 8466
  scale_factors.push_back(sf_1_25);

  RangeCurveRun* sf_1_20 = new RangeCurveRun;
  sf_1_20->scale_factor = 1.20;
  sf_1_20->run_numbers.push_back(8473); sf_1_20->run_numbers.push_back(8474);  sf_1_20->run_numbers.push_back(8475);
  sf_1_20->run_numbers.push_back(8476); sf_1_20->run_numbers.push_back(8477); 
  scale_factors.push_back(sf_1_20);

  RangeCurveRun* sf_1_15 = new RangeCurveRun;
  sf_1_15->scale_factor = 1.15;
  sf_1_15->run_numbers.push_back(8478); sf_1_15->run_numbers.push_back(8479);  sf_1_15->run_numbers.push_back(8480);
  sf_1_15->run_numbers.push_back(8481); sf_1_15->run_numbers.push_back(8482); 
  scale_factors.push_back(sf_1_15);

  RangeCurveRun* sf_1_10 = new RangeCurveRun;
  sf_1_10->scale_factor = 1.10;
  sf_1_10->run_numbers.push_back(8483); sf_1_10->run_numbers.push_back(8484);  sf_1_10->run_numbers.push_back(8485);
  sf_1_10->run_numbers.push_back(8486); sf_1_10->run_numbers.push_back(8487); 
  scale_factors.push_back(sf_1_10);

  RangeCurveRun* sf_1_05 = new RangeCurveRun;
  sf_1_05->scale_factor = 1.05;
  sf_1_05->run_numbers.push_back(8488); sf_1_05->run_numbers.push_back(8489);  sf_1_05->run_numbers.push_back(8490);
  sf_1_05->run_numbers.push_back(8491); sf_1_05->run_numbers.push_back(8492); 
  scale_factors.push_back(sf_1_05);

  RangeCurveRun* sf_1_03 = new RangeCurveRun;
  sf_1_03->scale_factor = 1.03;
  sf_1_03->run_numbers.push_back(8493); sf_1_03->run_numbers.push_back(8494);  sf_1_03->run_numbers.push_back(8495);
  sf_1_03->run_numbers.push_back(8496); sf_1_03->run_numbers.push_back(8497); 
  scale_factors.push_back(sf_1_03);

  RangeCurveRun* sf_1_01 = new RangeCurveRun;
  sf_1_01->scale_factor = 1.01;
  sf_1_01->run_numbers.push_back(8498); sf_1_01->run_numbers.push_back(8499);  sf_1_01->run_numbers.push_back(8500);
  sf_1_01->run_numbers.push_back(8501);
  scale_factors.push_back(sf_1_01);

  RangeCurveRun* sf_0_99 = new RangeCurveRun;
  sf_0_99->scale_factor = 0.99;
  sf_0_99->run_numbers.push_back(8502); sf_0_99->run_numbers.push_back(8503);  sf_0_99->run_numbers.push_back(8504);
  scale_factors.push_back(sf_0_99);

  RangeCurveRun* sf_0_97 = new RangeCurveRun;
  sf_0_97->scale_factor = 0.97;
  sf_0_97->run_numbers.push_back(8505); sf_0_97->run_numbers.push_back(8506);
  scale_factors.push_back(sf_0_97);

  RangeCurveRun* sf_0_95 = new RangeCurveRun;
  sf_0_95->scale_factor = 0.95;
  sf_0_95->run_numbers.push_back(8507); sf_0_95->run_numbers.push_back(8508);
  scale_factors.push_back(sf_0_95);

  RangeCurveRun* sf_0_93 = new RangeCurveRun;
  sf_0_93->scale_factor = 0.93;
  sf_0_93->run_numbers.push_back(8509);
  scale_factors.push_back(sf_0_93);

  //  sf_1->run_numbers.push_back(8483); sf_1->run_numbers.push_back(8484);  sf_1->run_numbers.push_back(8485);

  for (std::vector<RangeCurveRun*>::iterator i_scale_factor = scale_factors.begin(); i_scale_factor != scale_factors.end(); ++i_scale_factor) {
    (*i_scale_factor)->n_entering_muons = 0;
    for (int i_quadrant = 0; i_quadrant < n_quadrants; ++i_quadrant) {
      (*i_scale_factor)->n_SiR1_silicon_muons[i_quadrant] = 0;
    }

    TH1* xray_hist(NULL);
    for (std::vector<int>::const_iterator i_run = (*i_scale_factor)->run_numbers.begin(); i_run != (*i_scale_factor)->run_numbers.end(); ++i_run) {

      std::stringstream filename;
      filename << "/home/edmonds/AlcapDAQ/hist0" << (*i_run) << ".root";
      TFile* file = new TFile(filename.str().c_str(), "READ");
      if (file->IsZombie()) {
	std::cout << "Problem opening file " << filename.str() << std::endl;
	continue;
      }
      
      TH1F* hMuScHiTDC = (TH1F*) file->Get("hT402_RawTime");
      if (!hMuScHiTDC) {
	std::cout << "Problem getting hMuScHiTDC" << std::endl;
      }
      (*i_scale_factor)->n_entering_muons += hMuScHiTDC->GetEntries();
      
      for (int i_quadrant = 0; i_quadrant < n_quadrants; ++i_quadrant) {
	std::stringstream histname;
	histname << "hSIS3300_B1C" << i_quadrant+1 << "_Heights";
	TH1F* hSiR1_Heights = (TH1F*) file->Get(histname.str().c_str());
	if (!hSiR1_Heights) {
	  std::cout << "Problem getting hSiR1_Heights" << std::endl;
	}
	//	if (i_quadrant == 3) {
	//	  hSiR1_Heights->Draw("SAME");
	//	}
	(*i_scale_factor)->n_SiR1_silicon_muons[i_quadrant] += hSiR1_Heights->Integral(hSiR1_Heights->FindBin(SiR1_MuonThreshold[i_quadrant]), hSiR1_Heights->GetNbinsX());
      }

      std::stringstream histname;
      histname << "hSIS3300_B1C7_Heights";
      TH1F* hSiR2_Heights = (TH1F*) file->Get(histname.str().c_str());
      if (!hSiR2_Heights) {
	std::cout << "Problem getting hSiR2_Heights" << std::endl;
      }
      //	if (i_quadrant == 3) {
      //      hSiR2_Heights->Draw("SAME");
      //	}
      (*i_scale_factor)->n_SiR2_peak_pos = hSiR2_Heights->GetBinCenter(hSiR2_Heights->GetMaximumBin());

      TH1* tmp_hist = (TH1*) file->Get("hD402_Heights");
      if (xray_hist) {
	xray_hist->Add(tmp_hist);
      } else {
	xray_hist = (TH1*)tmp_hist->Clone("xray_hist");
	xray_hist->SetDirectory(0);
      }
      delete tmp_hist;

    }
    
    static double rt2pi = TMath::Sqrt(2.*TMath::Pi());
    xray_hist->GetXaxis()->SetRangeUser(2155., 2180.);
    TFitResultPtr res = xray_hist->Fit("gaus", "NSQ");
    (*i_scale_factor)->n_xrays = rt2pi*res->Parameter(0)*res->Parameter(2);
    (*i_scale_factor)->n_xrays /= (*i_scale_factor)->n_entering_muons;
    delete xray_hist;

    //  std::cout << "SF 1 = " << (*i_scale_factor)->n_entering_muons << std::endl;
    /*    std::cout << "SF " << (*i_scale_factor)->scale_factor << std::endl;
    std::cout << "=========" << std::endl;
    std::cout << "N_MuScHi = " << (*i_scale_factor)->n_entering_muons << std::endl;
    for (int i_quadrant = 0; i_quadrant < n_quadrants; ++i_quadrant) {
      std::cout << "N_Mu (SiR1-" << i_quadrant+1 << ") = " << (*i_scale_factor)->n_SiR1_silicon_muons[i_quadrant] << " total" << std::endl;
      std::cout << "N_Mu (SiR1-" << i_quadrant+1 << ") = " << (*i_scale_factor)->n_SiR1_silicon_muons[i_quadrant]/(*i_scale_factor)->n_entering_muons << " per entering muon" << std::endl;
    }
    std::cout << "SiR2 Peak Position = " << (*i_scale_factor)->n_SiR2_peak_pos << std::endl;
    */
    std::cout << (*i_scale_factor)->scale_factor << " ";
    for (int i_quadrant = 0; i_quadrant < n_quadrants; ++i_quadrant) {
      std::cout << (*i_scale_factor)->n_SiR1_silicon_muons[i_quadrant]/(*i_scale_factor)->n_entering_muons << " ";
    }
    std::cout  << std::endl;
    std::cout << "xrays:\t" << (*i_scale_factor)->n_xrays << std::endl;
  }

  
}
