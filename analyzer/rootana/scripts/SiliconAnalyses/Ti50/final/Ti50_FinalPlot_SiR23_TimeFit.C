void Ti50_FinalPlot_SiR23_TimeFit() {

  gStyle->SetOptFit(1);

  std::string plots_file_name = "~/data/results/Ti50/plots.root";
  std::string full_spectrum_name = "SiR23/hEnergyTime";
    
  TFile* plots_file = new TFile(plots_file_name.c_str(), "READ");
  TH2F* hEnergyTime = (TH2F*) plots_file->Get(full_spectrum_name.c_str());
  
  TCanvas* c_EnergyTime = new TCanvas("c_EnergyTime", "c_EnergyTime");
  c_EnergyTime->SetLogy();

  double min_energy = 0;
  double max_energy = 10000;
  double min_time_axis = -1000;
  double max_time_axis = 20000;
  int rebin_factor = 10;

  int min_energy_bin = hEnergyTime->GetYaxis()->FindBin(min_energy);
  int max_energy_bin = hEnergyTime->GetYaxis()->FindBin(max_energy)-1;
  TH1D* hTime = hEnergyTime->ProjectionX("Ti50", min_energy_bin, max_energy_bin);

  std::stringstream histtitle;
  histtitle << "SiR2 Time (!SiR1 && SiR3), " << min_energy << " keV < SiR2.E < " << max_energy << " keV";
  hTime->SetTitle(histtitle.str().c_str());
  hTime->Rebin(rebin_factor);
  hTime->GetXaxis()->SetRangeUser(min_time_axis,max_time_axis);
  hTime->Draw("HIST E");

  std::string equation = "[0]*TMath::Exp(-x/[1]) + [2] + [3]*x";
  TF1* lifetime_fit = new TF1("lifetime_fit", equation.c_str(), 500, 20000);
  lifetime_fit->SetParameters(1000, 100, 1000, -0.1);
  hTime->Fit(lifetime_fit, "R");
  lifetime_fit->Draw("LSAME");

  TLatex* latex = new TLatex();
  //  latex->DrawLatexNDC(0.15, 0.70, "AlCap Preliminary");
  latex->DrawLatexNDC(0.33, 0.70, "AlCap Preliminary");

  latex->SetTextSize(0.04);
  latex->DrawLatexNDC(0.20, 0.60, equation.c_str());

  /*  std::stringstream fit_parameters;
  fit_parameters.str("");
  fit_parameters << "#tau = " << std::setprecision(3) << lifetime_fit->GetParameter(1) << " #pm " << std::setprecision(2) << lifetime_fit->GetParError(1) << " ns";
  latex->DrawLatexNDC(0.40, 0.55, fit_parameters.str().c_str());
  */
}
