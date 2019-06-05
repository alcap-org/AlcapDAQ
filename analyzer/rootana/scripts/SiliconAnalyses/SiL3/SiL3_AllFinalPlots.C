void SiL3_AllFinalPlots() {

  std::vector<std::string> macronames;
  macronames.push_back("FullXRaySpectrum");

  std::string savedir = "~/data/results/SiL3/final_plots";
  
  for (std::vector<std::string>::const_iterator i_macro = macronames.begin(); i_macro != macronames.end(); ++i_macro) {
    std::string line_to_process = ".x scripts/SiliconAnalyses/SiL3/final/SiL3_FinalPlot_" + *i_macro + ".C(\"" + savedir + "\")";
    std::cout << line_to_process << std::endl;
    gROOT->ProcessLine(line_to_process.c_str());
  }
}
