{
  TFile* f = new TFile("~/R15bTME/Si16b/out10362.root");
  gDirectory->cd("TMETree");
  TMETree->Draw("timeToNextTME>>h(20000,0,2e5)");
  TF1* f1 = new TF1("exponly", "expo(0)");
  TF1* f2 = new TF1("expflat", "expo(0)+[2]");
  f1->SetParNames("ExpConst", "Rate");
  f2->SetParNames("ExpConst", "Rate", "FlatConst");
  f1->SetParameters(4.78199e+00, -5.26958e-06);
  f2->SetParameters(4.78211e+00, -5.27030e-06, 3.35063e-04);
  h->GetXaxis()->SetRangeUser(20e3, 2e5);
  TFitResultPtr res1 = h->Fit(f1, "SLL");
  TFitResultPtr res2 = h->Fit(f2, "S+LL");
  printf("Chi2/ndf exponly: %g/%u\n", res1->Chi2(), res1->Ndf());
  printf("Chi2/ndf expflat: %g/%u\n", res2->Chi2(), res2->Ndf());
}
