//#include "scripts/SiliconAnalyses/Si16b/fits/Si16b_FlatBkgFits.C"
//#include "scripts/SiliconAnalyses/Si16b/fits/Si16b_LifetimeRooFit.C"
#include "unfold/Si16b_Unfold_NormaliseRawSpectrum.C"
#include "unfold/Si16b_Unfold_EscapeCorrection.C"

#include <sstream>

void Si16b_AllFits() {

  // //  std::string infilename = "~/data/results/Si16b/raw_spectra.root";
  // std::string infilename = "~/data/results/Si16b/plots.root";
  // std::string outfilename = "~/data/results/Si16b/fits.root";
    
  // TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  // outfile->Write();
  // outfile->Close();
  
  // //  Si16b_FlatBkgFits(infilename, outfilename);
  // Si16b_LifetimeRooFit(infilename, outfilename);

  //  std::string infilename = "~/data/results/Si16b/raw_spectra_newPP_geq1TgtPulse_JohnVetoEff_1.root";
  std::string infilename = "/data/ssd3/R15bAnalysis/hists/si16b-folded.root";
  std::string outfilename = "~/data/results/Si16b/fits_newPP_geq1TgtPulse_2.root";
  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  outfile->Write();
  outfile->Close();

  const int n_particles = 4;
  std::string particles[n_particles] = {"proton", "deuteron", "triton", "alpha"};
  std::string ps[n_particles] = {"p", "d", "t", "a"};
  double min_Es[n_particles] = {4, 5, 6, 15};
  double max_Es[n_particles] = {20, 16, 17, 20};
  
  const int n_bin_widths = 1;
  std::string bin_widths[n_bin_widths] = {"500keVBins"};//, "200keVBins", "500keVBins"};
  //  int rebin_factors[n_bin_widths] = {10};//, 2, 5};
  double bin_width_vals[n_bin_widths] = {0.5};
  
  //  double min_E = 3;
  //  double max_E = 15;
  //  double E_step = 30;
  std::string arm = "SiR";
  std::stringstream fit_range_str;
  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    std::string particle = particles[i_particle];
    std::string p = ps[i_particle];
    
    for (int i_bin_width = 0; i_bin_width < n_bin_widths; ++i_bin_width) {
      std::string bin_width = bin_widths[i_bin_width];
      //      int rebin_factor = rebin_factors[i_bin_width];
      double bin_width_val = bin_width_vals[i_bin_width];

      std::string inhistname = "h" + p + arm;
      std::string outdirname = "Unfold_NormaliseRawSpectra_" + arm + "_" + particle;

      double pid_eff = 0.95;
      double time_cut_eff = 0.941;
      double geom_acceptance = 0.01253; // SiR-only
      double n_stopped_muons = 31.4e6;
      double n_captured_muons = n_stopped_muons*0.658;
      double factor = pid_eff*time_cut_eff*geom_acceptance*n_captured_muons*bin_width_val;
      Si16b_Unfold_NormaliseRawSpectrum(infilename, outfilename, inhistname, outdirname, factor);

      //for (double i_min_E = min_E; i_min_E < max_E; i_min_E += E_step) {
      //	for (double i_max_E = max_E; i_max_E > i_min_E; i_max_E -= E_step) {
      double i_min_E = min_Es[i_particle];
      double i_max_E = max_Es[i_particle];
      fit_range_str.str("");
      fit_range_str << "FitRange" << i_min_E << "_" << i_max_E << "_" << bin_width;
      inhistname = "Unfold_NormaliseRawSpectra_" + arm + "_" + particle + "/hCorrectedSpectrum";
      outdirname = "FinalFit_" + arm + "_" + particle;
      std::cout << outdirname << std::endl;
	  //	  if (particle=="alpha") {
	  //	    rebin_factor *= 2;
	  //	  }
      Si16b_Unfold_EscapeCorrection(outfilename, outfilename, inhistname, outdirname, i_min_E, i_max_E, particle, bin_width_val, arm);
	}
    //      }
    //    }
  }
}
