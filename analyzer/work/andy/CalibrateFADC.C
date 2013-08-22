// CalibrateFADC.C
// -- Calibrates the FADC channel histogram
// -- x-axis: sample number --> time/ns
// -- y-axis: FADC value --> voltage/mV --> energy/MeV

#include <sstream>

#include "TH1.h"

TH1* CalibrateFADC(TH1* hist, double x, double new_x) { // changes x --> new_x

  std::stringstream calibHistname;
  calibHistname << "Calib_" << hist->GetName();

  double scaling_factor = new_x / x;
  double low_val = hist->GetBinLowEdge(1) * scaling_factor;
  double high_val = hist->GetBinLowEdge(hist->GetXaxis()->GetNbins() + 1) * scaling_factor;

  TH1F* calibHist  = new TH1F(calibHistname.str().c_str(), calibHistname.str().c_str(), 100, low_val, high_val);

  calibHist->GetXaxis()->SetTitle(hist->GetXaxis()->GetTitle());
  calibHist->GetYaxis()->SetTitle(hist->GetYaxis()->GetTitle());


  // Now fill the calibrated histogram
  for (int iBin = 1; iBin <= hist->GetXaxis()->GetNbins(); iBin++) {
    double binCenter = hist->GetBinCenter(iBin) * scaling_factor;
    double binContent = hist->GetBinContent(iBin);

    if (binContent != 0) // ignore empty bins
      calibHist->Fill(binCenter, binContent);
  }
  
  return calibHist;
}


// Calibrates the energy by taking the mean value of hist and scaling this to energy_input
/*TH1* EnergyCalibration(TH1* hist, double energy_input) {

  // Calibrate peak height histogram
  //TF1* gauss = new TF1("gaus", "TMath::Gaus(x, [0], [1])", low_bin_val, high_bin_val);
  //gauss->SetParameter(0, peak_height_hist->GetMean());
  //gauss->SetParameter(1, 1);
  //peak_height_hist->Fit("gaus");

  double hist_mean = hist->GetMean();
  double scaling_factor = energy_input / peak_height_mean;

  std::stringstream calibHistname;
  calibHistname << "Calib_" << hist->GetName();

  double low_val = hist->GetBinLowEdge(1) * scaling_factor;
  double high_val = hist->GetBinLowEdge(hist->GetXaxis()->GetNbins() + 1) * scaling_factor;

  TH1F* calib_hist = new TH1F(calibHistname.str().c_str(), calibHistname.str().c_str(), 100, low_bin_val, high_bin_val);

  for (std::vector<double>::iterator it = peakHeights.begin(); it != peakHeights.end(); it++) {
    (*it) *= scaling_factor;
    calib_hist->Fill(*it);
  }

  calib_hist->GetXaxis()->SetTitle("energy / MeV");
  calib_hist->GetYaxis()->SetTitle("number of pulses");

  return calib_hist;
}
*/
