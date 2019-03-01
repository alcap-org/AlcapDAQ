void Si16b_FinalPlot_MichelElectrons() {
  std::string filename = "~/data/results/Si16b/subtrees.root";
  TFile* file = new TFile(filename.c_str(), "READ");


  double min_energy = 0;
  double max_energy = 2000;
  double energy_width = 50;
  int n_energy_bins = (max_energy - min_energy) / energy_width;

  double min_time = -1000;
  double max_time = 10000;
  double time_width = 50;
  int n_time_bins = (max_time - min_time) / time_width;
  
  std::string treename = "siBlockTree_SiR";
  TTree* SiRTree = (TTree*) file->Get(treename.c_str());

  TLegend* leg = new TLegend(0.6,0.70,0.9,0.89);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);

  
  const int n_cuts = 3;
  std::string cutcmds[n_cuts] = {"thin_tpi_id<0 && thick_tpi_id>=0",
				 "thin_tpi_id<0 && thick_tpi_id>=0 && third_tpi_id>=0",
				 "thin_tpi_id<0 && thick_tpi_id>=0 && third_tpi_id<0"};
  std::string leglabels[n_cuts] = {"!SiR1 && SiR2",
				   "!SiR1 && SiR2 && SiR3",
				   "!SiR1 && SiR2 && !SiR3"};
  Int_t colours[n_cuts] = {kBlack, kRed, kBlue};

  std::string basehisttitle = "Si16b Dataset";

  TCanvas* c_E2 = new TCanvas("c_E2", "c_E2");
  TCanvas* c_E3 = new TCanvas("c_E3", "c_E3");

  std::stringstream histname, drawcmd;
  for (int i_cut = 0; i_cut < n_cuts; ++i_cut) {
    std::string cutcmd = cutcmds[i_cut];
    
    histname.str("");
    histname << "hE2vsE3_Michel_Cut" << i_cut;
    drawcmd.str("");
    drawcmd << "thick_energy:third_energy>>" << histname.str();

    std::string histtitle = basehisttitle;
    
    TH2F* hE2vsE3 = new TH2F(histname.str().c_str(), histtitle.c_str(), n_energy_bins,min_energy,max_energy, n_energy_bins,min_energy,max_energy);
    hE2vsE3->SetYTitle("SiR2 Energy [keV]");
    hE2vsE3->SetXTitle("SiR3 Energy [keV]");
    SiRTree->Draw(drawcmd.str().c_str(), cutcmd.c_str(), "goff");
    
    histname.str("");
    histname << "hE3vst3_Michel_Cut" << i_cut;
    drawcmd.str("");
    drawcmd << "third_energy:third_time>>" << histname.str();

    TH2F* hE3vst3 = new TH2F(histname.str().c_str(), histtitle.c_str(), n_time_bins,min_time,max_time, n_energy_bins,min_energy,max_energy);
    hE3vst3->SetYTitle("SiR3 Energy [keV]");
    hE3vst3->SetXTitle("SiR3 Time [ns]");
    SiRTree->Draw(drawcmd.str().c_str(), cutcmd.c_str(), "goff");


    histname.str("");
    histname << "hE2vst2_Michel_Cut" << i_cut;
    drawcmd.str("");
    drawcmd << "thick_energy:thick_time>>" << histname.str();

    TH2F* hE2vst2 = new TH2F(histname.str().c_str(), histtitle.c_str(), n_time_bins,min_time,max_time, n_energy_bins,min_energy,max_energy);
    hE2vst2->SetYTitle("SiR2 Energy [keV]");
    hE2vst2->SetXTitle("SiR2 Time [ns]");
    SiRTree->Draw(drawcmd.str().c_str(), cutcmd.c_str(), "goff");

    
    c_E2->cd();
    histname.str("");
    histname << "hE2_Michel_Cut" << i_cut;
    TH1D* hE2 = hE2vst2->ProjectionY(histname.str().c_str());
    hE2->SetStats(false);
    hE2->SetLineColor(colours[i_cut]);
    hE2->Draw("HIST SAME");

    leg->AddEntry(hE2, leglabels[i_cut].c_str(), "l");

    if (i_cut == 1) {
      c_E3->cd();
      histname.str("");
      histname << "hE3_Michel_Cut" << i_cut;
      TH1D* hE3 = hE3vst3->ProjectionY(histname.str().c_str());
      hE3->SetStats(false);
      hE3->SetLineColor(colours[i_cut]);
      hE3->Draw("HIST SAME");
    }
    
    /*    double min_michel_energy = 600;
    double max_michel_energy = 650;
    //  double min_michel_energy = 350;
    //  double max_michel_energy = 400;
    int min_michel_energy_bin = hE2vst2->GetYaxis()->FindBin(min_michel_energy);
    int max_michel_energy_bin = hE2vst2->GetYaxis()->FindBin(max_michel_energy);
    TH1D* hLifetime = hE2vst2->ProjectionX("hLifetime", min_michel_energy_bin, max_michel_energy_bin);
    hLifetime->Rebin(2);
    hLifetime->GetXaxis()->SetRangeUser(-1000, 10000);
    hLifetime->Draw("HIST E");

    double min_fit_time = 200;
    double max_fit_time = 10000;
    TF1* muonic_atom_lifetime = new TF1("muonic_atom_lifetime", "[0]*TMath::Exp(-x/[1]) + [2]", min_fit_time, max_fit_time);
    muonic_atom_lifetime->SetParameters(1000, 700, 100);
    hLifetime->Fit(muonic_atom_lifetime, "R");
    muonic_atom_lifetime->Draw("LSAME");
    */
  }

  c_E2->cd();
  leg->Draw();
}
