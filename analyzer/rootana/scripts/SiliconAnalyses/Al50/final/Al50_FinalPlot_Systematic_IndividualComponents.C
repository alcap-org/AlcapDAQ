void Al50_FinalPlot_Systematic_IndividualComponents() {

  // Using Mark's histograms
  std::string basename = "/data/ssd3/R15bAnalysis/hists/al50-unfolded";
  std::string central_filename = basename + ".root";
  TFile* central_file = new TFile(central_filename.c_str(), "READ");

  std::string syst_filename = basename + "-systematics.root";
  TFile* syst_file = new TFile(syst_filename.c_str(), "READ");

  std::string histname = "hproton_SiR";
  TH1F* central_hist = (TH1F*) central_file->Get(histname.c_str());
  //  central_hist->Draw();


  histname = "hproton_NumIterErrSiR";
  TH1F* syst_hist = (TH1F*) syst_file->Get(histname.c_str());

  central_hist->Add(syst_hist,-1);
  central_hist->Divide(syst_hist);
  central_hist->Draw("HIST");

  //  syst_hist->Draw("SAME");

}
