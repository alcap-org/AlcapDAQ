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

int FillXRayInfo(XRay& xray, std::string channel) {
  if (xray.material == "Al") {
    std::cout << "Aluminium X-ray:" << std::endl;
    xray.transition = "2p-1s";
    xray.energy = 346.828;
    xray.intensity = 0.798;
    xray.intensity_error = 0.008;
    if (channel == "GeLoGain") {
      xray.efficiency = 6.6e-4;
      xray.efficiency_error = 0.2e-4;
    }
    else if (channel == "GeHiGain") {
      xray.efficiency = 6.6e-4;
      xray.efficiency_error = 0.2e-4;
    }
    else {
      std::cout << "Error: Unknown channel (" << channel << ")" << std::endl;
      return 1;
    }
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
      if (channel == "GeLoGain") {
	xray.efficiency = 5.88e-4;
	xray.efficiency_error = 0.09e-4;
      }
      else if (channel == "GeHiGain") {
	xray.efficiency = 6.15e-4;
	xray.efficiency_error = 0.09e-4;
      }
      else {
	std::cout << "Error: Unknown channel (" << channel << ")" << std::endl;
	return 1;
      }
    }
    else if (xray.transition == "3p-1s") {
      xray.energy = 476.8;
      xray.intensity = 0.074;
      xray.intensity_error = 0.001;//??
      if (channel == "GeLoGain") {
	xray.efficiency = 4.84e-4;
	xray.efficiency_error = 0.26e-4;
      }
      else if (channel == "GeHiGain") {
	xray.efficiency = 4.84e-4;
	xray.efficiency_error = 0.26e-4;
      }
      else {
	std::cout << "Error: Unknown channel (" << channel << ")" << std::endl;
	return 1;
      }
    }
    else if (xray.transition == "4p-1s") {
      xray.energy = 503.58;
      xray.intensity = 0.0427;
      xray.intensity_error = 0.0001;//??
      if (channel == "GeLoGain") {
	xray.efficiency = 3.87e-4;
	xray.efficiency_error = 0.45e-4;
      }
      else if (channel == "GeHiGain") {
	xray.efficiency = 3.87e-4;
	xray.efficiency_error = 0.45e-4;
      }
      else {
	std::cout << "Error: Unknown channel (" << channel << ")" << std::endl;
	return 1;
      }
    }
    else if (xray.transition == "5p-1s") {
      xray.energy = 515.97;
      xray.intensity = 0.0383;
      xray.intensity_error = 0.0001;//??
      std::cout << "Warning: Unknown direct efficiency for this peak" << std::endl;
    }
    else if (xray.transition == "6p-1s") {
      xray.energy = 522.74;
      xray.intensity = 0.0229;
      xray.intensity_error = 0.0001;//??
      std::cout << "Warning: Unknown direct efficiency for this peak" << std::endl;
    }
    else if (xray.transition == "Capture843keV") {
      xray.energy = 843.74;
      xray.intensity = 0.099;
      xray.intensity_error = 0.001;//??
      if (channel == "GeLoGain") {
	xray.efficiency = 2.90e-4;
	xray.efficiency_error = 0.07e-4;
      }
      else if (channel == "GeHiGain") {
	xray.efficiency = 2.88e-4;
	xray.efficiency_error = 0.07e-4;
      }
      else {
	std::cout << "Error: Unknown channel (" << channel << ")" << std::endl;
	return 1;
      }
    }
    else if (xray.transition == "Capture1014keV") {
      xray.energy = 1014.42;
      xray.intensity = 0.0921;
      xray.intensity_error = 0.001;//??
      if (channel == "GeLoGain") {
	xray.efficiency = 2.44e-4;
	xray.efficiency_error = 0.001e-4;//??
      }
      else if (channel == "GeHiGain") {
	xray.efficiency = 2.42e-4;
	xray.efficiency_error = 0.001e-4;//??
      }
      else {
	std::cout << "Error: Unknown channel (" << channel << ")" << std::endl;
	return 1;
      }
    }
    
    else {
      std::cout << "Error: Unknown transition (" << xray.transition << ")" << std::endl;
      return 1;
    }
  }
  else if (xray.material == "Ti") {
    std::cout << "Titanium x-ray:" << std::endl;
    xray.transition = "2p-1s";
    //    xray.energy = 942; // PRL V18 N26 (1967)
    xray.energy = 931; // elog:336 and Fig. 1b of the above
    xray.intensity = 0.752; 
    xray.intensity_error = 0.007;
    if (channel == "GeLoGain") {
      xray.efficiency = 2.6e-4;
      xray.efficiency_error = 0.1e-4;
    }
    else if (channel == "GeHiGain") {
      xray.efficiency = 2.6e-4;
      xray.efficiency_error = 0.1e-4;
    }
  }
  else if (xray.material == "511") {
    std::cout << "511 keV peak:" << std::endl;
    xray.transition = "511";
    xray.energy = 511;
    xray.intensity = 1;
    xray.intensity_error = 0;
  }
  else {
    std::cout << "Error: Unknown target material (" << xray.material << ")" << std::endl;
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
      // with Y88
      //      a = 0.1792; delta_a = 0.01533;
      //      b = -0.9332; delta_b = 0.01332;
      //      corr = -0.867502;
      // without Y88
      //      a = 0.164449; delta_a = 0.0157128;
      //      b = -0.919077; delta_b = 0.0151133;
      //      corr = -0.995273;
      // from elog:231 work
      a = 0.144381; delta_a = 0.0138032;
      b = -0.920011; delta_b = 0.0151194;
      corr = -0.995257;
    }
    else if (run == 9685) {
      // from elog:231 work
      a = 0.143518; delta_a = 0.0187903;
      b = -0.932379; delta_b = 0.0207396;
      corr = -0.995257;
    }
    else if (run == 9302) {
      //      a = 0.173809; delta_a = 0.0226852;
      //      b = -0.91031; delta_b = 0.020632;
      //      corr = -0.995468;
      // from elog:231 work
      a = 0.127727; delta_a = 0.0166573;
      b = -0.909965; delta_b = 0.0206178;
      corr = -0.995418;
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

RooWorkspace* FitPeak(std::string wsname, double energy_low, double energy_high, TH1* hSpectrum, XRay* xray) {
  
  // Keep track of the number of parameters
  int n_fit_params = 0;

  // Create the workspace and functions for fitting and create the summed pdf as we go
  RooWorkspace* ws = new RooWorkspace(wsname.c_str(), kFALSE); // don't export to CINT
  std::stringstream factory_cmd, sum_factory_cmd;
  sum_factory_cmd << "SUM::sum(";

  // First, the linear background
  factory_cmd << "Polynomial::pol1_bkg(edep[" << energy_low << ", " << energy_high << "], {bkg_offset[-10, 100], bkg_slope[-0.1, 0.1]})";
  //  factory_cmd << "Exponential::pol1_bkg(edep[" << energy_low << ", " << energy_high << "], expo_slope[-1, -0.00001])";
  ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");
  sum_factory_cmd << "nbkg[0, 500000]*pol1_bkg";
  n_fit_params += 3; // bkg_offset, bkg_slope, nbkg

  // Now the X-ray peak of interest
  factory_cmd << "Gaussian::xraypeak_pdf(edep[" << energy_low << ", " << energy_high << "], xray_mean[" << xray->energy-3 << ", " << xray->energy+3 << "], xray_sigma[1.0, 10])"; // the x-ray peak itself
  ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");
  sum_factory_cmd << ", xray_area[0,500000]*xraypeak_pdf";
  n_fit_params += 3; // xray_mean, xray_sigma, xray_area

  // For Al 2p-1s, we also have a second peak that's a background
  if (xray->material == "Al" && xray->transition == "2p-1s") {
    factory_cmd << "Gaussian::bkgpeak_pdf(edep[" << energy_low << ", " << energy_high << "], bkg_mean[349,352], bkg_sigma[0.1, 10])";
    ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");
    sum_factory_cmd << ", bkg_area[0,500000]*bkgpeak_pdf";
    n_fit_params += 3; // bkg_mean, bkg_sigma, bkg_area
  }

  if (xray->material == "Ti" && xray->transition == "2p-1s") {
    //    factory_cmd << "Gaussian::bkgpeak_pdf(edep[" << energy_low << ", " << energy_high << "], bkg_mean[938,942], bkg_sigma[0.1, 10])";
    factory_cmd << "Gaussian::bkgpeak_pdf(edep[" << energy_low << ", " << energy_high << "], bkg_mean[938,942], xray_sigma)";
    ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");
    sum_factory_cmd << ", bkg_area[0,500000]*bkgpeak_pdf";
    n_fit_params += 2;//3; // bkg_mean, bkg_sigma, bkg_area

    // add the x-ray fine splitting
    factory_cmd << "Gaussian::xraypeak2_pdf(edep[" << energy_low << ", " << energy_high << "], xray_mean2[" << xray->energy-3 << ", " << xray->energy+3 << "], xray_sigma)"; // the x-ray peak itself
    ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");
    sum_factory_cmd << ", xray_area2[0,500000]*xraypeak2_pdf";
    n_fit_params += 3; // xray_mean2, xray_area2
  }

  /* if (xray->material == "Si" && xray->transition == "2p-1s") { */
  /*   factory_cmd << "Gaussian::xraypeak2_pdf(edep[" << energy_low << ", " << energy_high << "], xray_mean2[" << xray->energy-3 << ", " << xray->energy+1 << "], xray_sigma)"; // the x-ray peak itself */
  /*   ws->factory(factory_cmd.str().c_str()); factory_cmd.str(""); */
  /*   sum_factory_cmd << ", xray_area2[0,500000]*xraypeak2_pdf"; */
  /*   n_fit_params += 3; // xray_mean2, xray_area2 */
  /* } */

  
  // Now create the SUM pdf
  sum_factory_cmd << ")";
  ws->factory(sum_factory_cmd.str().c_str()); sum_factory_cmd.str("");

  // Import the histogram into a RooFit data hist
  RooDataHist data("data", "Ge Spectrum", (ws->argSet("edep")), hSpectrum);
  RooFitResult* result = (ws->pdf("sum"))->fitTo(data, RooFit::Save(true), RooFit::Minimizer("Minuit2"));
  ws->import(data);
  ws->import((TObject&)*result);
  //  std::cout << "Goodness of fit: " << Eframe->chiSquare(n_fit_params) << std::endl;
  
  return ws;
}

#endif
