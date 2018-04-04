#include "scripts/PlotTME_Si16b_EvdE/PlotFromNtuple.C"

#include <sstream>

void Andy_Si16b_ProtonBandExtraction(std::string plotfile) {

  TFile* file = new TFile(plotfile.c_str(), "READ");

  std::vector<Cut*> cuts;
  std::stringstream eqn_string;

  // Electron Spot Cut
  // - define the equation
  double x_1 = 0, y_1 = 2250, x_2 = 5500, y_2 = 0;
  double electron_spot_gradient = (y_2 - y_1) / (x_2 - x_1);
  double electron_spot_yoffset = y_1;
  eqn_string.str("");
  eqn_string << electron_spot_gradient << "*x + " << electron_spot_yoffset;
  Cut* electron_spot_cut = new Cut("electron_spot_cut", eqn_string.str(), x_varname, y_varname, true);
  cuts.push_back(electron_spot_cut);

  // Punch Through Cut
  double punch_through_yoffset = 300;
  eqn_string.str("");
  eqn_string << punch_through_yoffset;
  Cut* punch_through_cut = new Cut("punch_through_cut", eqn_string.str(), x_varname, y_varname, true);
  cuts.push_back(punch_through_cut);

  // Deuteron Band Cut
  double deuteron_cut_peak = 4500;
  double deuteron_cut_slope = -0.0004;
  double deuteron_cut_yoffset = 500;
  eqn_string.str("");
  eqn_string << deuteron_cut_peak << "*TMath::Exp(" << deuteron_cut_slope << "*x) + " << deuteron_cut_yoffset;
  Cut* deuteron_cut = new Cut("deuteron_cut", eqn_string.str(), x_varname, y_varname, false);
  cuts.push_back(deuteron_cut);

  // High Energy Cut
  double high_energy_cut_xoffset = 10000;
  eqn_string.str("");
  eqn_string << high_energy_cut_xoffset;
  Cut* high_energy_cut = new Cut ("high_energy_cut", eqn_string.str(), x_varname, y_varname, false, true);
  //  cuts.push_back(high_energy_cut);

  // Output file
  TFile* file = new TFile("scripts/PlotTME_Si16b_EvdE/Si16b_plots.root", "RECREATE");

  // Create the full EvdE plot
  double min_x_energy = 0;
  double max_x_energy = 20000;
  double min_y_energy = 0;
  double max_y_energy = 3000;
  double energy_width = 10;
  int n_x_energy_bins = (max_x_energy - min_x_energy) / energy_width;
  int n_y_energy_bins = (max_y_energy - min_y_energy) / energy_width;

  std::vector<Cut*> no_cuts;
  std::string histname = "hFullEvdE";
  TH2F* hFullEvdEPlot = (TH2F*) PlotFromNTuple("~/data/out/v22/Si16b.root", no_cuts, "PlotTME_Si16b_EvdE", "SiRHits", histname, x_varname,n_x_energy_bins,min_x_energy,max_x_energy, y_varname,n_y_energy_bins,min_y_energy,max_y_energy);
  if (!hFullEvdEPlot) {
    std::cout << "Error creating plot" << std::endl;
    return;
  }
  std::cout << histname << " : " << hFullEvdEPlot->GetEntries() << " entries" << std::endl;

  file->cd();
  hFullEvdEPlot->Write();

  // Get the proton band
  histname = "hProtonBand";
  TH2F* hProtonBand = (TH2F*) PlotFromNTuple("~/data/out/v22/Si16b.root", cuts, "PlotTME_Si16b_EvdE", "SiRHits", histname, x_varname,n_x_energy_bins,min_x_energy,max_x_energy, y_varname,n_y_energy_bins,min_y_energy,max_y_energy);
  if (!hProtonBand) {
    std::cout << "Error creating plot" << std::endl;
    return;
  }
  std::cout << histname << " : " << hProtonBand->GetEntries() << " entries" << std::endl;

  // Integrate
  double integral_low = 2500;
  double integral_high = 10000;
  double bin_integral_low = hProtonBand->GetXaxis()->FindBin(integral_low);
  double bin_integral_high = hProtonBand->GetXaxis()->FindBin(integral_high);
  std::cout << "Integral (" << integral_low/1000 << " - " << integral_high/1000 << " MeV) = " 
	    << hProtonBand->ProjectionX()->Integral(bin_integral_low, bin_integral_high) << std::endl;

  TCanvas* c = new TCanvas("c", "c");
  hProtonBand->Draw("COLZ");
  // Also draw the cuts
  for (std::vector<Cut*>::const_iterator i_cut = cuts.begin(); i_cut != cuts.end(); ++i_cut) {
    (*i_cut)->Draw(c);
  }

  TCanvas* c_folded = new TCanvas("c_folded", "c_folded");
  TH1D* hFoldedSpectrum = hProtonBand->ProjectionX();
  hFoldedSpectrum->Rebin(10); // rebin to 100 keV bins
  hFoldedSpectrum->Draw("HIST E");

  file->cd();
  hProtonBand->Write();
  c->Write();
  hFoldedSpectrum->Write();

  // Now draw the lifetime plot
  histname = "hLifetime";
  x_varname="SiR1Time";
  double min_time = -5000;
  double max_time = 10000;
  double time_bin_width = 100;
  int n_time_bins = (max_time - min_time) / time_bin_width;
  TH1F* hLifetime = (TH1F*) PlotFromNTuple("~/data/out/v22/Si16b.root", cuts, "PlotTME_Si16b_EvdE", "SiRHits", histname, x_varname, n_time_bins,min_time,max_time);
  if (!hLifetime) {
    std::cout << "Error creating plot" << std::endl;
    return;
  }
  std::cout << histname << " : " << hLifetime->GetEntries() << " entries" << std::endl;

  TCanvas* c_lifetime = new TCanvas("c_lifetime", "c_lifetime");
  c_lifetime->SetLogy();
  
  TF1* lifetime_fn = new TF1("lifetime_fn", "[0]*TMath::Exp(-x/[1])", 100,2000);
  lifetime_fn->SetParameters(1000, 750);
  hLifetime->Fit(lifetime_fn, "R");
  hLifetime->Draw("HIST E");
  lifetime_fn->Draw("LSAME");
  
  file->cd();
  hLifetime->Write();
  c_lifetime->Write();
  
  file->Write();
  file->Close();
}
