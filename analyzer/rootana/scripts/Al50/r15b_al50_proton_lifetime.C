{
  double LIM[2] = { 0, 5e3 };

  TH1::SetDefaultSumw2();
  TFile* ifile = new TFile("data/Al50/psel.3layer.pidpow2.root");
  TTree* tr[2]    = {
    (TTree*)ifile->Get("PID_LP"),
    (TTree*)ifile->Get("PID_RP")
  };

  // tr[0]->Draw("t>>Left(450,500,5e3)", "abs(dt)<200 && e3==0", "goff");
  // tr[1]->Draw("t>>Right(450,500,5e3)", "abs(dt)<200 && e3==0", "goff");
  tr[0]->Draw("t>>Left(500,0,5e3)", "abs(dt)<200 && e3==0", "goff");
  tr[1]->Draw("t>>Right(500,0,5e3)", "abs(dt)<200 && e3==0", "goff");
  TH1* h[2] = { (TH1*)gDirectory->Get("Left"), (TH1*)gDirectory->Get("Right") };
  h[0]->SetTitle("TME Time SiL Al50;t [ns];Counts [/10ns]");
  h[1]->SetTitle("TME Time SiR Al50;t [ns];Counts [/10ns]");

  TF1* fit[2] = {
    new TF1("lfcn", "[0]+[1]*exp(-x/[2])+[3]*exp(-x/[4])"),
    new TF1("rfcn", "[0]+[1]*exp(-x/[2])+[3]*exp(-x/[4])")
    // new TF1("lfcn", "[0]+[1]*exp(-x/[2])"),
    // new TF1("rfcn", "[0]+[1]*exp(-x/[2])")
  };
  fit[0]->SetParNames("BG Flat", "BG Amp", "BG Lifetime", "Al Amp", "Al Lifetime");
  fit[1]->SetParNames("BG Flat", "BG Amp", "BG Lifetime", "Al Amp", "Al Lifetime");
  // fit[0]->SetParNames("BG Flat", "Al Amp", "Al Lifetime");
  // fit[1]->SetParNames("BG Flat", "Al Amp", "Al Lifetime");
  fit[0]->SetParameters(1, 100, 200, 300, 870);
  fit[1]->SetParameters(1, 100, 200, 300, 870);
  // fit[0]->SetParameters(3, 700, 860);
  // fit[1]->SetParameters(3, 700, 860);
  // fit[0]->SetParLimits(2, 40, 200);
  // fit[1]->SetParLimits(2, 40, 200);
  // h[0]->GetXaxis()->SetRangeUser(LIM[0], LIM[1]);
  // h[1]->GetXaxis()->SetRangeUser(LIM[0], LIM[1]);
  h[0]->Fit(fit[0], "LEMS");
  h[1]->Fit(fit[1], "LEMS");


  TRatioPlot* res[2] = { new TRatioPlot(h[0]), new TRatioPlot(h[1]) };
  // gStyle->SetOptStat(11);
  gStyle->SetOptFit(111);
  // gStyle->SetStatH(0.1);

  TCanvas* c = new TCanvas("c", "Proton Lifetime", 1400, 700);
  c->Divide(2);
  c->cd(1)->SetLogy();
  res[0]->Draw("noconfint");
  c->cd(2)->SetLogy();
  res[1]->Draw("noconfint");
  res[0]->GetLowerRefGraph()->SetMinimum(-3);
  res[0]->GetLowerRefGraph()->SetMaximum(3);
  res[1]->GetLowerRefGraph()->SetMinimum(-3);
  res[1]->GetLowerRefGraph()->SetMaximum(3);
  res[0]->GetLowerRefGraph()->GetXaxis()->SetRangeUser(0, 1e3);
  res[1]->GetLowerRefGraph()->GetXaxis()->SetRangeUser(0, 1e3);
  c->Update();
  c->SaveAs("img/proton_lifetime.png");
  // c->cd(1);
  // res[0]->DrawCopy("noconfint");
  // c->cd(2);
  // res[1]->DrawCopy("noconfint");
  // c->Update();
  // c->SaveAs("img/proton_lifetime_zoom.png");

}
