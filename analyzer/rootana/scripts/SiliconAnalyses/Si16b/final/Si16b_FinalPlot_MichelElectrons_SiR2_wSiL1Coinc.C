void Si16b_FinalPlot_MichelElectrons_SiR2_wSiL1Coinc() {
  std::string filename = "~/data/results/Si16b/subtrees.root";
  TFile* file = new TFile(filename.c_str(), "READ");


  double min_energy = 0;
  double max_energy = 1500;
  double energy_width = 50;
  int n_energy_bins = (max_energy - min_energy) / energy_width;

  double min_time = -15000;
  double max_time = 20000;
  double time_width = 50;
  int n_time_bins = (max_time - min_time) / time_width;
  
  std::string treename = "siBlockTree_Target";
  TTree* SiRTree = (TTree*) file->Get(treename.c_str());

  TLegend* leg = new TLegend(0.55,0.70,0.9,0.89);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);

  TLegend* leg_t = new TLegend(0.45,0.60,0.9,0.89);
  leg_t->SetBorderSize(0);
  leg_t->SetTextSize(0.03);
  leg_t->SetFillColor(kWhite);


  const int n_cuts = 3;
  std::string cutcmds[n_cuts] = {"thick_tpi_id<0 && third_tpi_id>=0",
				 "thin_tpi_id>=0 && thick_tpi_id<0 && third_tpi_id>=0",
				 "thin_tpi_id<0 && thick_tpi_id<0 && third_tpi_id>=0"};
  std::string histtags[n_cuts] = {"all", "wSiL1Hit", "woSiL1Hit"};
  std::string leglabels[n_cuts] = {"all", "with SiL1 hit", "without SiL1 hit"};
  Int_t colours[n_cuts] = {kBlack, kRed, kBlue};

  std::string histtitle = "Si16b Dataset (SiL1 && !SiR1 && SiR2)";

  std::stringstream histname, drawcmd;
  TCanvas* c_E = new TCanvas("c_E", "c_E");
  TCanvas* c_t = new TCanvas("c_t", "c_t");
  c_t->SetLogy(1);
  for (int i_cut = 0; i_cut < n_cuts; ++i_cut) {
    std::string cutcmd = cutcmds[i_cut];

    std::string i_layername = "third";
    std::string i_histtag = histtags[i_cut];
    histname.str("");
    histname << "hEnergyTime_Michel_" << i_histtag;
    drawcmd.str("");
    drawcmd << i_layername << "_energy:" << i_layername << "_time>>" << histname.str();

    TH2F* hEvst = new TH2F(histname.str().c_str(), histtitle.c_str(), n_time_bins,min_time,max_time, n_energy_bins,min_energy,max_energy);
    std::string yaxislabel = "SiR2 Energy [keV]";
    std::string xaxislabel = "SiR2 Time [ns]";
    hEvst->SetYTitle(yaxislabel.c_str());
    hEvst->SetXTitle(xaxislabel.c_str());
    SiRTree->Draw(drawcmd.str().c_str(), cutcmd.c_str(), "goff");

    histname.str("");
    histname << "hE_Michel_Cut" << i_histtag;
    TH1D* hE = hEvst->ProjectionY(histname.str().c_str());
    hE->SetStats(false);
    hE->SetLineColor(colours[i_cut]);
    hE->SetXTitle("SiR2 Energy [keV]");
    c_E->cd();
    hE->Draw("HIST SAME");

    int bin = -1;
    Double_t max_content = hE->GetMaximum();
    hE->GetBinWithContent(max_content, bin);
    double peak = hE->GetBinCenter(bin);
    if (peak < 350) {
      max_content = hE->GetMaximum(max_content);
      hE->GetBinWithContent(max_content, bin);
      peak = hE->GetBinCenter(bin);
    }
    double min_fit_range = peak - 50 - hE->GetXaxis()->GetBinWidth(1)*0.5;
    double max_fit_range = max_energy;
    TF1* landau = new TF1("landau", "[0]*TMath::Landau(x, [1], [2])", min_fit_range, max_fit_range);
    landau->SetParameters(1000, peak, 100);
    hE->Fit(landau, "R");
    landau->SetLineColor(colours[i_cut]);
    landau->Draw("LSAME");

    std::stringstream leglabel;
    leglabel.str("");
    leglabel << leglabels[i_cut] << std::setprecision(3) << ": #splitline{MPV = " << landau->GetParameter(1) << " #pm " << std::setprecision(1) << landau->GetParError(1) << " keV}"
	     << "{sigma = " << std::setprecision(3) << landau->GetParameter(2) << " #pm " << std::setprecision(1) << landau->GetParError(2) << " keV}";
    leg->AddEntry(hE, leglabel.str().c_str(), "l");

    //    std::cout << leglabels[i_cut] << ": Integral = " << hE->Integral() << std::endl;

    histname.str("");
    histname << "ht_Michel_Cut" << i_histtag;
    TH1D* ht = hEvst->ProjectionX(histname.str().c_str());
    ht->SetStats(false);
    ht->SetLineColor(colours[i_cut]);
    ht->SetXTitle("SiR2 Time [ns]");
    c_t->cd();
    ht->SetMinimum(100);
    ht->Draw("HIST SAME");

    double min_flat_fit_time_range = -15000;
    double max_flat_fit_time_range = -5000;
    TF1* flat_bkg_fit = new TF1("flat_bkg_fit", "[0]*abs(x) + [1]", min_flat_fit_time_range, max_flat_fit_time_range);
    ht->Fit(flat_bkg_fit, "R");

    
    double min_time_fit_range = 0;
    double max_time_fit_range = 20000;    
    //    TF1* lifetime = new TF1("lifetime", "[0]*TMath::Exp(-x/[1]) + [2] + [3]*TMath::Exp(-x/[4])", min_time_fit_range, max_time_fit_range);
    //    lifetime->SetParameters(1000, 100, 1000, 1000, 1000);
    //    TF1* lifetime = new TF1("lifetime", "[0]*TMath::Exp(-x/[1]) + [2]", min_time_fit_range, max_time_fit_range);
    //    lifetime->SetParameters(1000, 100, 1000);
    //    TF1* lifetime = new TF1("lifetime", "[0]*TMath::Exp(-x/[1]) + [2]*TMath::Exp(-x/[3])", min_time_fit_range, max_time_fit_range);
    //    lifetime->SetParameters(1000, 100, 1000, 1000);

    TF1* flat_bkg = new TF1("flat_bkg", "[0]*abs(x) + [1]", min_time_fit_range, max_time_fit_range);
    flat_bkg->FixParameter(0, flat_bkg_fit->GetParameter(0)); // want negative because we fitted in in the negative-x region
    flat_bkg->FixParameter(1, flat_bkg_fit->GetParameter(1));
    TF1* lifetime = new TF1("lifetime", "[0]*TMath::Exp(-x/[1]) + [2]*TMath::Exp(-x/[3]) + flat_bkg", min_time_fit_range, max_time_fit_range);
    lifetime->SetParameters(1000, 100, 1000, 1000);
    lifetime->FixParameter(4, flat_bkg_fit->GetParameter(0)); // want negative because we fitted in in the negative-x region
    lifetime->FixParameter(5, flat_bkg_fit->GetParameter(1));


    ht->Fit(lifetime, "R");
    lifetime->SetLineColor(colours[i_cut]);
    lifetime->Draw("LSAME");

    leglabel.str("");
    leglabel << leglabels[i_cut]
      //	     << std::setprecision(4) << ": #splitline{#tau_{1} = " << lifetime->GetParameter(1) << " #pm " << std::setprecision(3) << lifetime->GetParError(1) << " ns}"
      //      	     << std::setprecision(4) << "{#tau_{2} = " << lifetime->GetParameter(4) << " #pm " << std::setprecision(3) << lifetime->GetParError(4) << " ns}";
      
      //	     << std::setprecision(4) << ": #tau = " << lifetime->GetParameter(1) << " #pm " << std::setprecision(3) << lifetime->GetParError(1) << " ns";

	     << std::setprecision(4) << ": #splitline{#tau_{1} = " << lifetime->GetParameter(1) << " #pm " << std::setprecision(3) << lifetime->GetParError(1) << " ns}"
      	     << std::setprecision(4) << "{#tau_{2} = " << lifetime->GetParameter(3) << " #pm " << std::setprecision(3) << lifetime->GetParError(3) << " ns}";

    leg_t->AddEntry(ht, leglabel.str().c_str(), "l");

  }

  c_E->cd();
  leg->Draw();

  c_t->cd();
  leg_t->Draw();
}
