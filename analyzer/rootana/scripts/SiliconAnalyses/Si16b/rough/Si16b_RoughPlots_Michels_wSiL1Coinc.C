void Si16b_RoughPlots_Michels_wSiL1Coinc() {

  std::string filename = "~/data/results/Si16b/subtrees.root";
  TFile* file = new TFile(filename.c_str(), "READ");

  std::string treename = "siBlockTree_Target";
  TTree* tree = (TTree*) file->Get(treename.c_str());

  double min_energy = 0;
  double max_energy = 1500;
  double energy_width = 50;
  int n_energy_bins = (max_energy - min_energy) / energy_width;
  TH1F* hSiR2_Michels = new TH1F("hSiR2_Michels", "SiR2 Michels", n_energy_bins,min_energy,max_energy);

  tree->Draw("third_energy>>hSiR2_Michels", "thick_tpi_id<0 && third_tpi_id>=0", "goff");

  hSiR2_Michels->Draw("HIST E");

  TCanvas* c_SiR1 = new TCanvas("c_SiR1", "c_SiR1");
  c_SiR1->SetLogy(1);

  std::stringstream leglabel;
  TLegend* leg_SiR1 = new TLegend(0.45,0.70,0.75,0.89);
  leg_SiR1->SetBorderSize(0);
  leg_SiR1->SetTextSize(0.03);
  leg_SiR1->SetFillColor(kWhite);
  
  double min_energy_SiR1 = 0;
  double max_energy_SiR1 = 8000;
  double energy_width_SiR1 = 50;
  int n_energy_bins_SiR1 = (max_energy_SiR1 - min_energy_SiR1) / energy_width_SiR1;
  TH1F* hSiR1_All =  new TH1F("hSiR1_All", "SiR1", n_energy_bins_SiR1,min_energy_SiR1,max_energy_SiR1);
  hSiR1_All->SetLineColor(kBlack);
  hSiR1_All->SetStats(false);
  TH1F* hSiR1_wSiL1Coinc =  new TH1F("hSiR1_wSiL1Coinc", "SiR1 (with SiL1 coinc.)", n_energy_bins_SiR1,min_energy_SiR1,max_energy_SiR1);
  hSiR1_wSiL1Coinc->SetLineColor(kRed);
  TH1F* hSiR1_woutSiL1Coinc =  new TH1F("hSiR1_woutSiL1Coinc", "SiR1 (w/o SiL1 coinc.)", n_energy_bins_SiR1,min_energy_SiR1,max_energy_SiR1);
  hSiR1_woutSiL1Coinc->SetLineColor(kBlue);
  tree->Draw("thick_energy>>hSiR1_All", "thick_tpi_id>=0", "goff");
  tree->Draw("thick_energy>>hSiR1_wSiL1Coinc", "thick_tpi_id>=0 && thin_tpi_id>=0", "goff");
  tree->Draw("thick_energy>>hSiR1_woutSiL1Coinc", "thick_tpi_id>=0 && thin_tpi_id<0", "goff");

  leglabel.str("");
  leglabel << "SiR1 (all, " << hSiR1_All->GetEntries() << " entries)";
  leg_SiR1->AddEntry(hSiR1_All, leglabel.str().c_str(), "l");
  leglabel.str("");
  leglabel << "SiR1 (w/ SiL1 coinc., " << hSiR1_wSiL1Coinc->GetEntries() << " entries)";
  leg_SiR1->AddEntry(hSiR1_wSiL1Coinc, leglabel.str().c_str(), "l");
  leglabel.str("");
  leglabel << "SiR1 (w/o SiL1 coinc., " << hSiR1_woutSiL1Coinc->GetEntries() << " entries)";
  leg_SiR1->AddEntry(hSiR1_woutSiL1Coinc, leglabel.str().c_str(), "l");
		     
  hSiR1_All->Draw("HIST E");
  hSiR1_wSiL1Coinc->Draw("HIST E SAME");
  hSiR1_woutSiL1Coinc->Draw("HIST E SAME");

  leg_SiR1->Draw();

  TCanvas* c_SiR2 = new TCanvas("c_SiR2", "c_SiR2");
  c_SiR2->SetLogy(1);

  TLegend* leg_SiR2 = new TLegend(0.45,0.70,0.75,0.89);
  leg_SiR2->SetBorderSize(0);
  leg_SiR2->SetTextSize(0.03);
  leg_SiR2->SetFillColor(kWhite);

  double min_energy_SiR2 = 0;
  double max_energy_SiR2 = 20000;
  double energy_width_SiR2 = 100;
  int n_energy_bins_SiR2 = (max_energy_SiR2 - min_energy_SiR2) / energy_width_SiR2;
  TH1F* hSiR2_All =  new TH1F("hSiR2_All", "SiR2", n_energy_bins_SiR2,min_energy_SiR2,max_energy_SiR2);
  hSiR2_All->SetLineColor(kBlack);
  hSiR2_All->SetStats(false);
  TH1F* hSiR2_wSiL1Coinc =  new TH1F("hSiR2_wSiL1Coinc", "SiR2 (with SiL1 coinc.)", n_energy_bins_SiR2,min_energy_SiR2,max_energy_SiR2);
  hSiR2_wSiL1Coinc->SetLineColor(kRed);
  TH1F* hSiR2_woutSiL1Coinc =  new TH1F("hSiR2_woutSiL1Coinc", "SiR2 (w/o SiL1 coinc.)", n_energy_bins_SiR2,min_energy_SiR2,max_energy_SiR2);
  hSiR2_woutSiL1Coinc->SetLineColor(kBlue);
  tree->Draw("third_energy>>hSiR2_All", "third_tpi_id>=0", "goff");
  tree->Draw("third_energy>>hSiR2_wSiL1Coinc", "third_tpi_id>=0 && thin_tpi_id>=0", "goff");
  tree->Draw("third_energy>>hSiR2_woutSiL1Coinc", "third_tpi_id>=0 && thin_tpi_id<0", "goff");
  
  leglabel.str("");
  leglabel << "SiR2 (all, " << hSiR2_All->GetEntries() << " entries)";
  leg_SiR2->AddEntry(hSiR2_All, leglabel.str().c_str(), "l");
  leglabel.str("");
  leglabel << "SiR2 (w/ SiL1 coinc., " << hSiR2_wSiL1Coinc->GetEntries() << " entries)";
  leg_SiR2->AddEntry(hSiR2_wSiL1Coinc, leglabel.str().c_str(), "l");
  leglabel.str("");
  leglabel << "SiR2 (w/o SiL1 coinc., " << hSiR2_woutSiL1Coinc->GetEntries() << " entries)";
  leg_SiR2->AddEntry(hSiR2_woutSiL1Coinc, leglabel.str().c_str(), "l");

  hSiR2_All->Draw("HIST E");
  hSiR2_wSiL1Coinc->Draw("HIST E SAME");
  hSiR2_woutSiL1Coinc->Draw("HIST E SAME");

  leg_SiR2->Draw();
}
