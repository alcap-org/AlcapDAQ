{
  gROOT->ProcessLine(".L scripts/Al50/util.h+g");

  TFile* fjq = new TFile("data/Al50/en.p.t400ns.dt200ns.rebin5.pp10us.elo2e3keV.ehi17e3keV.root");
  TFile* fmw = new TFile("data/Al50/from_Mark/unfolded.al50.root");

  TH1* hjq[3] = { (TH1*)fjq->Get("hl_e_reco_bay"),
                  (TH1*)fjq->Get("hr_e_reco_bay"),
                  nullptr };
  TH1* hmw[3] = { (TH1*)fmw->Get("hRecoLeft"),
                  (TH1*)fmw->Get("hRecoRight"),
                  nullptr };
  hmw[2] = (TH1*)hmw[0]->Clone("hRecoAvg");
  hjq[2] = (TH1*)hjq[0]->Clone("ha_e_reco_bay");
  hmw[2]->Add(hmw[1]);
  hjq[2]->Add(hjq[1]);
  hmw[2]->Scale(0.5);
  hjq[2]->Scale(0.5);

  double norm_mw = 1/0.65/0.61/131e6;
  double norm_jq = 1/0.63/0.61/160e6;

  hmw[0]->Scale(norm_mw);
  hmw[1]->Scale(norm_mw);
  hmw[2]->Scale(norm_mw);
  hjq[0]->Scale(norm_jq);
  hjq[1]->Scale(norm_jq);
  hjq[2]->Scale(norm_jq);

  hjq[0]->SetLineColor(kBlue);
  hjq[0]->SetLineStyle(2);
  hjq[1]->SetLineColor(kRed);
  hjq[1]->SetLineStyle(2);
  hjq[2]->SetLineColor(kBlack);
  hjq[2]->SetLineStyle(2);
  hmw[0]->SetLineColor(kBlue);
  hmw[1]->SetLineColor(kRed);
  hmw[2]->SetLineColor(kBlack);

  hmw[0]->SetTitle("Proton Spectrum;E [keV];Rate [p/#mucap/500keV]");
  hmw[1]->SetTitle("Proton Spectrum;E [keV];Rate [p/#mucap/500keV]");
  hmw[2]->SetTitle("L/R Average Proton Spectrum;E [keV];Rate [p/#mucap/500keV]");
  hjq[0]->SetTitle("Proton Spectrum;E [keV];Rate [p/#mucap/500keV]");
  hjq[1]->SetTitle("Proton Spectrum;E [keV];Rate [p/#mucap/500keV]");
  hjq[2]->SetTitle("L/R Average Proton Spectrum;E [keV];Rate [p/#mucap/500keV]");

  TLegend* leg1 = new TLegend(0.7, 0.7, 0.9, 0.9);
  leg1->AddEntry(hmw[0], "SiL Mark");
  leg1->AddEntry(hmw[1], "SiR Mark");
  leg1->AddEntry(hjq[0], "SiL John");
  leg1->AddEntry(hjq[1], "SiR John");
  TLegend* leg2 = new TLegend(0.7, 0.7, 0.9, 0.9);
  leg2->AddEntry(hmw[2], "Mark");
  leg2->AddEntry(hjq[2], "John");


  gStyle->SetOptStat(0);
  TCanvas* c1 = new TCanvas("c1", "c1", 700, 500);
  hjq[1]->Draw("SAME");
  hmw[0]->Draw("SAME");
  hmw[1]->Draw("SAME");
  hjq[0]->Draw("SAME");
  leg1  ->Draw();
  c1->SaveAs("img/compare_mwjq_lr.png");
  TCanvas* c2 = new TCanvas("c2", "c2", 700, 500);
  hjq[2]->Draw("SAME");
  hmw[2]->Draw("SAME");
  leg2  ->Draw();
  c2->SaveAs("img/compare_mwjq_avg.png");

}
