void PlotRangeCurves() {

  TGraph* SiR1_1_gr = new TGraph("SiR1_HitsPerIncomingMuon_Threshold-100ADC.txt", "%lg %lg %*lg %*lg %*lg");
  SiR1_1_gr->SetLineColor(kBlue);
  SiR1_1_gr->SetLineWidth(2);
  SiR1_1_gr->Draw("AL*");
  SiR1_1_gr->GetXaxis()->SetTitle("Scale Factor [x30.8 MeV/c]");
  SiR1_1_gr->GetYaxis()->SetTitle("Hits Above 100 ADC per Entering Muon");
  SiR1_1_gr->GetYaxis()->SetRangeUser(0, 0.08);

  TGraph* SiR1_2_gr = new TGraph("SiR1_HitsPerIncomingMuon_Threshold-100ADC.txt", "%lg %*lg %lg %*lg %*lg");
  SiR1_2_gr->SetLineColor(kRed);
  SiR1_2_gr->SetLineWidth(2);
  SiR1_2_gr->Draw("L* SAME");

  TGraph* SiR1_3_gr = new TGraph("SiR1_HitsPerIncomingMuon_Threshold-100ADC.txt", "%lg %*lg %*lg %lg %*lg");
  SiR1_3_gr->SetLineColor(kBlack);
  SiR1_3_gr->SetLineWidth(2);
  SiR1_3_gr->Draw("L* SAME");

  TGraph* SiR1_4_gr = new TGraph("SiR1_HitsPerIncomingMuon_Threshold-100ADC.txt", "%lg %*lg %*lg %*lg %lg");
  SiR1_4_gr->SetLineColor(kMagenta);
  SiR1_4_gr->SetLineWidth(2);
  SiR1_4_gr->Draw("L* SAME");

  TLegend *leg = new TLegend(0.18,0.66,0.38,0.86);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);
  leg->AddEntry(SiR1_1_gr, "SiR1-1", "l");
  leg->AddEntry(SiR1_2_gr, "SiR1-2", "l");
  leg->AddEntry(SiR1_3_gr, "SiR1-3", "l");
  leg->AddEntry(SiR1_4_gr, "SiR1-4", "l");
  leg->Draw();
}
