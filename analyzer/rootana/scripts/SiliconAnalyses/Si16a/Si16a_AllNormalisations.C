#include "scripts/SiliconAnalyses/Si16a_useful/Si16a_CountStoppedMuons_XRaySpectrum.C"
#include "scripts/SiliconAnalyses/Si16a_useful/Si16a_CountStoppedMuons_TargetSpectrum.C"
#include "scripts/SiliconAnalyses/Si16a_useful/Si16a_CountStoppedMuons_XRaySpectrum_wTargetCoinc.C"

#include <sstream>

void Si16a_AllNormalisations(int run_number) {

  std::stringstream run_str;
  run_str << "0" << run_number;

  std::string infilename = "~/data/results/Si16a_useful/plots" + run_str.str() + ".root";
  std::string outfilename = "~/data/results/Si16a_useful/normalisation" + run_str.str() + ".root";

  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  outfile->Write();
  outfile->Close();

  //  Si16a_CountStoppedMuons_XRaySpectrum(infilename, outfilename);
  //  Si16a_CountStoppedMuons_TargetSpectrum(infilename, outfilename);

  double min_muon_energy = 0;
  double max_muon_energy = 2000;
  double muon_slice_width = 400;

  for (double i_muon_slice_start = min_muon_energy; i_muon_slice_start < max_muon_energy; i_muon_slice_start += muon_slice_width) {
    double i_muon_slice_end = i_muon_slice_start + muon_slice_width;

    Si16a_CountStoppedMuons_XRaySpectrum_wTargetCoinc(infilename, outfilename, i_muon_slice_start, i_muon_slice_end);
  }

  // For the ge efficiency check
  Si16a_CountStoppedMuons_XRaySpectrum_wTargetCoinc(infilename, outfilename, 500, 1500);
}
