void Si16b_FinalPlot_MuonSpot_ThickEnergy() {
  
  std::string infilename = "~/data/results/Si16b/plots_newPP.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");

  TLegend* leg = new TLegend(0.50,0.55,0.90,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillStyle(0);
  leg->SetFillColor(kWhite);
  
  const int n_cuts = 3;
  std::string cutnames[n_cuts] = {"both", "lower", "upper"};
  Int_t colours[n_cuts] = {kBlack, kRed, kBlue};

  std::stringstream axistitle;
  for (int i_cut = 0; i_cut < n_cuts; ++i_cut) {

    std::string histname = "muspot_" + cutnames[i_cut] + "_SiR/hSingleDetAxes_TwoLayer_12";
    TH2F* hEvdE = (TH2F*) infile->Get(histname.c_str());

    std::string newhistname = "hE_" + cutnames[i_cut];
    TH1D* hE = hEvdE->ProjectionX(newhistname.c_str());
    hE->Rebin(5);
    hE->GetXaxis()->SetRangeUser(0, 4000);

    axistitle.str("");
    axistitle << "Count / " << hE->GetXaxis()->GetBinWidth(1) << " keV";
    hE->SetStats(false);
    hE->SetTitle("Si16b Dataset, Muon Spots, Thick Energy");
    hE->SetYTitle(axistitle.str().c_str());
    
    hE->SetLineColor(colours[i_cut]);
    hE->Draw("HIST E SAME");

    leg->AddEntry(hE, cutnames[i_cut].c_str(), "l");
  }
  leg->Draw();

  TLatex* latex = new TLatex();
  latex->DrawLatexNDC(0.55, 0.45, "AlCap Preliminary");
}
