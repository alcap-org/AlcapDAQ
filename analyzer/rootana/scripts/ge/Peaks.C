#include "Peaks.h"
#include "Peak.h"
#include "Run.h"

#include "TF1.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TVirtualFitter.h"

#include <fstream>
#include <sstream>

Peaks::Peaks(int n, const Double_t E[], const Double_t I[],
	     const Double_t ADCs[]) {
  for (int i = 0; i < n; ++i) {
    peaks.push_back(Peak(E[i], I[i], ADCs[i]));
  }
  enfcn = new TF1("encal", "pol1(0)");
  efffcn = new TF1("effcal", "[0]*x^[1]");
  enfcn->SetParameters(0, 1);
  efffcn->SetParameters(1, -1);
  engr = effgr = NULL;
}

int Peaks::N() {
  return peaks.size();
}
Peak& Peaks::operator[](int i) {
  return peaks[i];
}

void Peaks::EstimateParameters(TH1* h) {
  Double_t std = 15.;
  Double_t err = 0.; // Our initial estimate error ((new-old)/old)
  for (int i = 0; i < peaks.size(); ++i) {
    peaks[i].Bg1(0.);
    peaks[i].Std(std);
    h->GetXaxis()->SetRangeUser(peaks[i].ADC()-2.*std+err*peaks[i].ADC(),
			    peaks[i].ADC()+2.*std+err*peaks[i].ADC());
    err += ((h->GetBinCenter(h->GetMaximumBin()) - peaks[i].ADC())/peaks[i].ADC()-err)/(i+1);
    peaks[i].ADC(h->GetBinCenter(h->GetMaximumBin()));
    peaks[i].Bg0(h->GetBinContent(peaks[i].ADC()-2.*std));
    peaks[i].A(h->GetBinContent(peaks[i].ADC()) - peaks[i].Bg0());
  }
  h->GetXaxis()->SetRange();
}

void Peaks::Fit(TH1* h, bool print) {
  for (int i = 0; i < peaks.size(); ++i) {
    peaks[i].Fit(h, print);
  }
  h->Write();
}

void Peaks::Calibrate(Run run, bool print) {
  Double_t adc[peaks.size()];
  Double_t adcerr[peaks.size()];
  Double_t en[peaks.size()];
  Double_t eff[peaks.size()];
  Double_t efferr[peaks.size()];
  for (int i = 0; i < peaks.size(); ++i) {
    adc[i]    = peaks[i].ADC();
    adcerr[i] = peaks[i].ADCErr();
    en[i]     = peaks[i].E();
    eff[i]    = peaks[i].Eff(run);
    efferr[i] = peaks[i].EffErr(run);
  }
  engr = new TGraphErrors(peaks.size(), adc, en, adcerr, 0);
  enfitresult = engr->Fit(enfcn, "SEQM");
  if (print) {
    char str[64]; sprintf(str, "Run %d Energy;ADC;Energy (keV)", run.N());
    engr->SetTitle(str);
    engr->SetName("hEnGr");
    engr->Draw("AP");
//    gPad->Print("ecal.png");
  }
  engr->Write();
  if (run.ValidForEff()) {
    effgr = new TGraphErrors(peaks.size(), en, eff, 0, efferr);
    efffitresult = effgr->Fit(efffcn, "SEQM");

    double start = 100;
    double end = 2000;//1500;
    double point_step = 1;
    int n_points = (end - start)/point_step;
    fit_interval = new TGraphErrors(n_points);
    for (int i_point = 0; i_point < n_points; ++i_point) {
      fit_interval->SetPoint(i_point, start + i_point*point_step, 0);
    }
    (TVirtualFitter::GetFitter())->GetConfidenceIntervals(fit_interval);
    if (print) {
      char str[64]; sprintf(str, "Run %d Efficiency;Energy (keV);Efficiency",
			    run.N());
      effgr->SetTitle(str);
      effgr->SetName("hEffGr");
      effgr->SetLineWidth(2);
      fit_interval->SetName("hEffGrIntervals");
      fit_interval->SetFillColor(kRed-9);
      fit_interval->SetLineColor(kRed-9);
      fit_interval->SetMarkerColor(kRed-9);
      fit_interval->Draw("A3");
      effgr->Draw("P SAME");
//      gPad->Print("effcal.png");
    }
    effgr->Write();
    fit_interval->Write();
  }
}

void Peaks::Save(int run_number) {
  std::stringstream run_str;
  run_str << run_number;
  std::string ofilename = "peakfits_" + run_str.str() + ".csv";
  std::ofstream ofile(ofilename.c_str());
  ofile << "Energy,Amp,AmpErr,Mu,MuErr,Sigma,SigmaErr,"
	<< "BG0,BG0Err,BG1,BG1Err,Chi2,NDF,Success" << std::endl;
  for (int i = 0; i < peaks.size(); ++i)
    ofile << peaks[i].E() << ','
	  << peaks[i].A() << ','    << peaks[i].AErr()   << ','
	  << peaks[i].ADC() << ','  << peaks[i].ADCErr() << ','
	  << peaks[i].Std() << ','  << peaks[i].StdErr() << ','
	  << peaks[i].Bg0() << ','  << peaks[i].Bg0Err() << ','
	  << peaks[i].Bg1() << ','  << peaks[i].Bg1Err() << ','
	  << peaks[i].Chi2() << ',' << peaks[i].Ndf()    << ','
	  << peaks[i].ValidFit() << std::endl;
  ofile.close();
  ofilename = "encalib_" + run_str.str() + ".csv";
  ofile.open(ofilename.c_str());
  ofile << "E0,E0Err,E1,E1Err,Chi2,Corr,NDF,Success" << std::endl
	<< enfitresult->Parameter(0) << ',' << enfitresult->ParError(0) << ','
	<< enfitresult->Parameter(1) << ',' << enfitresult->ParError(1) << ','
	<< enfitresult->Correlation(0, 1) << ','
	<< enfitresult->Chi2()       << ',' << enfitresult->Ndf()       << ','
	<< enfitresult->IsValid() << std::endl;
  ofile.close();
  if (effgr) {
    ofilename = "effcalib_" + run_str.str() + ".csv";
    ofile.open(ofilename.c_str());
    ofile << "A,AErr,Exp,ExpErr,Corr,Chi2,NDF,Success" << std::endl
	  << efffitresult->Parameter(0) << ',' << efffitresult->ParError(0) << ','
	  << efffitresult->Parameter(1) << ',' << efffitresult->ParError(1) << ','
	  << efffitresult->Correlation(0, 1) << ','
	  << efffitresult->Chi2()       << ',' << efffitresult->Ndf()       << ','
	  << efffitresult->IsValid() << std::endl;
    ofile.close();
  }
}
