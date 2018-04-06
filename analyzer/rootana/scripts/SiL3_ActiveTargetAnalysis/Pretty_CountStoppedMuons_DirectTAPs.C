TCanvas* Pretty_CountStoppedMuons_DirectTAPs() {

  //////////////////
  // Open all files
  std::string dataset = "SiL3_active";
  std::string filename = "/home/edmonds/data/results/" + dataset + "/CountStoppedMuons_DirectTAPs.root";
  std::string outfilename = "/home/edmonds/data/results/" + dataset + "/pretty-plots.root";
  
  TFile* file = new TFile(filename.c_str(), "READ");
  if (file->IsZombie()) {
    std::cout << "Trouble opening mustops_gelo " << filename << std::endl;
    return 1;
  }


  /////////////////////////
  // Create mustops canvas
  TCanvas* c_mustops = new TCanvas("c_mustops", "c_mustops");

  TTree* mustops_tree = (TTree*) file->Get("mustops");
  double direct_count = 0;
  double direct_count_error = 0;
  double min_muon_energy = 0;
  double max_muon_energy = 0;

  mustops_tree->SetBranchAddress("integral", &direct_count);
  mustops_tree->SetBranchAddress("integral_error", &direct_count_error);
  mustops_tree->SetBranchAddress("min_muon_energy", &min_muon_energy);
  mustops_tree->SetBranchAddress("max_muon_energy", &max_muon_energy);
  
  mustops_tree->GetEntry(0);
  
  TH1F* hSiL3_Spectrum = (TH1F*) file->Get("hEnergy");

  c_mustops->cd();
  hEnergy->SetLineWidth(2);
  hEnergy->SetStats(false);
  hEnergy->Draw("HIST E");

  TLine* min_muon_energy_line = new TLine(min_muon_energy, 0, min_muon_energy, hSiL3_Spectrum->GetMaximum());
  min_muon_energy_line->SetLineWidth(2);
  min_muon_energy_line->SetLineColor(kRed);
  min_muon_energy_line->Draw("LSAME");

  TLine* max_muon_energy_line = new TLine(max_muon_energy, 0, max_muon_energy, hSiL3_Spectrum->GetMaximum());
  max_muon_energy_line->SetLineWidth(2);
  max_muon_energy_line->SetLineColor(kRed);
  max_muon_energy_line->Draw("LSAME");

  std::stringstream text;
  text.str("");
  text << "N_{stop #mu} = (" << std::fixed << std::setprecision(3) << direct_count/1e6 << " #pm " << std::setprecision(3) << direct_count_error/1e6 << ") #times 10^{6}";
  TLatex* mustop_text = new TLatex(13000, 200e3, text.str().c_str());
  mustop_text->SetTextAlign(22);
  mustop_text->SetTextSize(0.05);
  mustop_text->Draw("");

  return c_mustops;
}
