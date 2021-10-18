void Si16b_FinalPlot_TimeCutEfficiency(std::string savedir = "", std::ostream& numbers_file = std::cout) {

  double min_total_time = -1000;
  double max_total_time = 10000;
  double min_time_cut = 0;
  double max_time_cut = max_total_time;
  
  std::string infilename = "~/data/results/Si16b/plots_newPP_geq1TgtPulse_1.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");

  TLegend* leg = new TLegend(0.50,0.45,0.90,0.75);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillStyle(0);
  leg->SetFillColor(kWhite);
  std::stringstream leglabel;
  
  const int n_particles = 4;
  std::string particles[n_particles] = {"proton", "deuteron", "triton", "alpha"};
  Int_t colours[n_particles] = {kRed, kCyan, kMagenta, kSpring};
  
  TH1F* hTotal = 0;
  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    std::string this_particle = particles[i_particle];
    //    std::string dirname = this_particle + "_SiR_layerCoinc_PSel";
    std::string dirname = this_particle + "_2sig_SiR_layerCoinc500ns_TCutG";
    std::string histname = dirname + "/hEvt_TwoLayer_12not3";

    TH2F* hEvt = (TH2F*) infile->Get(histname.c_str());
    if (!hEvt) {
      std::cout << "Can't find histogram " << histname << std::endl;
      continue;
    }

    std::string newhistname = this_particle + "_time";
    TH1F* hTime = (TH1F*) hEvt->ProjectionX(newhistname.c_str());
    hTime->Rebin(10);
    if (!hTotal) {
      hTotal = (TH1F*) hTime->Clone("hTotal");
    }
    else {
      hTotal->Add(hTime);
    }

    int min_total_time_bin = hTime->FindBin(min_total_time);
    int max_total_time_bin = hTime->FindBin(max_total_time)-1;
    int min_time_cut_bin = hTime->FindBin(min_time_cut);
    int max_time_cut_bin = hTime->FindBin(max_time_cut)-1;

    double denom = hTime->Integral(min_total_time_bin, max_total_time_bin);
    double numer = hTime->Integral(min_time_cut_bin, max_time_cut_bin);

    double eff = numer / denom;
    double eff_error = std::sqrt( (eff*(1-eff)) / denom );
    std::cout << this_particle << ": time cut efficiency = " << eff << " +/- " << eff_error << std::endl;
  }

  int min_total_time_bin = hTotal->FindBin(min_total_time);
  int max_total_time_bin = hTotal->FindBin(max_total_time)-1;
  int min_time_cut_bin = hTotal->FindBin(min_time_cut);
  int max_time_cut_bin = hTotal->FindBin(max_time_cut)-1;
    
  double total_denom = hTotal->Integral(min_total_time_bin, max_total_time_bin);
  double total_numer = hTotal->Integral(min_time_cut_bin, max_time_cut_bin);

  double total_eff = total_numer / total_denom;
  double total_eff_error = std::sqrt( (total_eff*(1-total_eff)) / total_denom );
  std::cout << "total: time cut efficiency = " << total_eff << " +/- " << total_eff_error << std::endl;

  numbers_file << "% From Si16b_FinalPlot_TimeCutEfficiency.C" << std::endl;
  numbers_file << "\\newcommand\\SibTimeCutEff{\\num[round-precision=3, round-mode=figures]{" << std::fixed << std::setprecision(3) << total_eff << " \\pm " << total_eff_error << "}}" << std::endl;
  numbers_file << std::endl;

  TCanvas* c_time = new TCanvas("c_time", "c_time");
  c_time->SetLogy(1);

  hTotal->GetXaxis()->SetRangeUser(-1000, 10000);
  hTotal->SetLineColor(kBlack);
  hTotal->SetStats(false);
  hTotal->SetTitle("Si16b Dataset, All Particle Types, Time Distribution");

  std::stringstream axistitle;
  axistitle.str("");
  axistitle << "Count / " << hTotal->GetXaxis()->GetBinWidth(1) << " ns";
  hTotal->SetYTitle(axistitle.str().c_str());
  hTotal->GetXaxis()->SetTitleOffset(0.9);
  hTotal->GetYaxis()->SetTitleOffset(0.9);
  hTotal->Draw("HIST E");

  TLine* min_total_line = new TLine(min_total_time, 0, min_total_time, hTotal->GetMaximum());
  min_total_line->SetLineColor(kBlue);
  min_total_line->SetLineWidth(3);
  min_total_line->Draw("LSAME");

  TLine* max_total_line = new TLine(max_total_time, 0, max_total_time, hTotal->GetMaximum());
  max_total_line->SetLineColor(kBlue);
  max_total_line->SetLineWidth(3);
  max_total_line->Draw("LSAME");

  TLine* min_cut_line = new TLine(min_time_cut, 0, min_time_cut, hTotal->GetMaximum());
  min_cut_line->SetLineColor(kRed);
  min_cut_line->SetLineStyle(kDashed);
  min_cut_line->SetLineWidth(3);
  min_cut_line->Draw("LSAME");

  TLine* max_cut_line = new TLine(max_time_cut, 0, max_time_cut, hTotal->GetMaximum());
  max_cut_line->SetLineColor(kRed);
  max_cut_line->SetLineWidth(3);
  max_cut_line->SetLineStyle(kDashed);
  max_cut_line->Draw("LSAME");

  std::stringstream text;
  TLatex* latex = new TLatex();
  latex->SetTextAlign(22);

  text.str("");
  text << "#frac{N(" << min_time_cut << " ns < t < " << max_time_cut << " ns)}{N(" << min_total_time << " ns < t < " << max_total_time << " ns)} = #frac{" << total_numer << "}{" << total_denom << "}";
  latex->DrawLatexNDC(0.53, 0.8, text.str().c_str());

  text.str("");
  text << " = " << std::setprecision(3) << total_eff << " #pm " << std::setprecision(1) << total_eff_error;
  latex->DrawLatexNDC(0.77, 0.70, text.str().c_str());

  alcaphistogram(hTotal);
  alcapPreliminary(hTotal);
  //  hTotal->SetDrawOption("COLZ");

  if (savedir != "") {
    std::string savename = savedir + "AlCapData_Si16bDataset_TimeCutEfficiency";

    std::string pdfname = savename + ".pdf";
    c_time->SaveAs(pdfname.c_str());
    std::string pngname = savename + ".png";
    c_time->SaveAs(pngname.c_str());
  }

}
