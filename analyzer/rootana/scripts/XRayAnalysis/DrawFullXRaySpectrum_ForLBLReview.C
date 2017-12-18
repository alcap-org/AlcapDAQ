void DrawFullXRaySpectrum_ForLBLReview() {

  gROOT->ProcessLine(".L scripts/XRayAnalysis/CountXRays.C+");
  
  TFile* file = new TFile("/gpfs/data/edmonds_a/merlin4/data/out/v11/Si16b.root", "READ");

  std::string channel = "GeHiGain";
  std::string histname = "PlotTAP_EnergyTime/h" + channel + "#FirstComplete#{constant_fraction=0.20}{no_time_shift= true}_EnergyTime;1";
  TH2F* hGe_2DPlot = (TH2F*) file->Get(histname.c_str());
  TH1D* hGe_FullSpectrum = hGe_2DPlot->ProjectionX();

  // Define the X-ray I am looking at
  XRay xray;
  xray.material = "Si";
  xray.transition = "2p-1s";
  xray.energy = 400.177;
  xray.intensity = 0.803;
  xray.intensity_error = 0.008;

  double a, b, delta_a, delta_b, corr;
  if (channel == "GeLoGain") {
    // Run 10319
    a = 0.223417; delta_a = 0.0213393;
    b = -0.919075; delta_b = 0.0151079;
    corr = -0.995269;

    // Run 9302
    //    a = 0.173809; delta_a = 0.0226852;
    //    b = -0.91031; delta_b = 0.020632;
    //    corr = -0.995468;
  }
  else if (channel == "GeHiGain") {
    // Run 10319
    a = 0.244223; delta_a = 0.0228416;
    b = -0.931899; delta_b = 0.0148333;
    corr = -0.995306;
    
    // Run 9302
    //    a = 0.19928; delta_a = 0.0256165;
    //    b = -0.931788; delta_b = 0.020387;
    //    corr = -0.995484;
  }
  else {
    std::cout << channel << " isn't a germanium channel. Aborting..." << std::endl;
    return 1;
  }
  TF1* ge_eff = new TF1("ge_eff", "[0]*(x^[1])");
  ge_eff->SetParameters(a, b);
  ge_eff->SetParError(0, delta_a);
  ge_eff->SetParError(1, delta_b);
  xray.efficiency = ge_eff->Eval(xray.energy);

  // Assuming uncertainty in the energy is small
  TF1* ge_eff_err = new TF1("ge_eff_err", "sqrt(x^(2*[1]) * ([2]^2 + TMath::Log(x)*[0]*[3]*([0]*[3]*TMath::Log(x) + 2*[4]*[2])))");
  ge_eff_err->SetParameters(a, b, delta_a, delta_b, corr);
  xray.efficiency_error = ge_eff_err->Eval(xray.energy);


  TCanvas* c = new TCanvas("c", "c");

  TPad* main_pad = new TPad("main", "main", 0.0, 0.0, 1.0, 1.0);
  main_pad->Draw();

  hGe_FullSpectrum->SetStats(false);
  hGe_FullSpectrum->SetXTitle("Energy [keV]");
  hGe_FullSpectrum->SetYTitle("Raw Count");
  hGe_FullSpectrum->GetYaxis()->SetTitleOffset(1.3);
  hGe_FullSpectrum->SetTitle("X-Ray Spectrum (Silicon Target)");
  hGe_FullSpectrum->GetXaxis()->SetRangeUser(50, 1200);

  hGe_FullSpectrum->Draw("HIST E");

  int bin_2p1s = hGe_FullSpectrum->FindBin(xray.energy);
  double arrow_y_end = hGe_FullSpectrum->GetBinContent(bin_2p1s)+500;
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

  TLatex* preliminary_text = new TLatex(800, 8000, "AlCap Preliminary");
  preliminary_text->SetTextAlign(22);
  preliminary_text->SetTextSize(0.05);
  preliminary_text->Draw("");

  TPad* inset_pad = new TPad("inset", "inset", 0.45, 0.45, 0.89, 0.89);
  inset_pad->Draw();
  TH1F* hGe_2p1s = (TH1F*) hGe_FullSpectrum->Clone("hGe_2p1s");
  hGe_2p1s->SetTitle("Si 2p-1s X-Ray");
  hGe_2p1s->Rebin(2);
  double xray_energy_low = xray.energy-10;
  double xray_energy_high = xray.energy+10;
  RooRealVar* area = GetAreaUnderPeak(xray_energy_low, xray_energy_high, hGe_2p1s, &xray, inset_pad);
  std::cout << "AE: " << area->getValV() << " " << xray.intensity << " " << xray.efficiency << std::endl;
  double n_stopped_muons = area->getValV() / (xray.intensity * xray.efficiency);
  double n_stopped_muons_error = n_stopped_muons* sqrt( (area->getError()/area->getValV())*(area->getError()/area->getValV()) +
							(xray.intensity_error/xray.intensity)*(xray.intensity_error/xray.intensity) +
							(xray.efficiency_error/xray.efficiency)*(xray.efficiency_error/xray.efficiency) );
  std::cout << "N Stopped Muons = " << n_stopped_muons << " +/- " << n_stopped_muons_error << std::endl;

  c->cd();
  text.str("");
  text << "N_{stopped~muons} = (" << std::setprecision(3) << n_stopped_muons/1e6 << " #pm " << std::setprecision(1) << n_stopped_muons_error/1e6 << ") #times 10^{6}";
  std::cout << text.str() << std::endl;
  TLatex* count_label = new TLatex(850, 6000, text.str().c_str());
  count_label->SetTextAlign(22);
  count_label->SetTextSize(0.034);
  count_label->Draw("");
}
