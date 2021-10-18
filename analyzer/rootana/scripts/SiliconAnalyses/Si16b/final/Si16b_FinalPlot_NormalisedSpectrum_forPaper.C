#include "../../Utils/IntegrateRates.C"

void Si16b_FinalPlot_NormalisedSpectrum_forPaper(std::string savedir = "", std::ostream& numbers_file = std::cout) {
    
  //  std::string filename = "~/data/results/Si16b/systematics_newPP_geq1TgtPulse_1.root";
  std::string filename = "~/data/results/Si16b/systematics_newPP_geq1TgtPulse_2.root";
  TFile* file = new TFile(filename.c_str(), "READ");

  double x_min = 0;
  double x_max = 20;//17;
  
  const int n_ranges = 1;
  double min_energies[n_ranges] = {15};
  double max_energies[n_ranges] = {17};
  
  const int n_settings = 4;
  std::string particle_names[n_settings] = {"proton", "deuteron", "triton", "alpha"};
  std::string Particle_names[n_settings] = {"Proton", "Deuteron", "Triton", "Alpha"};
  Int_t colours[n_settings] = {kRed, kBlue, kGreen, kMagenta};//{kRed, kCyan, kMagenta, kSpring};
  Int_t fill_styles[n_settings] = {3244, 3315, 3351, 3481};
  Int_t marker_styles[n_settings] = {kFullCircle, kStar, kFullSquare, kFullTriangleUp};
  std::string leglabels[n_settings] = {"protons", "deuterons", "tritons", "alphas"};
  double min_drawn_ranges[n_settings] = {2, 3, 3, 10};
  double max_drawn_ranges[n_settings] = {20, 19, 18, 20};
  double min_best_ranges[n_settings] = {3, 5, 5, 15};
  double max_best_ranges[n_settings] = {17, 17, 17, 20};
  
  TH2F* hAllRates[n_settings] = {0};
  TH2F* hAllStatErrors[n_settings] = {0};
  TH2F* hAllHighSystErrors[n_settings] = {0};
  TH2F* hAllLowSystErrors[n_settings] = {0};
  
  double rates[n_ranges][n_settings] = {0};
  double total_stat_errors[n_ranges][n_settings] = {0};
  double total_low_syst_errors[n_ranges][n_settings] = {0};
  double total_high_syst_errors[n_ranges][n_settings] = {0};

  double sum_rates[n_ranges] = {0};
  double sum_stat_errors[n_ranges] = {0};
  double sum_low_syst_errors[n_ranges] = {0};
  double sum_high_syst_errors[n_ranges] = {0};
  
  TLegend* leg = new TLegend(0.70,0.5,0.85,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.035);
  leg->SetFillColor(kWhite);

  TLegend* leg_for_all = new TLegend(0.60,0.60,0.85,0.85);
  leg_for_all->SetBorderSize(0);
  leg_for_all->SetTextSize(0.035);
  leg_for_all->SetFillColor(kWhite);
  leg_for_all->SetHeader("AlCap Si");

  std::stringstream leglabel;
  TCanvas* c_all_lin = new TCanvas();

  TCanvas* c_all_log = new TCanvas();
  c_all_log->SetLogy();
  numbers_file << "% Si16b_FinalPlot_NormalisedSpectrum.C" << std::endl;
  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
    //    c1->SetLogy();

    std::string particle = particle_names[i_setting];
    std::string Particle = Particle_names[i_setting];
    Int_t i_colour = colours[i_setting];
    Int_t i_marker_style = marker_styles[i_setting];

    std::string i_dirname = "FinalSystPlot_" + particle + "_TCutG_2sig_layerCoinc500ns_tGT0ns_BinW500keV";

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

    // Remove 18 MeV deuteron point
    if (particle == "deuteron") {
      for (int i_point = 0; i_point < stat_spectrum->GetN(); ++i_point) {
	if ( *(stat_spectrum->GetX()+i_point) > 18 && *(stat_spectrum->GetX()+i_point) < 19) {
	  stat_spectrum->RemovePoint(i_point);
	  statsyst_spectrum->RemovePoint(i_point);	  
	}
      }
    }
    
    //    stat_spectrum->SetFillColor(kGreen);
    //    statsyst_spectrum->SetFillColor(kOrange);
    stat_spectrum->SetLineColor(kGreen);
    statsyst_spectrum->SetLineColor(kOrange);
    stat_spectrum->SetLineWidth(2);
    statsyst_spectrum->SetLineWidth(2);
    
    //    statsyst_spectrum->Draw("APE");
    statsyst_spectrum->GetXaxis()->SetRangeUser(x_min, x_max);
    statsyst_spectrum->GetXaxis()->SetTitle("Energy [MeV]");
    statsyst_spectrum->GetYaxis()->SetTitle("Particles / muon capture / MeV");
    statsyst_spectrum->GetXaxis()->SetTitleOffset(1.0);
    statsyst_spectrum->GetYaxis()->SetTitleOffset(1.0);
    //    stat_spectrum->Draw("PE SAME");

    TLatex* latex = new TLatex();
    //    latex->DrawLatexNDC(0.6, 0.5, "AlCap #bf{#it{Preliminary} }");
    //    stat_spectrum->SetTitle("Si16b Dataset");
    //    stat_spectrum->SetLineColor(i_colour);
    //    stat_spectrum->GetXaxis()->SetRangeUser(0,26000);
    //    stat_spectrum->GetXaxis()->SetRangeUser(2000,16000);

    // alcaphistogram(stat_spectrum);
    // if (i_setting == 0) {
    //   alcapPreliminary(stat_spectrum);
    //   stat_spectrum->SetDrawOption("HIST E1");
    // }
    // else {
    //   stat_spectrum->SetDrawOption("HIST E1 SAME");
    // }

    
    c_all_lin->cd();
    TGraphErrors* for_all = (TGraphErrors*) statsyst_spectrum->Clone("for_all");
    for_all->SetLineColor(i_colour);
    for_all->SetLineWidth(2);
    for_all->SetMarkerStyle(i_marker_style);
    for_all->SetMarkerColor(i_colour);
    for (int i_point = 0; i_point < for_all->GetN(); ++i_point) {
      if (*(for_all->GetX()+i_point) < min_drawn_ranges[i_setting]
	  || *(for_all->GetX()+i_point) > max_drawn_ranges[i_setting]) {
	for_all->RemovePoint(i_point);
	--i_point;
      }
    }
    //    for_all->SetFillColor(i_colour);
    //    for_all->SetFillStyle(fill_styles[i_setting]);
    TGraphErrors* for_all_stats = (TGraphErrors*) stat_spectrum->Clone("for_all_stats");
    //    for_all_stats->SetLineColor(kBlack);
    for_all_stats->SetLineColor(i_colour);
    //    for_all_stats->SetMarkerColor(kBlack);
    for_all_stats->SetMarkerColor(i_colour);
    for_all_stats->SetMarkerStyle(i_marker_style);
    leglabel.str("");
    leglabel << leglabels[i_setting];// << " (stat.+syst. errors)";
    leg_for_all->AddEntry(for_all, leglabel.str().c_str(), "lp");
    if (i_setting==0) {
      for_all->Draw("APEL");
    }
    else {
      for_all->Draw("PEL SAME");
    }
    for_all_stats->Draw("PE SAME");
    //    latex->DrawLatexNDC(0.6, 0.5, "AlCap #bf{#it{Preliminary} }");


    c_all_log->cd();
    TGraphErrors* for_all_log = (TGraphErrors*) for_all->Clone("for_all_log");
    if (i_setting==0) {
      for_all_log->Draw("APEL");
      for_all_log->GetYaxis()->SetRangeUser(1e-5, 1e-1);
      //      latex->DrawLatexNDC(0.17, 0.75, "AlCap #bf{#it{Preliminary} }");
    }
    else {
      for_all_log->Draw("PEL SAME");
    }
    for_all_stats->Draw("PE SAME");
    //    arrow->Draw();
    //    arrow_latex->DrawLatex(arrow_latex_x, arrow_latex_y, text.str().c_str());
    
    for (int i_range = 0; i_range < n_ranges; ++i_range) {

      double& rate = rates[i_range][i_setting];
      double& total_stat_error = total_stat_errors[i_range][i_setting];
      double& total_low_syst_error = total_low_syst_errors[i_range][i_setting];
      double& total_high_syst_error = total_high_syst_errors[i_range][i_setting];
      double min_energy = min_energies[i_range];
      double max_energy = max_energies[i_range];

      IntegrateRates_wStatAndSystSpectra(stat_spectrum, syst_spectrum, min_energy, max_energy,
					 rate, total_stat_error, total_high_syst_error, total_low_syst_error);
      //    rate[i_range] = rate;
      //      std::cout << particle << " Rate (" << min_energy / 1000 << " MeV -- " << max_energy / 1000 << " MeV) = " << rate << " \\pm " << total_stat_error << " (stat.) (" << total_stat_error/rate * 100 << "%) + " << total_high_syst_error << " (" << total_high_syst_error/rate * 100 << "%) - " << total_low_syst_error << " (syst.) (" << total_low_syst_error/rate * 100 << "%)" << std::endl << std::endl;

      sum_rates[i_range] += rate;
      sum_low_syst_errors[i_range] += total_low_syst_error*total_low_syst_error;
      sum_high_syst_errors[i_range] += total_high_syst_error*total_high_syst_error;
      sum_stat_errors[i_range] += total_stat_error*total_stat_error;
    }

    double min_range = 0;
    double max_range = 25;
    double max_range_step = 25;
    double min_range_step = 1;
    int n_bins = (max_range - min_range) / min_range_step;
    std::string histname = "hAllRates_" + particle;
    hAllRates[i_setting] = new TH2F(histname.c_str(), "", n_bins,min_range,max_range, n_bins,min_range,max_range);
    hAllRates[i_setting]->SetXTitle("Min Energy");
    hAllRates[i_setting]->SetYTitle("Max Energy");
    histname = "hAllStatErrors_" + particle;
    hAllStatErrors[i_setting] = new TH2F(histname.c_str(), "", n_bins,min_range,max_range, n_bins,min_range,max_range);
    hAllStatErrors[i_setting]->SetXTitle("Min Energy");
    hAllStatErrors[i_setting]->SetYTitle("Max Energy");
    histname = "hAllHighSystErrors_" + particle;
    hAllHighSystErrors[i_setting] = new TH2F(histname.c_str(), "", n_bins,min_range,max_range, n_bins,min_range,max_range);
    hAllHighSystErrors[i_setting]->SetXTitle("Min Energy");
    hAllHighSystErrors[i_setting]->SetYTitle("Max Energy");
    histname = "hAllLowSystErrors_" + particle;
    hAllLowSystErrors[i_setting] = new TH2F(histname.c_str(), "", n_bins,min_range,max_range, n_bins,min_range,max_range);
    hAllLowSystErrors[i_setting]->SetXTitle("Min Energy");
    hAllLowSystErrors[i_setting]->SetYTitle("Max Energy");

    for (double range_step = max_range_step; range_step >= min_range_step; range_step -= min_range_step) {
      for (double min_energy = min_range; min_energy < max_range; min_energy += min_range_step) {
	double max_energy = min_energy+range_step;
	if (max_energy > max_range) {
	  continue;
	}
	else if (particle == "proton" && max_energy>17) {
	  continue;
	}
	else if (particle == "deuteron" && max_energy>17) {
	  continue;
	}
	else if (particle == "triton" && max_energy>17) {
	  continue;
	}

	double rate = 0;
	double total_stat_error = 0;
	double total_high_syst_error = 0;
	double total_low_syst_error = 0;
	IntegrateRates_wStatAndSystSpectra(stat_spectrum, syst_spectrum, min_energy, max_energy,
					   rate, total_stat_error, total_high_syst_error, total_low_syst_error);
	//	std::cout << particle << ": Rate (" << min_energy << " -- " << max_energy << " MeV) = " << rate << std::endl;
	hAllRates[i_setting]->Fill(min_energy,max_energy-0.500,rate);
	hAllStatErrors[i_setting]->Fill(min_energy,max_energy-0.500,total_stat_error);
	hAllHighSystErrors[i_setting]->Fill(min_energy,max_energy-0.500,total_high_syst_error);
	hAllLowSystErrors[i_setting]->Fill(min_energy,max_energy-0.500,total_low_syst_error);
      }
    }
    double min_frac_uncertainty = 99999;
    int i_best_bin = 0;
    int j_best_bin = 0;
    for (int i_bin = 1; i_bin <= hAllRates[i_setting]->GetNbinsX(); ++i_bin) {
      for (int j_bin = 1; j_bin <= hAllRates[i_setting]->GetNbinsY(); ++j_bin) {
	double min_energy = hAllRates[i_setting]->GetXaxis()->GetBinLowEdge(i_bin);
	double max_energy = hAllRates[i_setting]->GetXaxis()->GetBinUpEdge(j_bin);
	double rate = hAllRates[i_setting]->GetBinContent(i_bin, j_bin);
	if (rate <= 0) {
	  continue;
	}
	double stat_error = hAllStatErrors[i_setting]->GetBinContent(i_bin, j_bin);
	double high_syst_error = hAllHighSystErrors[i_setting]->GetBinContent(i_bin, j_bin);
	double low_syst_error = hAllLowSystErrors[i_setting]->GetBinContent(i_bin, j_bin);

	double largest_syst_error = high_syst_error;
	if (low_syst_error > largest_syst_error) {
	  largest_syst_error = low_syst_error;
	}
	double frac_uncertainty = std::sqrt(stat_error*stat_error + high_syst_error*high_syst_error + low_syst_error*low_syst_error);
	//	double frac_uncertainty = std::sqrt(stat_error*stat_error + largest_syst_error*largest_syst_error);
	frac_uncertainty /= rate;
	//	std::cout << particle << ": Rate (" << min_energy/1000 << " -- " << max_energy/1000 << " MeV) = " << rate << " (" << frac_uncertainty*100 << "%)" << std::endl;
	if (frac_uncertainty < min_frac_uncertainty) {
	  min_frac_uncertainty = frac_uncertainty;
	  i_best_bin = i_bin;
	  j_best_bin = j_bin;	    
	}
      }
    }
    
    double min_energy = min_best_ranges[i_setting];//hAllRates[i_setting]->GetXaxis()->GetBinLowEdge(i_best_bin);
    double max_energy = max_best_ranges[i_setting];//hAllRates[i_setting]->GetXaxis()->GetBinUpEdge(j_best_bin);
    i_best_bin = hAllRates[i_setting]->GetXaxis()->FindBin(min_energy);
    j_best_bin = hAllRates[i_setting]->GetYaxis()->FindBin(max_energy)-1;
    double rate = hAllRates[i_setting]->GetBinContent(i_best_bin, j_best_bin);
    double stat_error = hAllStatErrors[i_setting]->GetBinContent(i_best_bin, j_best_bin);
    double high_syst_error = hAllHighSystErrors[i_setting]->GetBinContent(i_best_bin, j_best_bin);
    double low_syst_error = hAllLowSystErrors[i_setting]->GetBinContent(i_best_bin, j_best_bin);
    //    std::cout << "Best: (" << min_energy/1000 << " -- " << max_energy/1000 << " MeV) = " << rate << " +/- " << stat_error << " (stat.) + " << high_syst_error << " - " << low_syst_error << " (syst.): " << min_frac_uncertainty*100 << "%" << std::endl;

    // particle_canvases[i_setting]->cd();
    // //    std::cout << "\tAE: " << std::fixed << std::setprecision(5) << hAllRates[i_setting]->GetBinContent(i_best_bin, i_best_bin) << std::endl;
    // double line_max_y = hAllRates[i_setting]->GetBinContent(i_best_bin, i_best_bin)/1000.0;
    // TLine* best_line_min = new TLine(min_energy, 0, min_energy, line_max_y);
    // best_line_min->SetLineColor(colours[i_setting]);
    // best_line_min->SetLineWidth(2);
    // best_line_min->Draw("LSAME");
    // TLine* best_line_max = new TLine(max_energy, 0, max_energy, line_max_y);
    // best_line_max->SetLineColor(colours[i_setting]);
    // best_line_max->SetLineWidth(2);
    // best_line_max->Draw("LSAME");

    // double rate_line_y = line_max_y/2;
    // TArrow* rate_line = new TArrow(min_energy, rate_line_y, max_energy, rate_line_y, 0.02, "<>");
    // rate_line->SetLineColor(colours[i_setting]);
    // rate_line->SetFillColor(colours[i_setting]);
    // rate_line->SetLineWidth(2);
    // rate_line->Draw();

    // TLatex* latex2 = new TLatex();
    // latex2->SetTextSize(0.035);
    // latex2->SetTextAlign(22);
    // latex2->SetTextColor(colours[i_setting]);
    // std::stringstream rate_text;
    // rate_text.str("");
    // rate_text << std::fixed << std::setprecision(0) << min_energy/1000 << " MeV -- " << max_energy/1000 << " MeV";
    // std::cout << rate_text.str() << std::endl;
    // latex2->DrawLatexNDC(0.5, 0.4, rate_text.str().c_str());

    // rate_text.str("");
    // rate_text << std::fixed << std::setprecision(2) << "(" << rate/1e-3 << " #pm " << stat_error/1e-3 << " (stat.) ^{+" << high_syst_error/1e-3 << "}_{-" << low_syst_error/1e-3 << "} (syst.)) #times 10^{-3} " << particle << "s per muon capture";
    // std::cout << rate_text.str() << std::endl;
    // latex2->DrawLatexNDC(0.5, 0.35, rate_text.str().c_str());

    numbers_file << std::fixed << std::setprecision(0);
    numbers_file << "\\newcommand\\Sib" << Particle << "BestRange{" << min_energy << " -- " << max_energy << "}" << std::endl;
    numbers_file << std::fixed << std::setprecision(2);
    numbers_file << "\\newcommand\\Sib" << Particle << "BestRateBare{" << "" << rate/1e-3 << "}" << std::endl;
    numbers_file << "\\newcommand\\Sib" << Particle << "BestStatErrBare{" << stat_error/1e-3 << "}" << std::endl;
    numbers_file << "\\newcommand\\Sib" << Particle << "BestHighSystErrBare{" << high_syst_error/1e-3 << "}" << std::endl;
    numbers_file << "\\newcommand\\Sib" << Particle << "BestLowSystErrBare{" << low_syst_error/1e-3 << "}" << std::endl;

    //    numbers_file << "\\newcommand\\Sib" << Particle << "BestRateFull{$(\\Sib" << Particle << "BestRateBare \\pm \\Sib" << Particle << "BestStatErrBare ~\\text{(stat.)} ^{+\\Sib" << Particle << "BestHighSystErrBare}_{-\\Sib" << Particle << "BestLowSystErrBare}~\\text{(syst.)}) \\times 10^{-3}$}" << std::endl;
    numbers_file << "\\newcommand\\Sib" << Particle << "BestRateFull{$\\Sib" << Particle << "BestRateBare \\pm \\Sib" << Particle << "BestStatErrBare ~\\text{(stat.)} \\pm";
    if (high_syst_error > low_syst_error){
      numbers_file << " \\Sib" << Particle << "BestHighSystErrBare";
    }
    else {
      numbers_file << " \\Sib" << Particle << "BestLowSystErrBare";
    }
    numbers_file << "~\\text{(syst.)}$}" << std::endl;//) \\times 10^{-3}$}" << std::endl;
    
    numbers_file << "\\newcommand\\Sib" << Particle << "BestStatFracUncertainty{$\\pm" << (stat_error/rate)*100 << "\\%$}" << std::endl;
    numbers_file << "\\newcommand\\Sib" << Particle << "BestSystFracUncertainty{$^{+" << (high_syst_error/rate)*100 << "\\%}_{-" << (low_syst_error/rate)*100 << "\\%}$}" << std::endl;
    numbers_file << std::setprecision(1);
    numbers_file << "\\newcommand\\Sib" << Particle << "BestFracUncertainty{\\Sib" << Particle << "BestStatFracUncertainty ~(stat.) \\Sib" << Particle << "BestSystFracUncertainty ~(syst.)}" << std::endl;
    //    numbers_file << "\\newcommand\\Sib" << Particle << "BestFracUncertainty{$" << min_frac_uncertainty*100 << "\\%$}" << std::endl;
  }

  for (int i_range = 0; i_range < n_ranges; ++i_range) {
    sum_low_syst_errors[i_range] = std::sqrt(sum_low_syst_errors[i_range]);
    sum_high_syst_errors[i_range] = std::sqrt(sum_high_syst_errors[i_range]);
    sum_stat_errors[i_range] = std::sqrt(sum_stat_errors[i_range]);
    
    for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
      std::string particle = particle_names[i_setting];
      std::string Particle = Particle_names[i_setting];
      Int_t i_colour = colours[i_setting];

      double ratio = rates[i_range][i_setting] / sum_rates[i_range];
      //	double ratio_err = std::sqrt( (ratio*(1-ratio)) / (sum_rates[i_range]) );
      //	std::cout << "Ratio (" << std::setprecision(4) << leglabels[i_setting] << ") = " << ratio << " +/- " << ratio_err << std::endl;
      
      double ratio_stat_err = ratio*std::sqrt( (total_stat_errors[i_range][i_setting]/rates[i_range][i_setting])*(total_stat_errors[i_range][i_setting]/rates[i_range][i_setting]) + (sum_stat_errors[i_range]/sum_rates[i_range])*(sum_stat_errors[i_range]/sum_rates[i_range]) );
      double ratio_low_syst_err = ratio*std::sqrt( (total_low_syst_errors[i_range][i_setting]/rates[i_range][i_setting])*(total_low_syst_errors[i_range][i_setting]/rates[i_range][i_setting]) + (sum_low_syst_errors[i_range]/sum_rates[i_range])*(sum_low_syst_errors[i_range]/sum_rates[i_range]) );
      double ratio_high_syst_err = ratio*std::sqrt( (total_high_syst_errors[i_range][i_setting]/rates[i_range][i_setting])*(total_high_syst_errors[i_range][i_setting]/rates[i_range][i_setting]) + (sum_high_syst_errors[i_range]/sum_rates[i_range])*(sum_high_syst_errors[i_range]/sum_rates[i_range]) );

      double ratio_high_total_err = std::sqrt(ratio_stat_err*ratio_stat_err + ratio_high_syst_err*ratio_high_syst_err);
      double ratio_low_total_err = std::sqrt(ratio_stat_err*ratio_stat_err + ratio_low_syst_err*ratio_low_syst_err);
	
      numbers_file << std::fixed << std::setprecision(1);
      numbers_file << "\\newcommand\\" << particle << "RatioBare{" << ratio*100 << "}" << std::endl;
      numbers_file << "\\newcommand\\" << particle << "RatioStatErrBare{" << ratio_stat_err*100 << "}" << std::endl;
      numbers_file << "\\newcommand\\" << particle << "RatioHighSystErrBare{" << ratio_high_syst_err*100 << "}" << std::endl;
      numbers_file << "\\newcommand\\" << particle << "RatioLowSystErrBare{" << ratio_low_syst_err*100 << "}" << std::endl;
      numbers_file << "\\newcommand\\" << particle << "RatioHighTotalErrBare{" << ratio_high_total_err*100 << "}" << std::endl;
      numbers_file << "\\newcommand\\" << particle << "RatioLowTotalErrBare{" << ratio_low_total_err*100 << "}" << std::endl;
      numbers_file << "\\newcommand\\" << particle << "RatioNoErr{\\" << particle << "RatioBare\\%}" << std::endl;
      numbers_file << "\\newcommand\\" << particle << "RatioFull{$(\\" << particle << "RatioBare \\pm \\" << particle << "RatioStatErrBare~\\text{(stat.)} ^{+\\" << particle << "RatioHighSystErrBare}_{-\\" << particle << "RatioLowSystErrBare}~\\text{(syst.)})\\%$}" << std::endl;
      numbers_file << "\\newcommand\\" << particle << "RatioCondensed{$\\" << particle << "RatioBare ^{+\\" << particle << "RatioHighTotalErrBare}_{-\\" << particle << "RatioLowTotalErrBare}\\%$}" << std::endl;
    }
  }
  
  c_all_lin->cd();
  leg_for_all->Draw();

  c_all_log->cd();
  leg_for_all->Draw();  
  
  if (savedir != "") {
    std::string savename = savedir + "AlCapData_Si16bDataset_NormalisedSpectrum_forPaper";

    std::string pdfname = savename + "_LinY.pdf";
    c_all_lin->SaveAs(pdfname.c_str());
    std::string pngname = savename + "_LinY.png";
    c_all_lin->SaveAs(pngname.c_str());

    pdfname = savename + "_LogY.pdf";
    c_all_log->SaveAs(pdfname.c_str());
    pngname = savename + "_LogY.png";
    c_all_log->SaveAs(pngname.c_str());
  }
}
