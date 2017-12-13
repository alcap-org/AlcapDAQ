void DrawFullXRaySpectrum_ForLBLReview() {

  gROOT->ProcessLine(".L scripts/XRayAnalysis/CountXRays.C+");
  
  TFile* file = new TFile("/gpfs/home/edmonds_a/data/out/v11/Si16b.root", "READ");

  TH2F* hGeLoGain_2DPlot = (TH2F*) file->Get("PlotTAP_EnergyTime/hGeLoGain#FirstComplete#{constant_fraction=0.20}{no_time_shift= true}_EnergyTime;1");
  TH1D* hGeLoGain_FullSpectrum = hGeLoGain_2DPlot->ProjectionX();

  // Define the X-ray I am looking at
  XRay xray;
  xray.material = "Si";
  xray.transition = "2p-1s";
  xray.energy = 400.177;
  xray.intensity = 0.803;
  xray.intensity_error = 0.008;

  TCanvas* c = new TCanvas("c", "c");

  TPad* main_pad = new TPad("main", "main", 0.0, 0.0, 1.0, 1.0);
  main_pad->Draw();

  hGeLoGain_FullSpectrum->SetStats(false);
  hGeLoGain_FullSpectrum->SetXTitle("Energy [keV]");
  hGeLoGain_FullSpectrum->SetYTitle("Raw Count");
  hGeLoGain_FullSpectrum->GetYaxis()->SetTitleOffset(1.3);
  hGeLoGain_FullSpectrum->SetTitle("X-Ray Spectrum (Silicon Target)");
  hGeLoGain_FullSpectrum->GetXaxis()->SetRangeUser(50, 1200);

  hGeLoGain_FullSpectrum->Draw("HIST E");

  int bin_2p1s = hGeLoGain_FullSpectrum->FindBin(xray.energy);
  double arrow_y_end = hGeLoGain_FullSpectrum->GetBinContent(bin_2p1s)+500;
  double arrow_y_start = arrow_y_end + 1500;
  TArrow* arrow = new TArrow(xray.energy, arrow_y_start, xray.energy, arrow_y_end, 0.01);
  arrow->SetLineColor(kRed);
  arrow->SetLineWidth(2);
  arrow->Draw("");

  TLatex* label = new TLatex(xray.energy, arrow_y_start+3000, "2p-1s");
  label->SetTextAlign(21);
  label->SetTextSize(0.04);
  label->Draw("");

  std::stringstream text;
  text.str("");
  text << "(" << xray.energy << " keV)";
  TLatex* energy_label = new TLatex(xray.energy, arrow_y_start+1000, text.str().c_str());
  energy_label->SetTextAlign(21);
  energy_label->SetTextSize(0.034);
  energy_label->Draw("");

  TLatex* preliminary_text = new TLatex(800, 12500, "AlCap Preliminary");
  preliminary_text->SetTextAlign(22);
  preliminary_text->SetTextSize(0.05);
  preliminary_text->Draw("");

  TPad* inset_pad = new TPad("inset", "inset", 0.45, 0.45, 0.89, 0.89);
  inset_pad->Draw();
  TH1F* hGeLoGain_2p1s = (TH1F*) hGeLoGain_FullSpectrum->Clone("hGeLoGain_2p1s");
  hGeLoGain_2p1s->SetTitle("Si 2p-1s X-Ray");
  hGeLoGain_2p1s->Rebin(2);
  double xray_energy_low = xray.energy-10;
  double xray_energy_high = xray.energy+10;
  RooRealVar* area = GetAreaUnderPeak(xray_energy_low, xray_energy_high, hGeLoGain_2p1s, &xray, inset_pad);

}
