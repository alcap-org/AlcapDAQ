{
  TFile* f = new TFile("data/Al50/nmu.pp10us.root");
  TH1* hprompt  = (TH1*)f->Get("hgehi_e_prompt");
  TH1* hdelayed = (TH1*)f->Get("hgehi_e_delayed");
  TH1* hoot     = (TH1*)f->Get("hgehi_e_oot");
  TH1* hall     = (TH1*)f->Get("hgehi_e_all");

  hprompt ->SetLineColor(3);
  hdelayed->SetLineColor(4);
  hoot    ->SetLineColor(2);
  hall    ->SetLineColor(1);
  hall    ->SetTitle("Ge Spectrum;E [keV];Counts");
  hall    ->SetMinimum(10);

  TLegend* leg = new TLegend(0.6, 0.6, 0.9, 0.9);
  leg->AddEntry(hall,     "Total");
  leg->AddEntry(hprompt,  "|t| < 200ns");
  leg->AddEntry(hdelayed, "200ns < t < 4#mus");
  leg->AddEntry(hoot,     "4#mus < t");

  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas;
  hall    ->Draw();
  hprompt ->Draw("SAME");
  hdelayed->Draw("SAME");
  hoot    ->Draw("SAME");
  leg     ->Draw();
  c->SetLogy();
  c->SaveAs("img/ge_spectrum.png");
  hall->GetXaxis()->SetRangeUser(335, 355);
  c->SaveAs("img/ge_spectrum_347keV.png");
}
