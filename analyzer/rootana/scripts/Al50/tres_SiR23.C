{
  TFile* ifile = new TFile("out/michel_electrons.root");
  TH2* hedtall  = (TH2*)ifile->Get("hredtall");
  TH2* hedtelec = (TH2*)ifile->Get("hredt0si1si3");
  TH2* hedtprot = (TH2*)ifile->Get("hredtsi1si3");

  TH1* hdtall  = hedtall ->ProjectionX("hdtall");
  TH1* hdtelec = hedtelec->ProjectionX("hdtelec");
  TH1* hdtprot = hedtprot->ProjectionX("hdtprot");

  hdtall ->SetLineColor(kBlack);
  hdtelec->SetLineColor(kRed);
  hdtprot->SetLineColor(kBlue);

  hdtall->SetTitle("Al50 SiR2 Hits");

  hdtall->SetMinimum(0.7);

  TLegend* leg = new TLegend(0.7, 0.7, 0.9, 0.9);
  leg->AddEntry(hdtall,  "All (23)");
  leg->AddEntry(hdtelec, "Michel (#bar{1}23)");
  leg->AddEntry(hdtprot, "Punch. Prot. (123)");

  gStyle->SetOptStat(0);

  TCanvas* c = new TCanvas("c", "c", 700, 500);
  hdtall ->Draw();
  hdtelec->Draw("SAME");
  hdtprot->Draw("SAME");
  leg->Draw();
  c->SetLogy();
  c->SaveAs("img/dt_sir2.png");
  hdtall->GetXaxis()->SetRangeUser(-5e3, 5e3);
  hdtall ->Draw();
  hdtelec->Draw("SAME");
  hdtprot->Draw("SAME");
  leg->Draw();
  c->SaveAs("img/dt_sir2_zoom.png");

  TF1* felec[2] = {
    new TF1("floelec", "[0]+gaus(1)", -250, 0),
    new TF1("fhielec", "[0]+gaus(1)", 0,    250)
  };
  TF1* fprot[2] = {
    new TF1("floprot", "[0]+gaus(1)", -250, 0),
    new TF1("fhiprot", "[0]+gaus(1)", 0,    250)
  };
  felec[0]->SetParameters(1e3, 2e5, 0., 50.);
  felec[1]->SetParameters(1e2, 2e5, 0., 50.);
  fprot[0]->SetParameters(2.,  1e3, 0., 50.);
  fprot[1]->SetParameters(3.,  1e3, 0., 50.);
  felec[0]->SetLineColor(kRed);
  felec[1]->SetLineColor(kRed);
  fprot[0]->SetLineColor(kBlue);
  fprot[1]->SetLineColor(kBlue);
  hdtelec->Fit(felec[0], "RM");
  hdtelec->Fit(felec[1], "RM+");
  hdtprot->Fit(fprot[0], "RM");
  hdtprot->Fit(fprot[1], "RM+");
  hdtall->GetXaxis()->SetRangeUser(-1e3, 1e3);

  hdtall ->Draw();
  hdtelec->Draw("SAME");
  hdtprot->Draw("SAME");
  leg    ->Draw();
  c->SaveAs("img/dt_sir2_gausfit.png");
}
