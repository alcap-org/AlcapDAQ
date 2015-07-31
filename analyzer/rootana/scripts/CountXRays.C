#include "TFile.h"
#include "TDirectoryFile.h"
#include "TH2.h"

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

int FillXRayInfo(XRay* xray);
RooRealVar* GetAreaUnderPeak(double energy_low, double energy_high, TH1* hSpectrum, XRay* xray);

// Takes a filename of a rootana output file as well as information on the timing cut and interesting x-ray
int CountXRays(std::string filename, double time_cut, int rebin_factor=1, double energy_low = 343.7, double energy_high = 355, std::string target_material="Al") {

  TFile* file = new TFile(filename.c_str(), "READ");
  if (file->IsZombie()) {
    std::cout << "Error: Problem opening file " << filename.c_str() << std::endl;
    return 1;
  }
  // Check that the GeSpectrum folder was created
  TDirectoryFile* ge_spectrum_dir = (TDirectoryFile*) file->Get("GeSpectrum");
  if (!ge_spectrum_dir) {
    std::cout << "Error: GeSpectrum folder doesn't exist in output file" << std::endl;
    return 1;
  }

  // Get the 2D time-energy histogram
  TH2F* hTimeEnergy = (TH2F*) ge_spectrum_dir->Get("hTimeEnergy");
  if (!hTimeEnergy) {
    std::cout << "Error: Could not find hTimeEnergy" << std::endl;
    return 1;
  }

  // Apply the time cut and rebin
  TH1* hEnergyTimeCut = hTimeEnergy->ProjectionY("_py", hTimeEnergy->GetXaxis()->FindBin(-time_cut), hTimeEnergy->GetXaxis()->FindBin(time_cut));
  hEnergyTimeCut->Rebin(rebin_factor);

  // Now define the X-ray we want to look at
  XRay xray;
  xray.material = target_material;
  if (FillXRayInfo(&xray) == 1) { // if we return an error code
    std::cout << "Error: Problem getting X-ray information" << std::endl;
    return 1;
  }

  // Now get the area under the X-ray peak by doing a fit to the spectrum
  RooRealVar* area = GetAreaUnderPeak(energy_low, energy_high, hEnergyTimeCut, &xray);
  //  std::cout << "Area under the curve = " << area->getValV() << " +- " << area->getError() << std::endl;

  // Factors to account for the detector effects
  std::vector<double> detector_effects;
  detector_effects.push_back(1.04); detector_effects.push_back(1.01); // For R13 (dead time and TRP reset respectively)

  // Now calculate the number of stopped muons
  double n_stopped_muons = area->getValV() / (xray.intensity * xray.efficiency);
  for (std::vector<double>::const_iterator i_factor = detector_effects.begin(); i_factor != detector_effects.end(); ++i_factor) {
    n_stopped_muons *= (*i_factor);
  }

  // ...and the uncertainty
  double n_stopped_muons_error = n_stopped_muons* sqrt( (area->getError()/area->getValV())*(area->getError()/area->getValV()) +
							(xray.intensity_error/xray.intensity)*(xray.intensity_error/xray.intensity) +
							(xray.efficiency_error/xray.efficiency)*(xray.efficiency_error/xray.efficiency) );
							    

  std::cout << "Number of Stopped Muons = " << n_stopped_muons << " +- " << n_stopped_muons_error << std::endl;
  //  hEnergyTimeCut->Draw();
  return 0;
}

int FillXRayInfo(XRay* xray) {
  if (xray->material == "Al") {
    std::cout << "Aluminium X-ray:" << std::endl;
    xray->transition = "2p-1s";
    xray->energy = 346.828;
    xray->intensity = 0.798;
    xray->intensity_error = 0.008;

    // For R13
    xray->efficiency = 5e-4;
    xray->efficiency_error = 0.1e-4;
    return 0;
  }
  else if (xray->material == "Si") {
    std::cout << "Silicon x-ray (not currently implemented)" << std::endl;
    return 1;
  }
  else {
    std::cout << "Error: Unknown target material" << std::endl;
    return 1;
  }
  return 0;
}

RooRealVar* GetAreaUnderPeak(double energy_low, double energy_high, TH1* hSpectrum, XRay* xray) {
  // Create the workspace and functions for fitting and create the summed pdf as we go
  RooWorkspace* ws = new RooWorkspace("ws", kTRUE);
  std::stringstream factory_cmd, sum_factory_cmd;
  sum_factory_cmd << "SUM::sum(";

  // First, the linear background
  factory_cmd << "Polynomial::pol1_bkg(edep[" << energy_low << ", " << energy_high << "], {bkg_offset[-10, 100], bkg_slope[-0.1, 0.1]})";
  ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");
  sum_factory_cmd << "nbkg[0, 50000]*pol1_bkg";

  // Now the X-ray peak of interest
  factory_cmd << "Gaussian::xraypeak_pdf(edep[" << energy_low << ", " << energy_high << "], xray_mean[" << xray->energy-1 << ", " << xray->energy+1 << "], xray_sigma[0.1, 10])"; // the x-ray peak itself
  ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");
  sum_factory_cmd << ", xray_amp[0,50000]*xraypeak_pdf";

  // For Al 2p-1s, we also have a second peak that's a background
  if (xray->material == "Al" && xray->transition == "2p-1s") {
    factory_cmd << "Gaussian::bkgpeak_pdf(edep[" << energy_low << ", " << energy_high << "], bkg_mean[351,352], bkg_sigma[0.1, 10])";
    ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");
    sum_factory_cmd << ", bkg_amp[0,50000]*bkgpeak_pdf";
  }

  // Now create the SUM pdf
  sum_factory_cmd << ")";
  ws->factory(sum_factory_cmd.str().c_str()); sum_factory_cmd.str("");

  // Import the histogram into a RooFit data hist
  RooDataHist data("data", "Ge Spectrum", (ws->argSet("edep")), hSpectrum);
  (ws->pdf("sum"))->fitTo(data);
  
  // Draw the fit
  RooPlot* Eframe = (ws->var("edep"))->frame();
  data.plotOn(Eframe);
  (ws->pdf("sum"))->plotOn(Eframe);
  std::cout << "Goodness of fit: " << Eframe->chiSquare(9) << std::endl;
  Eframe->Draw();

  return ws->var("xray_amp");
}
