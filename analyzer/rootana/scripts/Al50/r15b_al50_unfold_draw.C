{
  const char* IFNAME_P    = "~/data/R15b/enal50p.root";
  // const char* IFNAME_D    = "~/data/R15b/al50unfoldd.root";
  // const char* IFNAME_T    = "~/data/R15b/al50unfoldt.root";
  // const char* IFNAME_A    = "~/data/R15b/al50unfolda.root";

  TFile* fp = new TFile(IFNAME_P);
  // TFile* fd = new TFile(IFNAME_D);
  // TFile* ft = new TFile(IFNAME_T);
  // TFile* fa = new TFile(IFNAME_A);
  TH1* hlp = (TH1*)fp->Get("hl_e_reco");
  TH1* hrp = (TH1*)fp->Get("hr_e_reco");
  // TH1* hld = (TH1*)fd->Get("hl_e_reco");
  // TH1* hrd = (TH1*)fd->Get("hr_e_reco");
  // TH1* hlt = (TH1*)ft->Get("hl_e_reco");
  // TH1* hrt = (TH1*)ft->Get("hr_e_reco");
  // TH1* hla = (TH1*)fa->Get("hl_e_reco");
  // TH1* hra = (TH1*)fa->Get("hr_e_reco");

  double nmu_hi  = 132e6;
  // double nmu_lo  = 9.26e7;
  double enmu_hi = 2.2e6;
  // double enmu_lo = 0.15e7;
  double caprate = 0.609;
  // double nmu     = 0.5*(nmu_hi+nmu_lo);
  // double enmu    = sqrt(enmu_hi*enmu_hi+enmu_lo*enmu_lo)/2;
  double tcut    = 543.82/864; // f=Ae^(-t/864); int_400^inf(f)/int_0^inf(f)
  hlp->Scale(1./(nmu_hi*caprate));
  hlp->Scale(1./tcut);
  hrp->Scale(1./(nmu_hi*caprate));
  hrp->Scale(1./tcut);
  // for (int i = 1; i <= hp->GetNbinsX(); ++i) {
  //   printf("Err Bin Mu Hist: %d %g %g\n", i, enmu/nmu, hp->GetBinError(i));
  //   double err = hp->GetBinContent(i)*sqrt(hp->GetBinError(i)*hp->GetBinError(i)+pow(enmu/nmu,2));
  //   hp->SetBinError(i, err);
  // }

  hlp->SetLineColor(kBlue); hrp->SetLineColor(kRed);
  // hld->SetLineColor(kBlue); hrd->SetLineColor(kRed);
  // hlt->SetLineColor(kBlue); hrt->SetLineColor(kRed);
  // hla->SetLineColor(kBlue); hra->SetLineColor(kRed);

  hlp->GetXaxis()->SetRangeUser(2.0e3, 15e3);
  hrp->GetXaxis()->SetRangeUser(2.0e3, 15e3);
  // hld->GetXaxis()->SetRangeUser(2.5e3, 20e3);
  // hrd->GetXaxis()->SetRangeUser(2.5e3, 20e3);
  // hlt->GetXaxis()->SetRangeUser(3.0e3, 20e3);
  // hrt->GetXaxis()->SetRangeUser(3.0e3, 20e3);
  // hla->GetXaxis()->SetRangeUser(9.0e3, 20e3);
  // hra->GetXaxis()->SetRangeUser(9.0e3, 20e3);

  TF1* flp = new TF1("flp", "[0]*(1-[1]/x)^[2]*exp(-x/[3])", 0, 15e3);
  TF1* frp = new TF1("frp", "[0]*(1-[1]/x)^[2]*exp(-x/[3])", 0, 15e3);
  TF1* fap = new TF1("fap", "[0]*(1-[1]/x)^[2]*exp(-x/[3])", 0, 15e3);
  flp->SetParameters(0.05, 1500, 2, 2500);
  frp->SetParameters(0.05, 1500, 2, 2500);
  fap->SetParameters(0.05, 1500, 2, 2500);
  flp->SetLineColor(kBlue);
  frp->SetLineColor(kRed);
  fap->SetLineColor(kBlack);
  flp->SetParName(0, "A"); flp->SetParName(1, "T_{th}"); flp->SetParName(2, "#alpha"); flp->SetParName(3, "T_{0}");
  frp->SetParName(0, "A"); frp->SetParName(1, "T_{th}"); frp->SetParName(2, "#alpha"); frp->SetParName(3, "T_{0}");
  fap->SetParName(0, "A"); fap->SetParName(1, "T_{th}"); fap->SetParName(2, "#alpha"); fap->SetParName(3, "T_{0}");

  TLegend* l = new TLegend(0.7, 0.7, 0.9, 0.9);
  l->AddEntry(hlp, "Left");
  l->AddEntry(hrp, "Right");
  hlp->SetTitle("Reconstructed Proton Spectrum;E [keV];Counts [/(200 keV)]");
  // hld->SetTitle("Reconstructed Deuteron Spectrum;E [keV]");
  // hlt->SetTitle("Reconstructed Triton Spectrum;E [keV]");
  // hla->SetTitle("Reconstructed Alpha Spectrum;E [keV]");
  gStyle->SetOptStat(0);
  TCanvas* cp = new TCanvas();
  // hrp->Fit(frp); hlp->Fit(flp);
  hrp->Draw(); hlp->Draw("SAME"); l->Draw();
  char lrate_str[128], rrate_str[128];
  sprintf(lrate_str, "%.3g/cap", hlp->Integral());
  sprintf(rrate_str, "%.3g/cap", hrp->Integral());
  TText *lrate = new TText(4000, 0.002, lrate_str), *rrate = new TText(4000, 0.001, rrate_str);
  lrate->SetTextColor(kBlue); rrate->SetTextColor(kRed);
  lrate->Draw(); rrate->Draw();
  // frp->Draw("SAME"); flp->Draw("SAME");
  // TCanvas* cd = new TCanvas(); hld->Draw(); hrd->Draw("SAME"); l->Draw();
  // TCanvas* ct = new TCanvas(); hlt->Draw(); hrt->Draw("SAME"); l->Draw();
  // TCanvas* ca = new TCanvas(); hla->Draw(); hra->Draw("SAME"); l->Draw();
  // cp->SetLogy();
  // cd->SetLogy();
  // ct->SetLogy();
  // ca->SetLogy();
  cp->SaveAs("img/proton_e_reco_lr.png");
  // cd->SaveAs("deuteron_e_reco.png");
  // ct->SaveAs("triton_e_reco.png");
  // ca->SaveAs("alpha_e_reco.png");

  TCanvas* cpavg = new TCanvas();
  TH1* hp = (TH1*)hlp->Clone("hp");
  hp->Add(hrp);
  hp->Scale(0.5);
  for (int i = 1; i <= hp->GetNbinsX(); ++i) {
    double xl = hlp->GetBinContent(i);
    double xr = hrp->GetBinContent(i);
    double el = hlp->GetBinError(i);
    double er = hrp->GetBinError(i);
    double mean = 0.5*(xl+xr);
    double sys_err  = sqrt(pow(mean-xr,2) + pow(mean-xl, 2));
    double stat_err = sqrt(er*er+el*el);
    double err = sqrt(sys_err*sys_err+stat_err*stat_err);
    hp->SetBinError(i, err);
  }
  hp->SetMinimum(0);
  hp->SetLineColor(kBlack);
  // hp->Fit(fap);

  char avgrate_str[128];
  sprintf(avgrate_str, "%.3g/cap", hp->Integral());
  TText* avgrate = new TText(6e3, 0.003, avgrate_str);
  avgrate->SetTextColor(kBlack);
  avgrate->SetTextSize(0.12);
  cpavg->SetLogy(false);
  hp->Draw();
  avgrate->Draw();
  // fap->Draw("SAME");
  cpavg->SaveAs("img/proton_e_reco_avg.png");

  printf("Total rate: %.3g/cap\n", hp->Integral());
  printf("0-10 MeV:   %.3g/cap\n", hp->Integral(hp->FindFixBin(0),
                                                hp->FindFixBin(10e3)));
  printf("4-8 MeV:    %.3g/cap\n", hp->Integral(hp->FindFixBin(4e3),
                                                hp->FindFixBin(8e3)));
}
