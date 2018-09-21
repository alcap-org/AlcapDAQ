void Si16b_SiR3SolidAngleEfficiency() {
  std::string filename = "~/data/results/Si16b_passive/plots.root";
  TFile* file = new TFile(filename.c_str(), "READ");

  const int n_hists = 2;
  std::string histnames[n_hists] = {"hEvdE_TwoLayer_all_SiR", "hEvdE_ThreeLayer_12_all_SiR"};
  double integrals[n_hists] = {0, 0};
  
  double backleg_min_x = 6000;
  double backleg_max_x = 8000;
  double backleg_min_y = 0;
  double backleg_max_y = 300;

  for (int i_hist = 0; i_hist < n_hists; ++i_hist) {
    std::string i_histname = histnames[i_hist];
    std::string canvasname = "c_" + i_histname;
    TCanvas* c = new TCanvas(canvasname.c_str());
    
    TH2F* hEvdE = (TH2F*) file->Get(i_histname.c_str());
    hEvdE->GetXaxis()->SetRangeUser(0, 20000);
    hEvdE->GetYaxis()->SetRangeUser(0, 3000);
    hEvdE->Draw("COLZ");

    double integral_bin_x_low = hEvdE->GetXaxis()->FindBin(backleg_min_x);
    double integral_bin_x_high = hEvdE->GetXaxis()->FindBin(backleg_max_x);
    double integral_bin_y_low = hEvdE->GetYaxis()->FindBin(backleg_min_y);
    double integral_bin_y_high = hEvdE->GetYaxis()->FindBin(backleg_max_y);
    integrals[i_hist] = hEvdE->Integral(integral_bin_x_low, integral_bin_x_high, integral_bin_y_low, integral_bin_y_high);
    std::cout << integrals[i_hist] << std::endl;

    TBox* box = new TBox(backleg_min_x, backleg_min_y, backleg_max_x, backleg_max_y);
    box->SetLineColor(kRed);
    box->SetFillStyle(0);
    box->Draw("LSAME");

    std::stringstream text;
    text << integrals[i_hist] << " entries";
    
    TLatex* latex = new TLatex();
    latex->SetTextAlign(22);
    latex->SetTextColor(kRed);
    double latex_x = backleg_min_x + (backleg_max_x - backleg_min_x)/2;
    double latex_y = backleg_max_y + 100;
    latex->DrawLatex(latex_x, latex_y, text.str().c_str());
  }

  std::cout << "Ratio = " << integrals[1] / integrals[0] << std::endl;
}
