void Si16b_FinalPlot_MuonSpot_BkgEstimate2() {

  std::string infilename = "~/data/results/Si16b/subtrees_newPP_geq1TgtPulse_1.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");

  std::string treename = "siBlockTree_SiR";
  TTree* tree = (TTree*) infile->Get(treename.c_str());

  double min_time = -10000;
  double max_time = 10000;
  double time_bin_width = 50;
  int n_time_bins = (max_time - min_time) / time_bin_width;
  double min_energy = 0;
  double max_energy = 5000;
  double energy_bin_width = 50;
  int n_energy_bins = (max_energy - min_energy) / energy_bin_width;
  TH2F* h2D = new TH2F("hist", "SiR1 hits only", n_time_bins,min_time,max_time, n_energy_bins,min_energy,max_energy);
  
  std::string drawcmd = "thin_energy:thin_time>>hist";
  std::string cutcmd = "thin_tpi_id>=0";// && thick_tpi_id<0 && third_tpi_id<0";
  TCanvas* c_2D = new TCanvas("c_2D", "c_2D");
  h2D->SetTitle("SiR1 hits only");
  tree->Draw(drawcmd.c_str(), cutcmd.c_str(), "COLZ");

  double min_time_cut = -10000;
  double max_time_cut = -5000;
  int min_time_cut_bin = h2D->GetXaxis()->FindBin(min_time_cut);
  int max_time_cut_bin = h2D->GetXaxis()->FindBin(max_time_cut)-1;

  TLine* min_time_line = new TLine(min_time_cut, min_energy, min_time_cut, max_energy);
  min_time_line->SetLineColor(kRed);
  min_time_line->SetLineWidth(2);
  min_time_line->Draw("LSAME");

  TLine* max_time_line = new TLine(max_time_cut, min_energy, max_time_cut, max_energy);
  max_time_line->SetLineColor(kRed);
  max_time_line->SetLineWidth(2);
  max_time_line->Draw("LSAME");

  TCanvas* c_1D = new TCanvas("c_1D", "c_1D");
  c_1D->SetLogy();
  TH1D* hProj = h2D->ProjectionY("hProj", min_time_cut_bin, max_time_cut_bin);
  hProj->Draw("HIST E");

  double min_muon_energy = 500;
  double max_muon_energy = 1750;
  int min_muon_energy_bin = hProj->GetXaxis()->FindBin(min_muon_energy);
  int max_muon_energy_bin = hProj->GetXaxis()->FindBin(max_muon_energy)-1;
  double integral = hProj->Integral(min_muon_energy_bin, max_muon_energy_bin);
  std::cout << "Number of SiR1 stops = " << integral << std::endl;

  TLine* min_energy_line = new TLine(min_muon_energy, 0, min_muon_energy, hProj->GetMaximum());
  min_energy_line->SetLineColor(kRed);
  min_energy_line->SetLineWidth(2);
  min_energy_line->Draw("LSAME");

  TLine* max_energy_line = new TLine(max_muon_energy, 0, max_muon_energy, hProj->GetMaximum());
  max_energy_line->SetLineColor(kRed);
  max_energy_line->SetLineWidth(2);
  max_energy_line->Draw("LSAME");

  double latex_x = min_muon_energy + (max_muon_energy - min_muon_energy)/2;
  double latex_y = hProj->GetMaximum() * 0.75;
  std::stringstream text;
  text.str("");
  text << min_time_cut << " ns < t < " << max_time_cut << " ns";
  TLatex* latex = new TLatex();
  latex->SetTextAlign(22);
  latex->DrawLatex(latex_x, latex_y, text.str().c_str());

  latex_y -= 500;
  text.str("");
  text << min_muon_energy << " keV < E < " << max_muon_energy << " keV";
  latex->DrawLatex(latex_x, latex_y, text.str().c_str());

  latex_y -= 400;
  text.str("");
  text << "Integral = " << integral;
  latex->DrawLatex(latex_x, latex_y, text.str().c_str());
}
