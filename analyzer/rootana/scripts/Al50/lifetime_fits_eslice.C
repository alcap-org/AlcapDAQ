{
  const int N = 11;
  TFile* f = new TFile("data/Al50/psel.3layer.root");
  TH1* hs[N][2];
  TGraphErrors* gr[2] = { new TGraphErrors(N), new TGraphErrors(N) };
  for (int i = 0; i < N; ++i) {
    int elim[2] = { 1000*i+2000, 1000*i+3000 };
    char hname[2][16], cut[128], cmd[2][128];
    std::sprintf(hname[0], "hl%d", i);
    std::sprintf(hname[1], "hr%d", i);
    std::sprintf(cut, "e3==0 && abs(dt)<200 && %d<e && e<%d", elim[0], elim[1]);
    std::sprintf(cmd[0], "t>>%s(460,400,5e3)", hname[0]);
    std::sprintf(cmd[1], "t>>%s(460,400,5e3)", hname[1]);
    PID_LP->Draw(cmd[0], cut, "goff");
    PID_RP->Draw(cmd[1], cut, "goff");
    hs[i][0] = (TH1*)gDirectory->Get(hname[0]);
    hs[i][1] = (TH1*)gDirectory->Get(hname[1]);
    TF1* fit = new TF1("fit", "[0]+[1]*exp(-x/[2])", 0, 5e3);
    fit->SetParameters(0., 100., 864.);
    TFitResultPtr res_l = hs[i][0]->Fit(fit, "LEMS");
    TFitResultPtr res_r = hs[i][1]->Fit(fit, "LEMS");
    gr[0]->SetPoint     (i, elim[0], res_l->Parameter(2));
    gr[1]->SetPoint     (i, elim[0], res_r->Parameter(2));
    gr[0]->SetPointError(i, 0,       res_l->ParError(2));
    gr[1]->SetPointError(i, 0,       res_r->ParError(2));
    printf("%dkeV:\t%g/%d\n", elim[0], res_l->Chi2(), res_l->Ndf());
    printf("      \t%g/%d\n",           res_r->Chi2(), res_r->Ndf());
  }
  gr[0]->SetLineColor(kBlue);
  gr[1]->SetLineColor(kRed);
  TMultiGraph* mg = new TMultiGraph("mg", "Proton Lifetime (|t_{Si1} - t_{Si2}|<200 && e_{Si3}==0);E Slice [keV];Lifetime [ns]");
  mg->Add(gr[0]);
  mg->Add(gr[1]);
  TLegend* leg = new TLegend(0.1, 0.1, 0.3, 0.3);
  leg->AddEntry(gr[0], "SiL");
  leg->AddEntry(gr[1], "SiR");

  TCanvas* c = new TCanvas();
  mg->Draw("ALP");
  mg->GetXaxis()->SetRangeUser(2e3, 12e3);
  // mg->GetYaxis()->SetRangeUser(500, 1e3);
  TLine* l = new TLine(2e3, 864, 12e3, 864);
  l->SetLineColor(kBlack);
  l->SetLineStyle(2);
  l->Draw();
  leg->Draw();

  c->SaveAs("img/proton_lifetime_varying_lower_bound.png");
}
