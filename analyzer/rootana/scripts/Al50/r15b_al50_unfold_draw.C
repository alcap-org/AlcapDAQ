{
  const char* IFNAME_P    = "~/data/R15b/al50unfoldp.root";
  const char* IFNAME_D    = "~/data/R15b/al50unfoldd.root";
  const char* IFNAME_T    = "~/data/R15b/al50unfoldt.root";
  const char* IFNAME_A    = "~/data/R15b/al50unfolda.root";

  TFile* fp = new TFile(IFNAME_P);
  TFile* fd = new TFile(IFNAME_D);
  TFile* ft = new TFile(IFNAME_T);
  TFile* fa = new TFile(IFNAME_A);
  TH1* hlp = (TH1*)fp->Get("hl_e_reco");
  TH1* hrp = (TH1*)fp->Get("hr_e_reco");
  TH1* hld = (TH1*)fd->Get("hl_e_reco");
  TH1* hrd = (TH1*)fd->Get("hr_e_reco");
  TH1* hlt = (TH1*)ft->Get("hl_e_reco");
  TH1* hrt = (TH1*)ft->Get("hr_e_reco");
  TH1* hla = (TH1*)fa->Get("hl_e_reco");
  TH1* hra = (TH1*)fa->Get("hr_e_reco");

  hlp->SetLineColor(kBlue); hrp->SetLineColor(kRed);
  hld->SetLineColor(kBlue); hrd->SetLineColor(kRed);
  hlt->SetLineColor(kBlue); hrt->SetLineColor(kRed);
  hla->SetLineColor(kBlue); hra->SetLineColor(kRed);

  hlp->GetXaxis()->SetRangeUser(2.0e3, 15e3);
  hrp->GetXaxis()->SetRangeUser(2.0e3, 15e3);
  hld->GetXaxis()->SetRangeUser(2.5e3, 20e3);
  hrd->GetXaxis()->SetRangeUser(2.5e3, 20e3);
  hlt->GetXaxis()->SetRangeUser(3.0e3, 20e3);
  hrt->GetXaxis()->SetRangeUser(3.0e3, 20e3);
  hla->GetXaxis()->SetRangeUser(9.0e3, 20e3);
  hra->GetXaxis()->SetRangeUser(9.0e3, 20e3);

  TLegend* l = new TLegend(0.7, 0.7, 0.9, 0.9);
  l->AddEntry(hlp, "Left");
  l->AddEntry(hrp, "Right");
  hlp->SetTitle("Reconstructed Proton Spectrum;E [keV]");
  hld->SetTitle("Reconstructed Deuteron Spectrum;E [keV]");
  hlt->SetTitle("Reconstructed Triton Spectrum;E [keV]");
  hla->SetTitle("Reconstructed Alpha Spectrum;E [keV]");
  gStyle->SetOptStat(0);
  TCanvas* cp = new TCanvas(); hlp->Draw(); hrp->Draw("SAME"); l->Draw();
  TCanvas* cd = new TCanvas(); hld->Draw(); hrd->Draw("SAME"); l->Draw();
  TCanvas* ct = new TCanvas(); hlt->Draw(); hrt->Draw("SAME"); l->Draw();
  TCanvas* ca = new TCanvas(); hla->Draw(); hra->Draw("SAME"); l->Draw();
  cp->SetLogy();
  cd->SetLogy();
  ct->SetLogy();
  ca->SetLogy();
  cp->SaveAs("proton_e_reco.png");
  cd->SaveAs("deuteron_e_reco.png");
  ct->SaveAs("triton_e_reco.png");
  ca->SaveAs("alpha_e_reco.png");
}
