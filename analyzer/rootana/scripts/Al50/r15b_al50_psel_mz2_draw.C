{
  TFile* f = new TFile("data/Al50/psel.p102.2layer.root", "", "goff");
  // TFile* f = new TFile("data/Al100/pselal100.root", "", "goff");
  PID_RP->Draw("de:e>>hrp(1000,0,18e3,1000,3e3)", "abs(dt) < 200", "goff");
  PID_RD->Draw("de:e>>hrd(1000,0,18e3,1000,3e3)", "abs(dt) < 200", "goff");
  PID_RT->Draw("de:e>>hrt(1000,0,18e3,1000,3e3)", "abs(dt) < 200", "goff");
  PID_RA->Draw("de:e>>hra(1000,0,18e3,1000,3e3)", "abs(dt) < 200", "goff");
  PID_RU->Draw("de:e>>hru(1000,0,18e3,1000,3e3)", "abs(dt) < 200", "goff");
  PID_LP->Draw("de:e>>hlp(1000,0,18e3,1000,3e3)", "abs(dt) < 200", "goff");
  PID_LD->Draw("de:e>>hld(1000,0,18e3,1000,3e3)", "abs(dt) < 200", "goff");
  PID_LT->Draw("de:e>>hlt(1000,0,18e3,1000,3e3)", "abs(dt) < 200", "goff");
  PID_LA->Draw("de:e>>hla(1000,0,18e3,1000,3e3)", "abs(dt) < 200", "goff");
  PID_LU->Draw("de:e>>hlu(1000,0,18e3,1000,3e3)", "abs(dt) < 200", "goff");
  hrp->SetMarkerColor(3);
  hrd->SetMarkerColor(4);
  hrt->SetMarkerColor(6);
  hra->SetMarkerColor(5);
  hru->SetMarkerColor(2);
  hlp->SetMarkerColor(3);
  hld->SetMarkerColor(4);
  hlt->SetMarkerColor(6);
  hla->SetMarkerColor(5);
  hlu->SetMarkerColor(2);

  TCanvas* c = new TCanvas("c", "c", 1400, 500);
  c->Divide(2);
  c->cd(1);
  hlp->Draw();
  hld->Draw("SAME");
  hlt->Draw("SAME");
  hla->Draw("SAME");
  hlu->Draw("SAME");
  c->cd(2);
  hrp->Draw();
  hrd->Draw("SAME");
  hrt->Draw("SAME");
  hra->Draw("SAME");
  hru->Draw("SAME");
}
