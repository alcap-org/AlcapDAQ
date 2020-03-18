void Si16b_FinalPlot_BeamSim_InputEnergy() {

  const int n_settings = 3;
  std::string filenames[n_settings] = {"~/data/mc/Si16b/MC_Si16b_BeamEDep.root",
				       "~/data/mc/Si16b/MC_Si16b_BeamEDep_higherE.root",
				       "~/data/mc/Si16b/MC_Si16b_BeamEDep_lowerE.root"};
  std::string leglabels[n_settings] = {"MC (nominal)", "MC (higher E)", "MC (lower E)"};
  Int_t colours[n_settings] = {kRed, kBlue, kMagenta};
  int rebin_factors[n_settings] = {1, 1, 1};
  std::string inhistnames[n_settings] = { "hInputEnergy", "hInputEnergy", "hInputEnergy"};

  TLegend* leg = new TLegend(0.40,0.45,0.80,0.75);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillStyle(0);
  leg->SetFillColor(kWhite);

  std::stringstream run_str, sf_str, axislabel, leglabel;
  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
    std::string infilename = filenames[i_setting];
    TFile* infile = new TFile(infilename.c_str(), "READ");
    if (infile->IsZombie()) {
      std::cout << "ERROR: Can't find file " << infilename << std::endl;
      return;
    }

    std::string inhistname = inhistnames[i_setting];
    TH1F* hEnergy = (TH1F*) infile->Get(inhistname.c_str());
    if (!hEnergy) {
      std::cout << "ERROR: Can't find histogram " << inhistname << std::endl;
      return;
    }

    std::string histtitle;
    histtitle = "Si16b Dataset, SF = " + sf_str.str();
    hEnergy->SetTitle(histtitle.c_str());
    hEnergy->Sumw2();
    hEnergy->SetStats(false);
    hEnergy->SetMarkerColor(colours[i_setting]);
    hEnergy->SetLineColor(colours[i_setting]);
    hEnergy->SetLineStyle(1);
    hEnergy->GetXaxis()->SetTitle("Energy [keV]");
    hEnergy->Rebin(rebin_factors[i_setting]);
    //      hEnergy->Scale(1.0 / hEnergy->Integral());
    hEnergy->Scale(1.0 / hEnergy->GetMaximum());
    hEnergy->GetXaxis()->SetRangeUser(0,5000);
      
    axislabel.str("");
    axislabel << "Counts / " << hEnergy->GetXaxis()->GetBinWidth(1) << " keV";
    hEnergy->GetYaxis()->SetTitle(axislabel.str().c_str());

    hEnergy->Draw("HIST E SAME");

    leglabel.str("");
    leglabel << leglabels[i_setting];
    leg->AddEntry(hEnergy, leglabel.str().c_str(), "l");
  }
  leg->Draw();
}
