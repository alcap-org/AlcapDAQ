void Ti50_FinalPlot_MichelElectrons_DataVsMC() {

  std::string filename = "~/data/results/Ti50/subtrees.root";
  TFile* file = new TFile(filename.c_str(), "READ");

  double min_energy = 0;
  double max_energy = 1500;
  double energy_width = 50;
  int n_energy_bins = (max_energy - min_energy) / energy_width;

  double min_time = -10000;
  double max_time = 10000;
  double time_width = 50;
  int n_time_bins = (max_time - min_time) / time_width;

  std::string arm = "SiL";
  
  std::string treename = "siBlockTree_" + arm;
  TTree* SiRTree = (TTree*) file->Get(treename.c_str());

  TLegend* leg = new TLegend(0.55,0.70,0.9,0.89);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);


  std::string histtitle = "Ti50 Dataset";

  //  std::string i_detname = "SiR3";
  //  std::string i_layername = "third";
  //  std::string i_detname = "SiR2";
  //  std::string i_layername = "thick";
  std::string i_detname = "SiL3";
  std::string i_layername = "thick";
  histtitle += ", " + i_detname + " Detector";

  std::stringstream histname, drawcmd;
  //  std::string cutcmd = "thin_tpi_id<0 && thick_tpi_id>=0 && third_tpi_id>=0";
  //  histtitle += " (!SiR1 && SiR2 && SiR3)";
  std::string cutcmd = "thin_tpi_id<0 && thick_tpi_id>=0";
  histtitle += " (!thin && thick)";

  histname.str("");
  histname << "hEnergyTime_Michel_" << i_layername;
  drawcmd.str("");
  drawcmd << i_layername << "_energy:" << i_layername << "_time>>" << histname.str();

  TH2F* hEvst = new TH2F(histname.str().c_str(), histtitle.c_str(), n_time_bins,min_time,max_time, n_energy_bins,min_energy,max_energy);
  std::string yaxislabel = i_detname + " Energy [keV]";
  std::string xaxislabel = i_detname + " Time [ns]";
  hEvst->SetYTitle(yaxislabel.c_str());
  hEvst->SetXTitle(xaxislabel.c_str());
  SiRTree->Draw(drawcmd.str().c_str(), cutcmd.c_str(), "goff");

  TCanvas* c_Evst = new TCanvas("c_Evst", "c_Evst");
  hEvst->Draw("COLZ");

  histname.str("");
  histname << "hE_Michel_FlatBkg_Cut" << i_layername;
  double flat_bkg_min = -10000;
  double flat_bkg_max = -5000;
  int flat_bkg_bin_min = hEvst->GetXaxis()->FindBin(flat_bkg_min);
  int flat_bkg_bin_max = hEvst->GetXaxis()->FindBin(flat_bkg_max)-1;
  std::cout << "Flat Bkg Bins: " << flat_bkg_bin_min << " - " << flat_bkg_bin_max << std::endl;
  TH1D* hEFlatBkg = hEvst->ProjectionY(histname.str().c_str(), flat_bkg_bin_min, flat_bkg_bin_max);
  std::cout << hEFlatBkg->GetEntries() << std::endl;
  std::cout << hEFlatBkg->GetMaximum() << std::endl;
  hEFlatBkg->Scale(2);

  histname.str("");
  histname << "hE_Michel_Cut" << i_layername;
  double michel_min = 0;
  double michel_max = 10000;
  int michel_bin_min = hEvst->GetXaxis()->FindBin(michel_min);
  int michel_bin_max = hEvst->GetXaxis()->FindBin(michel_max)-1;
  std::cout << "Michel Bins: " << michel_bin_min << " - " << michel_bin_max << std::endl;
  TH1D* hE = hEvst->ProjectionY(histname.str().c_str(), michel_bin_min, michel_bin_max);

  TCanvas* c_Ana = new TCanvas("c_Ana", "c_Ana");
  TH1F* hEBefore = (TH1F*) hE->Clone("hEBefore");
  hEBefore->SetLineColor(kBlack);
  hEBefore->Draw("HIST");
  hEFlatBkg->SetLineColor(kBlue);
  hEFlatBkg->Draw("HIST SAME");
  hE->SetLineColor(kRed);
  hE->Add(hEFlatBkg, -1);
  TH1F* hEAfter = (TH1F*) hE->Clone("hEAfter");
  hEAfter->SetLineColor(kRed);
  hEAfter->Draw("HIST SAME");

  TLegend* leg_ana = new TLegend(0.55,0.70,0.9,0.89);
  leg_ana->SetBorderSize(0);
  leg_ana->SetTextSize(0.03);
  leg_ana->SetFillColor(kWhite);
  leg_ana->AddEntry(hEBefore, (i_detname+" Energy (all)").c_str(), "l");
  leg_ana->AddEntry(hEFlatBkg, (i_detname+" Energy (flat bkg)").c_str(), "l");
  leg_ana->AddEntry(hEAfter, (i_detname+" Energy (after flat bkg subtraction)").c_str(), "l");
  leg_ana->Draw();

  TCanvas* c_E = new TCanvas("c_E", "c_E");
  std::cout << hE->GetEntries() << std::endl;
  std::cout << hE->GetMaximum() << std::endl;
  hE->SetStats(false);
  Int_t data_colour = kBlack;
  hE->SetLineColor(data_colour);
  //  hE->SetXTitle("Energy [keV]");
  int n_stopped_muons = 4.6e7;
  double capture_fraction = 0.85;
  int n_decayed_muons = (1-capture_fraction)*n_stopped_muons;
  //  hE->Scale(1.0/n_decayed_muons);
  //  hE->SetYTitle("per decayed muon");
  hE->Scale(1.0/hE->GetMaximum());
  hE->SetYTitle("peak normalized");
  hE->Draw("HIST SAME");

  double min_integral = 0;
  double max_integral = 1500;
  int min_integral_bin = hE->GetXaxis()->FindBin(min_integral);
  int max_integral_bin = hE->GetXaxis()->FindBin(max_integral) - 1;
  double data_integral = hE->Integral(min_integral_bin, max_integral_bin);

  hE->GetXaxis()->SetRangeUser(300,max_energy); // want to make sure that I don't get the noise bin
  int max_data_bin = hE->GetMaximumBin();
  hE->GetXaxis()->SetRangeUser(min_energy,max_energy); // change it back
  double max_data_content = hE->GetBinContent(max_data_bin);
  data_integral = max_data_content;
  min_integral = hE->GetXaxis()->GetBinLowEdge(max_data_bin);
  max_integral = hE->GetXaxis()->GetBinUpEdge(max_data_bin);

  // TLatex* data_latex = new TLatex();
  // data_latex->SetTextSize(0.035);
  // data_latex->SetTextColor(data_colour);
  // std::stringstream data_sstream;
  // data_sstream.str("");
  // data_sstream << "Integral: " << min_integral << " keV < E < " << max_integral << " keV";
  // data_latex->DrawLatexNDC(0.5, 0.67, data_sstream.str().c_str());
  // data_sstream.str("");
  // data_sstream << "Capture Fraction = " << capture_fraction;
  // data_latex->DrawLatexNDC(0.5, 0.60, data_sstream.str().c_str());
  // data_sstream.str("");
  // data_sstream << "Data Efficiency = " << std::setprecision(3) << data_integral << std::endl;
  // data_latex->DrawLatexNDC(0.5, 0.57, data_sstream.str().c_str());

  leg->AddEntry(hE, "Data", "l");

  //  std::string mc_filename = "~/data/mc/Ti50/MC_Ti50Dataset_Michels.root";
  //  std::string mc_filename = "~/data/mc/Ti50/MC_Ti50Dataset_Michels_new.root";
  std::string mc_filename = "~/data/mc/Ti50/MC_Ti50Dataset_Michels_new_K-SiL3-0-9.root";
  //  std::string mc_filename = "~/data/mc/Ti50/MC_Ti50Dataset_Michels_new_K-SiL3-0-85.root";
  //  std::string mc_filename = "~/data/mc/Ti50/MC_Ti50Dataset_Michels_new_K-SiL3-0-8.root";
  TFile* mc_file = new TFile(mc_filename.c_str(), "READ");
  std::string mc_histname = "hEDep_" + i_detname;
  //  mc_histname += "_two_layer";
  TH1F* hE_MC = (TH1F*) mc_file->Get(mc_histname.c_str());
  int n_stopped_muons_mc = 1e6;
  double capture_fraction_mc = 0; // because simulated muplus
  int n_decayed_muons_mc = (1-capture_fraction_mc)*n_stopped_muons_mc;
  //  hE_MC->Scale(1.0/n_decayed_muons_mc);
  //  double max_mc_content = hE_MC->GetMaximum();
  //  hE_MC->Scale(max_data_content / max_mc_content);
  hE_MC->Scale(1.0 / hE_MC->GetMaximum());
  Int_t mc_colour = kRed;
  hE_MC->SetLineColor(mc_colour);
  hE_MC->Draw("HIST SAME");

  double mc_integral = hE_MC->Integral(min_integral_bin, max_integral_bin);

  int max_mc_bin = hE_MC->GetMaximumBin();
  double max_mc_content = hE_MC->GetBinContent(max_mc_bin);
  mc_integral = max_mc_content;
  min_integral = hE_MC->GetXaxis()->GetBinLowEdge(max_mc_bin);
  max_integral = hE_MC->GetXaxis()->GetBinUpEdge(max_mc_bin);

  // TLatex* mc_latex = new TLatex();
  // mc_latex->SetTextSize(0.035);
  // mc_latex->SetTextColor(mc_colour);
  // std::stringstream mc_sstream;
  // mc_sstream.str("");
  // mc_sstream << "Integral: " << min_integral << " keV < E < " << max_integral << " keV";
  // mc_latex->DrawLatexNDC(0.5, 0.50, mc_sstream.str().c_str());
  // mc_sstream.str("");
  // mc_sstream << "MC Efficiency = " << std::setprecision(3) << mc_integral << std::endl;
  // mc_latex->DrawLatexNDC(0.5, 0.47, mc_sstream.str().c_str());


  leg->AddEntry(hE_MC, "MC", "l");
  
  leg->Draw();

  double ratio = data_integral / mc_integral;

  // TLatex* ratio_latex = new TLatex();
  // ratio_latex->SetTextSize(0.035);
  // ratio_latex->SetTextColor(kBlue);
  // std::stringstream ratio_sstream;
  // ratio_sstream.str("");
  // ratio_sstream << "Data / MC = " << std::setprecision(3) << ratio << std::endl;
  // ratio_latex->DrawLatexNDC(0.5, 0.43, ratio_sstream.str().c_str());

  
  std::cout << "Data Intgral = " << data_integral << ", MC Integral = " << mc_integral << std::endl;
  std::cout << "Data / MC = " << ratio << std::endl;

}
