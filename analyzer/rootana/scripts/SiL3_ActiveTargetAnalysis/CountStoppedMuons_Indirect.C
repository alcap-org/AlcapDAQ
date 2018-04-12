#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"

#include "scripts/XRayAnalysis/XRayUtils.h"

int CountStoppedMuons_Indirect() {

  
  //////////////////
  // User Parameters
  std::string channel = "GeHiGain";
  std::string dataset = "SiL3_active";
  std::string filename = "/home/edmonds/data/results/" + dataset + "/XRaySpectrum_" + channel + ".root";
  std::string histname = "hGe_FullSpectrum";
  std::string outfilename = "/home/edmonds/data/results/" + dataset + "/CountStoppedMuons_" + channel + ".root";
  std::string material = "Si";
  int ge_eff_run = 10319; // can be 10319 or 9302


  ///////////////////////////////////
  // Get previous plots
  TFile* file = new TFile(filename.c_str(), "READ");
  if (file->IsZombie()) {
    std::cout << "Trouble opening file " << filename << std::endl;
    return 1;
  }
  
  TH1F* hGe_Spectrum = (TH1F*) file->Get(histname.c_str());
  if (!hGe_Spectrum) {
    std::cout << "Trouble finding histogram " << histname << std::endl;
    return 1;
  }
  hGe_Spectrum->SetName("hGe_Spectrum");

  ////////////////////////////////
  // Make new plots
  
  // Get germanium efficiency curves
  double a, b, delta_a, delta_b, corr;
  if (FillGeEffParams(channel, ge_eff_run, a, b, delta_a, delta_b, corr) == 1) {
    std::cout << "Error from FillGeEffParams. Aborting..." << std::endl;
    return 1;
  }

  TF1* ge_eff = NULL;
  TF1* ge_eff_err = NULL;
  if (FillGeEffFunctions(a, b, delta_a, delta_b, corr, ge_eff, ge_eff_err) == 1) {
    std::cout << "Error from FillGeEffFunctions. Aborting..." << std::endl;
    return 1;
  }
  
  // Define the X-ray we want to look at
  XRay xray;
  xray.material = material;
  if (FillXRayInfo(xray) == 1) { // if we return an error code
    std::cout << "Error: Problem getting X-ray information" << std::endl;
    return 1;
  }
  xray.efficiency = ge_eff->Eval(xray.energy);
  xray.efficiency_error = ge_eff_err->Eval(xray.energy);
  std::cout << "Efficiency = " << xray.efficiency << " +/- " << xray.efficiency_error << std::endl;

  double xray_energy_low = xray.energy-10;
  double xray_energy_high = xray.energy+10;
  RooWorkspace* ws = FitPeak(xray_energy_low, xray_energy_high, hGe_Spectrum, &xray);

  RooRealVar* area = ws->var("xray_area");
  std::cout << "AE: " << area->getValV() << " " << xray.intensity << " " << xray.efficiency << std::endl;
  double n_stopped_muons = area->getValV() / (xray.intensity * xray.efficiency);
  // Factors to account for the detector effects
  std::vector<double> detector_effects;
  //  detector_effects.push_back(1.04); detector_effects.push_back(1.01); // For R13 (dead time and TRP reset respectively)
  // TODO: Add detector effects for R15b
  for (std::vector<double>::const_iterator i_factor = detector_effects.begin(); i_factor != detector_effects.end(); ++i_factor) {
    n_stopped_muons *= (*i_factor);
  }

  double n_stopped_muons_error = n_stopped_muons* sqrt( (area->getError()/area->getValV())*(area->getError()/area->getValV()) +
							(xray.intensity_error/xray.intensity)*(xray.intensity_error/xray.intensity) +
							(xray.efficiency_error/xray.efficiency)*(xray.efficiency_error/xray.efficiency) );
  
  std::cout << "XRay: " << xray.material << " " << xray.transition << " " << xray.energy << " keV" << std::endl;
  std::cout << "Area = " << area->getValV() << " +/- " << area->getError() << " (" << (area->getError() / area->getValV()) * 100 << "%)" << std::endl;
  std::cout << "Intensity = " << xray.intensity << " +/- " << xray.intensity_error << " (" << (xray.intensity_error / xray.intensity) * 100 << "%)" << std::endl;
  std::cout << "Efficiency = " << xray.efficiency << " +/- " << xray.efficiency_error << " (" << (xray.efficiency_error / xray.efficiency) * 100 << "%)" << std::endl;
  std::cout << "Number of Stopped Muons = " << n_stopped_muons << " +- " << n_stopped_muons_error << " (" << (n_stopped_muons_error / n_stopped_muons) * 100 << "%)" << std::endl;

  TTree* mustops_tree = new TTree("mustops", "");
  mustops_tree->Branch("ge_eff_a", &a);
  mustops_tree->Branch("ge_eff_b", &b);
  mustops_tree->Branch("ge_eff_delta_a", &delta_a);
  mustops_tree->Branch("ge_eff_delta_b", &delta_b);
  mustops_tree->Branch("ge_eff_corr", &corr);
  mustops_tree->Branch("xray_energy", &xray.energy);
  mustops_tree->Branch("xray_material", &xray.material);
  mustops_tree->Branch("xray_transition", &xray.transition);
  mustops_tree->Branch("xray_intensity", &xray.intensity);
  mustops_tree->Branch("xray_efficiency", &xray.efficiency);
  mustops_tree->Branch("n_stopped_muons", &n_stopped_muons);
  mustops_tree->Branch("n_stopped_muons_error", &n_stopped_muons_error);

  mustops_tree->Fill();
  
  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  hGe_Spectrum->Write();
  ws->Write();
  mustops_tree->Write();
  outfile->Write();
  outfile->Close();
}
