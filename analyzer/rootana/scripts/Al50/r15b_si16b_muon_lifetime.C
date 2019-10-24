{
  TChain* ch = new TChain("TMETree/TMETree");
  // ch->Add("~/R15bTME/Si16b/out10362.root");
  ch->Add("~/R15bTME/Si16b/out1036*.root");
  ch->Add("~/R15bTME/Si16b/out1037*.root");

  TH1* h = new TH1D("h", "Si16b Target Times;tTME [ns];cts/10ns", 500, 0, 5e3);
  ch->Draw("SiL1_2.tTME>>+h",  "500<SiL1_2.E  && SiL1_2.E<1500",  "goff");
  ch->Draw("SiL1_3.tTME>>+h",  "500<SiL1_3.E  && SiL1_3.E<1500",  "goff");
  ch->Draw("SiL1_4.tTME>>+h",  "500<SiL1_4.E  && SiL1_4.E<1500",  "goff");
  ch->Draw("SiL1_5.tTME>>+h",  "500<SiL1_5.E  && SiL1_5.E<1500",  "goff");
  ch->Draw("SiL1_6.tTME>>+h",  "500<SiL1_6.E  && SiL1_6.E<1500",  "goff");
  ch->Draw("SiL1_7.tTME>>+h",  "500<SiL1_7.E  && SiL1_7.E<1500",  "goff");
  ch->Draw("SiL1_8.tTME>>+h",  "500<SiL1_8.E  && SiL1_8.E<1500",  "goff");
  ch->Draw("SiL1_9.tTME>>+h",  "500<SiL1_9.E  && SiL1_9.E<1500",  "goff");
  ch->Draw("SiL1_10.tTME>>+h", "500<SiL1_10.E && SiL1_10.E<1500", "goff");
  ch->Draw("SiL1_11.tTME>>+h", "500<SiL1_11.E && SiL1_11.E<1500", "goff");
  ch->Draw("SiL1_12.tTME>>+h", "500<SiL1_12.E && SiL1_12.E<1500", "goff");
  ch->Draw("SiL1_13.tTME>>+h", "500<SiL1_13.E && SiL1_13.E<1500", "goff");
  ch->Draw("SiL1_14.tTME>>+h", "500<SiL1_14.E && SiL1_14.E<1500", "goff");
  ch->Draw("SiL1_15.tTME>>+h", "500<SiL1_15.E && SiL1_15.E<1500", "goff");

  TF1* fit = new TF1("fit", "[0]+[1]*exp(-x/[2])", 1500, 4000);
  fit->SetParameters(2, 100, 750);
  fit->SetParNames("Flat BG", "Exp Amp", "Lifetime");
  h->Fit(fit, "EMR");

  h->GetYaxis()->SetTitleOffset(1.3);

  // gStyle->SetOptStat(10);
  gStyle->SetOptFit(111);
  TCanvas* c = new TCanvas;
  h->Draw();
  c->SetLogy();
  c->SaveAs("img/si16b_beam_lifetime.png");

}
