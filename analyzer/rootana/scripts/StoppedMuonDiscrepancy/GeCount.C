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

struct XRay {
  std::string transition;
  std::string material;
  double energy;
  double intensity;
  double intensity_error;
  double efficiency;
  double efficiency_error;
};

RooRealVar* GetAreaUnderPeak(double energy_low, double energy_high, TH1* hSpectrum, XRay* xray, TPad* pad);

// Takes a filename of a rootana output file as well as information on the timing cut and interesting x-ray
void GeCount(TFile* file, TFile* out_file, TH1F* hHistToFill) {

  int rebin_factor = 1;

  // Now define the X-ray we want to look at
  XRay xray;
  xray.material = "Si";
  xray.transition = "2p-1s";
  xray.energy = 400.177;
  xray.intensity = 0.803;
  xray.intensity_error = 0.008;
  
  // Here is the germanium effiency fit for R15b
  // Run 9302 GeLoGain
  double a = 0.173809; double delta_a = 0.0226852;
  double b = -0.91031; double delta_b = 0.020632;
  double corr = -0.995468;
  
  TF1* ge_eff = new TF1("ge_eff", "[0]*(x^[1])");
  ge_eff->SetParameters(a, b);
  ge_eff->SetParError(0, delta_a);
  ge_eff->SetParError(1, delta_b);
  xray.efficiency = ge_eff->Eval(xray.energy);
  
  // Assuming uncertainty in the energy is small
  TF1* ge_eff_err = new TF1("ge_eff_err", "sqrt(x^(2*[1]) * ([2]^2 + TMath::Log(x)*[0]*[3]*([0]*[3]*TMath::Log(x) + 2*[4]*[2])))");
  ge_eff_err->SetParameters(a, b, delta_a, delta_b, corr);
  xray.efficiency_error = ge_eff_err->Eval(xray.energy);
  
  // Now get the area under the X-ray peak by doing a fit to the spectrum
  double energy_low = xray.energy-10;
  double energy_high = xray.energy+10;
  
  const int n_si_channels_total = 17;
  const int n_sil1_channels = 16;
  std::string histnames[n_si_channels_total];

  std::stringstream histname;
  for (int i_sil1_channel = 0; i_sil1_channel < n_sil1_channels; ++i_sil1_channel) {
    histname.str("");
    histname << "PlotTAP_StoppedMuonDiscrepancy/hGeLoGain_Energy_wSiL1-" << i_sil1_channel+1 << "-SCoincidence";
    histnames[i_sil1_channel] = histname.str();
  }
  histnames[16] = "PlotTAP_StoppedMuonDiscrepancy/hGeLoGain_Energy_wSiL3-SCoincidence";

  for (int i_si_channel = 0; i_si_channel < n_si_channels_total; ++i_si_channel) {
    if (i_si_channel == 0 || i_si_channel == 15) { // ignoring SiL1-1-S and SiL1-16-S
      continue;
    }

    std::string histname = histnames[i_si_channel];
    TH1F* hEnergy = (TH1F*) (file->Get(histname.c_str())->Clone());
    hEnergy->Rebin(rebin_factor);

    TPad* pad = new TPad();
    RooRealVar* area = GetAreaUnderPeak(energy_low, energy_high, hEnergy, &xray, pad);
    //  std::cout << "Area under the curve = " << area->getValV() << " +- " << area->getError() << std::endl;

    // Factors to account for the detector effects
    std::vector<double> detector_effects;
    //  detector_effects.push_back(1.04); detector_effects.push_back(1.01); // For R13 (dead time and TRP reset respectively)
    // TODO: Add detector effects for R15b
    
    // Now calculate the number of stopped muons
    double n_stopped_muons = area->getValV() / (xray.intensity * xray.efficiency);
    for (std::vector<double>::const_iterator i_factor = detector_effects.begin(); i_factor != detector_effects.end(); ++i_factor) {
      n_stopped_muons *= (*i_factor);
    }

    // ...and the uncertainty
    double n_stopped_muons_error = n_stopped_muons* sqrt( (area->getError()/area->getValV())*(area->getError()/area->getValV()) +
							  (xray.intensity_error/xray.intensity)*(xray.intensity_error/xray.intensity) +
							  (xray.efficiency_error/xray.efficiency)*(xray.efficiency_error/xray.efficiency) );
							    

    std::cout << "XRay: " << xray.material << " " << xray.transition << " " << xray.energy << " keV" << std::endl;
    std::cout << "Area = " << area->getValV() << " +/- " << area->getError() << std::endl;
    std::cout << "Intensity = " << xray.intensity << " +/- " << xray.intensity_error << std::endl;
    std::cout << "Efficiency = " << xray.efficiency << " +/- " << xray.efficiency_error << std::endl;
    std::cout << "Number of Stopped Muons = " << n_stopped_muons << " +/- " << n_stopped_muons_error << std::endl;

    hHistToFill->SetBinContent(i_si_channel+1, n_stopped_muons);
    hHistToFill->SetBinError(i_si_channel+1, n_stopped_muons_error);
  }
}

RooRealVar* GetAreaUnderPeak(double energy_low, double energy_high, TH1* hSpectrum, XRay* xray, TPad* pad) {
  
  // Keep track of the number of parameters
  int n_fit_params = 0;

  // Create the workspace and functions for fitting and create the summed pdf as we go
  RooWorkspace* ws = new RooWorkspace("ws", kTRUE);
  std::stringstream factory_cmd, sum_factory_cmd;
  sum_factory_cmd << "SUM::sum(";

  // First, the linear background
  factory_cmd << "Polynomial::pol1_bkg(edep[" << energy_low << ", " << energy_high << "], {bkg_offset[0, 10000]})";//, bkg_slope[-0.1, 0.1]})";
  ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");
  sum_factory_cmd << "nbkg[0, 10000]*pol1_bkg";
  n_fit_params += 2; // bkg_offset, bkg_slope, nbkg

  // Now the X-ray peak of interest
  factory_cmd << "Gaussian::xraypeak_pdf(edep[" << energy_low << ", " << energy_high << "], xray_mean[" << xray->energy-3 << ", " << xray->energy+3 << "], xray_sigma[0.1, 10])"; // the x-ray peak itself
  ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");
  sum_factory_cmd << ", xray_area[0,10000]*xraypeak_pdf";
  n_fit_params += 3; // xray_mean, xray_sigma, xray_area

  // For Al 2p-1s, we also have a second peak that's a background
  if (xray->material == "Al" && xray->transition == "2p-1s") {
    factory_cmd << "Gaussian::bkgpeak_pdf(edep[" << energy_low << ", " << energy_high << "], bkg_mean[351,352], bkg_sigma[0.1, 10])";
    ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");
    sum_factory_cmd << ", bkg_area[0,500]*bkgpeak_pdf";
    n_fit_params += 3; // bkg_mean, bkg_sigma, bkg_area
  }

  // Now create the SUM pdf
  sum_factory_cmd << ")";
  ws->factory(sum_factory_cmd.str().c_str()); sum_factory_cmd.str("");

  // Import the histogram into a RooFit data hist
  RooDataHist data("data", "Ge Spectrum", (ws->argSet("edep")), hSpectrum);
  (ws->pdf("sum"))->fitTo(data);
  
  // Draw the fit
  pad->cd();
  RooPlot* Eframe = (ws->var("edep"))->frame();
  Eframe->SetTitle(hSpectrum->GetTitle());
  Eframe->GetXaxis()->SetTitle("Energy [keV]");
  data.plotOn(Eframe);
  (ws->pdf("sum"))->plotOn(Eframe);
  //  (ws->pdf("sum"))->plotOn(Eframe, RooFit::Components(ws->argSet("xraypeak_pdf")), RooFit::LineColor(kRed), RooFit::LineStyle(kDashed));
  std::cout << "Goodness of fit: " << Eframe->chiSquare(n_fit_params) << std::endl;
  Eframe->Draw();

  return ws->var("xray_area");
}
