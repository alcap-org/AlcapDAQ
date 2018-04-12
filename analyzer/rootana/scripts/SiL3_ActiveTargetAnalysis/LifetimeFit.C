void LifetimeFit() {

  ///////////////////////
  // User parameters
  std::string filename = "~/data/results/SiL3_active/EventSelection_NewCuts.root";
  std::string histname = "hTimeEnergy";
  //  std::string filename = "~/data/results/Si16b_passive/SiL3Plots.root";
  //  std::string histname = "hSiL3_Si16b_EnergyTime";
  std::string outfilename = "~/data/results/SiL3_active/LifetimeFit.root";

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
  hTime->Scale(1.0/bin_width); // to be per ns
  hTime->GetXaxis()->SetRangeUser(-1000, 20000);
  hTime->Draw("HIST E");

  double min_flat_time_fit = 15000;
  double max_flat_time_fit = 20000;
  TF1* flat_lifetime_fit = new TF1("flat_lifetime_fit", "[0]", min_flat_time_fit, max_flat_time_fit);
  flat_lifetime_fit->SetParameter(0, 100);
  hTime->Fit(flat_lifetime_fit, "R");
  flat_lifetime_fit->Draw("LSAME");

  double min_time_fit = 1000;
  double max_time_fit = 4000;
  TF1* lifetime_fit = new TF1("lifetime_fit", "[0]*TMath::Exp(-x/[1]) + [2]", min_time_fit, max_time_fit);
  lifetime_fit->SetParameters(hTime->GetMaximum(), 500, 10);
  lifetime_fit->FixParameter(2, flat_lifetime_fit->GetParameter(0));
  hTime->Fit(lifetime_fit, "R");
  lifetime_fit->Draw("LSAME");

  std::stringstream text;
  text << "#tau = " << std::setprecision(4) << lifetime_fit->GetParameter(1) << " #pm " << std::setprecision(2) << lifetime_fit->GetParError(1) << " ns";
  TLatex* lifetime_text = new TLatex(12500, hTime->GetMaximum()/1000, text.str().c_str());
  lifetime_text->SetTextAlign(22);
  lifetime_text->Draw();

  TF1* full_lifetime_fn = (TF1*) lifetime_fit->Clone("full_lifetime_fn");
  full_lifetime_fn->SetParameter(1, 800);
  full_lifetime_fn->SetRange(min_time_fit, max_flat_time_fit);
  full_lifetime_fn->SetLineColor(kBlue);
  full_lifetime_fn->SetLineStyle(kDashed);
  full_lifetime_fn->Draw("LSAME");

  double min_integral = min_time_fit;
  double max_integral = max_flat_time_fit;
  double fit_integral = full_lifetime_fn->Integral(min_integral, max_integral);
  std::cout << "AE: " << fit_integral << std::endl;

  int min_integral_bin = hTime->FindBin(min_integral);
  int max_integral_bin = hTime->FindBin(max_integral);
  double integral_error = 0;
  double hist_integral = hTime->IntegralAndError(min_integral_bin, max_integral_bin, integral_error, "width");
  std::cout << "AE: " << hist_integral << std::endl;
  std::cout << "Fraction Seen Events = " << hist_integral / fit_integral << std::endl;

  TFile* output = new TFile(outfilename.c_str(), "RECREATE");
  lifetime_fit->Write();
  output->Write();
  output->Close();
}
