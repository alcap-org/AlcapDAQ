{
  TChain *revs = new TChain(), *levs = new TChain();
  revs->Add("data/pselal50.root/PID_R1P");
  revs->Add("data/pselal50.root/PID_R2P");
  revs->Add("data/pselal50.root/PID_R3P");
  revs->Add("data/pselal50.root/PID_R4P");
  levs->Add("data/pselal50.root/PID_L2P");
  levs->Add("data/pselal50.root/PID_L3P");
  levs->Add("data/pselal50.root/PID_L4P");
  levs->Add("data/pselal50.root/PID_L5P");
  levs->Add("data/pselal50.root/PID_L6P");
  levs->Add("data/pselal50.root/PID_L7P");
  levs->Add("data/pselal50.root/PID_L8P");
  levs->Add("data/pselal50.root/PID_L9P");
  levs->Add("data/pselal50.root/PID_L10P");
  levs->Add("data/pselal50.root/PID_L11P");
  levs->Add("data/pselal50.root/PID_L12P");
  levs->Add("data/pselal50.root/PID_L13P");
  levs->Add("data/pselal50.root/PID_L14P");
  levs->Add("data/pselal50.root/PID_L15P");

  revs->Draw("t>>hr(2000,0,20e3)");
  levs->Draw("t>>hl(2000,0,20e3)");

  hr->SetTitle("TME Time SiR Al50;t [ns];Counts [/10ns]");
  hl->SetTitle("TME Time SiL Al50;t [ns];Counts [/10ns]");
  // hr->SetLineColor(632);
  // hl->SetLineColor(600);

  TF1* rfcn = new TF1("rfcn", "[0]+[1]*exp(-x/[2])+[3]*exp(-x/[4])");
  TF1* lfcn = new TF1("lfcn", "[0]+[1]*exp(-x/[2])+[3]*exp(-x/[4])");
  rfcn->SetParNames("BG Flat", "BG Amp", "BG Lifetime", "Al Amp", "Al Lifetime");
  lfcn->SetParNames("BG Flat", "BG Amp", "BG Lifetime", "Al Amp", "Al Lifetime");
  rfcn->SetParameters(2, 150, 170, 410, 890);
  lfcn->SetParameters(2, 150, 170, 410, 890);
  hr->Fit(rfcn);
  hl->Fit(lfcn);
  TRatioPlot* rres = new TRatioPlot(hr);
  TRatioPlot* lres = new TRatioPlot(hl);

  gStyle->SetOptFit(111);

  TCanvas* c = new TCanvas("c", "Proton Lifetime", 1400, 700);
  c->Divide(2);
  c->cd(1)->SetLogy();
  lres->Draw("noconfint");
  c->cd(2)->SetLogy();
  rres->Draw("noconfint");
  lres->GetLowerRefGraph()->SetMinimum(-3);
  lres->GetLowerRefGraph()->SetMaximum(3);
  rres->GetLowerRefGraph()->SetMinimum(-3);
  rres->GetLowerRefGraph()->SetMaximum(3);
  c->Update();
  c->SaveAs("img/proton_lifetime.png");
  lres->GetLowerRefGraph()->GetXaxis()->SetRangeUser(0, 2e3);
  rres->GetLowerRefGraph()->GetXaxis()->SetRangeUser(0, 2e3);
  c->cd(1);
  lres->Draw("noconfint");
  c->cd(2);
  rres->Draw("noconfint");
  c->Update();
  c->SaveAs("img/proton_lifetime_zoom.png");
}
