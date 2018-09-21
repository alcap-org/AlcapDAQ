void Andy_SiL3_QuickAnalysis_LifetimeFits() {

  std::string filename = "Andy_SiL3_QuickAnalysis.root";
  TFile* file = new TFile(filename.c_str(), "READ");

  TH2F* hTimeEnergy = (TH2F*) file->Get("hTimeEnergy");

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetLogz();
  hTimeEnergy->Draw("COLZ");

  TCanvas* c2 = new TCanvas("c2", "c2");
  c2->SetLogy();
  TH1D* hTime = hTimeEnergy->ProjectionX("hTime");
  double min_fit_time = 5000;
  double max_fit_time = 20000;
  TF1* expo_fit_fn = new TF1("expo_fit_fn", "[0]*TMath::Exp(-x/[1])+[2]", min_fit_time, max_fit_time);
  expo_fit_fn->SetParameters(hTime->GetMaximum(), 700, 10);
  hTime->Fit(expo_fit_fn, "R+");
  hTime->Draw("HIST E");
  expo_fit_fn->Draw("LSAME");
  std::stringstream text;
  text << "#splitline{Expo Fit (t = " << min_fit_time << " -- " << max_fit_time << " ns):}{#tau = " << expo_fit_fn->GetParameter(1) << " #pm " << expo_fit_fn->GetParError(1) << "}";
  double x_latex_pos = (max_fit_time + min_fit_time)/2 + 250;
  double y_latex_pos = hTime->GetBinContent(hTime->FindBin(x_latex_pos))*5;
  std::cout << x_latex_pos << " " << y_latex_pos << std::endl;
  TLatex* latex = new TLatex(x_latex_pos, y_latex_pos, text.str().c_str());
  latex->SetTextSize(0.03);
  //  latex->SetTextAlign(22);
  latex->SetTextColor(kRed);
  latex->Draw();
  
  double min_fit_time2 = 1500;
  double max_fit_time2 = 4000;
  double x_latex2_pos = (max_fit_time2 + min_fit_time2)/2 + 250;
  double y_latex2_pos = hTime->GetBinContent(hTime->FindBin(x_latex2_pos))*5;
  std::cout << x_latex2_pos << " " << y_latex2_pos << std::endl;
  TF1* expo_fit_fn2 = new TF1("expo_fit_fn2", "[0]*TMath::Exp(-x/[1])", min_fit_time2, max_fit_time2);
  expo_fit_fn2->SetParameters(hTime->GetMaximum(), 700);
  hTime->Fit(expo_fit_fn2, "R+");
  expo_fit_fn2->SetLineColor(kBlue);
  expo_fit_fn2->Draw("LSAME");
  text.str("");
  text << "#splitline{Expo Fit (t = " << min_fit_time2 << " -- " << max_fit_time2 << " ns):}{#tau = " << expo_fit_fn2->GetParameter(1) << " #pm " << expo_fit_fn2->GetParError(1) << "}";
  TLatex* latex_2 = new TLatex(x_latex2_pos, y_latex2_pos, text.str().c_str());
  latex_2->SetTextSize(0.03);
  //  latex_2->SetTextAlign(22);
  latex_2->SetTextColor(kBlue);
  latex_2->Draw();


}
