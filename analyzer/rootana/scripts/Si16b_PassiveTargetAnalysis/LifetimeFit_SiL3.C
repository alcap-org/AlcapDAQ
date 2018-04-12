void LifetimeFit_SiL3() {

  ///////////////////////
  // User parameters
  std::string filename = "~/data/results/Si16b_passive/EvdEPlots_NewCuts.root";
  std::string histname = "hSiL3_Si16b_EnergyTime";
  std::string outfilename = "~/data/results/Si16b_passive/LifetimeFit_SiL3.root";

  TFile* file = new TFile(filename.c_str(), "READ");
  TH2F* hTimeEnergy = (TH2F*) file->Get(histname.c_str());

  
  TCanvas* c_time = new TCanvas("c_time", "c_time");
  c_time->Divide(2,1);
  c_time->GetPad(1)->SetLogz();
  c_time->cd(1);
  hTimeEnergy->Draw("COLZ");


  double min_energy = 3000;
  double max_energy = 15000;
  int min_energy_bin = hTimeEnergy->GetYaxis()->FindBin(min_energy);
  int max_energy_bin = hTimeEnergy->GetYaxis()->FindBin(max_energy);

  TLine* lifetime_line_min = new TLine(hTimeEnergy->GetXaxis()->GetXmin(), min_energy, hTimeEnergy->GetXaxis()->GetXmax(), min_energy);
  lifetime_line_min->SetLineWidth(2);
  lifetime_line_min->SetLineColor(kRed);
  lifetime_line_min->Draw("LSAME");
  TLine* lifetime_line_max = new TLine(hTimeEnergy->GetXaxis()->GetXmin(), max_energy, hTimeEnergy->GetXaxis()->GetXmax(), max_energy);
  lifetime_line_max->SetLineWidth(2);
  lifetime_line_max->SetLineColor(kRed);
  lifetime_line_max->Draw("LSAME");

  c_time->cd(2);
  c_time->GetPad(2)->SetLogy(1);
  TH1D* hTime = hTimeEnergy->ProjectionX("hTime", min_energy_bin, max_energy_bin);
  hTime->Sumw2();
  //  hTime->Rebin(rebin_factor);
  double bin_width = hTime->GetBinWidth(1);
  //  hTime->Scale(1.0/bin_width); // to be per ns
  hTime->GetXaxis()->SetRangeUser(-1000, 20000);
  hTime->Draw("HIST E");

  double min_time_fit = 1000;
  double max_time_fit = 20000;
  TF1* lifetime_fit = new TF1("lifetime_fit", "[0]*TMath::Exp(-x/[1]) + [2]", min_time_fit, max_time_fit);
  lifetime_fit->SetParameters(hTime->GetMaximum(), 500, 10);
  hTime->Fit(lifetime_fit, "R");
  lifetime_fit->Draw("LSAME");

  std::stringstream text;
  text << "#tau = " << std::setprecision(4) << lifetime_fit->GetParameter(1) << " #pm " << std::setprecision(2) << lifetime_fit->GetParError(1) << " ns";
  TLatex* lifetime_text = new TLatex(12500, hTime->GetMaximum()/1000, text.str().c_str());
  lifetime_text->SetTextAlign(22);
  lifetime_text->Draw();


  TFile* output = new TFile(outfilename.c_str(), "RECREATE");
  lifetime_fit->Write();
  output->Write();
  output->Close();
}
