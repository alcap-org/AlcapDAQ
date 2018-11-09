{
  const int NF = 10;
  TFile* fs[NF] = {
    new TFile("data/Al50/en.2layer.t400.dt200.rebin5.elo2e3keV.ehi20e3keV.root"),
    new TFile("data/Al50/en.2layer.t400.dt200.rebin5.elo2e3keV.ehi15e3keV.root"),
    new TFile("data/Al50/en.2layer.t400.dt200.rebin5.elo2e3keV.ehi14e3keV.root"),
    new TFile("data/Al50/en.2layer.t400.dt200.rebin5.elo2e3keV.ehi13e3keV.root"),
    new TFile("data/Al50/en.2layer.t400.dt200.rebin5.elo2e3keV.ehi12e3keV.root"),
    new TFile("data/Al50/en.2layer.t400.dt200.rebin5.elo2e3keV.ehi11e3keV.root"),
    new TFile("data/Al50/en.2layer.t400.dt200.rebin5.elo2e3keV.ehi10e3keV.root"),
    new TFile("data/Al50/en.2layer.t400.dt200.rebin5.elo2.5e3keV.ehi10e3keV.root"),
    new TFile("data/Al50/en.2layer.t400.dt200.rebin5.elo3e3keV.ehi10e3keV.root"),
    new TFile("data/Al50/en.2layer.t400.dt200.rebin5.elo3.5e3keV.ehi10e3keV.root")
  };

  TH1* hs[NF][2] = {
    { (TH1*)fs[0]->Get("hl_e_reco_bay"), (TH1*)fs[0]->Get("hr_e_reco_bay") },
    { (TH1*)fs[1]->Get("hl_e_reco_bay"), (TH1*)fs[1]->Get("hr_e_reco_bay") },
    { (TH1*)fs[2]->Get("hl_e_reco_bay"), (TH1*)fs[2]->Get("hr_e_reco_bay") },
    { (TH1*)fs[3]->Get("hl_e_reco_bay"), (TH1*)fs[3]->Get("hr_e_reco_bay") },
    { (TH1*)fs[4]->Get("hl_e_reco_bay"), (TH1*)fs[4]->Get("hr_e_reco_bay") },
    { (TH1*)fs[5]->Get("hl_e_reco_bay"), (TH1*)fs[5]->Get("hr_e_reco_bay") },
    { (TH1*)fs[6]->Get("hl_e_reco_bay"), (TH1*)fs[6]->Get("hr_e_reco_bay") },
    { (TH1*)fs[7]->Get("hl_e_reco_bay"), (TH1*)fs[7]->Get("hr_e_reco_bay") },
    { (TH1*)fs[8]->Get("hl_e_reco_bay"), (TH1*)fs[8]->Get("hr_e_reco_bay") },
    { (TH1*)fs[9]->Get("hl_e_reco_bay"), (TH1*)fs[9]->Get("hr_e_reco_bay") }
  };
  hs[0][0]->SetLineColor(1);  hs[0][1]->SetLineColor(1);
  hs[1][0]->SetLineColor(2);  hs[1][1]->SetLineColor(2);
  hs[2][0]->SetLineColor(3);  hs[2][1]->SetLineColor(3);
  hs[3][0]->SetLineColor(4);  hs[3][1]->SetLineColor(4);
  hs[4][0]->SetLineColor(5);  hs[4][1]->SetLineColor(5);
  hs[5][0]->SetLineColor(6);  hs[5][1]->SetLineColor(6);
  hs[6][0]->SetLineColor(7);  hs[6][1]->SetLineColor(7);
  hs[7][0]->SetLineColor(8);  hs[7][1]->SetLineColor(8);
  hs[8][0]->SetLineColor(9);  hs[8][1]->SetLineColor(9);
  hs[9][0]->SetLineColor(11); hs[9][1]->SetLineColor(11);


  double nmu = 137e6;
  double caprate = 0.609;
  TF1* ftime = new TF1("f", "exp([0]^2/(2*[1]^2)-x/[1])*TMath::Erfc(([0]^2-[1]*x)/(sqrt(2)*[0]*[1]))", 0, 10000);
  ftime->SetParameters(52.7, 864.); // (Si timing resolution, Al lifetime)
  double tcuteff = ftime->Integral(400, 1e6)/ftime->Integral(0, 1e6);
  double norm = nmu*caprate*tcuteff;

  double n[NF][2][2];
  for (int i = 0; i < 10; ++i) {
    hs[i][0]->SetTitle("Left;E [keV];p/cap/500 keV");
    hs[i][1]->SetTitle("Right;E [keV];p/cap/500 keV");
    for (int j = 0; j < 2; ++j) {
      hs[i][j]->Scale(1./norm);
      n[i][j][0] = hs[i][j]->Integral(hs[i][j]->FindFixBin(3.5e3),
                                      hs[i][j]->FindFixBin(10e3));
      n[i][j][1] = hs[i][j]->Integral(hs[i][j]->FindFixBin(4e3),
                                      hs[i][j]->FindFixBin(8e3));
    }
  }

  TLegend* leg = new TLegend(0.7, 0.6, 0.9, 0.9);
  leg->SetHeader("Unfolding Range");
  leg->AddEntry(hs[0][0], "2-20 MeV");
  leg->AddEntry(hs[1][0], "2-15 MeV");
  leg->AddEntry(hs[2][0], "2-14 MeV");
  leg->AddEntry(hs[3][0], "2-13 MeV");
  leg->AddEntry(hs[4][0], "2-12 MeV");
  leg->AddEntry(hs[5][0], "2-11 MeV");
  leg->AddEntry(hs[6][0], "2-10 MeV");
  leg->AddEntry(hs[7][0], "2.5-10 MeV");
  leg->AddEntry(hs[8][0], "3-10 MeV");
  leg->AddEntry(hs[9][0], "3.5-10 MeV");


  gStyle->SetOptStat(false);
  TCanvas* c = new TCanvas("c", "c", 1400, 500);
  c->Divide(2);
  c->cd(1);
  hs[0][0]->Draw();
  hs[1][0]->Draw("SAME");
  hs[2][0]->Draw("SAME");
  hs[3][0]->Draw("SAME");
  hs[4][0]->Draw("SAME");
  hs[5][0]->Draw("SAME");
  hs[6][0]->Draw("SAME");
  hs[7][0]->Draw("SAME");
  hs[8][0]->Draw("SAME");
  hs[9][0]->Draw("SAME");
  leg     ->Draw();
  c->cd(2);
  hs[0][1]->Draw();
  hs[1][1]->Draw("SAME");
  hs[2][1]->Draw("SAME");
  hs[3][1]->Draw("SAME");
  hs[4][1]->Draw("SAME");
  hs[5][1]->Draw("SAME");
  hs[6][1]->Draw("SAME");
  hs[7][1]->Draw("SAME");
  hs[8][1]->Draw("SAME");
  hs[9][1]->Draw("SAME");
  leg     ->Draw();
  c->SaveAs("img/compare_unfolding_cutoff.png");

  printf("Unfold range\t3.5-10 MeV\t\t\t4-8 MeV\n");
  printf("\t\tLeft\t\tRight\t\tLeft\t\tRight\n");
  printf("2-20 MeV\t%g\t%g\t%g\t%g\n",   n[0][0][0], n[0][1][0], n[0][0][1], n[0][1][1]);
  printf("2-15 MeV\t%g\t%g\t%g\t%g\n",   n[1][0][0], n[1][1][0], n[1][0][1], n[1][1][1]);
  printf("2-14 MeV\t%g\t%g\t%g\t%g\n",   n[2][0][0], n[2][1][0], n[2][0][1], n[2][1][1]);
  printf("2-13 MeV\t%g\t\t%g\t%g\t%g\n", n[3][0][0], n[3][1][0], n[3][0][1], n[3][1][1]);
  printf("2-12 MeV\t%g\t%g\t%g\t%g\n",   n[4][0][0], n[4][1][0], n[4][0][1], n[4][1][1]);
  printf("2-11 MeV\t%g\t%g\t%g\t%g\n",   n[5][0][0], n[5][1][0], n[5][0][1], n[5][1][1]);
  printf("2-10 MeV\t%g\t%g\t%g\t%g\n",   n[6][0][0], n[6][1][0], n[6][0][1], n[6][1][1]);
  printf("2.5-10 MeV\t%g\t%g\t%g\t%g\n", n[7][0][0], n[7][1][0], n[7][0][1], n[7][1][1]);
  printf("3-10 MeV\t%g\t%g\t%g\t%g\n",   n[8][0][0], n[8][1][0], n[8][0][1], n[8][1][1]);
  printf("3.5-10 MeV\t%g\t%g\t%g\t%g\n", n[9][0][0], n[9][1][0], n[9][0][1], n[9][1][1]);
}
