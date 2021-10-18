//#include "scripts/SiliconAnalyses/Ti50/fits/Ti50_FlatBkgFits.C"
//#include "scripts/SiliconAnalyses/Ti50/fits/Ti50_LifetimeRooFit.C"
#include "unfold/Ti50_Unfold_EscapeCorrection.C"
#include "unfold/Ti50_Unfold_NormaliseRawSpectrum.C"

#include <sstream>

void Ti50_AllFits() {

  // //  std::string infilename = "~/data/results/Ti50/raw_spectra.root";
  // std::string infilename = "~/data/results/Ti50/plots.root";
  // std::string outfilename = "~/data/results/Ti50/fits.root";
    
  // TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  // outfile->Write();
  // outfile->Close();
  
  // //  Ti50_FlatBkgFits(infilename, outfilename);
  // Ti50_LifetimeRooFit(infilename, outfilename);

  //  std::string infilename = "/data/ssd3/R15bAnalysis/jrquirk/hists.Ti50.pp10us.bw1.3layers.pidpow2.root";
  //  std::string infilename = "~/data/results/Ti50/raw_spectra_fromMarkTree.root";
  //    std::string infilename = "/home/m-wong/data/out/Ti50-output.root";
  std::string infilename = "/data/ssd3/R15bAnalysis/hists/ti50-folded.root";
  std::string outfilename = "~/data/results/Ti50/fits_fromMarkHists.root";
  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  outfile->Write();
  outfile->Close();

  const int n_particles = 4;
  std::string particles[n_particles] = {"proton", "deuteron", "triton", "alpha"};
  std::string ps[n_particles] = {"p", "d", "t", "a"};
  //  double geom_acceptances[n_particles] = {837335/100e6, 873107/100e6, 983362/100e6, 700969/100e6};
  
  const int n_bin_widths = 1;
  std::string bin_widths[n_bin_widths] = {"500keVBins"};//, "200keVBins", "500keVBins"};
  double bin_width_vals[n_bin_widths] = {0.5};

  const int n_arms = 2;
  std::string arms[n_arms] = {"SiR", "SiL"};

  //  double min_E = 3;
  //  double max_E = 15;
  //  double E_step = 30;
  double geom_acceptances[n_arms] = {0.01253, 0.01062};
  double min_Es[n_arms][n_particles] = { {4, 5, 6, 15}, {4, 5, 6, 15} };
  double max_Es[n_arms][n_particles] = { {20, 18, 17, 20}, {15, 18, 17, 20} };
  std::stringstream fit_range_str;
  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    std::string particle = particles[i_particle];
    std::string p = ps[i_particle];
    
    for (int i_bin_width = 0; i_bin_width < n_bin_widths; ++i_bin_width) {
      std::string bin_width = bin_widths[i_bin_width];
      //      int rebin_factor = rebin_factors[i_bin_width];
      double bin_width_val = bin_width_vals[i_bin_width];

      // Normalize the raw spectra to be per captured muon
      for (int i_arm = 0; i_arm < n_arms; ++i_arm) {
	std::string arm = arms[i_arm];
	std::string inhistname = "h" + p + arm;//"SiR";
	std::string outdirname = "Unfold_NormaliseRawSpectra_" + arm + "_" + particle;

	double pid_eff = 0.95;
	double time_cut_eff = 0.297;
	double geom_acceptance = geom_acceptances[i_arm];
	double n_stopped_muons = 80e6;
	double n_captured_muons = n_stopped_muons*0.8529;
	double factor = pid_eff*time_cut_eff*geom_acceptance*n_captured_muons*bin_width_val;
	Ti50_Unfold_NormaliseRawSpectrum(infilename, outfilename, inhistname, outdirname, factor);

	//for (double i_min_E = min_E; i_min_E < max_E; i_min_E += E_step) {
	//	for (double i_max_E = max_E; i_max_E > i_min_E; i_max_E -= E_step) {
	double i_min_E = min_Es[i_arm][i_particle];
	double i_max_E = max_Es[i_arm][i_particle];
	fit_range_str.str("");
	fit_range_str << "FitRange" << i_min_E << "_" << i_max_E << "_" << bin_width;
	inhistname = "Unfold_NormaliseRawSpectra_" + arm + "_" + particle + "/hCorrectedSpectrum";
	outdirname = "FinalFit_" + arm + "_" + particle;
	std::cout << outdirname << std::endl;
	// if (particle=="alpha") {
	//   rebin_factor *= 2;
	// }
	Ti50_Unfold_EscapeCorrection(outfilename, outfilename, inhistname, outdirname, i_min_E, i_max_E, particle, bin_width_val, arm);
      }
    }
  }
}
