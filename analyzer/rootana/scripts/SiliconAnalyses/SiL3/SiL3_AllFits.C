//#include "scripts/SiliconAnalyses/SiL3/fits/SiL3_FlatBkgFits.C"
//#include "scripts/SiliconAnalyses/SiL3/fits/SiL3_LifetimeRooFit.C"
#include "unfold/SiL3_Unfold_EscapeCorrection.C"

#include <sstream>

void SiL3_AllFits() {

  // //  std::string infilename = "~/data/results/SiL3/raw_spectra.root";
  // std::string infilename = "~/data/results/SiL3/plots.root";
  // std::string outfilename = "~/data/results/SiL3/fits.root";
    
  // TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  // outfile->Write();
  // outfile->Close();
  
  // //  SiL3_FlatBkgFits(infilename, outfilename);
  // SiL3_LifetimeRooFit(infilename, outfilename);

  std::string infilename = "~/data/results/SiL3/systematics_geq2TgtPulse_newPP20us_2.root";
  std::string outfilename = "~/data/results/SiL3/fits_geq2TgtPulse_newPP20us_2.root";
  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  outfile->Write();
  outfile->Close();

  const int n_bin_widths = 3;
  std::string bin_widths[n_bin_widths] = {"100keVBins", "200keVBins", "500keVBins"};
  double bin_width_vals[n_bin_widths] = {0.1, 0.2, 0.5};
  int rebin_factors[n_bin_widths] = {1, 2, 5};

  const int n_response_matrices = 5;
  std::string response_matrices[n_response_matrices] = {"combined", "proton", "alpha", "combined", "syst"};
  std::string recoils[n_response_matrices] = {"allRecoil", "pRecoil", "aRecoil", "noRecoil", "systRecoil"};

  const int n_mustop_positions = 1;//3;
  std::string mustop_positions[n_mustop_positions] = {""};//, "lowerE", "higherE"};
  
  double min_E = 1.5;
  double max_E = 30;
  double E_step = 30;
  std::stringstream fit_range_str;
  for (int i_bin_width = 0; i_bin_width < n_bin_widths; ++i_bin_width) {
    std::string bin_width = bin_widths[i_bin_width];
    //    int rebin_factor = rebin_factors[i_bin_width];
    double bin_width_val = bin_width_vals[i_bin_width];

    for (int i_response_matrix = 0; i_response_matrix < n_response_matrices; ++i_response_matrix) {
      std::string response_matrix = response_matrices[i_response_matrix];
      std::string recoil = recoils[i_response_matrix];

      for (int i_mustop_position = 0; i_mustop_position < n_mustop_positions; ++i_mustop_position) {
	std::string mustop_position = mustop_positions[i_mustop_position];
	
	for (double i_min_E = min_E; i_min_E < max_E; i_min_E += E_step) {
	  for (double i_max_E = max_E; i_max_E > i_min_E; i_max_E -= E_step) {
	    fit_range_str.str("");
	    fit_range_str << "FitRange" << i_min_E << "_" << i_max_E << "_" << bin_width;
	    std::string inhistname = "FinalSystPlot_TimeSlice3000_4000_" + recoil + "_" + bin_width + "/hFinalStatSyst";
	    std::string outdirname = "FinalFit_" + fit_range_str.str() + "_" + response_matrix + "_" + mustop_position + "_" + recoil;//std::to_string(i_response_matrix);
	    std::cout << outdirname << std::endl;
	    SiL3_Unfold_EscapeCorrection(infilename, outfilename, inhistname, outdirname, response_matrix, mustop_position, i_min_E, i_max_E, bin_width_val);
	  }
	}
      }
    }
  }
}
