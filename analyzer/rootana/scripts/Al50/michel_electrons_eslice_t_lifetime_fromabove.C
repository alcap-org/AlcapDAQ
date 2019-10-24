{
  TFile* f = new TFile("out/michel_electrons.root");
  TH2* hlet[4] = { (TH2*)f->Get("hletall"),    (TH2*)f->Get("hlet0si1"),
                   nullptr,                    nullptr };
  TH2* hret[4] = { (TH2*)f->Get("hretall"),     (TH2*)f->Get("hret0si1"),
                  (TH2*)f->Get("hret0si1si3"), nullptr };
  hlet[3] = (TH2*)hlet[0]->Clone("hletsi1"); hlet[3]->Add(hlet[1], -1);
  hret[3] = (TH2*)hret[0]->Clone("hretsi1"); hret[3]->Add(hret[1], -1);
  const int N = 10;
  TH1 *hlt[N], *hrt[N];
  for (int i = 0; i < N; ++i) {
    double e = 200*(i+1);
    char lname[64], rname[64];
    sprintf(lname, "hlt%dkeV", int(e));
    sprintf(rname, "hrt%dkeV", int(e));
    hlt[i] = hlet[1]->ProjectionX(lname, 1, hlet[1]->GetYaxis()->FindFixBin(e)-1);
    hrt[i] = hret[2]->ProjectionX(rname, 1, hret[2]->GetYaxis()->FindFixBin(e)-1);
  };
  for (int i = 0; i < N; ++i) {
    hlt[i]->SetTitle("Al50 SiL3 Hits (!SiL1+SiL3);Time [ns];Count/10 ns");
    hrt[i]->SetTitle("Al50 SiR2 Hits (!SiR1+SiR2+SiR3);Time [ns];Count/10 ns");
    hlt[i]->GetXaxis()->SetRangeUser(500., 5e3);
    hrt[i]->GetXaxis()->SetRangeUser(500., 5e3);
    hlt[i]->SetLineColor(i+1);
    hrt[i]->SetLineColor(i+1);
    hlt[i]->SetMaximum(7e3);
    hrt[i]->SetMaximum(5e3);
    hlt[i]->SetMinimum(60);
    hrt[i]->SetMinimum(0.9);
  }

  const TString fnc("[0]+[1]*exp(-x/[2])");
  TF1* fit[2] = { new TF1("", fnc), new TF1("", fnc) };
  fit[0]->SetParameters(1000, 7000, 864.);
  fit[1]->SetParameters(600,  3400, 864.);
  TFitResultPtr res[N][2];
  for (int i = 0; i < N; ++i) {
    res[i][0] = hlt[i]->Fit(fit[0], "MSE");
    res[i][1] = hrt[i]->Fit(fit[1], "MSE");
  }
  for (int i = 0; i < N; ++i) {
    std::printf("%d/%d\t%d/%d\n",
               (int)res[i][0]->Chi2(), (int)res[i][0]->Ndf(),
               (int)res[i][1]->Chi2(), (int)res[i][1]->Ndf());
  }

  TLegend* leg = new TLegend(0.6, 0.65, 0.9, 0.9);
  leg->AddEntry(hlt[0], "E<200keV");
  leg->AddEntry(hlt[1], "E<400keV");
  leg->AddEntry(hlt[2], "E<600keV");
  leg->AddEntry(hlt[3], "E<800keV");
  leg->AddEntry(hlt[4], "E<1000keV");
  leg->AddEntry(hlt[5], "E<1200keV");
  leg->AddEntry(hlt[6], "E<1400keV");
  leg->AddEntry(hlt[7], "E<1600keV");
  leg->AddEntry(hlt[8], "E<1800keV");
  leg->AddEntry(hlt[9], "E<2000keV");

  gStyle->SetOptStat(0);

  TCanvas* cet = new TCanvas("cet", "cet", 1400, 500);
  cet->Divide(2);
  cet->cd(1);
  hlet[1]->Draw("COLZ");
  cet->cd(2);
  hret[2]->Draw("COLZ");
  cet->SaveAs("img/michel_eletrons_et.png");

  TCanvas* ct = new TCanvas("ct", "ct", 1400, 500);
  ct->Divide(2);
  ct->cd(1)->SetLogy();
  for (int i = 0; i < N; ++i)
    hlt[i]->Draw("SAME");
  leg   ->Draw("SAME");
  ct->cd(2)->SetLogy();
  for (int i = 0; i < N; ++i)
    hrt[i]->Draw("SAME");
  leg   ->Draw("SAME");
  ct->SaveAs("img/michel_electrons_eslice_lifetime_fromabove.png");

  double elo[N], lt[2][N], elt[2][N];
  for (int i = 0; i < N; ++i) {
    elo[i]    = 200*(i+1);
    lt[0][i]  = res[i][0]->Parameter(2);
    lt[1][i]  = res[i][1]->Parameter(2);
    elt[0][i] = res[i][0]->ParError(2);
    elt[1][i] = res[i][1]->ParError(2);
  }
  TGraph* gr[2] = { new TGraphErrors(N, elo, lt[0], nullptr, elt[0]),
                    new TGraphErrors(N, elo, lt[1], nullptr, elt[1]) };
  TLegend* leglt = new TLegend(0.7, 0.1, 0.9, 0.3);
  leglt->AddEntry(gr[0], "SiL");
  leglt->AddEntry(gr[1], "SiR");
  TCanvas* clt = new TCanvas("clt", "clt", 700, 500);
  gr[0]->SetTitle("Lifetime after energy cut;Upper Bound [keV];Lifetime [ns]");
  gr[1]->SetTitle("Lifetime after energy cut;Upper Bound [keV];Lifetime [ns]");
  gr[0]->SetLineColor(kBlue);
  gr[1]->SetLineColor(kRed);
  gr[0]->Draw("AL");
  gr[1]->Draw("SAME L");
  leglt->Draw("SAME");
  gr[0]->GetYaxis()->SetRangeUser(600., 1200.);
  clt->SaveAs("img/michel_electrons_eslice_lifetime_graph_fromabove.png");
}
