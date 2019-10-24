{
  TFile* fold = new TFile("data/Al50/old_thickness/en.p.t400ns.dt200ns.rebin5.pp10us.elo2e3keV.ehi17e3keV.root");
  TFile* fnew = new TFile("data/Al50/en.p.t400ns.dt200ns.rebin5.pp10us.elo2e3keV.ehi17e3keV.root");

  TH1* h[2][2] = {
    { (TH1*)fold->Get("hl_e_reco_bay"), (TH1*)fold->Get("hr_e_reco_bay") },
    { (TH1*)fnew->Get("hl_e_reco_bay"), (TH1*)fnew->Get("hr_e_reco_bay") }
  };

  h[0][0]->SetLineColor(kBlue);
  h[0][1]->SetLineColor(kRed);
  h[1][0]->SetLineColor(kBlue);
  h[1][1]->SetLineColor(kRed);
  h[0][0]->SetLineStyle(2);
  h[0][1]->SetLineStyle(2);
  h[1][0]->SetLineStyle(1);
  h[1][1]->SetLineStyle(1);
  h[0][0]->SetTitle("Compare proton counts after changing SiL3 thickness;E [keV];Al50 Counts [/500keV]");
  h[0][1]->SetTitle("Compare proton counts after changing SiR1 thickness;E [keV];Al50 Counts [/500keV]");

  TLegend* leg[2] = { new TLegend(0.7, 0.7, 0.9, 0.9), new TLegend(0.7, 0.7, 0.9, 0.9) };
  leg[0]->AddEntry(h[0][0], "Pre-fix");
  leg[0]->AddEntry(h[1][0], "Pos-fix");
  leg[1]->AddEntry(h[0][1], "Pre-fix");
  leg[1]->AddEntry(h[1][1], "Pos-fix");

  gStyle->SetOptStat(0);

  TCanvas* c = new TCanvas("c", "c", 1400, 500);
  c->Divide(2);
  c->cd(1);
  h[0][0]->Draw("SAME");
  h[1][0]->Draw("SAME");
  leg[0]->Draw();
  c->cd(2);
  h[0][1]->Draw("SAME");
  h[1][1]->Draw("SAME");
  leg[1]->Draw();
  c->SaveAs("img/compare_new_Si_thicknesses.png");

}
