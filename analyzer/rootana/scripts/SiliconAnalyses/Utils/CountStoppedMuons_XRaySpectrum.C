#ifndef CountStoppedMuons_XRaySpectrum_C_
#define CountStoppedMuons_XRaySpectrum_C_

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"

#include "scripts/XRayAnalysis/XRayUtils.h"

struct CountStoppedMuons_XRaySpectrumArgs {
  std::string infilename;
  std::string channel;
  std::string inhistname;
  std::string outfilename;
  std::string outtreename;
  std::string outwsname;
  
  std::string material;
  std::string transition;
  int rebin_factor;
  int ge_eff_run;

  double min_time;
  double max_time;
};

void CountStoppedMuons_XRaySpectrum(const CountStoppedMuons_XRaySpectrumArgs& args) {

  TFile* file = new TFile(args.infilename.c_str(), "READ");
  TH2F* hEnergyTime = (TH2F*) file->Get(args.inhistname.c_str());

  // time cuts can go in here
  int min_time_bin = hEnergyTime->GetXaxis()->FindBin(args.min_time);
  int max_time_bin = hEnergyTime->GetXaxis()->FindBin(args.max_time)-1;
  TH1F* hGe_Spectrum = (TH1F*) hEnergyTime->ProjectionY("_py", min_time_bin, max_time_bin);
  hGe_Spectrum->Rebin(args.rebin_factor);
  
  // Get germanium efficiency curves
  double a, b, delta_a, delta_b, corr;
  if (FillGeEffParams(args.channel, args.ge_eff_run, a, b, delta_a, delta_b, corr) == 1) {
    std::cout << "Error from FillGeEffParams. Aborting..." << std::endl;
    return;
  }
  TF1* ge_eff = NULL;
  TF1* ge_eff_err = NULL;
  if (FillGeEffFunctions(a, b, delta_a, delta_b, corr, ge_eff, ge_eff_err) == 1) {
    std::cout << "Error from FillGeEffFunctions. Aborting..." << std::endl;
    return;
  }
  
  // Define the X-ray we want to look at
  XRay xray;
  xray.material = args.material;
  xray.transition = args.transition;
  if (FillXRayInfo(xray) == 1) { // if we return an error code
    std::cout << "Error: Problem getting X-ray information" << std::endl;
    return;
  }
  xray.efficiency = ge_eff->Eval(xray.energy);
  xray.efficiency_error = ge_eff_err->Eval(xray.energy);
  std::cout << "Efficiency = " << xray.efficiency << " +/- " << xray.efficiency_error << std::endl;

  double xray_energy_low = xray.energy-10;
  double xray_energy_high = xray.energy+10;
  RooWorkspace* ws = FitPeak(args.outwsname, xray_energy_low, xray_energy_high, hGe_Spectrum, &xray);

  RooRealVar* area = ws->var("xray_area");
  double xray_count = area->getValV();
  double xray_count_error = area->getError();
  std::cout << "AE: " << xray_count << " " << xray.intensity << " " << xray.efficiency << std::endl;
  double n_stopped_muons = xray_count / (xray.intensity * xray.efficiency);
  // Factors to account for the detector effects
  std::vector<double> detector_effects;
  //  detector_effects.push_back(1.04); detector_effects.push_back(1.01); // For R13 (dead time and TRP reset respectively)
  // TODO: Add detector effects for R15b
  for (std::vector<double>::const_iterator i_factor = detector_effects.begin(); i_factor != detector_effects.end(); ++i_factor) {
    n_stopped_muons *= (*i_factor);
  }

  double n_stopped_muons_error = n_stopped_muons* sqrt( (xray_count_error/xray_count)*(xray_count_error/xray_count) +
							(xray.intensity_error/xray.intensity)*(xray.intensity_error/xray.intensity) +
							(xray.efficiency_error/xray.efficiency)*(xray.efficiency_error/xray.efficiency) );
  
  std::cout << "XRay: " << xray.material << " " << xray.transition << " " << xray.energy << " keV" << std::endl;
  std::cout << "Area = " << xray_count << " +/- " << xray_count_error << " (" << (xray_count_error / xray_count) * 100 << "%)" << std::endl;
  std::cout << "Intensity = " << xray.intensity << " +/- " << xray.intensity_error << " (" << (xray.intensity_error / xray.intensity) * 100 << "%)" << std::endl;
  std::cout << "Efficiency = " << xray.efficiency << " +/- " << xray.efficiency_error << " (" << (xray.efficiency_error / xray.efficiency) * 100 << "%)" << std::endl;
  std::cout << "Number of Stopped Muons = " << n_stopped_muons << " +- " << n_stopped_muons_error << " (" << (n_stopped_muons_error / n_stopped_muons) * 100 << "%)" << std::endl;
  
  TTree* indirect_count_tree = new TTree(args.outtreename.c_str(), "");
  indirect_count_tree->Branch("xray_count", &xray_count);
  indirect_count_tree->Branch("xray_count_error", &xray_count_error);
  indirect_count_tree->Branch("xray_energy", &xray.energy);
  indirect_count_tree->Branch("xray_material", &xray.material);
  indirect_count_tree->Branch("xray_transition", &xray.transition);
  indirect_count_tree->Branch("xray_intensity", &xray.intensity);
  indirect_count_tree->Branch("xray_intensity_error", &xray.intensity_error);
  indirect_count_tree->Branch("xray_efficiency", &xray.efficiency);
  indirect_count_tree->Branch("xray_efficiency_error", &xray.efficiency_error);
  indirect_count_tree->Branch("n_stopped_muons", &n_stopped_muons);
  indirect_count_tree->Branch("n_stopped_muons_error", &n_stopped_muons_error);

  indirect_count_tree->Fill();
  
  TFile* outfile = new TFile(args.outfilename.c_str(), "UPDATE");
  hGe_Spectrum->Write();
  ws->Write();
  ge_eff->Write();
  ge_eff_err->Write();
  indirect_count_tree->Write();
  outfile->Write();
  outfile->Close();
}

#endif
