{
  TFile* f = new TFile("out/michel_electrons.root");
  TH2* het[2][2] = {
    { (TH2*)f->Get("hletall"), (TH2*)f->Get("hlet0si1") },
    { (TH2*)f->Get("hretall"), (TH2*)f->Get("hret0si1si3") }
  };
  TH1* hltall[5] = {
    het[0][0]->ProjectionX("hltall_0"),
    het[0][0]->ProjectionX("hltall_1", het[0][0]->GetYaxis()->FindFixBin(0.),    het[0][0]->GetYaxis()->FindFixBin(500.)),
    het[0][0]->ProjectionX("hltall_2", het[0][0]->GetYaxis()->FindFixBin(500.),  het[0][0]->GetYaxis()->FindFixBin(1000.)),
    het[0][0]->ProjectionX("hltall_3", het[0][0]->GetYaxis()->FindFixBin(1000.), het[0][0]->GetYaxis()->FindFixBin(2000.)),
    het[0][0]->ProjectionX("hltall_4", het[0][0]->GetYaxis()->FindFixBin(2000.), het[0][0]->GetYaxis()->FindFixBin(5000.))
  };
  TH1* hrtall[5] = {
    het[1][0]->ProjectionX("hrtall_0"),
    het[1][0]->ProjectionX("hrtall_1", het[1][0]->GetYaxis()->FindFixBin(0.),    het[1][0]->GetYaxis()->FindFixBin(500.)),
    het[1][0]->ProjectionX("hrtall_2", het[1][0]->GetYaxis()->FindFixBin(500.),  het[1][0]->GetYaxis()->FindFixBin(1000.)),
    het[1][0]->ProjectionX("hrtall_3", het[1][0]->GetYaxis()->FindFixBin(1000.), het[1][0]->GetYaxis()->FindFixBin(2000.)),
    het[1][0]->ProjectionX("hrtall_4", het[1][0]->GetYaxis()->FindFixBin(2000.), het[1][0]->GetYaxis()->FindFixBin(5000.))
  };
  TH1* hltmichel[5] = {
    het[0][1]->ProjectionX("hltmichel_0"),
    het[0][1]->ProjectionX("hltmichel_1", het[0][1]->GetYaxis()->FindFixBin(0.),    het[0][1]->GetYaxis()->FindFixBin(500.)),
    het[0][1]->ProjectionX("hltmichel_2", het[0][1]->GetYaxis()->FindFixBin(500.),  het[0][1]->GetYaxis()->FindFixBin(1000.)),
    het[0][1]->ProjectionX("hltmichel_3", het[0][1]->GetYaxis()->FindFixBin(1000.), het[0][1]->GetYaxis()->FindFixBin(2000.)),
    het[0][1]->ProjectionX("hltmichel_4", het[0][1]->GetYaxis()->FindFixBin(2000.), het[0][1]->GetYaxis()->FindFixBin(5000.))
  };
  TH1* hrtmichel[5] = {
    het[1][1]->ProjectionX("hrtmichel_0"),
    het[1][1]->ProjectionX("hrtmichel_1", het[1][1]->GetYaxis()->FindFixBin(0.),    het[1][1]->GetYaxis()->FindFixBin(500.)),
    het[1][1]->ProjectionX("hrtmichel_2", het[1][1]->GetYaxis()->FindFixBin(500.),  het[1][1]->GetYaxis()->FindFixBin(1000.)),
    het[1][1]->ProjectionX("hrtmichel_3", het[1][1]->GetYaxis()->FindFixBin(1000.), het[1][1]->GetYaxis()->FindFixBin(2000.)),
    het[1][1]->ProjectionX("hrtmichel_4", het[1][1]->GetYaxis()->FindFixBin(2000.), het[1][1]->GetYaxis()->FindFixBin(5000.))
  };
  TF1* fltall[5] = {
    new TF1("fltall_0",    "[0]+[1]*TMath::Gaus(x,[2],[3])", -200., 0.),
    new TF1("fltall_1",    "[0]+[1]*TMath::Gaus(x,[2],[3])", -200., 0.),
    new TF1("fltall_2",    "[0]+[1]*TMath::Gaus(x,[2],[3])", -200., 0.),
    new TF1("fltall_3",    "[0]+[1]*TMath::Gaus(x,[2],[3])", -200., 0.),
    new TF1("fltall_4",    "[0]+[1]*TMath::Gaus(x,[2],[3])", -200., 0.)
  };
  TF1* frtall[5] = {
    new TF1("frtall_0",    "[0]+[1]*TMath::Gaus(x,[2],[3])", -200., 0.),
    new TF1("frtall_1",    "[0]+[1]*TMath::Gaus(x,[2],[3])", -200., 0.),
    new TF1("frtall_2",    "[0]+[1]*TMath::Gaus(x,[2],[3])", -200., 0.),
    new TF1("frtall_3",    "[0]+[1]*TMath::Gaus(x,[2],[3])", -200., 0.),
    new TF1("frtall_4",    "[0]+[1]*TMath::Gaus(x,[2],[3])", -200., 0.)
  };
  TF1* fltmichel[5] = {
    new TF1("fltmichel_0", "[0]+[1]*TMath::Gaus(x,[2],[3])", -200., 0.),
    new TF1("fltmichel_1", "[0]+[1]*TMath::Gaus(x,[2],[3])", -200., 0.),
    new TF1("fltmichel_2", "[0]+[1]*TMath::Gaus(x,[2],[3])", -200., 0.),
    new TF1("fltmichel_3", "[0]+[1]*TMath::Gaus(x,[2],[3])", -200., 0.),
    new TF1("fltmichel_4", "[0]+[1]*TMath::Gaus(x,[2],[3])", -200., 0.)
  };
  TF1* frtmichel[5] = {
    new TF1("frtmichel_0", "[0]+[1]*TMath::Gaus(x,[2],[3])", -200., 0.),
    new TF1("frtmichel_1", "[0]+[1]*TMath::Gaus(x,[2],[3])", -200., 0.),
    new TF1("frtmichel_2", "[0]+[1]*TMath::Gaus(x,[2],[3])", -200., 0.),
    new TF1("frtmichel_3", "[0]+[1]*TMath::Gaus(x,[2],[3])", -200., 0.),
    new TF1("frtmichel_4", "[0]+[1]*TMath::Gaus(x,[2],[3])", -200., 0.)
  };
  hltall   [0]->SetTitle("All SiL3 Hits (Integral=1);Time [ns];Norm. Counts/10ns");
  hrtall   [0]->SetTitle("All SiR2 Hits (Integral=1);Time [ns];Norm. Counts/10ns");
  hltmichel[0]->SetTitle("SiL3 Hits !SiL1 (Integral=1);Time [ns];Norm. Counts/10ns");
  hrtmichel[0]->SetTitle("SiR2 Hits !SiR1+SiR3 (Integral=1);Time [ns];Norm. Counts/10ns");

  TFitResultPtr res_lall[5], res_rall[5], res_lmichel[5], res_rmichel[5];
  Color_t cols[5] = { kBlack, kGreen, kBlue, kYellow, kMagenta };
  for (int i = 0; i < 5; ++i) {
    hltall   [i]->SetLineColor(cols[i]);
    hrtall   [i]->SetLineColor(cols[i]);
    hltmichel[i]->SetLineColor(cols[i]);
    hrtmichel[i]->SetLineColor(cols[i]);
    hltall   [i]->GetXaxis()->SetRangeUser(-500., 500.);
    hrtall   [i]->GetXaxis()->SetRangeUser(-500., 500.);
    hltmichel[i]->GetXaxis()->SetRangeUser(-500., 500.);
    hrtmichel[i]->GetXaxis()->SetRangeUser(-500., 500.);
    hltall   [i]->Scale(1./hltall   [i]->Integral());
    hrtall   [i]->Scale(1./hrtall   [i]->Integral());
    hltmichel[i]->Scale(1./hltmichel[i]->Integral());
    hrtmichel[i]->Scale(1./hrtmichel[i]->Integral());
    fltall   [i]->SetParameters(0.0002, 0.005, 0, 50.);
    frtall   [i]->SetParameters(0.0002, 0.005, 0, 50.);
    fltmichel[i]->SetParameters(0.0002, 0.005, 0, 50.);
    frtmichel[i]->SetParameters(0.0002, 0.005, 0, 50.);
    fltall   [i]->SetLineColor(cols[i]);
    frtall   [i]->SetLineColor(cols[i]);
    fltmichel[i]->SetLineColor(cols[i]);
    frtmichel[i]->SetLineColor(cols[i]);
    res_lall   [i] = hltall   [i]->Fit(fltall   [i], "EMRS");
    res_rall   [i] = hrtall   [i]->Fit(frtall   [i], "EMRS");
    res_lmichel[i] = hltmichel[i]->Fit(fltmichel[i], "EMRS");
    res_rmichel[i] = hrtmichel[i]->Fit(frtmichel[i], "EMRS");
    hltall   [i]->SetMaximum(0.056);
    hrtall   [i]->SetMaximum(0.056);
    hltmichel[i]->SetMaximum(0.056);
    hrtmichel[i]->SetMaximum(0.056);
  }

  TLegend* leg = new TLegend(0.6, 0.6, 0.9, 0.9);
  leg->AddEntry(hltall[0], "0<E<8000keV");
  leg->AddEntry(hltall[1], "0<E<500keV");
  leg->AddEntry(hltall[2], "500<E<1000keV");
  leg->AddEntry(hltall[3], "1000<E<2000keV");
  leg->AddEntry(hltall[4], "2000<E<5000keV");

  gStyle->SetOptStat(0);

  TCanvas* c = new TCanvas("c", "c", 1400, 1000);
  c->Divide(2, 2);
  c->cd(1);
  hltall   [0]->Draw("SAME");
  hltall   [1]->Draw("SAME");
  hltall   [2]->Draw("SAME");
  hltall   [3]->Draw("SAME");
  hltall   [4]->Draw("SAME");
  leg         ->Draw("SAME");
  c->cd(2);
  hrtall   [0]->Draw("SAME");
  hrtall   [1]->Draw("SAME");
  hrtall   [2]->Draw("SAME");
  hrtall   [3]->Draw("SAME");
  hrtall   [4]->Draw("SAME");
  leg         ->Draw("SAME");
  c->cd(3);
  hltmichel[0]->Draw("SAME");
  hltmichel[1]->Draw("SAME");
  hltmichel[2]->Draw("SAME");
  hltmichel[3]->Draw("SAME");
  hltmichel[4]->Draw("SAME");
  leg         ->Draw("SAME");
  c->cd(4);
  hrtmichel[0]->Draw("SAME");
  hrtmichel[1]->Draw("SAME");
  hrtmichel[2]->Draw("SAME");
  hrtmichel[3]->Draw("SAME");
  hrtmichel[4]->Draw("SAME");
  leg         ->Draw("SAME");
  c->SaveAs("img/tres_eslice.png");

  std::printf("SiL All:    %g(%g)\t%g(%g)\t%g(%g)\t%g(%g)\t%g(%g)\n",
              res_lall[0]->Parameter(3),    res_lall[0]->Error(3),
              res_lall[1]->Parameter(3),    res_lall[1]->Error(3),
              res_lall[2]->Parameter(3),    res_lall[2]->Error(3),
              res_lall[3]->Parameter(3),    res_lall[3]->Error(3),
              res_lall[4]->Parameter(3),    res_lall[4]->Error(3) );
  std::printf("SiR All:    %g(%g)\t%g(%g)\t%g(%g)\t%g(%g)\t%g(%g)\n",
              res_rall[0]->Parameter(3),    res_rall[0]->Error(3),
              res_rall[1]->Parameter(3),    res_rall[1]->Error(3),
              res_rall[2]->Parameter(3),    res_rall[2]->Error(3),
              res_rall[3]->Parameter(3),    res_rall[3]->Error(3),
              res_rall[4]->Parameter(3),    res_rall[4]->Error(3));
  std::printf("SiL Michel: %g(%g)\t%g(%g)\t%g(%g)\t%g(%g)\t%g(%g)\n",
              res_lmichel[0]->Parameter(3), res_lmichel[0]->Error(3),
              res_lmichel[1]->Parameter(3), res_lmichel[1]->Error(3),
              res_lmichel[2]->Parameter(3), res_lmichel[2]->Error(3),
              res_lmichel[3]->Parameter(3), res_lmichel[3]->Error(3),
              res_lmichel[4]->Parameter(3), res_lmichel[4]->Error(3));
  std::printf("SiR Michel: %g(%g)\t%g(%g)\t%g(%g)\t%g(%g)\t%g(%g)\n",
              res_rmichel[0]->Parameter(3), res_rmichel[0]->Error(3),
              res_rmichel[1]->Parameter(3), res_rmichel[1]->Error(3),
              res_rmichel[2]->Parameter(3), res_rmichel[2]->Error(3),
              res_rmichel[3]->Parameter(3), res_rmichel[3]->Error(3),
              res_rmichel[4]->Parameter(3), res_rmichel[4]->Error(3));

  std::printf("SiL All:    %g\t%g\t%g\t%g\t%g\n",
              res_lall[0]->Chi2()/res_lall[0]->Ndf(),
              res_lall[1]->Chi2()/res_lall[1]->Ndf(),
              res_lall[2]->Chi2()/res_lall[2]->Ndf(),
              res_lall[3]->Chi2()/res_lall[3]->Ndf(),
              res_lall[4]->Chi2()/res_lall[4]->Ndf() );
  std::printf("SiR All:    %g\t%g\t%g\t%g\t%g\n",
              res_rall[0]->Chi2()/res_rall[0]->Ndf(),
              res_rall[1]->Chi2()/res_rall[1]->Ndf(),
              res_rall[2]->Chi2()/res_rall[2]->Ndf(),
              res_rall[3]->Chi2()/res_rall[3]->Ndf(),
              res_rall[4]->Chi2()/res_rall[4]->Ndf());
  std::printf("SiL Michel: %g\t%g\t%g\t%g\t%g\n",
              res_lmichel[0]->Chi2()/res_lmichel[0]->Ndf(),
              res_lmichel[1]->Chi2()/res_lmichel[1]->Ndf(),
              res_lmichel[2]->Chi2()/res_lmichel[2]->Ndf(),
              res_lmichel[3]->Chi2()/res_lmichel[3]->Ndf(),
              res_lmichel[4]->Chi2()/res_lmichel[4]->Ndf());
  std::printf("SiR Michel: %g\t%g\t%g\t%g\t%g\n",
              res_rmichel[0]->Chi2()/res_rmichel[0]->Ndf(),
              res_rmichel[1]->Chi2()/res_rmichel[1]->Ndf(),
              res_rmichel[2]->Chi2()/res_rmichel[2]->Ndf(),
              res_rmichel[3]->Chi2()/res_rmichel[3]->Ndf(),
              res_rmichel[4]->Chi2()/res_rmichel[4]->Ndf());
}
