//#include "scripts/SiliconAnalyses/SiL3RangeScan/plot/SiL3RangeScan_XRaySpectrum.C"
#include "scripts/SiliconAnalyses/SiL3RangeScan/plot/SiL3RangeScan_TargetSpectrum.C"
//#include "scripts/SiliconAnalyses/SiL3RangeScan/plot/SiL3RangeScan_XRaySpectrum_wTargetCoinc.C"

void SiL3RangeScan_AllPlots() {

  const int n_runs = 11;
  int run_numbers[n_runs] = {8986, 8987, 8988, 8989, 8990, 8991, 8992, 8993, 8994, 8995, 8996};

  for (int i_run = 0; i_run < n_runs; ++i_run) {
    std::stringstream run_str;
    run_str << "0" << run_numbers[i_run];

    std::string infilename = "~/data/results/SiL3RangeScan/subtrees" + run_str.str() + ".root";
    std::string outfilename = "~/data/results/SiL3RangeScan/plots" + run_str.str() + ".root";

    TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
    outfile->Write();
    outfile->Close();
    
    //  SiL3RangeScan_XRaySpectrum(infilename, outfilename);
    SiL3RangeScan_TargetSpectrum(infilename, outfilename);
    //  SiL3RangeScan_XRaySpectrum_wTargetCoinc(infilename, outfilename);
  }
}
