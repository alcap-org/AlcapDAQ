void Si16b_FinalPlot_MuonSpot_MixedEnergies() {
  
  std::string infilename = "~/data/results/Si16b/plots_newPP.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");

  TLegend* leg = new TLegend(0.50,0.55,0.90,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillStyle(0);
  leg->SetFillColor(kWhite);
  
  const int n_cuts = 2;
  std::string cutnames[n_cuts] = {"upper", "lower"};
  bool project_x[n_cuts] = {false, true};
  Int_t colours[n_cuts] = {kBlue, kRed};

  TH1F* hSum = 0;
  
  std::stringstream axistitle, leglabel;
  for (int i_cut = 0; i_cut < n_cuts; ++i_cut) {

    std::string histname = "muspot_" + cutnames[i_cut] + "_SiR/hEvdE_TwoLayer_12not3";
    TH2F* hEvdE = (TH2F*) infile->Get(histname.c_str());

    std::string newhistname = "hE_" + cutnames[i_cut];
    TH1D* hE = 0;
    if (project_x[i_cut]) {
      hE = hEvdE->ProjectionX(newhistname.c_str());
    }
    else {
      hE = hEvdE->ProjectionY(newhistname.c_str());
    }

    hE->Rebin(5);
    hE->GetXaxis()->SetRangeUser(0, 4000);

    axistitle.str("");
    axistitle << "Count / " << hE->GetXaxis()->GetBinWidth(1) << " keV";
    hE->SetStats(false);
    hE->SetTitle("Si16b Dataset, Muon Spots, Energy");
    hE->SetYTitle(axistitle.str().c_str());
    
    hE->SetLineColor(colours[i_cut]);
    hE->Draw("HIST E SAME");

    leglabel.str("");
    leglabel << cutnames[i_cut] << " (";
    if (project_x[i_cut]) {
      leglabel << "total energy";
    }
    else {
      leglabel << "thin energy";
    }
    leglabel << ")";
    leg->AddEntry(hE, leglabel.str().c_str(), "l");

    if (!hSum) {
      hSum = (TH1F*) hE->Clone("hSum");
      hSum->SetLineColor(kBlack);
    }
    else {
      hSum->Add(hE);
    }
  }
  leg->AddEntry(hSum, "sum", "l");
  hSum->Draw("HIST E SAME");
  
  leg->Draw();

  TLatex* latex = new TLatex();
  latex->DrawLatexNDC(0.55, 0.45, "AlCap Preliminary");
}
