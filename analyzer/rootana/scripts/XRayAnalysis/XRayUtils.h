#ifndef XRayUtils_h_
#define XRayUtils_h_

struct XRay {
  std::string transition;
  std::string material;
  double energy;
  double intensity;
  double intensity_error;
  double efficiency;
  double efficiency_error;
};

#include "TFile.h"
#include "TDirectoryFile.h"
#include "TH2.h"
#include "TF1.h"
#include "TPad.h"

#include "RooFit.h"
#include "RooAbsPdf.h"
#include "RooPlot.h"
#include "RooRealVar.h"
#include "RooWorkspace.h"
#include "RooDataHist.h"

#include <iostream>
#include <sstream>

int FillXRayInfo(XRay& xray) {
  if (xray.material == "Al") {
    std::cout << "Aluminium X-ray:" << std::endl;
    xray.transition = "2p-1s";
    xray.energy = 346.828;
    xray.intensity = 0.798;
    xray.intensity_error = 0.008;

    // For R13
    //    xray.efficiency = 5e-4;
    //    xray.efficiency_error = 0.1e-4;
    return 0;
  }
  else if (xray.material == "Si") {
    std::cout << "Silicon x-ray:" << std::endl;
    if (xray.transition == "2p-1s") {
      xray.energy = 400.177;
      xray.intensity = 0.803;
      xray.intensity_error = 0.008;
    }
  }
  else if (xray.material == "Ti") {
    std::cout << "Titanium x-ray:" << std::endl;
    xray.transition = "2p-1s";
    //    xray.energy = 942; // PRL V18 N26 (1967)
    xray.energy = 931; // elog:336 and Fig. 1b of the above
    xray.intensity = 0.752; 
    xray.intensity_error = 0.007;
  }
  else if (xray.material == "511") {
    std::cout << "511 keV peak:" << std::endl;
    xray.transition = "511";
    xray.energy = 511;
    xray.intensity = 1;
    xray.intensity_error = 0;
  }
  else {
    std::cout << "Error: Unknown target material" << std::endl;
    return 1;
  }
  std::cout << xray.material << " " << xray.transition << std::endl;
  std::cout << "Energy = " << xray.energy << " keV" << std::endl;
  std::cout << "Intensity = " << xray.intensity << " +/- " << xray.intensity_error << std::endl;
  return 0;
}

int FillGeEffParams(const std::string& channel, const int& run, double& a, double& b, double& delta_a, double& delta_b, double& corr) {
  if (channel == "GeLoGain") {
    if (run == 10319) {
      // from GoogleSheet
      //      a = 0.223417; delta_a = 0.0213393;
      //      b = -0.919075; delta_b = 0.0151079;
      //      corr = -0.995269;
      a = 0.1792; delta_a = 0.01533;
      b = -0.9332; delta_b = 0.01332;
      corr = -0.867502;
    }
    else if (run == 9302) {
      a = 0.173809; delta_a = 0.0226852;
      b = -0.91031; delta_b = 0.020632;
      corr = -0.995468;
    }
    else {
      std::cout << "Error: FillGeEffParams has been given an unknown run (" << run << ")" << std::endl;
      return 1;
    }
  }
  else if (channel == "GeHiGain") {
    if (run == 10319) {
      // from GoogleSheet
      //      a = 0.244223; delta_a = 0.0228416;
      //      b = -0.931899; delta_b = 0.0148333;
      //      corr = -0.995306;
      a = 0.193; delta_a = 0.01635;
      b = -0.9437; delta_b = 0.01323;
      corr = -0.86864;
    }
    else if (run == 9302) {
      a = 0.19928; delta_a = 0.0256165;
      b = -0.931788; delta_b = 0.020387;
      corr = -0.995484;
    }
    else {
      std::cout << "Error: FillGeEffParams has been given an unknown run (" << run << ")" << std::endl;
      return 1;
    }
  }
  else {
    std::cout << "Error: FillGeEffParams has been given an unknown channel (" << channel << ")" << std::endl;
    return 1;
  }
  return 0;
}

int FillGeEffFunctions(const double& a, const double& b, const double& delta_a, const double& delta_b, const double& corr, TF1*& ge_eff, TF1*& ge_eff_err) {
  ge_eff = new TF1("ge_eff", "[0]*(x^[1])");
  ge_eff->SetParameters(a, b);
  ge_eff->SetParError(0, delta_a);
  ge_eff->SetParError(1, delta_b);

  // Assuming uncertainty in the energy is small
  ge_eff_err = new TF1("ge_eff_err", "sqrt(x^(2*[1]) * ([2]^2 + TMath::Log(x)*[0]*[3]*([0]*[3]*TMath::Log(x) + 2*[4]*[2])))");
  ge_eff_err->SetParameters(a, b, delta_a, delta_b, corr);
  
  return 0;
}

RooWorkspace* FitPeak(double energy_low, double energy_high, TH1* hSpectrum, XRay* xray) {
  
  // Keep track of the number of parameters
  int n_fit_params = 0;

  // Create the workspace and functions for fitting and create the summed pdf as we go
  RooWorkspace* ws = new RooWorkspace("ws", kTRUE);
  std::stringstream factory_cmd, sum_factory_cmd;
  sum_factory_cmd << "SUM::sum(";

  // First, the linear background
  factory_cmd << "Polynomial::pol1_bkg(edep[" << energy_low << ", " << energy_high << "], {bkg_offset[-10, 100], bkg_slope[-0.1, 0.1]})";
  ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");
  sum_factory_cmd << "nbkg[0, 500000]*pol1_bkg";
  n_fit_params += 3; // bkg_offset, bkg_slope, nbkg

  // Now the X-ray peak of interest
  factory_cmd << "Gaussian::xraypeak_pdf(edep[" << energy_low << ", " << energy_high << "], xray_mean[" << xray->energy-1 << ", " << xray->energy+1 << "], xray_sigma[0.1, 10])"; // the x-ray peak itself
  ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");
  sum_factory_cmd << ", xray_area[0,500000]*xraypeak_pdf";
  n_fit_params += 3; // xray_mean, xray_sigma, xray_area

  // For Al 2p-1s, we also have a second peak that's a background
  if (xray->material == "Al" && xray->transition == "2p-1s") {
    factory_cmd << "Gaussian::bkgpeak_pdf(edep[" << energy_low << ", " << energy_high << "], bkg_mean[351,352], bkg_sigma[0.1, 10])";
    ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");
    sum_factory_cmd << ", bkg_area[0,500000]*bkgpeak_pdf";
    n_fit_params += 3; // bkg_mean, bkg_sigma, bkg_area
  }

  // Now create the SUM pdf
  sum_factory_cmd << ")";
  ws->factory(sum_factory_cmd.str().c_str()); sum_factory_cmd.str("");

  // Import the histogram into a RooFit data hist
  RooDataHist data("data", "Ge Spectrum", (ws->argSet("edep")), hSpectrum);
  (ws->pdf("sum"))->fitTo(data);
  ws->import(data);
  //  std::cout << "Goodness of fit: " << Eframe->chiSquare(n_fit_params) << std::endl;
  
  return ws;
}

#endif
