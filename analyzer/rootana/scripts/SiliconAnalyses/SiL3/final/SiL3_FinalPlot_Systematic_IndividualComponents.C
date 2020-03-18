#include "../../Utils/IntegrateRates.C"

void SiL3_FinalPlot_Systematic_IndividualComponents(std::string savedir = "", std::ostream& numbers_file = std::cout) {
  
  std::string filename = "~/data/results/SiL3/systematics_geq2TgtPulse_newPP20us.root";
  TFile* file = new TFile(filename.c_str(), "READ");

  double min_best_energy = 1400;
  double max_best_energy = 26000;

  const int n_systematics = 4;
  std::string systematics[n_systematics] = {"KReg", "ParticleComposition", "MuStopPos", "TimeSlice"};
  double total_high_syst[n_systematics] = {0};
  double total_low_syst[n_systematics] = {0};

  numbers_file << "% SiL3_FinalPlot_Systematic_IndividualComponents.C" << std::endl;
  for (int i_systematic = 0; i_systematic < n_systematics; ++i_systematic) {
    std::string systematic = systematics[i_systematic];
    total_high_syst[i_systematic] = 0;
    total_low_syst[i_systematic] = 0;
    
    // for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
    // 	std::string setting = systematic + settings[i_systematic][i_setting];
    // 	if (setting == systematic) {
    // 	  continue; // don't always have the same number of settings for each systematic
    // 	}
    //	Int_t i_colour = colours[i_setting];
	
    //    std::string i_dirname = "FinalSystPlot_" + systematic + "_TimeSlice2000_4000_allRecoil_100keVBins";
    std::string i_dirname = "FinalSystPlot_" + systematic + "_TimeSlice2000_4000_allRecoil_500keVBins";
    std::string i_histname = i_dirname + "/hFinalSyst";
    
    std::cout << i_dirname << std::endl;
    TGraphAsymmErrors* syst_spectrum = (TGraphAsymmErrors*) file->Get(i_histname.c_str());
    if (!syst_spectrum) {
      std::cout << "Error: Problem getting syst_spectrum " << i_histname << std::endl;
      return;
    }
    
    i_histname = i_dirname + "/hFinalStat";	
    TGraphAsymmErrors* stat_spectrum = (TGraphAsymmErrors*) file->Get(i_histname.c_str());
    if (!stat_spectrum) {
      std::cout << "Error: Problem getting stat_spectrum " << i_histname << std::endl;
      return;
    }

    double rate = 0;
    double total_stat_error = 0;
    double total_high_syst_error = 0;
    double total_low_syst_error = 0;
    IntegrateRates_wStatAndSystSpectra(stat_spectrum, syst_spectrum, min_best_energy, max_best_energy,
				       rate, total_stat_error, total_high_syst_error, total_low_syst_error);
    double frac_high_syst_error = (total_high_syst_error / rate)*100;
    double frac_low_syst_error = (total_low_syst_error / rate)*100;
    
    std::cout << systematic << ": range " << min_best_energy/1000 << " MeV and " << max_best_energy/1000 << " MeV = +" << frac_high_syst_error << " -" << frac_low_syst_error << std::endl;

    // total_high_syst[i_systematic] = std::sqrt(total_high_syst[i_systematic]);
    // total_low_syst[i_systematic] = std::sqrt(total_low_syst[i_systematic]);
    numbers_file << std::fixed << std::setprecision(2);
    numbers_file << "\\newcommand\\SiL" << systematic << "FracSyst{$^{+" << frac_high_syst_error << "\\%}_{-" << frac_low_syst_error << "\\%}$}" << std::endl;
  }
  numbers_file << std::endl;
}
