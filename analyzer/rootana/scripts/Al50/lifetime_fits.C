{
  const int N = 30;
  TFile* f = new TFile("data/Al50/psel.3layer.root");
  PID_LP->Draw("t>>hl(500,0,5e3)", "e3==0 && abs(dt)<200", "goff");
  PID_RP->Draw("t>>hr(500,0,5e3)", "e3==0 && abs(dt)<200", "goff");
  TF1* fit = new TF1("fit", "[0]+[1]*exp(-x/[2])");
  fit->SetParameters(0, 1e3, 864);
  TGraphErrors* gr[2] = { new TGraphErrors(N), new TGraphErrors(N) };
  for (int i = 0; i <= N; ++i) {
    double t = i*100;
    hl->GetXaxis()->SetRangeUser(t, 5e3);
    hr->GetXaxis()->SetRangeUser(t, 5e3);
    TFitResultPtr res_l = hl->Fit(fit, "LEMS");
    TFitResultPtr res_r = hr->Fit(fit, "LEMS");
    gr[0]->SetPoint     (i, t, res_l->Parameter(2));
    gr[1]->SetPoint     (i, t, res_r->Parameter(2));
    gr[0]->SetPointError(i, 0, res_l->ParError(2));
    gr[1]->SetPointError(i, 0, res_r->ParError(2));
    printf("%gns:\t%g/%d\n", t, res_l->Chi2(), res_l->Ndf());
    printf("\t%g/%d\n",    res_r->Chi2(), res_r->Ndf());
  }
  gr[0]->SetLineColor(kBlue);
  gr[1]->SetLineColor(kRed);
  TMultiGraph* mg = new TMultiGraph("mg", "Proton Lifetime (|t_{Si1}-t_{Si2}|<200 && e_{Si3}==0);Lower Limit [ns];Lifetime [ns]");
  mg->Add(gr[0]);
  mg->Add(gr[1]);
  TLegend* leg = new TLegend(0.1, 0.1, 0.3, 0.3);
  leg->AddEntry(gr[0], "SiL");
  leg->AddEntry(gr[1], "SiR");

  TCanvas* c = new TCanvas();
  mg->Draw("ALP");
  mg->GetXaxis()->SetRangeUser(0, 3e3);
  mg->GetYaxis()->SetRangeUser(500, 1e3);
  TLine* l = new TLine(0, 864, 3e3, 864);
  l->SetLineColor(kBlack);
  l->SetLineStyle(2);
  l->Draw();
  leg->Draw();

  c->SaveAs("img/proton_lifetime_varying_lower_bound.png");
}
