#include "scripts/PlotTME_Si16b_EvdE/ExtractBand.C"

#include <sstream>

void ProtonBandExtraction() {

  std::string x_varname = "(SiR1Energy+SiR2Energy)"; // need () so that the cuts work as expected
  std::string y_varname = "SiR1Energy";

  std::vector<Cut*> cuts;
  std::stringstream eqn_string;

  // Electron Spot Cut
  // - define the equation
  double x_1 = 0, y_1 = 2250, x_2 = 5500, y_2 = 0;
  double electron_spot_gradient = (y_2 - y_1) / (x_2 - x_1);
  double electron_spot_yoffset = y_1;
  eqn_string.str("");
  eqn_string << electron_spot_gradient << "*x + " << electron_spot_yoffset;
  Cut* electron_spot_cut = new Cut("electron_spot_cut", eqn_string.str(), x_varname, y_varname, true);
  cuts.push_back(electron_spot_cut);

  // Punch Through Cut
  double punch_through_yoffset = 300;
  eqn_string.str("");
  eqn_string << punch_through_yoffset;
  Cut* punch_through_cut = new Cut("punch_through_cut", eqn_string.str(), x_varname, y_varname, true);
  cuts.push_back(punch_through_cut);

  // Deuteron Band Cut
  double deuteron_cut_peak = 4500;
  double deuteron_cut_slope = -0.0004;
  double deuteron_cut_yoffset = 500;
  eqn_string.str("");
  eqn_string << deuteron_cut_peak << "*TMath::Exp(" << deuteron_cut_slope << "*x) + " << deuteron_cut_yoffset;
  Cut* deuteron_cut = new Cut("deuteron_cut", eqn_string.str(), x_varname, y_varname, false);
  cuts.push_back(deuteron_cut);

  // High Energy Cut
  double high_energy_cut_xoffset = 10000;
  eqn_string.str("");
  eqn_string << high_energy_cut_xoffset;
  Cut* high_energy_cut = new Cut ("high_energy_cut", eqn_string.str(), x_varname, y_varname, false, true);
  cuts.push_back(high_energy_cut);
  

  //  ExtractBand("all-tin-absorber.root", cuts);
  ExtractBand("~/data/out/v22/Si16b.root", cuts, "PlotTME_Si16b_EvdE", "SiRHits", x_varname, y_varname);
}
