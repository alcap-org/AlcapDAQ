#include "../../Utils/IntegrateRates.C"

void Si16b_FinalPlot_Systematic_IndividualComponents(std::string savedir = "", std::ostream& numbers_file = std::cout) {
  
  std::string filename = "~/data/results/Si16b/systematics_newPP_geq1TgtPulse_2.root";
  TFile* file = new TFile(filename.c_str(), "READ");

  const int n_particles = 4;
  std::string particles[n_particles] = {"proton", "deuteron", "triton", "alpha"};
  std::string Particles[n_particles] = {"Proton", "Deuteron", "Triton", "Alpha"};
  double min_best_energy[n_particles] = {4, 5, 6, 15};
  double max_best_energy[n_particles] = {20, 17, 17, 20};//16};
  //  double min_best_energy[n_particles] = {15.0,15.0,15.0,15.0};
  //  double max_best_energy[n_particles] = {17,17,17,17};

  // const int n_systematics = 5;
  // std::string systematics[n_systematics] = {"MuStopPos", "NIter", "BinW", "SiRPos", "PID"};
  // double total_high_syst[n_systematics] = {0};
  // double total_low_syst[n_systematics] = {0};

  const int n_systematics = 5;
  std::string systematics[n_systematics] = {"MuStopPos", "MisCalib", "NIter", "BinW", ""};//{"MuStopPos", "MisCalib"};//{"NIter", "PID", "MuStopPos", "SiRPos", "BinW"};
  //  const int n_systematics = 6;
  //  std::string systematics[n_systematics] = {"MuStopPosHigh", "MuStopPosLow", "MisCalibUp", "MisCalibDown", "NIter", "BinW"};//{"MuStopPos", "MisCalib"};//{"NIter", "PID", "MuStopPos", "SiRPos", "BinW"};
  double total_high_syst[n_systematics] = {0};
  double total_low_syst[n_systematics] = {0};

  // const int n_settings = 2;
  // std::string settings[n_systematics][n_settings] = { {"_higherE", "_lowerE"},
  // 						      {"100", "" },
  // 						      {"500", ""},
  // 						      {"_closerSiR", "_furtherSiR"},
  // 						      {"2sigma", ""} };
  // Int_t colours[n_settings] = {kBlack, kRed, kBlue};
  // std::string leglabels[n_settings] = {"nominal", "higher energy", "lower energy"};

  numbers_file << "% Si16b_FinalPlot_Systematic_IndividualComponents.C" << std::endl;
  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    std::string particle = particles[i_particle];
    std::string Particle = Particles[i_particle];

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
	
      std::string i_dirname = "FinalSystPlot_";
      if (systematic != "") {
	i_dirname += systematic + "_";
      }
      i_dirname += particle + "_TCutG_2sig_layerCoinc500ns_tGT0ns_BinW500keV";
      std::string i_histname = i_dirname + "/hFinalSyst";
	
      //      std::cout << i_dirname << std::endl;
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

      TCanvas* c = new TCanvas();
      syst_spectrum->Draw("A*E");

      double rate = 0;
      double total_stat_error = 0;
      double total_high_syst_error = 0;
      double total_low_syst_error = 0;
      IntegrateRates_wStatAndSystSpectra(stat_spectrum, syst_spectrum, min_best_energy[i_particle], max_best_energy[i_particle],
					 rate, total_stat_error, total_high_syst_error, total_low_syst_error);
      double frac_high_syst_error = (total_high_syst_error / rate)*100;
      double frac_low_syst_error = (total_low_syst_error / rate)*100;

      std::cout << particle << ": " << systematic << ": range " << min_best_energy[i_particle] << " MeV and " << max_best_energy[i_particle] << " MeV = +" << frac_high_syst_error << " -" << frac_low_syst_error << std::endl;

      // total_high_syst[i_systematic] = std::sqrt(total_high_syst[i_systematic]);
      // total_low_syst[i_systematic] = std::sqrt(total_low_syst[i_systematic]);
      numbers_file << std::fixed << std::setprecision(2);
      //      numbers_file << "\\newcommand\\Sib" << Particle << systematic << "FracSyst{$^{+" << frac_high_syst_error << "\\%}_{-" << frac_low_syst_error << "\\%}$}" << std::endl;
      //      std::cout << Particle << ", " << systematic << ": +" << frac_high_syst_error << "% -" << frac_low_syst_error << "%" << std::endl;
      
    }
  }
  numbers_file << std::endl;
}
