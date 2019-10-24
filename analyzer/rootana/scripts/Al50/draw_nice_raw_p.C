{
  TFile* f = new TFile("data/Al50/psel.3layer.root");
  TTree* tr[2] = { (TTree*)f->Get("PID_LP"), (TTree*)f->Get("PID_RP") };
  TH1* h[2] = { new TH1D("hl", "Measured Proton Energy;E [keV];cts/500 keV",
                         30, 0, 15e3),
                new TH1D("hr", "Measured Proton Energy;E [keV];cts/500 keV",
                         30, 0, 15e3) };
  tr[0]->Draw("e>>+hl", "abs(dt)<200 && t>400", "goff");
  tr[1]->Draw("e>>+hr", "abs(dt)<200 && t>400", "goff");

  h[0]->SetLineColor(4);
  h[1]->SetLineColor(2);

  TLegend* leg = new TLegend(0.7, 0.7, 0.9, 0.9);
  leg->AddEntry(h[0], "Left");
  leg->AddEntry(h[1], "Right");

  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas;
  h[1]->Draw();
  h[0]->Draw("SAME");
  leg->Draw();
  c->SaveAs("raw_p.png");
}
