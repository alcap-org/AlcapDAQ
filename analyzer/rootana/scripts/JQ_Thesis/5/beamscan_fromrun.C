{
  const double p0 = 25.3946;
  double sf[6] = { 1.005, 1.010, 1.015, 1.020, 1.025, 1.030 };
  double p[6];
  double rt_sit  [6] = {3.71, 4.60, 4.94, 5.24, 6.08, 6.415};
  double rt_stop [6] = {0.94, 1.10, 1.02, 1.22, 1.34, 1.325};
  double stopfrac[6];
  for (int i = 0; i < 6; ++i) {
    p[i] = p0*sf[i];
    stopfrac[i]=rt_stop[i]/rt_sit[i];
  }

  TGraph* gr_sit      = new TGraph(6, p, rt_sit);
  TGraph* gr_stop     = new TGraph(6, p, rt_stop);
  TGraph* gr_stopfrac = new TGraph(6, p, stopfrac);

  gr_sit     ->SetTitle("SiT Hits;Muon Beam Momentum [MeV/c];Hit Rate [Hz]");
  gr_stop    ->SetTitle("#mu Stops;Muon Beam Momentum [MeV/c];#mu Stop Rate [Hz]");
  gr_stopfrac->SetTitle("Stopping Fraction;Muon Beam Momentum [MeV/c];#mu stop fraction");

  gr_sit     ->SetMarkerColor(kBlue);
  gr_stop    ->SetMarkerColor(kRed);
  gr_stopfrac->SetMarkerColor(kBlack);
  gr_sit     ->SetMarkerStyle(23);
  gr_stop    ->SetMarkerStyle(22);
  gr_stopfrac->SetMarkerStyle(33);

  gr_stop    ->GetYaxis()->SetTitleOffset(1.52);
  gr_stopfrac->GetYaxis()->SetTitleOffset(1.51);

  TCanvas* c = new TCanvas("c", "c", 2100, 500);
  c->Divide(3);
  c->cd(1); gr_sit     ->Draw("AP");
  c->cd(2); gr_stop    ->Draw("AP");
  c->cd(3); gr_stopfrac->Draw("AP");
  c->SaveAs("img/thesis/5/beamscan.png");
}
