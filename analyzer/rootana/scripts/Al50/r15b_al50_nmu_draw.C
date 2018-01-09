{
  const char IFNAME[] = "~/data/R15b/nmual50.root";

  TFile* f = new TFile(IFNAME);
  gROOT->ProcessLine(".L scripts/Al50/util.h+g");

  // Draw SiT energy spectrum (muons)
  gStyle->SetOptStat("e");
  TCanvas* cmu = new TCanvas();
  hemu->SetTitle("Energy of Central Muon in each TME;E [keV];Count [/2.5keV]");
  hemu->GetXaxis()->SetRangeUser(0, 8e3);
  hemu->Draw();
  cmu->SetLogy();
  cmu->SaveAs("incoming_muons.png");

  // Fit and draw Ge spectra
  TH1* hhi = hgehi_e_tcut;
  TH1* hlo = hgelo_e_tcut;
  hgehi_e_all->SetStats(kFALSE);
  hgelo_e_all->SetStats(kFALSE);
  hgehi_e_all->SetTitle("Ge Spectrum (Al50 Target) all times;Energy [keV];Raw Count");
  hgehi_e_all->GetXaxis()->SetTitleOffset(1.2);
  hgehi_e_all->GetYaxis()->SetTitleOffset(1.4);
  hgehi_e_all->SetLineColor(kRed);
  hgehi_e_all->GetXaxis()->SetRangeUser(150., 1200.);
  TLegend* l_all = new TLegend(0.7, 0.7, 0.9, 0.9);
  l_all->AddEntry(hgehi_e_all, "Ge High Gain");
  l_all->AddEntry(hgelo_e_all, "Ge Low Gain");

  TCanvas* c_all = new TCanvas;
  hgehi_e_all->Draw();
  hgelo_e_all->Draw("SAME");
  l_all->Draw();
  c_all->SaveAs("ge_e.png");

  TCanvas* c_fit = new TCanvas;
  hhi->GetXaxis()->SetRangeUser(335., 355.);
  hlo->GetXaxis()->SetRangeUser(335., 355.);
  hhi->SetTitle("X-Ray Spectrum (Al50 Target) #pm200ns;Energy [keV];Raw Count");
  hhi->GetXaxis()->SetTitleOffset(1.2);
  hhi->GetYaxis()->SetTitleOffset(1.4);
  hhi->SetLineColor(kRed);
  TF1* fcn_hi = new TF1("fcn_hi", "pol1(0)+[2]*exp(-0.5*(x-[3])^2/[6]^2)+[4]*exp(-0.5*(x-[5])^2/[6]^2)");
  TF1* fcn_lo = new TF1("fcn_lo", "pol1(0)+[2]*exp(-0.5*(x-[3])^2/[6]^2)+[4]*exp(-0.5*(x-[5])^2/[6]^2)");
  fcn_hi->SetParameters(15e3, 0., 45e3, 345., 10e3, 349.5, 1.);
  fcn_lo->SetParameters(15e3, 0., 40e3, 344., 5e3, 348.5, 1.);
  fcn_hi->SetParName(0, "Bkg y-int"); fcn_lo->SetParName(0, "Bkg y-int");
  fcn_hi->SetParName(1, "Bkg Slope"); fcn_lo->SetParName(1, "Bkg Slope");
  fcn_hi->SetParName(2, "Al Amp");    fcn_lo->SetParName(2, "Al Amp");
  fcn_hi->SetParName(3, "Al Energy"); fcn_lo->SetParName(3, "Al Energy");
  fcn_hi->SetParName(4, "Pb Amp");    fcn_lo->SetParName(4, "Pb Amp");
  fcn_hi->SetParName(5, "Pb Energy"); fcn_lo->SetParName(5, "Pb Energy");
  fcn_hi->SetParName(6, "Sigma");     fcn_lo->SetParName(6, "Sigma");
  fcn_lo->SetLineColor(kBlue);
  hhi->Fit(fcn_hi, "S");
  hlo->Fit(fcn_lo, "S");
  TFitResultPtr fit_hi = hhi->Fit(fcn_hi, "S");
  TFitResultPtr fit_lo = hlo->Fit(fcn_lo, "S");

  int nhi = GeFcn::NMuHi(fit_hi->Parameter(2), fit_hi->Parameter(6), hhi->GetBinWidth(0), 0.798, 347.);
  int nlo = GeFcn::NMuLo(fit_lo->Parameter(2), fit_lo->Parameter(6), hlo->GetBinWidth(0), 0.798, 347.);
  double nhi_err = GeFcn::NMuErrHi(fit_hi->Parameter(2), fit_hi->ParError(2), fit_hi->Parameter(6), fit_hi->ParError(6), hhi->GetBinWidth(0), 0.798, 0.008, 347.);
  double nlo_err = GeFcn::NMuErrLo(fit_lo->Parameter(2), fit_lo->ParError(2), fit_lo->Parameter(6), fit_lo->ParError(6), hlo->GetBinWidth(0), 0.798, 0.008, 347.);

  gStyle->SetOptStat("n");
  gStyle->SetOptFit(111);
  hhi->SetStats(kTRUE);
  hlo->SetStats(kTRUE);
  hhi->SetName("GeHiGain");
  hlo->SetName("GeLoGain");
  hhi->Draw();
  c_fit->Update();
  TPaveStats* stats_hi = (TPaveStats*)hhi->GetListOfFunctions()->FindObject("stats");
  hlo->Draw();
  c_fit->Update();
  TPaveStats* stats_lo = (TPaveStats*)hlo->GetListOfFunctions()->FindObject("stats");
  stats_hi->SetX1NDC(0.10); stats_hi->SetX2NDC(0.35);
  stats_hi->SetY1NDC(0.50); stats_hi->SetY2NDC(0.90);
  stats_lo->SetX1NDC(0.10); stats_lo->SetX2NDC(0.35);
  stats_lo->SetY1NDC(0.10); stats_lo->SetY2NDC(0.50);
  TLegend* l_fit = new TLegend(0.55, 0.70, 0.95, 0.90);
  char l_fit_hi[64], l_fit_lo[64];
  sprintf(l_fit_hi, "Ge High Gain (%.3g#pm%.2g #mu)", nhi, nhi_err);
  sprintf(l_fit_lo, "Ge Low Gain (%.3g#pm%.2g #mu)", nlo, nlo_err);
  l_fit->AddEntry(hhi, l_fit_hi);
  l_fit->AddEntry(hlo, l_fit_lo);
  hhi->Draw();
  hlo->Draw("SAME");
  l_fit->Draw();
  c_fit->Update();
  c_fit->SaveAs("ge_e_zoom.png");

  printf("Hi chi2 %d/%d (%f), n %d +/- %f\n", fit_hi->Chi2(), fit_hi->Ndf(), ((double)fit_hi->Chi2())/fit_hi->Ndf(), nhi, nhi_err);
  printf("Lo chi2 %d/%d (%f), n %d +/- %f\n", fit_lo->Chi2(), fit_lo->Ndf(), ((double)fit_lo->Chi2())/fit_lo->Ndf(), nlo, nlo_err);
}
