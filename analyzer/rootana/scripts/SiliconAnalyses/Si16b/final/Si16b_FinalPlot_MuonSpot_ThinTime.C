void Si16b_FinalPlot_MuonSpot_ThinTime() {
  
  std::string infilename = "~/data/results/Si16b/plots_newPP.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");

  TLegend* leg = new TLegend(0.55,0.55,0.90,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillStyle(0);
  leg->SetFillColor(kWhite);
  
  const int n_cuts = 3;
  std::string cutnames[n_cuts] = {"both", "lower", "upper"};
  Int_t colours[n_cuts] = {kBlack, kRed, kBlue};

  std::stringstream axistitle;
  for (int i_cut = 0; i_cut < n_cuts; ++i_cut) {
    std::string histname = "muspot_" + cutnames[i_cut] + "_SiR/hThinTime_12not3";
    TH1F* hTime = (TH1F*) infile->Get(histname.c_str());

    hTime->Rebin(2);
    hTime->GetXaxis()->SetRangeUser(-1000, 1000);

    axistitle.str("");
    axistitle << "Count / " << hTime->GetXaxis()->GetBinWidth(1) << " ns";
    hTime->SetStats(false);
    hTime->SetTitle("Si16b Dataset, Muon Spots, Thin Time");
    hTime->SetYTitle(axistitle.str().c_str());
    
    hTime->SetLineColor(colours[i_cut]);
    hTime->Draw("HIST E SAME");

    leg->AddEntry(hTime, cutnames[i_cut].c_str(), "l");
  }
  leg->Draw();

  TLatex* latex = new TLatex();
  latex->DrawLatexNDC(0.55, 0.45, "AlCap Preliminary");
}
