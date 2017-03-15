#include "ExtractBand.C"

#include <sstream>

void ProtonBandExtraction() {

  std::string x_varname = "(SiR1Energy+SiR2Energy)";
  std::string y_varname = "SiR1Energy";

  double x_1 = 0, y_1 = 2000, x_2 = 4000, y_2 = 0;
  double electron_spot_gradient = (y_2 - y_1) / (x_2 - x_1);
  double electron_spot_offset = y_1;
  std::stringstream cut_string;
  cut_string.str("");
  cut_string << y_varname << ">" << "(" << electron_spot_gradient << "*" << x_varname << "+" << electron_spot_offset << ")";
  std::cout << "Cut 1 (electron spot): " << cut_string.str() << std::endl;
  TCut electron_spot_cut = cut_string.str().c_str();

  double punch_through_yoffset = 300;
  cut_string.str("");
  cut_string << y_varname << ">" << punch_through_yoffset;
  std::cout << "Cut 2 (punch through protons): " << cut_string.str() << std::endl;
  TCut punch_through_cut = cut_string.str().c_str();

  double deuteron_cut_peak = 4500;
  double deuteron_cut_slope = -0.0004;
  double deuteron_cut_yoffset = 500;
  cut_string.str("");
  cut_string << y_varname << "<" << deuteron_cut_peak << "*TMath::Exp(" << deuteron_cut_slope << "*" << x_varname << ")+" << deuteron_cut_yoffset;
  std::cout << "Cut 3 (deuteron): " << cut_string.str() << std::endl;
  TCut deuteron_cut = cut_string.str().c_str();

  //  ExtractBand("all-tin-absorber.root", cuts);
  ExtractBand("~/data/out/v22/Si16b.root", electron_spot_cut&&punch_through_cut&&deuteron_cut, "PlotTME_Si16b_EvdE", "SiRHits", x_varname, y_varname);
}
