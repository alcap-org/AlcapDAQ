void Si16b_FinalPlot_CountTMEs(std::ostream& numbers_file = std::cout) {
  
  std::string infilename = "~/data/results/Si16b/subtrees_newPP_geq1TgtPulse.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");

  std::string treename = "infotree";
  TTree* infotree = (TTree*) infile->Get(treename.c_str());

  int n_total_tmes = 0;
  int n_analysed_tmes = 0;
  infotree->SetBranchAddress("n_total_tmes", &n_total_tmes);
  infotree->SetBranchAddress("n_analysed_tmes", &n_analysed_tmes);
  infotree->GetEntry(0);

  numbers_file << "% from Si16b_FinalPlot_CountTMEs.C" << std::endl;
  numbers_file << "\\newcommand\\SibNTotalTMEs{\\num[round-precision=3, round-mode=figures, scientific-notation=engineering]{" << n_total_tmes << "}}" << std::endl;
  numbers_file << "\\newcommand\\SibNAnalysedTMEs{\\num[round-precision=3, round-mode=figures, scientific-notation=engineering]{" << n_analysed_tmes << "}}" << std::endl;
    numbers_file << "\\newcommand\\SibNTotalTMEsTab{\\num[round-precision=3, round-mode=figures, scientific-notation=engineering]{" << n_total_tmes/1e6 << "}}" << std::endl;
  numbers_file << "\\newcommand\\SibNAnalysedTMEsTab{\\num[round-precision=3, round-mode=figures, scientific-notation=engineering]{" << n_analysed_tmes/1e6 << "}}" << std::endl;
  numbers_file << std::endl;
}
