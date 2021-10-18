#include "../../Utils/IntegrateRates.C"

void SiL3_FinalPlot_NormalisedSpectrum(std::string savedir = "", std::ostream& numbers_file = std::cout) {
    
  std::string filename = "~/data/results/SiL3/systematics_geq2TgtPulse_newPP20us_1_KFactor0-85_test.root";
  TFile* file = new TFile(filename.c_str(), "READ");

  const int n_bin_widths = 1;
  std::string bin_widths[n_bin_widths] = {"100keVBins"};//, "500keVBins"};
  
  const int n_ranges = 2;
  double min_energies[n_ranges] = {15, 1.4};
  double max_energies[n_ranges] = {17, 26};
  int i_bin_width_to_use[n_ranges] = {1, 0};
    
  double rates[n_ranges] = {0};
  double total_stat_errors[n_ranges] = {0};
  double total_low_syst_errors[n_ranges] = {0};
  double total_high_syst_errors[n_ranges] = {0};

  const int n_slices = 1;
  double min_time_slices[n_slices] = {3000};
  double max_time_slices[n_slices] = {4000};
  Int_t colours[n_slices] = {kRed};
  std::string leglabels[n_slices] = {"SiL3 (active target)"};
  
  /*  const int n_slices = 5;
  double min_time_slices[n_slices] = {2000, 2000, 2500, 3000, 3500};
  double max_time_slices[n_slices] = {4000, 2500, 3000, 3500, 4000};
  Int_t colours[n_slices] = {kBlack, kBlue, kRed, kSpring, kMagenta};
  std::string leglabels[n_slices] = {"2000 ns < t < 4000 ns", "2000 ns < t < 2500 ns", "2500 ns < t < 3000 ns", "3000 ns < t < 3500 ns", "3500 ns < t < 4000 ns"};
  */
  
  TLegend* leg = new TLegend(0.25,0.65,0.65,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.035);
  leg->SetFillColor(kWhite);
  
  int rebin_factor = 1;
  std::stringstream time_slice_str, leglabel;
  numbers_file << "% SiL3_FinalPlot_NormalisedSpectrum.C" << std::endl;
  for (int i_slice = 0; i_slice < n_slices; ++i_slice) {
    double i_min_time_slice = min_time_slices[i_slice];
    double i_max_time_slice = max_time_slices[i_slice];

    time_slice_str.str("");
    time_slice_str << "TimeSlice" << i_min_time_slice << "_" << i_max_time_slice;

    Int_t i_colour = colours[i_slice];

    for (int i_bin_width = 0; i_bin_width < n_bin_widths; ++i_bin_width) {
      std::string bin_width = bin_widths[i_bin_width];

      TCanvas* c1 = new TCanvas("c1", "c1");
      c1->SetLogy();
      for (int i_range = 0; i_range < n_ranges; ++i_range) {      
	rates[i_range] = 0;
	total_stat_errors[i_range] = 0;
	total_low_syst_errors[i_range] = 0;
	total_high_syst_errors[i_range] = 0;
      }
      
      std::string i_dirname = "FinalSystPlot_" + time_slice_str.str() + "_allRecoil_" + bin_width;
      //    std::string i_dirname = "FinalSystPlot_" + time_slice_str.str() + "_nuRecoil_500keVBins";
      std::cout << "AE: " << i_dirname << std::endl;
      std::string i_statname = i_dirname + "/hFinalStat";
      TGraphAsymmErrors* stat_spectrum = (TGraphAsymmErrors*) file->Get(i_statname.c_str());
      if (!stat_spectrum) {
	std::cout << "Error: Problem getting stat_spectrum " << i_statname << std::endl;
	return;
      }

      std::string i_systname = i_dirname + "/hFinalSyst";
      TGraphAsymmErrors* syst_spectrum = (TGraphAsymmErrors*) file->Get(i_systname.c_str());
      if (!syst_spectrum) {
	std::cout << "Error: Problem getting syst_spectrum " << i_systname << std::endl;
	return;
      }

      std::string i_statsystname = i_dirname + "/hFinalStatSyst";
      TGraphAsymmErrors* statsyst_spectrum = (TGraphAsymmErrors*) file->Get(i_statsystname.c_str());
      if (!statsyst_spectrum) {
	std::cout << "Error: Problem getting statsyst_spectrum " << i_statsystname << std::endl;
	return;
      }
      
      stat_spectrum->SetFillColor(kGreen);
      statsyst_spectrum->SetFillColor(kYellow);
      stat_spectrum->SetLineColor(kGreen);
      statsyst_spectrum->SetLineColor(kYellow);
      stat_spectrum->SetLineWidth(2);
      statsyst_spectrum->SetLineWidth(2);
    
      statsyst_spectrum->Draw("APE");
      statsyst_spectrum->GetXaxis()->SetTitle("Energy [keV]");
      statsyst_spectrum->GetYaxis()->SetTitle("Charged Particles / captured muon / keV");
      statsyst_spectrum->GetXaxis()->SetRangeUser(0, 26);
      statsyst_spectrum->GetYaxis()->SetRangeUser(1e-5, 1);
      stat_spectrum->Draw("PE SAME");

      if (i_bin_width == 0) {
	leglabel.str("");
	//      leglabel << leglabels[i_setting];
	leglabel << "stat. errors";
	leg->AddEntry(stat_spectrum, leglabel.str().c_str(), "l");
	
	leglabel.str("");
	leglabel << "stat. and syst. errors";
	leg->AddEntry(statsyst_spectrum, leglabel.str().c_str(), "l");
      }

      for (int i_range = 0; i_range < n_ranges; ++i_range) {
      
	double& rate = rates[i_range];
	double& total_stat_error = total_stat_errors[i_range];
	double& total_low_syst_error = total_low_syst_errors[i_range];
	double& total_high_syst_error = total_high_syst_errors[i_range];
	double min_energy = min_energies[i_range];
	double max_energy = max_energies[i_range];

	IntegrateRates_wStatAndSystSpectra(stat_spectrum, syst_spectrum, min_energy, max_energy,
					   rate, total_stat_error, total_high_syst_error, total_low_syst_error);

	std::cout << "SiL3 Rate (" << min_energy << " MeV -- " << max_energy << " MeV) = " << rate << " \\pm " << total_stat_error << " (stat.) (" << total_stat_error/rate * 100 << "%) + " << total_high_syst_error << " (" << total_high_syst_error/rate * 100 << "%) - " << total_low_syst_error << " (syst.) (" << total_low_syst_error/rate * 100 << "%)" << std::endl << std::endl;

	if (min_energy==15 && max_energy==17 && i_bin_width==i_bin_width_to_use[i_range]) {
	  numbers_file << std::fixed << std::setprecision(2);
	  numbers_file << "\\newcommand\\SiLCompRateBare{" << "" << rates[i_range]/1e-3 << "}" << std::endl;
	  numbers_file << "\\newcommand\\SiLCompStatErrBare{" << total_stat_errors[i_range]/1e-3 << "}" << std::endl;
	  numbers_file << "\\newcommand\\SiLCompHighSystErrBare{" << total_high_syst_errors[i_range]/1e-3 << "}" << std::endl;
	  numbers_file << "\\newcommand\\SiLCompLowSystErrBare{" << total_low_syst_errors[i_range]/1e-3 << "}" << std::endl;
	  numbers_file << "\\newcommand\\SiLCompSystFracUncertainty{$^{+" << (total_high_syst_errors[i_range]/rate)*100 << "\\%}_{-" << (total_low_syst_errors[i_range]/rate)*100 << "\\%}$}" << std::endl;
	  numbers_file << "\\newcommand\\SiLCompRateFull{$(\\SiLCompRateBare \\pm \\SiLCompStatErrBare ~\\text{(stat.)} ^{+\\SiLCompHighSystErrBare}_{-\\SiLCompLowSystErrBare}~\\text{(syst.)}) \\times 10^{-3}$}" << std::endl;
	}
	if (min_energy==1.4 && max_energy==26 && i_bin_width==i_bin_width_to_use[i_range]) {
	  numbers_file << std::fixed << std::setprecision(1);
	  numbers_file << "\\newcommand\\SiLActiveRange{" << min_energy << " -- " << std::setprecision(0) << max_energy << "}" << std::endl;
	  numbers_file << std::fixed << std::setprecision(4);
	  numbers_file << "\\newcommand\\SiLActiveRateBare{" << "" << rates[i_range] << "}" << std::endl;
	  numbers_file << "\\newcommand\\SiLActiveStatErrBare{" << total_stat_errors[i_range] << "}" << std::endl;
	  numbers_file << "\\newcommand\\SiLActiveHighSystErrBare{" << total_high_syst_errors[i_range] << "}" << std::endl;
	  numbers_file << "\\newcommand\\SiLActiveLowSystErrBare{" << total_low_syst_errors[i_range] << "}" << std::endl;
	  numbers_file << std::fixed << std::setprecision(2);
	  numbers_file << "\\newcommand\\SiLActiveSystFracUncertainty{$^{+" << (total_high_syst_errors[i_range]/rate)*100 << "\\%}_{-" << (total_low_syst_errors[i_range]/rate)*100 << "\\%}$}" << std::endl;
	  numbers_file << "\\newcommand\\SiLActiveRate{$\\SiLActiveRateBare \\pm \\SiLActiveStatErrBare ~\\text{(stat.)} ^{+\\SiLActiveHighSystErrBare}_{-\\SiLActiveLowSystErrBare}~\\text{(syst.)}$}" << std::endl;
	}
      }

      leg->Draw();

      if (savedir != "") {
	std::string savename = savedir + "AlCapData_SiL3Dataset_NormalisedSpectrum_StatSystErrors_" + bin_width;

	std::string pdfname = savename + ".pdf";
	c1->SaveAs(pdfname.c_str());
	std::string pngname = savename + ".png";
	c1->SaveAs(pngname.c_str());
      }
    }
  }

  numbers_file << std::endl;
}
