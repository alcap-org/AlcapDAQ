{
  TFile* ifile = new TFile("data/Al50/psel.3layer.pidpow2.root");
  TH1* hl  = new TH1D("hl",  "hl",  200, -1e3, 1e3);
  TH1* hlp = new TH1D("hlp", "hlp", 200, -1e3, 1e3);
  TH1* hld = new TH1D("hld", "hld", 200, -1e3, 1e3);
  TH1* hlt = new TH1D("hlt", "hlt", 200, -1e3, 1e3);
  TH1* hla = new TH1D("hla", "hla", 200, -1e3, 1e3);
  TH1* hlu = new TH1D("hlu", "hlu", 200, -1e3, 1e3);
  PID_LP->Draw("dt>>+hlp", "2e3<e && 0<t && t<10e3 && e3==0", "goff");
  PID_LD->Draw("dt>>+hld", "2e3<e && 0<t && t<10e3 && e3==0", "goff");
  PID_LT->Draw("dt>>+hlt", "2e3<e && 0<t && t<10e3 && e3==0", "goff");
  PID_LA->Draw("dt>>+hla", "2e3<e && 0<t && t<10e3 && e3==0", "goff");
  PID_LU->Draw("dt>>+hlu", "2e3<e && 0<t && t<10e3 && e3==0", "goff");
  hl->Add(hlp);
  hl->Add(hld);
  hl->Add(hlt);
  hl->Add(hla);
  hl->Add(hlu);
  TH1* hr  = new TH1D("hr",  "hr",  200, -1e3, 1e3);
  TH1* hrp = new TH1D("hrp", "hrp", 200, -1e3, 1e3);
  TH1* hrd = new TH1D("hrd", "hrd", 200, -1e3, 1e3);
  TH1* hrt = new TH1D("hrt", "hrt", 200, -1e3, 1e3);
  TH1* hra = new TH1D("hra", "hra", 200, -1e3, 1e3);
  TH1* hru = new TH1D("hru", "hru", 200, -1e3, 1e3);
  PID_RP->Draw("dt>>+hrp", "2e3<e && 0<t && t<10e3 && e3==0", "goff");
  PID_RD->Draw("dt>>+hrd", "2e3<e && 0<t && t<10e3 && e3==0", "goff");
  PID_RT->Draw("dt>>+hrt", "2e3<e && 0<t && t<10e3 && e3==0", "goff");
  PID_RA->Draw("dt>>+hra", "2e3<e && 0<t && t<10e3 && e3==0", "goff");
  PID_RU->Draw("dt>>+hru", "2e3<e && 0<t && t<10e3 && e3==0", "goff");
  hr->Add(hrp);
  hr->Add(hrd);
  hr->Add(hrt);
  hr->Add(hra);
  hr->Add(hru);

  hl ->SetLineColor(4); hr ->SetLineColor(2);
  hlp->SetLineColor(2); hrp->SetLineColor(2);
  hld->SetLineColor(3); hrd->SetLineColor(3);
  hlt->SetLineColor(4); hrt->SetLineColor(4);
  hla->SetLineColor(5); hra->SetLineColor(5);
  hlu->SetLineColor(6); hru->SetLineColor(6);

  TCanvas* c0 = new TCanvas("c0", "c0", 700, 500);
  hr->Draw("SAME");
  hl->Draw("SAME");
  c0->SetLogy();
  c0->SaveAs("img/dt_allparticles.png");

  TCanvas* c = new TCanvas("c", "c", 1400, 500);
  c->Divide(2);
  c->cd(1)->SetLogy();
  hlp->Draw("SAME");
  hld->Draw("SAME");
  hlt->Draw("SAME");
  hla->Draw("SAME");
  hlu->Draw("SAME");
  c->cd(2)->SetLogy();
  hrp->Draw("SAME");
  hrd->Draw("SAME");
  hrt->Draw("SAME");
  hra->Draw("SAME");
  hru->Draw("SAME");

  TCanvas* cdeve = new TCanvas("cdeve", "cdeve", 1400, 500);
  cdeve->Divide(2);
  cdeve->cd(1);
  PID_LU->Draw("de:e", "0<t && t<10e3 && e3==0");
  cdeve->cd(2);
  PID_RU->Draw("de:e", "0<t && t<10e3 && e3==0");

}
