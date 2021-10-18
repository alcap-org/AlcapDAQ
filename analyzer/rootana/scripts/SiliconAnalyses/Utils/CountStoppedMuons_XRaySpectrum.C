#ifndef CountStoppedMuons_XRaySpectrum_C_
#define CountStoppedMuons_XRaySpectrum_C_

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"

#include "RooFitResult.h"

#include "../../XRayAnalysis/XRayUtils.h"

struct CountStoppedMuons_XRaySpectrumArgs {
  std::string infilename;
  std::string channel;
  std::string inhistname;
  std::string outfilename;
  std::string outdirname;
  
  std::string material;
  std::string transition;
  int rebin_factor;
  double fit_window_min;
  double fit_window_max;

  double min_time;
  double max_time;
  bool is2DHist;
};

void CountStoppedMuons_XRaySpectrum(const CountStoppedMuons_XRaySpectrumArgs& args) {

  TFile* file = new TFile(args.infilename.c_str(), "READ");
  if (file->IsZombie()) {
    std::cout << "Problem openeing file " << args.infilename.c_str() << std::endl;
    return;
  }
  TH1F* hGe_Spectrum = 0;
  if (args.is2DHist) {
    TH2F* hEnergyTime = (TH2F*) file->Get(args.inhistname.c_str());
    if (!hEnergyTime) {
      std::cout << "Problem getting histogram " << args.inhistname.c_str() << std::endl;
      return;
    }
  
    // time cuts can go in here
    int min_time_bin = hEnergyTime->GetXaxis()->FindBin(args.min_time);
    int max_time_bin = hEnergyTime->GetXaxis()->FindBin(args.max_time)-1;
    hGe_Spectrum = (TH1F*) hEnergyTime->ProjectionY("_py", min_time_bin, max_time_bin);
  }
  else {
    hGe_Spectrum = (TH1F*) file->Get(args.inhistname.c_str());
  }
  if (!hGe_Spectrum) {
    std::cout << "Problem getting histogram" << std::endl;
    return;
  }

  
  // Define the X-ray we want to look at
  XRay xray;
  xray.material = args.material;
  xray.transition = args.transition;
  if (FillXRayInfo(xray, args.channel) == 1) { // if we return an error code
    std::cout << "Error: Problem getting X-ray information" << std::endl;
    return;
  }
  std::cout << "Efficiency = " << xray.efficiency << " +/- " << xray.efficiency_error << std::endl;

  TH1F* hGe_OriginalBinning = (TH1F*) hGe_Spectrum->Clone("hGe_OriginalBinning");
  int rebin_factor = args.rebin_factor;
  RooFitResult* result = 0;
  RooWorkspace* ws = 0;
  do {
    hGe_Spectrum = (TH1F*) hGe_OriginalBinning->Clone("hGe_Spectrum");
    hGe_Spectrum->Rebin(rebin_factor);
    double xray_energy_low = xray.energy+args.fit_window_min;
    double xray_energy_high = xray.energy+args.fit_window_max;
    
    ws = FitPeak("ws", xray_energy_low, xray_energy_high, hGe_Spectrum, &xray);
    
    result = (RooFitResult*)ws->genobj("fitresult_sum_data");
    std::cout << "Rebin_Factor " << rebin_factor << ", Status = " << result->status() << std::endl;
    ++rebin_factor;
    int n_statuses = result->numStatusHistory();
    std::cout << "AE: n_statuses = " << n_statuses << std::endl;
    for (int i_status = 0; i_status < n_statuses; ++i_status) {
      std::cout << result->statusLabelHistory(i_status) << " " << result->statusCodeHistory(i_status) << std::endl;
    }
  } while(result->status() != 0 && rebin_factor<=1);

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
  
  TTree* indirect_count_tree = new TTree("counttree", "");
  indirect_count_tree->Branch("min_time", args.min_time);
  indirect_count_tree->Branch("max_time", args.max_time);
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
  TDirectory* outdir = outfile->mkdir(args.outdirname.c_str());
  outdir->cd();
  hGe_Spectrum->Write();
  ws->Write();
  indirect_count_tree->Write();
  outfile->Write();
  outfile->Close();
  file->Close();
}

#endif
