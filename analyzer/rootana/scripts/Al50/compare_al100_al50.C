{
  TFile* fal50     = new TFile("data/Al50/en.2layer.root");
  TFile* fal100    = new TFile("data/Al100/en.2layer.root");
  TH1* hal50[2]    = { (TH1*)fal50 ->Get("hl_e_reco"),
                       (TH1*)fal50 ->Get("hr_e_reco") };
  TH1* hal100[2]   = { (TH1*)fal100->Get("hl_e_reco"),
                       (TH1*)fal100->Get("hr_e_reco") };

  double nmu_al50  = 122e6;
  double nmu_al100 = 105e6;

  TF1* ftime = new TF1("f", "exp([0]^2/(2*[1]^2)-x/[1])*TMath::Erfc(([0]^2-[1]*x)/(sqrt(2)*[0]*[1]))", 0, 10000);
  ftime->SetParameters(52.7, 864.);
  double tcuteff_al50  = ftime->Integral(400, 1e6)/ftime->Integral(0, 1e6);
  double tcuteff_al100 = 1;

  hal50 [0]->Scale(1./(nmu_al50*tcuteff_al50));
  hal50 [1]->Scale(1./(nmu_al50*tcuteff_al50));
  hal100[0]->Scale(1./(nmu_al100*tcuteff_al100));
  hal100[1]->Scale(1./(nmu_al100*tcuteff_al100));

  hal50 [0]->SetLineColor(4);
  hal50 [1]->SetLineColor(4);
  hal100[0]->SetLineColor(2);
  hal100[1]->SetLineColor(2);
  hal50[0]->SetTitle("Al SiL Unfolded;E [keV];counts/500 keV");
  hal50[1]->SetTitle("Al SiR Unfolded;E [keV];counts/500 keV");


  TLegend* leg_lr = new TLegend(0.7, 0.7, 0.9, 0.9);
  leg_lr->AddEntry(hal50[0],  "Al50",  "l");
  leg_lr->AddEntry(hal100[0], "Al100", "l");
  gStyle->SetOptStat(0);

  TCanvas* clr = new TCanvas("clr", "c", 1400, 500);
  clr->Divide(2);
  clr->cd(1);
  hal50 [0]->DrawClone();
  hal100[0]->DrawClone("SAME");
  leg_lr->Draw();
  clr->cd(2);
  hal50 [1]->DrawClone();
  hal100[1]->DrawClone("SAME");
  leg_lr->Draw();
  clr->SaveAs("img/compare_lr_al50_al100.png");

  hal50 [0]->SetLineColor(4);
  hal50 [1]->SetLineColor(2);
  hal100[0]->SetLineColor(4);
  hal100[1]->SetLineColor(2);
  hal50 [0]->SetTitle("Al50 Unfolded;E [keV];/500 keV");
  hal100[0]->SetTitle("Al100 Unfolded;E [keV];/500 keV");

  TLegend* leg_th = new TLegend(0.7, 0.7, 0.9, 0.9);
  leg_th->AddEntry(hal50 [0], "Left");
  leg_th->AddEntry(hal100[1], "Right");

  TCanvas* cth = new TCanvas("cth", "c", 1400, 500);
  cth->Divide(2);
  cth->cd(1);
  hal50[0]->Draw();
  hal50[1]->Draw("SAME");
  leg_th->Draw();
  cth->cd(2);
  hal100[0]->Draw();
  hal100[1]->Draw("SAME");
  leg_th->Draw();
  cth->SaveAs("img/compare_thickness_al50_al100.png");
}
