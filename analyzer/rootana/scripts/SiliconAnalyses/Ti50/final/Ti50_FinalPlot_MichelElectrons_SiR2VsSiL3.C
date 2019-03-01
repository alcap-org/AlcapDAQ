void Ti50_FinalPlot_MichelElectrons_SiR2VsSiL3() {
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
  
  TLegend* leg = new TLegend(0.55,0.70,0.9,0.89);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);

  
  const int n_arms = 2;
  std::string armnames[n_arms] = {"SiR", "SiL"};
  std::string leglabels[n_arms] = {"SiR2", "SiL3"};
  Int_t colours[n_arms] = {kRed, kBlue};

  std::string histtitle = "Ti50 Dataset (!thin && thick)";

  TCanvas* c_E2 = new TCanvas("c_E2", "c_E2");

  std::stringstream histname, drawcmd;
  for (int i_arm = 0; i_arm < n_arms; ++i_arm) {

    std::string treename = "siBlockTree_" + armnames[i_arm];
    TTree* tree = (TTree*) file->Get(treename.c_str());

    std::string cutcmd = "thin_tpi_id<0 && thick_tpi_id>=0";
    

    histname.str("");
    histname << "hE2vst2_Michel_" << armnames[i_arm];
    drawcmd.str("");
    drawcmd << "thick_energy:thick_time>>" << histname.str();

    TH2F* hE2vst2 = new TH2F(histname.str().c_str(), histtitle.c_str(), n_time_bins,min_time,max_time, n_energy_bins,min_energy,max_energy);
    std::string yaxislabel = leglabels[i_arm] + " Energy [keV]";
    std::string xaxislabel = leglabels[i_arm] + " Time [ns]";
    hE2vst2->SetYTitle(yaxislabel.c_str());
    hE2vst2->SetXTitle(xaxislabel.c_str());
    tree->Draw(drawcmd.str().c_str(), cutcmd.c_str(), "goff");

    
    c_E2->cd();
    histname.str("");
    histname << "hE2_Michel_" << armnames[i_arm];
    TH1D* hE2 = hE2vst2->ProjectionY(histname.str().c_str());
    hE2->SetStats(false);
    hE2->SetXTitle("Energy [keV]");
    hE2->SetLineColor(colours[i_arm]);
    hE2->Draw("HIST SAME");

    int bin = -1;
    Double_t max_content = hE2->GetMaximum();
    hE2->GetBinWithContent(max_content, bin);
    double peak = hE2->GetBinCenter(bin);
    if (peak < 350) {
      max_content = hE2->GetMaximum(max_content);
      hE2->GetBinWithContent(max_content, bin);
      peak = hE2->GetBinCenter(bin);
    }
    double min_fit_range = peak - 100;
    double max_fit_range = max_energy;
    TF1* landau = new TF1("landau", "[0]*TMath::Landau(x, [1], [2])", min_fit_range, max_fit_range);
    landau->SetParameters(1000, peak, 100);
    hE2->Fit(landau, "R");
    landau->SetLineColor(colours[i_arm]);
    landau->Draw("LSAME");

    std::stringstream leglabel;
    leglabel.str("");
    leglabel << leglabels[i_arm] << std::setprecision(3) << ": #splitline{MPV = " << landau->GetParameter(1) << " #pm " << std::setprecision(1) << landau->GetParError(1) << " keV}"
	     << "{sigma = " << std::setprecision(3) << landau->GetParameter(2) << " #pm " << std::setprecision(1) << landau->GetParError(2) << " keV}";
    leg->AddEntry(hE2, leglabel.str().c_str(), "l");
  }

  c_E2->cd();
  leg->Draw();
}
