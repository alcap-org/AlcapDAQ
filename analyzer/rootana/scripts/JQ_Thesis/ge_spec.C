{
  TFile* f = new TFile("data/Al50/nmu.pp10us.root");
  TH1* he[4] = { nullptr,                         (TH1*)f->Get("hgehi_e_prompt"),
                 (TH1*)f->Get("hgehi_e_delayed"), (TH1*)f->Get("hgehi_e_oot") };
  he[0] = (TH1*)he[1]->Clone("hgehi_e_all");
  he[0]->Add(he[2]);
  he[0]->Add(he[3]);
  he[0]->SetLineColor(kBlack);
  he[1]->SetLineColor(kGreen);
  he[2]->SetLineColor(kBlue);
  he[3]->SetLineColor(kRed);
  he[0]->SetTitle("Ge Spectrum;E [keV];Counts [/0.34keV]");

  TLegend* lall = new TLegend(0.6, 0.6, 0.9, 0.9);
  lall->AddEntry(he[0], "Total");
  lall->AddEntry(he[1], "|t|<200ns");
  lall->AddEntry(he[2], "200ns<t<4#mus");
  lall->AddEntry(he[3], "4#mus<t");
  TLegend* lzoom = new TLegend(0.1, 0.6, 0.4, 0.9);
  lzoom->AddEntry(he[0], "Total");
  lzoom->AddEntry(he[1], "|t|<200ns");
  lzoom->AddEntry(he[2], "200ns<t<4#mus");
  lzoom->AddEntry(he[3], "4#mus<t");

  gStyle->SetOptStat(0);

  he[0]->GetXaxis()->SetRangeUser(0, 2e3);
  he[0]->SetMaximum(2e4);
  he[0]->SetMinimum(10);
  TCanvas* call = new TCanvas("call", "All", 700, 500);
  he[0]->Draw("SAME");
  he[1]->Draw("SAME");
  he[2]->Draw("SAME");
  he[3]->Draw("SAME");
  lall ->Draw();
  call->SetLogy();
  call->SaveAs("img/ge_spectrum.png");

  he[0]->GetXaxis()->SetRangeUser(335., 355.);
  he[0]->SetMaximum(2e4);
  he[0]->SetMinimum(200);
  TCanvas* czoom = new TCanvas("czoom", "Zoom", 700, 500);
  he[0]->Draw("SAME");
  he[1]->Draw("SAME");
  he[2]->Draw("SAME");
  he[3]->Draw("SAME");
  lzoom->Draw();
  czoom->SetLogy();
  czoom->SaveAs("img/ge_spectrum_347keV.png");
}
