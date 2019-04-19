void SiL3_FinalPlot_CutTargetSpectrum() {

  std::string norm_file_name = "/home/edmonds/data/results/SiL3/normalisation_geq2TgtPulse.root";
  std::string dir_name = "TargetSpectrum_forCrossCheck_MuonSlice3000_6000";
  std::string cut_spectrum_name = dir_name + "/hSpectrum";
  std::string counttree_name = dir_name + "/counttree";
  
  TFile* norm_file = new TFile(norm_file_name.c_str(), "READ");
  TH1D* hTargetSpectrum = (TH1D*) norm_file->Get(cut_spectrum_name.c_str());

  TTree* counttree = (TTree*) norm_file->Get(counttree_name.c_str());
  double n_stopped_muons = 0;
  double n_stopped_muons_error = 0;
  double min_muon_time = 0;
  double max_muon_time = 0;
  double min_muon_energy = 0;
  double max_muon_energy = 0;
  counttree->SetBranchAddress("n_stopped_muons", &n_stopped_muons);
  counttree->SetBranchAddress("n_stopped_muons_error", &n_stopped_muons_error);
  counttree->SetBranchAddress("min_muon_time", &min_muon_time);
  counttree->SetBranchAddress("max_muon_time", &max_muon_time);
  counttree->SetBranchAddress("min_muon_energy", &min_muon_energy);
  counttree->SetBranchAddress("max_muon_energy", &max_muon_energy);
  counttree->GetEntry(0);
						  
  TCanvas* c_TargetSpectrum = new TCanvas("c_TargetSpectrum", "c_TargetSpectrum");

  //  hTargetSpectrum->Rebin(4);
  //  hTargetSpectrum->SetLineColor(kBlue);
  std::string histtitle = "SiL3 Dataset, Cut Target Spectrum";
  hTargetSpectrum->SetTitle(histtitle.c_str());
  hTargetSpectrum->SetStats(false);
  hTargetSpectrum->GetXaxis()->SetRangeUser(0, 30000);

  std::stringstream axislabel;
  axislabel << "Counts / " << hTargetSpectrum->GetBinWidth(1) << " keV";
  hTargetSpectrum->SetYTitle(axislabel.str().c_str());
  hTargetSpectrum->GetYaxis()->SetTitleOffset(1.3);
  
  hTargetSpectrum->Draw("HIST E");

  TLatex* latex = new TLatex();
  latex->DrawLatexNDC(0.50, 0.75, "AlCap Preliminary");

  std::stringstream text;
  text.str("");
  text << min_muon_time << " ns < t < " << max_muon_time << " ns";
  latex->SetTextAlign(22);
  latex->DrawLatexNDC(0.55, 0.60, text.str().c_str());

  text.str("");
  text << min_muon_energy << " keV < E < " << max_muon_energy << " keV";
  latex->SetTextAlign(22);
  latex->DrawLatexNDC(0.55, 0.55, text.str().c_str());

  TLine* low_line = new TLine(min_muon_energy, 0, min_muon_energy, hTargetSpectrum->GetMaximum());
  low_line->SetLineWidth(2);
  low_line->Draw("LSAME");

  TLine* high_line = new TLine(max_muon_energy, 0, max_muon_energy, hTargetSpectrum->GetMaximum());
  high_line->SetLineWidth(2);
  high_line->Draw("LSAME");

  text.str("");
  text << "#rightarrow N_{stop #mu} = (" << std::fixed << std::setprecision(3) << n_stopped_muons/1e6 << " #pm " << n_stopped_muons_error/1e6 << ") #times 10^{6}";
  latex->SetTextAlign(22);
  latex->DrawLatexNDC(0.55, 0.45, text.str().c_str());
}
