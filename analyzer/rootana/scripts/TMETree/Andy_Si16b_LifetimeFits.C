void Andy_Si16b_LifetimeFits() {
  TFile* file = new TFile("Andy_Si16b_EvdEPlot.root");
  //  TFile* file = new TFile("Andy_Si16b_EvdEPlot_TimeCutGT200ns.root");
  TH1F* hProton_NoVeto = (TH1F*) file->Get("hTime_Proton_NoVeto");
  TH1F* hProton_Veto = (TH1F*) file->Get("hTime_Proton_Veto");

  hProton_Veto->Rebin(10);
  hProton_Veto->GetXaxis()->SetRangeUser(-1000, 10000);
  
  TCanvas* c1=  new TCanvas("c1", "c1");
  c1->SetLogy();

  /*  double min_fit_time = 1000;
  double max_fit_time = 5000;
  TF1* expo_fit_fn = new TF1("expo_fit_fn", "[0]*TMath::Exp(-x/[1]) + [2]", min_fit_time, max_fit_time);
  expo_fit_fn->SetParameters(hProton_Veto->GetMaximum(), 700, 10);
  */
  double min_fit_time = 0;
  double max_fit_time = 6000;
  TF1* expo_fit_fn = new TF1("expo_fit_fn", "[0]*TMath::Exp(-x/[1]) + [2]*TMath::Exp(-x/[3]) + [4]", min_fit_time, max_fit_time);
  expo_fit_fn->SetParameters(hProton_Veto->GetMaximum(), 700, hProton_Veto->GetMaximum(), 100, 10);
  
  
  hProton_Veto->Fit(expo_fit_fn, "R+");
  hProton_Veto->Draw("HIST E");
  expo_fit_fn->Draw("LSAME");
  std::stringstream text;
  text << "#splitline{Expo Fit (t = " << min_fit_time << " -- " << max_fit_time << " ns):}{#tau_{Si} = " << expo_fit_fn->GetParameter(1) << " #pm " << expo_fit_fn->GetParError(1) << "}";
  double x_latex_pos = (max_fit_time + min_fit_time)/2 + 250;
  double y_latex_pos = hProton_Veto->GetBinContent(hProton_Veto->FindBin(x_latex_pos))*5;
  TLatex* latex = new TLatex(x_latex_pos, y_latex_pos, text.str().c_str());
  latex->SetTextSize(0.03);
  //  latex->SetTextAlign(22);
  latex->SetTextColor(kRed);
  latex->Draw();

  text.str("");
  text << "#splitline{Expo Fit (t = " << min_fit_time << " -- " << max_fit_time << " ns):}{#tau_{Pb} = " << expo_fit_fn->GetParameter(3) << " #pm " << expo_fit_fn->GetParError(3) << "}";
  x_latex_pos = (max_fit_time + min_fit_time)/2 + 250;
  y_latex_pos = hProton_Veto->GetBinContent(hProton_Veto->FindBin(x_latex_pos))*2;
  TLatex* latex2 = new TLatex(x_latex_pos, y_latex_pos, text.str().c_str());
  latex2->SetTextSize(0.03);
  //  latex->SetTextAlign(22);
  latex2->SetTextColor(kBlue);
  latex2->Draw();

  std::cout << "Amp(Si) = " << expo_fit_fn->GetParameter(0) << " +/- " << expo_fit_fn->GetParError(0) << std::endl;
  std::cout << "Amp(Pb) = " << expo_fit_fn->GetParameter(2) << " +/- " << expo_fit_fn->GetParError(2) << std::endl;
}
