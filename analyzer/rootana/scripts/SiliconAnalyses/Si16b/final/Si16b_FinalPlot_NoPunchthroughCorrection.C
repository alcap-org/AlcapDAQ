void Si16b_FinalPlot_NoPunchthroughCorrection(std::string savedir = "", std::ostream& numbers_file = std::cout) {

  std::string filename = "~/data/results/Si16b/plots_newPP_geq1TgtPulse_1.root";
  TFile* file = new TFile(filename.c_str(), "READ");

  const int n_hists = 2;
  std::string dirnames[n_hists] = {"all_SiR_timecut0_10000ns_layerCoinc500ns", "all_SiR_timecut0_10000ns_layerCoinc500ns"};//, "all_SiR_timecut0_10000ns_layerCoinc500ns"};//"proton_2sig_SiR_timecut0_10000ns_layerCoinc500ns", "all_SiR_timecut0_10000ns_layerCoinc500ns"};
  std::string histnames[n_hists] = {"hEvdE_TwoLayer_12", "hEvdE_TwoLayer_123"};//, "hEvdE_TwoLayer_123"};//, "hEvdE_ThreeLayer_123"};
  std::string histtitles[n_hists] = {"Si16b Dataset, Right Arm, 12", "Si16b Dataset, Right Arm, 123"};//, "Si16b Dataset, Right Arm, 123"};//, "Si16b Dataset, Right Arm, With SiR3 Hit"};
  std::string uppertexts[n_hists] = {"Hits in SiR1 and SiR2 (no SiR3 veto applied)", "Hits in SiR1, SiR2 and SiR3 (what we want to take away after scaling up)"};//, "Hits in SiR1, SiR2 and SiR3 (what we want to take away after scaling down)"};
  std::string cutdirnames[n_hists] = {"proton_2sig_SiR_timecut0_10000ns_layerCoinc500ns", "proton_2sig_SiR_timecut0_10000ns_layerCoinc500ns"};//, "proton3L_2sig_SiR_timecut0_10000ns_layerCoinc500ns"};
  std::string cutnames[n_hists] = {"r_hLg_SiR_EvDeltaE_proton_2sigma_cutoff", "r_hLg_SiR_EvDeltaE_proton_2sigma_cutoff"};//, "r_hLg_SiR_EvDeltaE_proton_2sigma"};

  std::string rawdirnames[n_hists] = {"proton_2sig_SiR_timecut0_10000ns_layerCoinc500ns", "proton_2sig_SiR_timecut0_10000ns_layerCoinc500ns"};//, "proton3L_2sig_SiR_timecut0_10000ns_layerCoinc500ns"};
  std::string suffixes[n_hists] = {"_base", "_reduction"};//withHitProtons", "threeLayer"};
  std::string leglabels[n_hists] = {"main measurement", "reduction"};
  int rebin_factors[n_hists] = {1, 1};//10, 10};
  double max_x_axis[n_hists] = {25, 25};//, 25};
  double max_y_axis[n_hists] = {3, 3};//, 20};
  Int_t colours[n_hists] = {kBlack, kBlue};
  double sir3_veto_eff = 0.74;
  double scale_factors[n_hists] = {1, sir3_veto_eff};//sir3_veto_eff/(1-sir3_veto_eff)};
  double sum_factors[n_hists] = {1, -1};

  TCanvas* c_all = new TCanvas("c_all", "c_all");
  c_all->SetLogy();
  TH1D* hSum = 0;
  TH1D* hRatio = 0;
  TLegend* leg = new TLegend(0.35,0.65,0.80,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.04);
  leg->SetFillColor(kWhite);
  std::stringstream leglabel;
  for (int i_hist = 0; i_hist < n_hists; ++i_hist) {

    std::string dirname = dirnames[i_hist];
    std::string i_histname = dirname + "/" + histnames[i_hist];
    std::string canvasname = "c_" + histnames[i_hist] + std::to_string(i_hist);
    TCanvas* c = new TCanvas(canvasname.c_str());
    c->Divide(1,2);
    c->cd(1);
    
    TH2F* hEvdE = (TH2F*) file->Get(i_histname.c_str());
    if (!hEvdE) {
      std::cout << "Can't find histogram " << i_histname << std::endl;
      continue;
    }
    hEvdE->Rebin2D(rebin_factors[i_hist], rebin_factors[i_hist]);
    hEvdE->GetXaxis()->SetRangeUser(0, max_x_axis[i_hist]);
    hEvdE->GetYaxis()->SetRangeUser(0, max_y_axis[i_hist]);
    hEvdE->GetXaxis()->SetTitleOffset(0.9);
    hEvdE->GetYaxis()->SetTitleOffset(0.9);
    hEvdE->SetTitle(histtitles[i_hist].c_str());
    hEvdE->SetStats(false);
    hEvdE->Draw("COLZ");

    std::string cutname = cutdirnames[i_hist] + "/" + cutnames[i_hist];
    std::cout << "cutname = " << cutname << std::endl;
    TCutG* tcutg = (TCutG*) file->Get(cutname.c_str());
    tcutg->SetLineColor(kRed);
    tcutg->SetLineWidth(2);
    tcutg->Draw("LSAME");

    std::stringstream text;
    text << uppertexts[i_hist];
    TLatex* latex = new TLatex();
    latex->SetTextAlign(22);
    latex->SetTextColor(kRed);
    double latex_x = 0.5;//box_min_x[i_hist] + (box_max_x[i_hist] - box_min_x[i_hist])/2;
    double latex_y = 0.8;//box_max_y[i_hist] + 100;
    latex->DrawLatexNDC(latex_x, latex_y, text.str().c_str());
    
    c->cd(2);
    std::string rawhistname = rawdirnames[i_hist] + "/" + histnames[i_hist];
    std::cout << "rawhistname = " << rawhistname << std::endl;
    TH2F* hRaw2D = (TH2F*) file->Get(rawhistname.c_str());
    TH1D* hRaw = hRaw2D->ProjectionX();
    hRaw->SetLineWidth(2);
    hRaw->SetLineColor(colours[i_hist]);
    hRaw->Rebin(50);
    hRaw->Draw("HIST E");

    text.str("");
    text << "Raw Count = " << hRaw->GetEntries();
    latex->SetTextColor(colours[i_hist]);
    latex->DrawLatexNDC(latex_x, latex_y, text.str().c_str());

    c_all->cd();
    TH1D* hRawForAll = (TH1D*) hRaw->Clone("hRawForAll");
    hRawForAll->SetLineColor(colours[i_hist]);
    hRawForAll->SetMaximum(2e4);
    hRawForAll->Draw("HIST E SAME");
    leglabel.str("");
    leglabel << leglabels[i_hist];
    leg->AddEntry(hRawForAll, leglabel.str().c_str(), "l");

    TH1D* hRawForAllScaled = (TH1D*) hRawForAll->Clone("hRawForAllScaled");
    hRawForAllScaled->Scale(1.0/scale_factors[i_hist]);
    hRawForAllScaled->SetLineStyle(kDashed);
    hRawForAllScaled->Draw("HIST E SAME");
    leglabel.str("");
    leglabel << leglabels[i_hist] << " (scaled by 1.0/" << scale_factors[i_hist] << ")";
    leg->AddEntry(hRawForAllScaled, leglabel.str().c_str(), "l");

    if (!hSum) {
      hSum = (TH1D*) hRawForAllScaled->Clone("hSum");
    }
    else {
      hSum->Add(hRawForAllScaled, sum_factors[i_hist]);
    }
    hSum->SetLineColor(kMagenta);
    hSum->Draw("HIST E SAME");

    if (!hRatio) {
      hRatio = (TH1D*) hRawForAll->Clone("hRatio");
    }
    if (i_hist == n_hists-1) {
      hRatio->Divide(hSum);
    }
  }
  c_all->cd();
  leg->Draw();

  TCanvas* c_ratio = new TCanvas();
  hRatio->Draw("HIST");
  hRatio->SetMaximum(2.0);
  // if (savedir != "") {
  //   std::string savename = savedir + "AlCapData_Si16bDataset_SiR3VetoDemo";

  //   std::string pdfname = savename + ".pdf";
  //   c->SaveAs(pdfname.c_str());
  //   std::string pngname = savename + ".png";
  //   c->SaveAs(pngname.c_str());
  // }
}
