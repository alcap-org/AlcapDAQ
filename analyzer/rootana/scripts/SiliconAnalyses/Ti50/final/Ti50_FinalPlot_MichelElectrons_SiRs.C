void Ti50_FinalPlot_MichelElectrons_SiRs() {
  std::string filename = "~/data/results/Ti50/subtrees.root";
  TFile* file = new TFile(filename.c_str(), "READ");


  double min_energy = 0;
  double max_energy = 1500;
  double energy_width = 50;
  int n_energy_bins = (max_energy - min_energy) / energy_width;

  double min_time = -1000;
  double max_time = 10000;
  double time_width = 50;
  int n_time_bins = (max_time - min_time) / time_width;
  
  std::string treename = "siBlockTree_SiR";
  TTree* SiRTree = (TTree*) file->Get(treename.c_str());

  TLegend* leg = new TLegend(0.55,0.70,0.9,0.89);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);


  const int n_layers = 2;
  std::string layernames[n_layers] = {"thick", "third"};
  std::string leglabels[n_layers] = {"SiR2", "SiR3"};
  Int_t colours[n_layers] = {kRed, kRed+1};

  std::string histtitle = "Ti50 Dataset (!SiR1 && SiR2 && SiR3)";

  std::stringstream histname, drawcmd;
  for (int i_layer = n_layers-1; i_layer >= 0; --i_layer) {
    std::string cutcmd = "thin_tpi_id<0 && thick_tpi_id>=0 && third_tpi_id>=0";

    std::string i_layername = layernames[i_layer];
    histname.str("");
    histname << "hEnergyTime_Michel_" << i_layername;
    drawcmd.str("");
    drawcmd << i_layername << "_energy:" << i_layername << "_time>>" << histname.str();

    TH2F* hEvst = new TH2F(histname.str().c_str(), histtitle.c_str(), n_time_bins,min_time,max_time, n_energy_bins,min_energy,max_energy);
    std::string yaxislabel = leglabels[i_layer] + " Energy [keV]";
    std::string xaxislabel = leglabels[i_layer] + " Time [ns]";
    hEvst->SetYTitle(yaxislabel.c_str());
    hEvst->SetXTitle(xaxislabel.c_str());
    SiRTree->Draw(drawcmd.str().c_str(), cutcmd.c_str(), "goff");

    histname.str("");
    histname << "hE_Michel_Cut" << i_layername;
    TH1D* hE = hEvst->ProjectionY(histname.str().c_str());
    hE->SetStats(false);
    hE->SetLineColor(colours[i_layer]);
    hE->SetXTitle("Energy [keV]");
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
    landau->SetLineColor(colours[i_layer]);
    landau->Draw("LSAME");

    std::stringstream leglabel;
    leglabel.str("");
    leglabel << leglabels[i_layer] << std::setprecision(3) << ": #splitline{MPV = " << landau->GetParameter(1) << " #pm " << std::setprecision(1) << landau->GetParError(1) << " keV}"
	     << "{sigma = " << std::setprecision(3) << landau->GetParameter(2) << " #pm " << std::setprecision(1) << landau->GetParError(2) << " keV}";
    leg->AddEntry(hE, leglabel.str().c_str(), "l");
  }

  leg->Draw();
}
