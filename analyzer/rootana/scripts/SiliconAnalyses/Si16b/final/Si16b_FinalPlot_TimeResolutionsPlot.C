void Si16b_FinalPlot_TimeResolutionsPlot() {

  TCanvas* c1 = new TCanvas();

  TLegend* leg = new TLegend(0.55, 0.4, 0.8, 0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.04);
  leg->SetFillColor(kWhite);
  //  leg->SetHeader("Mean #pm Std. Dev");


  const int N_MAX_RES = 15;
  const int n_GeHiGain = 2;
  double GeHiGain[n_GeHiGain] = { 90.2, 84.3};
  const int n_SiT_1 = 10;
  double SiT_1[n_SiT_1] = { 12.1, 35.4, 28.3, 30.6, 31.8, 36.9, 36.5, 38.0, 28.5, 33.4};
  const int n_SiT_2 = 4;
  double SiT_2[n_SiT_2] = { 35.3, 34.5, 21.5, 39.7};
  const int n_SiT_3 = 4;
  double SiT_3[n_SiT_3] = { 33.7, 28.7, 31.6, 30.5};
  const int n_SiT_4 = 6;
  double SiT_4[n_SiT_4] = { 36.3, 32.2, 28.6, 36.8, 36.5, 18.6};
  const int n_SiL1_8 = 14;
  double SiL1_8[n_SiL1_8] = { 37.7, 20.5, 17.6, 21.8, 27.7, 26.5, 25.1, 14.3, 19.7, 15.1, 27.4, 23.0, 30.0, 20.2};
  const int n_SiL1_9 = 10;
  double SiL1_9[n_SiL1_9] = { 27.9, 33.0, 30.5, 23.8, 11.5, 21.5, 35.8, 31.3, 37.5, 12.8};
  const int n_SiL3 = 7;
  double SiL3[n_SiL3] = { 37.8, 43.4, 38.5, 41.4, 45.0, 43.1, 45.6};
  const int n_SiR1_1 = 2;
  double SiR1_1[n_SiR1_1] = { 55.5, 54.8};
  const int n_SiR1_2 = 1;
  double SiR1_2[n_SiR1_2] = { 45.1};
  const int n_SiR1_3 = 1;
  double SiR1_3[n_SiR1_3] = { 55.6};
  const int n_SiR1_4 = 2;
  double SiR1_4[n_SiR1_4] = { 49.7, 48.6};
  const int n_SiR2 = 8;
  double SiR2[n_SiR2] = { 48.2, 44.9, 44.4, 51.9, 46.6, 47.3, 40.5, 51.3};
  const int n_SiR3 = 1;
  double SiR3[n_SiR3] = { 56.4};
  double points[N_MAX_RES] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

  TLatex* latex = new TLatex();
  latex->SetTextSize(0.04);
  latex->SetTextAlign(23);
  std::stringstream text;
  
  TGraph* grSiL1_8 = new TGraph(n_SiL1_8, points, SiL1_8);
  grSiL1_8->SetMarkerColor(kBlack);
  grSiL1_8->SetLineColor(kBlack);
  grSiL1_8->SetMarkerStyle(kOpenCircle);
  grSiL1_8->Draw("APL");
  grSiL1_8->GetYaxis()->SetRangeUser(0, 120);
  grSiL1_8->GetXaxis()->SetRangeUser(0, 16);
  grSiL1_8->GetXaxis()->SetTitle("Measurement");
  grSiL1_8->GetYaxis()->SetTitle("Time Resolution [ns]");

  text.str("");
  text << std::fixed << std::setprecision(0) << "SiL1-8: " << grSiL1_8->GetMean(2) << " #pm " << grSiL1_8->GetRMS(2) << " ns";
  //  latex->SetTextColor(kBlack);
  //  latex->DrawLatex(points[n_SiL1_8-1], SiL1_8[n_SiL1_8-1], text.str().c_str());
  leg->AddEntry(grSiL1_8, text.str().c_str(), "lp");

  TGraph* grSiL1_9 = new TGraph(n_SiL1_9, points, SiL1_9);
  grSiL1_9->SetMarkerColor(kRed);
  grSiL1_9->SetLineColor(kRed);
  grSiL1_9->SetMarkerStyle(kFullCircle);
  grSiL1_9->Draw("PL SAME");

  latex->SetTextAlign(13);
  
  text.str("");
  text << std::fixed << std::setprecision(0) << "SiL1-9: " << grSiL1_9->GetMean(2) << " #pm " << grSiL1_9->GetRMS(2) << " ns";
  //  latex->SetTextColor(kRed);
  //  latex->DrawLatex(points[n_SiL1_9-1], SiL1_9[n_SiL1_9-1], text.str().c_str());
  leg->AddEntry(grSiL1_9, text.str().c_str(), "lp");

  TGraph* grGeHiGain = new TGraph(n_GeHiGain, points, GeHiGain);
  grGeHiGain->SetMarkerStyle(kOpenCircle);
  grGeHiGain->SetMarkerColor(kBlack);
  grGeHiGain->SetLineColor(kBlack);
  grGeHiGain->Draw("PL SAME");
  text.str("");
  text << std::fixed << std::setprecision(0) << "GeHiGain: " << grGeHiGain->GetMean(2) << " #pm " << grGeHiGain->GetRMS(2) << " ns";
  //  latex->SetTextColor(kBlack);
  //  latex->DrawLatex(points[n_GeHiGain-1], GeHiGain[n_GeHiGain-1], text.str().c_str());
  leg->AddEntry(grGeHiGain, text.str().c_str(), "lp");

  TGraph* grSiT_1 = new TGraph(n_SiT_1, points, SiT_1);
  grSiT_1->SetMarkerStyle(kFullSquare);
  grSiT_1->SetMarkerColor(kBlack);
  grSiT_1->SetLineColor(kBlack);
  grSiT_1->Draw("PL SAME");
  text.str("");
  text << std::fixed << std::setprecision(0) << "SiT-1: " << grSiT_1->GetMean(2) << " #pm " << grSiT_1->GetRMS(2) << " ns";
  //  latex->SetTextColor(kBlack);
  //  latex->DrawLatex(points[n_SiT_1-1], SiT_1[n_SiT_1-1], text.str().c_str());
  leg->AddEntry(grSiT_1, text.str().c_str(), "lp");

  TGraph* grSiT_2 = new TGraph(n_SiT_2, points, SiT_2);
  grSiT_2->SetMarkerStyle(kFullSquare);
  grSiT_2->SetMarkerColor(kRed);
  grSiT_2->SetLineColor(kRed);
  grSiT_2->Draw("PL SAME");
  text.str("");
  text << std::fixed << std::setprecision(0) << "SiT-2: " << grSiT_2->GetMean(2) << " #pm " << grSiT_2->GetRMS(2) << " ns";
  //  latex->SetTextColor(kRed);
  //  latex->DrawLatex(points[n_SiT_2-1], SiT_2[n_SiT_2-1], text.str().c_str());
  leg->AddEntry(grSiT_2, text.str().c_str(), "lp");


  TGraph* grSiT_3 = new TGraph(n_SiT_3, points, SiT_3);
  grSiT_3->SetMarkerStyle(kFullSquare);
  grSiT_3->SetMarkerColor(kBlue);
  grSiT_3->SetLineColor(kBlue);
  grSiT_3->Draw("PL SAME");
  text.str("");
  text << std::fixed << std::setprecision(0) << "SiT-3: " << grSiT_3->GetMean(2) << " #pm " << grSiT_3->GetRMS(2) << " ns";
  //  latex->SetTextColor(kBlue);
  //  latex->DrawLatex(points[n_SiT_3-1], SiT_3[n_SiT_3-1], text.str().c_str());
  leg->AddEntry(grSiT_3, text.str().c_str(), "lp");

  TGraph* grSiT_4 = new TGraph(n_SiT_4, points, SiT_4);
  grSiT_4->SetMarkerStyle(kFullSquare);
  grSiT_4->Draw("PL SAME");
  grSiT_4->SetMarkerColor(kMagenta);
  grSiT_4->SetLineColor(kMagenta);
  grSiT_4->Draw("PL SAME");
  text.str("");
  text << std::fixed << std::setprecision(0) << "SiT-4: " << grSiT_4->GetMean(2) << " #pm " << grSiT_4->GetRMS(2) << " ns";
  //  latex->SetTextColor(kMagenta);
  //  latex->DrawLatex(points[n_SiT_4-1], SiT_4[n_SiT_4-1], text.str().c_str());
  leg->AddEntry(grSiT_4, text.str().c_str(), "lp");

  TGraph* grSiR1_1 = new TGraph(n_SiR1_1, points, SiR1_1);
  grSiR1_1->SetMarkerStyle(kFullTriangleUp);
  grSiR1_1->SetMarkerColor(kBlack);
  grSiR1_1->SetLineColor(kBlack);
  grSiR1_1->Draw("PL SAME");
  text.str("");
  text << std::fixed << std::setprecision(0) << "SiR1-1: " << grSiR1_1->GetMean(2) << " #pm " << grSiR1_1->GetRMS(2) << " ns";
  //  latex->SetTextColor(kBlack);
  //  latex->DrawLatex(points[n_SiR1_1-1], SiR1_1[n_SiR1_1-1], text.str().c_str());
  leg->AddEntry(grSiR1_1, text.str().c_str(), "lp");

  TGraph* grSiR1_2 = new TGraph(n_SiR1_2, points, SiR1_2);
  grSiR1_2->SetMarkerStyle(kFullTriangleUp);
  grSiR1_2->SetMarkerColor(kRed);
  grSiR1_2->SetLineColor(kRed);
  grSiR1_2->Draw("PL SAME");
  text.str("");
  text << std::fixed << std::setprecision(0) << "SiR1-2: " << grSiR1_2->GetMean(2) << " #pm " << grSiR1_2->GetRMS(2) << " ns";
  //  latex->SetTextColor(kRed);
  //  latex->DrawLatex(points[n_SiR1_2-1], SiR1_2[n_SiR1_2-1], text.str().c_str());
  leg->AddEntry(grSiR1_2, text.str().c_str(), "lp");


  TGraph* grSiR1_3 = new TGraph(n_SiR1_3, points, SiR1_3);
  grSiR1_3->SetMarkerStyle(kFullTriangleUp);
  grSiR1_3->SetMarkerColor(kBlue);
  grSiR1_3->SetLineColor(kBlue);
  grSiR1_3->Draw("PL SAME");
  text.str("");
  text << std::fixed << std::setprecision(0) << "SiR1-3: " << grSiR1_3->GetMean(2) << " #pm " << grSiR1_3->GetRMS(2) << " ns";
  //  latex->SetTextColor(kBlue);
  //  latex->DrawLatex(points[n_SiR1_3-1], SiR1_3[n_SiR1_3-1], text.str().c_str());
  leg->AddEntry(grSiR1_3, text.str().c_str(), "lp");

  TGraph* grSiR1_4 = new TGraph(n_SiR1_4, points, SiR1_4);
  grSiR1_4->SetMarkerStyle(kFullTriangleUp);
  grSiR1_4->Draw("PL SAME");
  grSiR1_4->SetMarkerColor(kMagenta);
  grSiR1_4->SetLineColor(kMagenta);
  grSiR1_4->Draw("PL SAME");
  text.str("");
  text << std::fixed << std::setprecision(0) << "SiR1-4: " << grSiR1_4->GetMean(2) << " #pm " << grSiR1_4->GetRMS(2) << " ns";
  leg->AddEntry(grSiR1_4, text.str().c_str(), "lp");

  TGraph* grSiR2 = new TGraph(n_SiR2, points, SiR2);
  grSiR2->SetMarkerStyle(kFullDiamond);
  grSiR2->SetMarkerColor(kBlack);
  grSiR2->SetLineColor(kBlack);
  grSiR2->Draw("PL SAME");
  text.str("");
  text << std::fixed << std::setprecision(0) << "SiR2: " << grSiR2->GetMean(2) << " #pm " << grSiR2->GetRMS(2) << " ns";
  leg->AddEntry(grSiR2, text.str().c_str(), "lp");


  TGraph* grSiR3 = new TGraph(n_SiR3, points, SiR3);
  grSiR3->SetMarkerStyle(kFullDiamond);
  grSiR3->SetMarkerColor(kRed);
  grSiR3->SetLineColor(kRed);
  grSiR3->Draw("PL SAME");
  text.str("");
  text << std::fixed << std::setprecision(0) << "SiR3: " << grSiR3->GetMean(2) << " #pm " << grSiR3->GetRMS(2) << " ns";
  leg->AddEntry(grSiR3, text.str().c_str(), "lp");

  
  TGraph* grSiL3 = new TGraph(n_SiL3, points, SiL3);
  grSiL3->SetMarkerStyle(kFullDiamond);
  grSiL3->SetMarkerColor(kBlue);
  grSiL3->SetLineColor(kBlue);
  grSiL3->Draw("PL SAME");
  text.str("");
  text << std::fixed << std::setprecision(0) << "SiL3: " << grSiL3->GetMean(2) << " #pm " << grSiL3->GetRMS(2) << " ns";
  leg->AddEntry(grSiL3, text.str().c_str(), "lp");

  leg->Draw();
}
