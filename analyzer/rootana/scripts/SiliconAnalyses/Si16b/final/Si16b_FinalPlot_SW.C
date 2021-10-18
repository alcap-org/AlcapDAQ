void Si16b_FinalPlot_SW(std::string savedir = "") {

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetLogy();

  double x_max = 26;
  const int n_points = 2;
  double axis_limits[n_points] = {0, x_max};
  double ys[n_points] = {0, 0};
  TGraph* axis_gre = new TGraph(n_points, axis_limits, ys);
  axis_gre->Draw("AP");
  axis_gre->GetYaxis()->SetRangeUser(1e-5, 4e0);
  axis_gre->SetTitle("Charged Particle Emission");
  axis_gre->GetXaxis()->SetTitle("Energy [MeV]");
  axis_gre->GetYaxis()->SetTitle("Charged Particles / Muon Capture / MeV");
  

  std::string SW_filename = "~/data/results/SiL3/SW-plots_betterErrors_MeV.root";
  //  std::string SW_filename = "~/data/results/Si16b/SW-plots-above1400keV.root";
  TFile* SW_file = new TFile(SW_filename.c_str(), "READ");
  TGraphErrors* SW_gre = (TGraphErrors*) SW_file->Get("Graph");
  SW_gre->Draw("PE SAME");
  SW_gre->GetXaxis()->SetRangeUser(0,x_max);

  TLegend* leg = new TLegend(0.50,0.55,0.85,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);
  leg->AddEntry(SW_gre, "Sobottka-Wills Data", "pl");

  TF1* fit = new TF1("fit", "[0]*(1 - [1]/x)^[2]*TMath::Exp(-x/[3])", 1.4, x_max);
  fit->SetParameters(0.1, 1.4, 1.3, 4.8);
  fit->SetParName(0, "N0");
  fit->SetParName(1, "T_{th}");
  fit->SetParName(2, "#alpha");
  fit->SetParName(3, "T0");
  SW_gre->Fit("fit", "RM");
  SW_gre->GetFunction("fit")->SetLineColor(kRed);

  TF1* fit_dble = new TF1("fit_dble", "[0]*(1 - [1]/x)^[2]*(TMath::Exp(-x/[3]) + [4]*TMath::Exp(-x/[5]))", 1.4, x_max);
  fit_dble->SetParameters(0.1, 1.4, 1.3, 4.8, 0.1, 7.0);
  fit_dble->SetParName(0, "N0");
  fit_dble->SetParName(1, "T_{th}");
  fit_dble->SetParName(2, "#alpha");
  fit_dble->SetParName(3, "T0");
  fit_dble->SetParName(4, "r");
  fit_dble->SetParName(5, "T1");
  SW_gre->Fit("fit_dble", "RM+");
  SW_gre->GetFunction("fit_dble")->SetLineColor(kMagenta);

  //  SW_gre->Fit(SW_gre->GetFunction("tdr_fit"));
  //  SW_gre->GetFunction("tdr_fit")->SetBit(TF1::kNotDraw);
  //  SW_gre->GetFunction("tdr_fit")->SetLineColor(kBlack);
  leg->AddEntry(SW_gre->GetFunction("fit"), "Threshold + Single Exp Fit", "l");
  leg->AddEntry(SW_gre->GetFunction("fit_dble"), "Threshold + Double Exp Fit", "l");
    
  leg->Draw();

  TCanvas* c2 = new TCanvas();
  c2->SetLogy();
  axis_gre->Draw("AP");
  TGraph* SW_gre_noErr = (TGraph*) SW_file->Get("Graph");
  SW_gre_noErr->GetFunction("tdr_fit")->SetBit(TF1::kNotDraw);
  SW_gre_noErr->Draw("PX SAME");

  if (savedir != "") {
    std::string basename = savedir + "AlCapData_SWSpectrum";
    std::string savename = basename + "_wFits";

    std::string pdfname = savename + ".pdf";
    c1->SaveAs(pdfname.c_str());
    std::string pngname = savename + ".png";
    c1->SaveAs(pngname.c_str());

    savename = basename + "_noErr";
    pdfname = savename + ".pdf";
    c2->SaveAs(pdfname.c_str());
    pngname = savename + ".png";
    c2->SaveAs(pngname.c_str());
  }
}
