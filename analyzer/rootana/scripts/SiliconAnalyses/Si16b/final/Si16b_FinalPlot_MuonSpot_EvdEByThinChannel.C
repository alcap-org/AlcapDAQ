void Si16b_FinalPlot_MuonSpot_EvdEByThinChannel() {

  std::string infilename = "~/data/results/Si16b/plots_newPP.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");

  const int n_cuts = 3;
  std::string cutnames[n_cuts] = {"both", "lower", "upper"};
  TCutG* tcutgs[n_cuts] = {0};
  for (int i_cut = 0; i_cut < n_cuts; ++i_cut) {
    
    std::string cutname = "muspot_" + cutnames[i_cut] + "_SiR/" + cutnames[i_cut] + "_two_layer";
    std::cout << cutname << std::endl;
    tcutgs[i_cut] = (TCutG*) infile->Get(cutname.c_str());
  }

  const int n_thin_channels = 4;

  //  double integrals[n_thin_channels][n_cuts] = {0};

  //  double split_line_dE
  //  TLine* split_spot = new TLine(spot_cut_E_min, spot_cut_E_max);
  
  std::stringstream thin_channel;
  for (int i_thin_channel = 0; i_thin_channel < n_thin_channels; ++i_thin_channel) {
    thin_channel.str("");
    thin_channel << i_thin_channel+1;

    std::string dirname = "all_SiR1_" + thin_channel.str();
    std::string layer1 = "SiR1-" + thin_channel.str(); std::string layer2 = "SiR2"; std::string layer3 = "SiR3";
    if (i_thin_channel == 4) {
      dirname = "all_SiR";
      layer1 = "SiR1";
    }

    std::string canvasname = "c_" + thin_channel.str();
    TCanvas* c_EvdE = new TCanvas(canvasname.c_str(), canvasname.c_str());
    std::string inhistname = dirname + "/hEvdE_TwoLayer_12not3";
    TH2F* hEvdE = (TH2F*) infile->Get(inhistname.c_str());
    std::string histtitle;
    histtitle = "Si16b Dataset, Right Arm (" + layer1 + " && " + layer2 + " && !" + layer3 + ")";

    hEvdE->SetTitle(histtitle.c_str());
    hEvdE->SetStats(false);
    hEvdE->GetXaxis()->SetRangeUser(0, 10000);
    hEvdE->GetYaxis()->SetRangeUser(0, 2000);
    //  hEvdE->GetYaxis()->SetRangeUser(0, 10000);
    hEvdE->Draw("COLZ");

    for (int i_cut = 0; i_cut < n_cuts; ++i_cut) {
      tcutgs[i_cut]->Draw("LSAME");
    }
    
    TLatex* latex = new TLatex();
    latex->DrawLatexNDC(0.55, 0.65, "AlCap Preliminary");
  }

  for (int i_thin_channel = 0; i_thin_channel < n_thin_channels; ++i_thin_channel) {
    thin_channel.str("");
    thin_channel << i_thin_channel+1;

    for (int i_cut = 0; i_cut < n_cuts; ++i_cut) {
      std::string cutname = cutnames[i_cut];
      std::string dirname = "muspot_" + cutname + "_SiR1_" + thin_channel.str();
      std::string inhistname = dirname + "/hEvdE_TwoLayer_12not3";
      TH2F* hEvdE = (TH2F*) infile->Get(inhistname.c_str());

      std::cout << "SiR1-" << thin_channel.str() << ": " << cutname << " = " << hEvdE->GetEntries() << std::endl;
    }
  }

  /*  TCanvas* c_integrals = new TCanvas("c_integrals", "c_integrals");

  TH1F* hIntegrals[n_cuts] = {0};
  std::stringstream histname, binlabel;
  int n_bins = 4;
  bool seenRed = false;
  for (int i_cut = 0; i_cut < n_cuts; ++i_cut) {
    histname.str("");
    histname << "hIntegrals_Cut" << i_cut;
    TH1F* hist = new TH1F(histname.str().c_str(), "", n_bins,0,n_bins);
    hist->SetLineColor(colours[i_cut]);
    hist->SetStats(false);
    hist->SetTitle("Si16b Dataset, SiR, Muon Spot Integrals");
    hist->SetXTitle("Thin Channel");
    hist->SetYTitle("Counts");
    hist->SetMarkerSize(1.5);

    for (int i_thin_channel = 0; i_thin_channel < n_bins; ++i_thin_channel) {
      binlabel.str("");
      binlabel << "SiR1";
      if (i_thin_channel <= 3) {
	binlabel << "-" << i_thin_channel+1;
      }
      hist->GetXaxis()->SetBinLabel(i_thin_channel+1, binlabel.str().c_str());
      hist->SetBinContent(i_thin_channel+1, integrals[i_thin_channel][i_cut]);
    }

    hist->SetMinimum(0);
    hist->SetMaximum(1200);
    if (colours[i_cut] != kRed) {
      hist->Draw("HIST TEXT0 E SAME");
    }
    else if (seenRed) {
      hist->Add(hIntegrals[i_cut-1]);
      hist->Draw("HIST TEXT0 E SAME");
    }
    else if (!seenRed) {
      seenRed = true;
    }
    hIntegrals[i_cut] = hist;
  }

  TLegend* leg = new TLegend(0.55,0.70,0.9,0.89);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);
  
  leg->AddEntry(hIntegrals[0], "both muon spots", "l");
  leg->AddEntry(hIntegrals[1], "lower muon spot", "l");
  leg->AddEntry(hIntegrals[3], "upper muon spot", "l");
  leg->Draw();
  */
}
