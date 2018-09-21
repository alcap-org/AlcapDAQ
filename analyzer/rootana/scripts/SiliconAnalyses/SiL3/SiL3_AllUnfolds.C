#include "scripts/SiliconAnalyses/SiL3_active/unfold/SiL3_Unfold_FlatBackground.C"
#include "scripts/SiliconAnalyses/SiL3_active/unfold/SiL3_Unfold_DecayElectronCorrection.C"
#include "scripts/SiliconAnalyses/SiL3_active/unfold/SiL3_Unfold_TimeCut.C"

void SiL3_AllUnfolds() {

  std::string infilename = "~/data/results/SiL3_active/raw_spectra.root";
  std::string outfilename = "~/data/results/SiL3_active/unfold.root";

  
  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  outfile->Write();
  outfile->Close();

  double min_time_slice = 1500;
  double max_time_slice = 5000;
  double time_slice_step = 500;

  for (double i_min_time_slice = min_time_slice; i_min_time_slice < max_time_slice; i_min_time_slice += time_slice_step) {
    double i_max_time_slice = i_min_time_slice + time_slice_step;
    //    SiL3_Unfold_FlatBackground(infilename, outfilename, i_min_time_slice, i_max_time_slice);
    //    SiL3_Unfold_DecayElectronCorrection(outfilename, outfilename, i_min_time_slice, i_max_time_slice);
    SiL3_Unfold_DecayElectronCorrection(infilename, outfilename, i_min_time_slice, i_max_time_slice);
    SiL3_Unfold_TimeCut(outfilename, outfilename, i_min_time_slice, i_max_time_slice);
  }
}
