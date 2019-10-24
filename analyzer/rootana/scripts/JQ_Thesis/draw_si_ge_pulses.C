{
  TFile* ifile = new TFile("data/pulses.root");
  si->SetTitle("Silicon Pulse;Time [Clock ticks];ADC");
  ge->SetTitle("Germanium Pulse;Time [Clock ticks];ADC");
  si->SetStats(0);
  si->GetYaxis()->SetTitleOffset(1.3);
  si->GetYaxis()->SetLabelSize(0.04);
  si->GetXaxis()->SetTitleOffset(1.2);
  si->GetXaxis()->SetLabelSize(0.04);
  si->SetTitleSize(0.04, "XY");
  si->SetLineColor(1);
  si->GetYaxis()->SetRangeUser(0, 2500);
  ge->SetStats(0);
  ge->GetYaxis()->SetTitleOffset(1.3);
  ge->GetYaxis()->SetLabelSize(0.04);
  ge->GetXaxis()->SetTitleOffset(1.2);
  ge->GetXaxis()->SetLabelSize(0.04);
  ge->SetTitleSize(0.04, "XY");
  ge->SetLineColor(1);
  // ge->GetYaxis()->SetRangeUser(0, 2500);
  gStyle->SetTitleFontSize(0.08);

  TLine* lth  = new TLine(0, 200, 128, 200);
  TLine* lped = new TLine(0, 110, 128, 110);
  lth ->SetLineColor(kRed);
  lped->SetLineColor(kBlue);

  TArrow* pre = new TArrow(0.5, 300, 26, 300, 0.03, "|-|");
  pre->SetLineColor(kBlack);
  pre->SetLineStyle(5);

  TArrow* hght= new TArrow(43.5, 110, 43.5, 2424, 0.03, "<|>");
  hght->SetLineStyle(2);

  TGraph* cf = new TGraph(1);
  cf->SetPoint(0, 27.8, 342.);
  cf->SetMarkerColor(kGreen);
  cf->SetMarkerStyle(8);
  cf->SetMarkerSize(1.5);

  TLegend* leg = new TLegend(0.5, 0.5, 0.9, 0.9);
  leg->AddEntry(lth,  "Threshold",     "l");
  leg->AddEntry(pre,  "Presamples",    "l");
  leg->AddEntry(lped, "Pedestal",      "l");
  leg->AddEntry(hght, "Pulse Height",  "l");
  leg->AddEntry(cf,   "Time @10% Max", "p");

  TCanvas* csi = new TCanvas();
  si  ->Draw();
  // lped->Draw();
  // lth ->Draw();
  // si  ->Draw("SAME");
  // hght->Draw();
  // cf  ->Draw("P");
  // pre ->Draw();
  // leg ->Draw();
  csi->SaveAs("img/si_pulse.png");

  TCanvas* cge = new TCanvas();
  ge->Draw();
  cge->SaveAs("img/ge_pulse.png");

}
