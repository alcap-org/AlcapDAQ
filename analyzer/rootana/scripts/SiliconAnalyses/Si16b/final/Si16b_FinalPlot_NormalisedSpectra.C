#include "../..//Utils/IntegrateRates.C"

void Si16b_FinalPlot_NormalisedSpectra() {

  const int n_particles = 4;
  std::string particles[n_particles] = {"proton", "deuteron", "triton", "alpha"};
  Int_t colours[n_particles] = {kRed, kBlue, kMagenta, kSpring};
  TGraphAsymmErrors* hOutCombineds[n_particles];
  TGraphAsymmErrors* hOutStats[n_particles];
  double rates[n_particles];
  double total_stat_errors[n_particles];
  double total_high_syst_errors[n_particles];
  double total_low_syst_errors[n_particles];
  double min_cut_offs[n_particles] = {3, 5, 6, 15};
  double max_cut_offs[n_particles] = {20, 18, 18, 20};
  
  std::string infilename = "~/data/results/Si16b/systematics_newPP_geq1TgtPulse_2.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");

  double min_integral_energy = 3;
  double max_integral_energy = 17;
  double sum_rate = 0;
  double sum_stat_error = 0;
  double sum_high_syst_error = 0;
  double sum_low_syst_error = 0;
  
  //  double n_captured_muons = 2.06848e+07;
  //  THStack* hStack = new THStack("hStack", "");
  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    std::string particle = particles[i_particle];
    Int_t colour = colours[i_particle];

    std::string i_dirname = "FinalSystPlot_" + particle + "_TCutG_2sig_layerCoinc500ns_tGT0ns_BinW500keV";
    std::string combined_histname = i_dirname + "/hFinalStatSyst";
    std::string outhisttitle = "Si16b Dataset, Right Arm, " + particle;
  
    TGraphAsymmErrors* hCombinedSpectrum = (TGraphAsymmErrors*) infile->Get(combined_histname.c_str());

    std::string stats_histname = i_dirname + "/hFinalStat";
    TGraphAsymmErrors* hStatsSpectrum = (TGraphAsymmErrors*) infile->Get(stats_histname.c_str());

    hCombinedSpectrum->SetTitle("");
    //    hCombinedSpectrum->SetStats(false);
    hCombinedSpectrum->SetLineColor(colour);
    hCombinedSpectrum->SetLineWidth(2);
    //    hCombinedSpectrum->GetXaxis()->SetRangeUser(0, 25);
    if (i_particle == 0) {
      hCombinedSpectrum->Draw("APE");
      hCombinedSpectrum->GetXaxis()->SetRangeUser(0, 25);
    }
    else {
      hCombinedSpectrum->Draw("PE SAME");
    }
    //    hStack->Add(hCombinedSpectrum);

    std::string stat_histname = i_dirname + "/hFinalStat";
    TGraphAsymmErrors* stat_spectrum = (TGraphAsymmErrors*) infile->Get(stat_histname.c_str());
    std::string syst_histname = i_dirname + "/hFinalSyst";
    TGraphAsymmErrors* syst_spectrum = (TGraphAsymmErrors*) infile->Get(syst_histname.c_str());


    double min_energy = min_integral_energy;
    double max_energy = max_integral_energy;
    double rate = 0;
    double total_stat_error = 0;
    double total_high_syst_error = 0;
    double total_low_syst_error = 0;
    IntegrateRates_wStatAndSystSpectra(stat_spectrum, syst_spectrum,
				       min_energy, max_energy,
				       rate, total_stat_error, total_high_syst_error, total_low_syst_error);
    std::cout << particle << " (" << min_energy << " MeV -- " << max_energy << " MeV) = " << rate << " (+- " << total_stat_error << ") (+" << total_high_syst_error << ") (-" << total_low_syst_error << ")" << std::endl;
    rates[i_particle] = rate;
    total_stat_errors[i_particle] = total_stat_error;
    total_high_syst_errors[i_particle] = total_high_syst_error;
    total_low_syst_errors[i_particle] = total_low_syst_error;
    sum_rate += rate;
    sum_stat_error += total_stat_error*total_stat_error;
    sum_high_syst_error += total_high_syst_error*total_high_syst_error;
    sum_low_syst_error += total_low_syst_error*total_low_syst_error;
    
    // if (particle == "proton") {
    //   for (int i_bin = 1; i_bin <= hCombinedSpectrum->GetNbinsX(); ++i_bin) {
    // 	double E = hCombinedSpectrum->GetXaxis()->GetBinCenter(i_bin);
    // 	std::cout << E << " MeV = " << hCombinedSpectrum->GetBinContent(i_bin) << " +/- " << hCombinedSpectrum->GetBinError(i_bin) << std::endl;
    //   }
    // }


    std::string outhistname = "gCombined-si16b-";
    outhistname += particle;
    hOutCombineds[i_particle] = (TGraphAsymmErrors*) hCombinedSpectrum->Clone(outhistname.c_str());
    hOutCombineds[i_particle]->SetTitle(particle.c_str());

    outhistname = "gStats-si16b-";
    outhistname += particle;
    hOutStats[i_particle] = (TGraphAsymmErrors*) hStatsSpectrum->Clone(outhistname.c_str());
    hOutStats[i_particle]->SetTitle(particle.c_str());
    //       hOutCombineds[i_particle]->SetDirectory(0);

    for (int i_point = 0; i_point < hOutCombineds[i_particle]->GetN(); ++i_point) {
      double x_point = *(hOutCombineds[i_particle]->GetX() + i_point);
      if (x_point < min_cut_offs[i_particle]) {
    	hOutCombineds[i_particle]->RemovePoint(i_point);
    	--i_point;
      }
      if (x_point > max_cut_offs[i_particle]) {
    	hOutCombineds[i_particle]->RemovePoint(i_point);
    	--i_point;
      }
    }

    for (int i_point = 0; i_point < hOutStats[i_particle]->GetN(); ++i_point) {
      double x_point = *(hOutStats[i_particle]->GetX() + i_point);
      if (x_point < min_cut_offs[i_particle]) {
    	hOutStats[i_particle]->RemovePoint(i_point);
    	--i_point;
      }
      if (x_point > max_cut_offs[i_particle]) {
    	hOutStats[i_particle]->RemovePoint(i_point);
    	--i_point;
      }
    }
    rate = 0; total_stat_error = 0; total_high_syst_error = 0; total_low_syst_error = 0;
    IntegrateRates_wStatAndStatSystSpectra(hOutStats[i_particle],
					   hOutCombineds[i_particle],
					   min_energy, max_energy,
					   rate, total_stat_error, total_high_syst_error, total_low_syst_error);
    std::cout << particle << " (" << min_energy << " MeV -- " << max_energy << " MeV) = " << rate << " (+- " << total_stat_error << ") (+" << total_high_syst_error << ") (-" << total_low_syst_error << ")" << std::endl;

  }
  
  //  TCanvas* c_Spectra = new TCanvas("c_Spectra", "c_Spectra");
  //  hStack->Draw("HIST E nostack");
  //  hStack->Draw("HIST E");
  //  hStack->GetXaxis()->SetRangeUser(0, 15000);
  //  c_Spectra->Update();
  //  
  //  TLatex* latex = new TLatex();
  //  latex->DrawLatexNDC(0.55, 0.65, "AlCap Preliminary");

  TFile* outfile = new TFile("~/data/results/Si16b/final-si16b-plots.root", "RECREATE");
  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    hOutCombineds[i_particle]->Write();
    hOutStats[i_particle]->Write();
  }
  outfile->Write();
  outfile->Close();

  std::cout << "Total Rate = " << sum_rate << std::endl;
  sum_stat_error = std::sqrt(sum_stat_error);
  sum_high_syst_error = std::sqrt(sum_high_syst_error);
  sum_low_syst_error = std::sqrt(sum_low_syst_error);
  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    std::string particle = particles[i_particle];
    double ratio = rates[i_particle] / sum_rate;

    double ratio_stat_error = ratio*std::sqrt( (total_stat_errors[i_particle]/rates[i_particle])*(total_stat_errors[i_particle]/rates[i_particle]) + (sum_stat_error/sum_rate)*(sum_stat_error/sum_rate) );

    double ratio_high_syst_error = ratio*std::sqrt( (total_high_syst_errors[i_particle]/rates[i_particle])*(total_high_syst_errors[i_particle]/rates[i_particle]) + (sum_high_syst_error/sum_rate)*(sum_high_syst_error/sum_rate) );

    double ratio_low_syst_error = ratio*std::sqrt( (total_low_syst_errors[i_particle]/rates[i_particle])*(total_low_syst_errors[i_particle]/rates[i_particle]) + (sum_low_syst_error/sum_rate)*(sum_low_syst_error/sum_rate) );

    std::cout << particle << " ratio = (" << ratio*100 << " +/- " << ratio_stat_error*100 << " ^" << ratio_high_syst_error*100 << " _" << ratio_low_syst_error*100 << ") %" << std::endl;

    double total_ratio_high_error = std::sqrt(ratio_stat_error*ratio_stat_error + ratio_high_syst_error*ratio_high_syst_error);
    double total_ratio_low_error = std::sqrt(ratio_stat_error*ratio_stat_error + ratio_low_syst_error*ratio_low_syst_error);
    std::cout << " = (" <<ratio*100 << " ^" << total_ratio_high_error*100 << " _" << total_ratio_low_error*100 << ")" << "%" << std::endl;
    // double ratio_low_syst_err = ratio*std::sqrt( (total_low_syst_errors[i_range][i_setting]/rates[i_range][i_setting])*(total_low_syst_errors[i_range][i_setting]/rates[i_range][i_setting]) + (sum_low_syst_errors[i_range]/sum_rates[i_range])*(sum_low_syst_errors[i_range]/sum_rates[i_range]) );

    // double ratio_high_total_err = std::sqrt(ratio_stat_err*ratio_stat_err + ratio_high_syst_err*ratio_high_syst_err);
    // double ratio_low_total_err = std::sqrt(ratio_stat_err*ratio_stat_err + ratio_low_syst_err*ratio_low_syst_err);
  }
}
