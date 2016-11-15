#include <iostream>
#include <sstream>

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"

void MuScanAnalysis() {

  int first_run = 10260;
  int last_run = 10271;

  const int n_runs = 12;//last_run - first_run + 1;
  double x_coords[n_runs] = {20, 45, 55, 65, 80, 100, 65, 65, 65, 65, 45, 45};
  double y_coords[n_runs] = {70, 70, 70, 70, 70, 70, 60, 70, 80, 95, 65, 85};
  //  double run_times[n_runs] = {364, 301, 263, 217, 220, -1, 176, 220, 217, 304, 221, 203}; // -1 because there was a problem with the ODB file not recording the correct start time
  double run_times[n_runs] = {338.198, 276.752, 236.670, 193.452, 196.686, 161.994, 160.132, 196.980, 193.942, 279.104, 196.980, 179.34}; // from nblock*0.098
  double elog_348_rates[n_runs] = {0.4, 64.0, 193.6, 252.4, 108.1, 1.4, 37.7, 254.3, 207.6, 5.8, 39.2, 8.2};
  double elog_348_relative_errors[n_runs] = {0.09, 0.008, 0.005, 0.005, 0.007, 0.066, 0.013, 0.004, 0.005, 0.025, 0.011, 0.026};
  double scaler_rates[n_runs] = {11.8, 172.9, 465.0, 592.3, 459.6, 70.6, 154.6, 592.3, 540.9, 15.3, 136.4, 33.5};

  TH2F* hMuScanXY = new TH2F("hMuScanXY", "hMuScanXY", 10,0,100, 10,0,100);

  double electron_adc_cut = 1000;
  double muon_adc_cut = 3000;
  double muon_adc_upper_cut = 8000;

  std::stringstream filename;
  std::cout << "|  Run  | N_muons | Run Time [s] | Rate [Hz] | Elog:348 Rate [Hz] | Difference [Hz] | Scaler Rate [Hz] |" << std::endl;
  std::cout << "|-------+---------+--------------+-----------+--------------------+-----------------+------------------|" << std::endl;
  for (int i_run = first_run; i_run <= last_run; ++i_run) {
    int i_counter = i_run - first_run;

    filename.str("");
    filename << "hist" << i_run << ".root";
    
    TFile* file = new TFile(filename.str().c_str(), "READ");
    TH1F* hHeights = (TH1F*) file->Get("hD700_Heights");
    hHeights->SetDirectory(0);
    //    hHeights->Rebin(128);
    hHeights->SetLineColor(i_run - first_run+1);
    hHeights->Draw("SAMES");
    //    std::cout << hHeights->GetEntries() << " entries" << std::endl;
    int low_integral_bin = hHeights->FindBin(electron_adc_cut);
    //    int low_integral_bin = hHeights->FindBin(muon_adc_cut);
    //    int high_integral_bin = hHeights->GetNbinsX();
    int high_integral_bin = hHeights->FindBin(muon_adc_upper_cut);
    double integral = 0;
    double error = 0;
    integral = hHeights->IntegralAndError(low_integral_bin, high_integral_bin, error);
    double rate = integral / run_times[i_counter];
    double difference = elog_348_rates[i_counter] - rate;

    std::cout << "| " << i_run << " | " << integral << " +/- " << error << " | " << run_times[i_counter] << " | " << rate << " | " << elog_348_rates[i_counter] << " +/- " << elog_348_rates[i_counter] * elog_348_relative_errors[i_counter] << " | " << difference << " | " << scaler_rates[i_counter] << " | " << std::endl;

    int fill_bin = hMuScanXY->FindBin(x_coords[i_counter], y_coords[i_counter]);
    hMuScanXY->SetBinContent(fill_bin, integral);
    hMuScanXY->SetBinError(fill_bin, error);

    file->Close();
  }

  //  hMuScanXY->Draw("COLZ");
}
