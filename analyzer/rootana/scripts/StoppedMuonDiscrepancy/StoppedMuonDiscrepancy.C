void StoppedMuonDiscrepancy() {

  gROOT->ProcessLine(".L scripts/StoppedMuonDiscrepancy/GeCount.C+");

  int run_to_analyse = 10404;
  int ge_eff_from_run = 10319;

  std::stringstream histtitle_suffix;
  histtitle_suffix << " (Run " << run_to_analyse << ", Ge Eff from Run " << ge_eff_from_run << ")";

  // Define the X-ray I am looking at
  XRay xray;
  xray.material = "Si";
  xray.transition = "2p-1s";
  xray.energy = 400.177;
  xray.intensity = 0.803;
  xray.intensity_error = 0.008;
  
  // Here is the germanium effiency fit for R15b
  double a, delta_a, b, delta_b, corr;
  if (ge_eff_from_run == 9302) {
    // Run 9302 GeLoGain
    a = 0.173809; delta_a = 0.0226852;
    b = -0.91031; delta_b = 0.020632;
    corr = -0.995468;
  }
  else if (ge_eff_from_run == 9685) {
    // Run 9685 GeLoGain
    a = 0.207868; delta_a = 0.0271575;
    b = -0.93348; delta_b = 0.0207003;
    corr = -0.995263;
  }
  else if (ge_eff_from_run == 10319) {
    // Run 10319 GeLoGain
    a = 0.223417; delta_a = 0.0213393;
    b = -0.919075; delta_b = 0.0151079;
    corr = -0.995269;
  }
  else {
    std::cout << ge_eff_from_run << " is not a valid run from which we have Ge efficiency constants" << std::endl;
    return;
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
  
  // Now get the area under the X-ray peak by doing a fit to the spectrum
  double xray_energy_low = xray.energy-10;
  double xray_energy_high = xray.energy+10;


  std::stringstream filename;
  filename << "/gpfs/home/edmonds_a/AlcapDAQ/analyzer/rootana/out" << std::setw(5) << std::setfill('0') << run_to_analyse << "_coincidences.root";
  TFile* file = new TFile(filename.str().c_str());
  if ( file->IsZombie() ) {
    std::cout << "Problems opening file " << filename.str() << std::endl;
    return;
  }

  // Create a root file with all the results
  TFile* out_file = new TFile("stopped-muon-discrepancy-plots.root", "RECREATE");
  
  // Create canvases of the original SiL spectra with their spectra after the cuts
  const int n_dets = 2;
  std::string detnames[n_dets] = {"SiL1", "SiL3"};

  double min_energy_cut = 800;
  double max_energy_cut = 1200;
  double cut_width = 400;
  int n_bins = (max_energy_cut - min_energy_cut) / cut_width;
  TH1F* hRatioPerSiL1EnergyCut = new TH1F("hRatioPerSiL1EnergyCut", "", n_bins,min_energy_cut, max_energy_cut);
  hRatioPerSiL1EnergyCut->SetXTitle("SiL1 Muon Energy Cut [keV]");
  hRatioPerSiL1EnergyCut->SetYTitle("Ge / SiL1");

  for (double min_sil1_muon_energy = min_energy_cut; min_sil1_muon_energy < max_energy_cut; min_sil1_muon_energy += cut_width) {
    //  double min_sil1_muon_energy = 1000;
    //  double max_sil1_muon_energy = 1400;
    double max_sil1_muon_energy = min_sil1_muon_energy + cut_width;
  double min_sil3_muon_energy = 300;
  double max_sil3_muon_energy = 800;

  TH1F* hNMuonsPerChannel = new TH1F("hNMuonsPerChannel", "", n_dets,0,n_dets);
  hNMuonsPerChannel->SetMinimum(0);
  for (int i_det = 0; i_det < n_dets; ++i_det) {
    hNMuonsPerChannel->GetXaxis()->SetBinLabel(i_det+1, detnames[i_det].c_str());
  }
  TH1F* hNMuonsPerChannel_Ge = (TH1F*) hNMuonsPerChannel->Clone("hNMuonsPerChannel_Ge");

  TH1F* hCuts = (TH1F*) file->Get("PlotTAP_StoppedMuonDiscrepancy/hCuts");
  TCanvas* c_coincidences = new TCanvas("c_coincidences", "c_coincidences");
  c_coincidences->Divide(2,2);

  c_coincidences->cd(1);
  TH1F* hSiL3_SiL1_TDiff = (TH1F*) file->Get("PlotTAP_StoppedMuonDiscrepancy/hSiL3_SiL1_TDiff");
  std::string histtitle = "SiL3-SiL1 Absolute TDiff"+histtitle_suffix.str();
  hSiL3_SiL1_TDiff->SetTitle(histtitle.c_str());
  hSiL3_SiL1_TDiff->Draw();
  gPad->Update();
  double coinc_cut_low_SiL3_SiL1 = hCuts->GetBinContent(hCuts->GetXaxis()->FindBin("SiL3-SiL1 (Low)"));
  TLine* line_cut_low_SiL3_SiL1 = new TLine(coinc_cut_low_SiL3_SiL1, gPad->GetFrame()->GetY1(), coinc_cut_low_SiL3_SiL1, gPad->GetFrame()->GetY2());
  line_cut_low_SiL3_SiL1->Draw();
  double coinc_cut_high_SiL3_SiL1 = hCuts->GetBinContent(hCuts->GetXaxis()->FindBin("SiL3-SiL1 (High)"));
  TLine* line_cut_high_SiL3_SiL1 = new TLine(coinc_cut_high_SiL3_SiL1, gPad->GetFrame()->GetY1(), coinc_cut_high_SiL3_SiL1, gPad->GetFrame()->GetY2());
  line_cut_high_SiL3_SiL1->Draw();

  c_coincidences->cd(2);
  TH1F* hSiL3_GeLoGain_TDiff = (TH1F*) file->Get("PlotTAP_StoppedMuonDiscrepancy/hSiL3_GeLoGain_TDiff");
  histtitle = "SiL3-GeLoGain Absolute TDiff"+histtitle_suffix.str();
  hSiL3_GeLoGain_TDiff->SetTitle(histtitle.c_str());
  hSiL3_GeLoGain_TDiff->Draw();
  gPad->Update();
  double coinc_cut_low_SiL3_GeLoGain = hCuts->GetBinContent(hCuts->GetXaxis()->FindBin("SiL3-GeLoGain (Low)"));
  TLine* line_cut_low_SiL3_GeLoGain = new TLine(coinc_cut_low_SiL3_GeLoGain, gPad->GetFrame()->GetY1(), coinc_cut_low_SiL3_GeLoGain, gPad->GetFrame()->GetY2());
  line_cut_low_SiL3_GeLoGain->Draw();
  double coinc_cut_high_SiL3_GeLoGain = hCuts->GetBinContent(hCuts->GetXaxis()->FindBin("SiL3-GeLoGain (High)"));
  TLine* line_cut_high_SiL3_GeLoGain = new TLine(coinc_cut_high_SiL3_GeLoGain, gPad->GetFrame()->GetY1(), coinc_cut_high_SiL3_GeLoGain, gPad->GetFrame()->GetY2());
  line_cut_high_SiL3_GeLoGain->Draw();

  c_coincidences->cd(3);
  TH1F* hSiL1_GeLoGain_TDiff = (TH1F*) file->Get("PlotTAP_StoppedMuonDiscrepancy/hSiL1_GeLoGain_TDiff");
  histtitle = "SiL1-GeLoGain Absolute TDiff"+histtitle_suffix.str();
  hSiL1_GeLoGain_TDiff->SetTitle(histtitle.c_str());
  hSiL1_GeLoGain_TDiff->Draw();
  gPad->Update();
  double coinc_cut_low_SiL1_GeLoGain = hCuts->GetBinContent(hCuts->GetXaxis()->FindBin("SiL1-GeLoGain (Low)"));
  TLine* line_cut_low_SiL1_GeLoGain = new TLine(coinc_cut_low_SiL1_GeLoGain, gPad->GetFrame()->GetY1(), coinc_cut_low_SiL1_GeLoGain, gPad->GetFrame()->GetY2());
  line_cut_low_SiL1_GeLoGain->Draw();
  double coinc_cut_high_SiL1_GeLoGain = hCuts->GetBinContent(hCuts->GetXaxis()->FindBin("SiL1-GeLoGain (High)"));
  TLine* line_cut_high_SiL1_GeLoGain = new TLine(coinc_cut_high_SiL1_GeLoGain, gPad->GetFrame()->GetY1(), coinc_cut_high_SiL1_GeLoGain, gPad->GetFrame()->GetY2());
  line_cut_high_SiL1_GeLoGain->Draw();


  c_coincidences->Write();


  for (int i_det = 0; i_det < n_dets; ++i_det) {
    std::string canvasname = "c_" + detnames[i_det];
    TCanvas* c_det = new TCanvas(canvasname.c_str(), canvasname.c_str());
    c_det->Divide(1,2);

    std::string basehistname = "PlotTAP_StoppedMuonDiscrepancy/h" + detnames[i_det] + "_Energy";
    std::string histname = basehistname + "_NoCuts";
    std::string histtitle = detnames[i_det] + histtitle_suffix.str();
    TH1F* h_det_BeforeCut = (TH1F*) (file->Get(histname.c_str()))->Clone();
    h_det_BeforeCut->SetTitle(histtitle.c_str());
    std::cout << histname << ": " << h_det_BeforeCut->GetEntries() << " entries" << std::endl;

    if (i_det == 0) { // grr...
      histname = basehistname + "_wNoSiL3Coincidence";
    }
    else if (i_det == 1) {
      histname = basehistname + "_wSiL1Coincidence";
    }
    TH1F* h_det_AfterCut = (TH1F*) (file->Get(histname.c_str()))->Clone();
    h_det_AfterCut->SetTitle(histtitle.c_str());
    std::cout << histname << ": " << h_det_AfterCut->GetEntries() << " entries" << std::endl;

    h_det_BeforeCut->SetStats(false);
    h_det_BeforeCut->SetMaximum(h_det_BeforeCut->GetMaximum()*50);
    h_det_BeforeCut->SetMinimum(0.5);
    h_det_BeforeCut->Rebin(10);
    h_det_AfterCut->Rebin(10);
    
    h_det_BeforeCut->SetLineColor(kBlue);
    h_det_AfterCut->SetLineColor(kRed);

    c_det->cd(1);

    h_det_BeforeCut->Draw("HIST E");
    h_det_AfterCut->Draw("SAME");
    
    TLegend *leg = new TLegend(0.5,0.8,0.7,0.6);
    leg->SetBorderSize(0);
    leg->SetTextSize(0.05);
    leg->SetFillColor(0);
    std::string legentry = detnames[i_det] + " Pulses (no cuts)";
    leg->AddEntry(h_det_BeforeCut, legentry.c_str(), "l");
    if (i_det == 0) { // grr...
      legentry = detnames[i_det] + " Pulses (with no SiL3 coincidences)";
    }
    else if (i_det == 1) {
      legentry = detnames[i_det] + " Pulses (with SiL1 coincidence)";
    }
    leg->AddEntry(h_det_AfterCut, legentry.c_str(), "l");
    leg->Draw();

    // Get the direct count of stopped muons between the energies given
    double integral_low, integral_high;
    if (i_det == 0) {
      integral_low = min_sil1_muon_energy;
      integral_high = max_sil1_muon_energy;
    }
    else if (i_det == 1) {
      integral_low = min_sil3_muon_energy;
      integral_high = max_sil3_muon_energy;
    }
    int low_integral_bin = h_det_AfterCut->FindBin(integral_low);
    int high_integral_bin = h_det_AfterCut->FindBin(integral_high)-1; // want to integrate up to the bin below this
    double error = 0;
    double integral = h_det_AfterCut->IntegralAndError(low_integral_bin, high_integral_bin, error);
    std::cout<< "Checking Bins: Bin #" << low_integral_bin << ": center = " << h_det_AfterCut->GetBinCenter(low_integral_bin) << std::endl;
    std::cout<< "Checking Bins: Bin #" << high_integral_bin << ": center = " << h_det_AfterCut->GetBinCenter(high_integral_bin) << std::endl;
    std::cout << histname << " (" << integral_low << " -- " << integral_high << " keV): " << integral << " +/- " << error << " muons" << std::endl;
    hNMuonsPerChannel->SetBinContent(i_det+1, integral);
    hNMuonsPerChannel->SetBinError(i_det+1, error);

    // Draw the integral lines
    TLine* low_muon_energy_cut_line = new TLine(integral_low, 0, integral_low, h_det_AfterCut->GetBinContent(low_integral_bin));
    low_muon_energy_cut_line->SetLineColor(kRed);
    low_muon_energy_cut_line->SetLineStyle(kDashed);
    low_muon_energy_cut_line->SetLineWidth(2);
    low_muon_energy_cut_line->Draw("LSAME");

    TLine* high_muon_energy_cut_line = new TLine(integral_high, 0, integral_high, h_det_AfterCut->GetBinContent(high_integral_bin));
    high_muon_energy_cut_line->SetLineColor(kRed);
    high_muon_energy_cut_line->SetLineStyle(kDashed);
    high_muon_energy_cut_line->SetLineWidth(2);
    high_muon_energy_cut_line->Draw("LSAME");


    c_det->cd(2);
    histname = "h_"+detnames[i_det]+"_BeforeCut_ZoomIn";
    TH1F* h_det_BeforeCut_ZoomIn = (TH1F*) h_det_BeforeCut->Clone(histname.c_str());
    histtitle = detnames[i_det]+histtitle_suffix.str();
    h_det_BeforeCut_ZoomIn->SetTitle(histtitle.c_str());
    h_det_BeforeCut_ZoomIn->GetXaxis()->SetRangeUser(0,2000);
    h_det_BeforeCut_ZoomIn->Draw("HIST E");
    histname = "h_"+detnames[i_det]+"_AfterCut_ZoomIn";
    TH1F* h_det_AfterCut_ZoomIn = (TH1F*) h_det_AfterCut->Clone(histname.c_str());
    h_det_AfterCut_ZoomIn->GetXaxis()->SetRangeUser(0,2000);
    h_det_AfterCut_ZoomIn->Draw("HIST E SAME");
    low_muon_energy_cut_line->Draw("LSAME");
    high_muon_energy_cut_line->Draw("LSAME");

    TArrow* integral_arrow = new TArrow(integral_low, 10, integral_high, 10, 0.02, "<->");
    integral_arrow->SetLineColor(kRed);
    integral_arrow->SetLineWidth(2);
    integral_arrow->Draw();

    std::stringstream text;
    text << "" << integral << " #pm " << error << " muons";
    TLatex* latex = new TLatex( integral_low + (integral_high - integral_low) / 2, 100, text.str().c_str());
    latex->SetTextSize(0.05);
    latex->SetTextAlign(22);
    latex->Draw();
    text.str("");
    text << "(" << integral_low << " -- " << integral_high << " keV)";
    latex = new TLatex( integral_low + (integral_high - integral_low) / 2, 25, text.str().c_str());
    latex->SetTextSize(0.05);
    latex->SetTextAlign(22);
    latex->Draw();
			       
    
    c_det->GetPad(1)->SetLogy();
    c_det->GetPad(2)->SetLogy();
    c_det->Write();


    // Now get the coincident Ge X-rays
    canvasname = "c_GeLoGain_"+detnames[i_det];
    TCanvas* c_GeLoGain_det = new TCanvas(canvasname.c_str(), canvasname.c_str());
    c_GeLoGain_det->Divide(2,2);

    histname = "PlotTAP_StoppedMuonDiscrepancy/hGeLoGain_"+detnames[i_det]+"_CoincidentEnergies";
    TH2F* hGeLoGain_det_CoincidentEnergies = (TH2F*) file->Get(histname.c_str());
    c_GeLoGain_det->cd(1);
    hGeLoGain_det_CoincidentEnergies->GetXaxis()->SetRangeUser(0,2000);
    hGeLoGain_det_CoincidentEnergies->GetYaxis()->SetRangeUser(0,2000);
    histtitle = "Energies of GeLoGain and " + detnames[i_det] + " pulses that are coincident"+histtitle_suffix.str();
    hGeLoGain_det_CoincidentEnergies->SetTitle(histtitle.c_str());
    hGeLoGain_det_CoincidentEnergies->Draw("COLZ");
    gPad->Update();
    TPaveStats* stats_box = (TPaveStats*) hGeLoGain_det_CoincidentEnergies->FindObject("stats");
    stats_box->SetOptStat(1000110011); // name and overflow underflows only

    TH1D* hGeLoGain_CoincidentEnergies = hGeLoGain_det_CoincidentEnergies->ProjectionY();
    c_GeLoGain_det->cd(2);
    histtitle = "Energy of GeLoGain pulses that are coincident (Y Projection)";
    hGeLoGain_CoincidentEnergies->SetTitle(histtitle.c_str());
    histtitle = "Number of Pulses / keV";
    hGeLoGain_CoincidentEnergies->SetYTitle(histtitle.c_str());
    //    hGeLoGain_CoincidentEnergies->Rebin(4);
    hGeLoGain_CoincidentEnergies->GetXaxis()->SetRangeUser(0,2000);
    hGeLoGain_CoincidentEnergies->Draw("HIST E");

    TH1D* h_det_CoincidentEnergies = hGeLoGain_det_CoincidentEnergies->ProjectionX();
    c_GeLoGain_det->cd(3);
    histtitle = "Energy of "+detnames[i_det]+" pulses that are coincident (X Projection)";
    h_det_CoincidentEnergies->SetTitle(histtitle.c_str());
    histtitle = "Number of Pulses / 10 keV";
    h_det_CoincidentEnergies->SetYTitle(histtitle.c_str());
    //    h_det_CoincidentEnergies->Rebin(2);
    h_det_CoincidentEnergies->GetXaxis()->SetRangeUser(0,2000);
    h_det_CoincidentEnergies->Draw("HIST E");

    c_GeLoGain_det->cd(4);

    // Get the GeLoGain spectrum where we know a muon was coincident
    low_integral_bin = hGeLoGain_det_CoincidentEnergies->GetXaxis()->FindBin(integral_low);
    high_integral_bin = hGeLoGain_det_CoincidentEnergies->GetXaxis()->FindBin(integral_high);
    histname = "hGeLoGainEnergies_"+detnames[i_det]+"_wMuonEnergyCut";
    TH1D* hGeLoGain_CoincidentEnergies_wMuonEnergyCut = hGeLoGain_det_CoincidentEnergies->ProjectionY(histname.c_str(), low_integral_bin, high_integral_bin);
    std::stringstream new_histtitle;
    new_histtitle << "Energy of GeLoGain pulses that are coincident with a " << detnames[i_det] << " pulse with energy between " << integral_low << " keV and " << integral_high << " keV";
    hGeLoGain_CoincidentEnergies_wMuonEnergyCut->SetTitle(new_histtitle.str().c_str());
    hGeLoGain_CoincidentEnergies_wMuonEnergyCut->Rebin(2);

    RooRealVar* area = GetAreaUnderPeak(xray_energy_low, xray_energy_high, hGeLoGain_CoincidentEnergies_wMuonEnergyCut, &xray, gPad);

    // Now calculate the number of stopped muons
    double n_stopped_muons = area->getValV() / (xray.intensity * xray.efficiency);
      //    for (std::vector<double>::const_iterator i_factor = detector_effects.begin(); i_factor != detector_effects.end(); ++i_factor) {
      //      n_stopped_muons *= (*i_factor);
      //    }

    // ...and the uncertainty
    double n_stopped_muons_error = n_stopped_muons* sqrt( (area->getError()/area->getValV())*(area->getError()/area->getValV()) +
							  (xray.intensity_error/xray.intensity)*(xray.intensity_error/xray.intensity) +
							  (xray.efficiency_error/xray.efficiency)*(xray.efficiency_error/xray.efficiency) );
							    

    std::cout << "XRay: " << xray.material << " " << xray.transition << " " << xray.energy << " keV" << std::endl;
    std::cout << "Area = " << area->getValV() << " +/- " << area->getError() << std::endl;
    std::cout << "Intensity = " << xray.intensity << " +/- " << xray.intensity_error << std::endl;
    std::cout << "Efficiency = " << xray.efficiency << " +/- " << xray.efficiency_error << std::endl;
    std::cout << "Number of Stopped Muons = " << n_stopped_muons << " +/- " << n_stopped_muons_error << std::endl;

    text.str("");
    text << "#splitline{" << detnames[i_det] << " energy cut}{ " << integral_low << " -- " << integral_high << " keV}";
    double first_line_x = 395;
    gPad->Update();
    double first_line_y = gPad->GetFrame()->GetY2();
    double y_step = first_line_y*0.05*2; // *textsize

    latex = new TLatex(first_line_x, first_line_y, text.str().c_str());
    latex->SetTextSize(0.05);
    latex->SetTextAlign(22);
    latex->Draw();

    text.str("");
    text << std::setprecision(3);
    text << "Area = " << area->getValV() << " #pm " << area->getError() << "";
    latex = new TLatex(first_line_x+17, first_line_y-2*y_step, text.str().c_str());
    latex->SetTextSize(0.05);
    latex->SetTextAlign(32);
    latex->Draw();

    text.str("");
    text << "Intensity = " << xray.intensity << " #pm " << xray.intensity_error << "";
    latex = new TLatex(first_line_x+17, first_line_y-3*y_step, text.str().c_str());
    latex->SetTextSize(0.05);
    latex->SetTextAlign(32);
    latex->Draw();

    text.str("");
    text << "Eff = " << xray.efficiency << " #pm " << xray.efficiency_error << "";
    latex = new TLatex(first_line_x+17, first_line_y-4*y_step, text.str().c_str());
    latex->SetTextSize(0.05);
    latex->SetTextAlign(32);
    latex->Draw();

    text.str("");
    text << "N Stopped Muons = ";
    latex = new TLatex(first_line_x, first_line_y-6*y_step, text.str().c_str());
    latex->SetTextSize(0.05);
    latex->SetTextAlign(22);
    latex->Draw();

    text.str("");
    text << n_stopped_muons << " #pm " << n_stopped_muons_error;
    latex = new TLatex(first_line_x, first_line_y-7*y_step, text.str().c_str());
    latex->SetTextSize(0.05);
    latex->SetTextAlign(22);
    latex->Draw();


    c_GeLoGain_det->Write();

    hNMuonsPerChannel_Ge->SetBinContent(i_det+1, n_stopped_muons);
    hNMuonsPerChannel_Ge->SetBinError(i_det+1, n_stopped_muons_error);        
  }

  TCanvas* c_NMuonsPerChannel = new TCanvas("c_NMuonsPerChannel", "c_NMuonsPerChannel");
  double y_max = hNMuonsPerChannel->GetMaximum() > hNMuonsPerChannel_Ge->GetMaximum() ? hNMuonsPerChannel->GetMaximum() : hNMuonsPerChannel_Ge->GetMaximum();
  y_max *= 1.5; // go 50% higher
  hNMuonsPerChannel->SetTitle(histtitle_suffix.str().c_str());
  hNMuonsPerChannel->SetMaximum(y_max);
  hNMuonsPerChannel->SetStats(false);
  hNMuonsPerChannel->SetYTitle("Number of Stopped Muons");
  hNMuonsPerChannel_Ge->SetStats(false);
  hNMuonsPerChannel->Draw("HIST E");
  hNMuonsPerChannel_Ge->SetLineColor(kRed);
  hNMuonsPerChannel_Ge->Draw("HIST E SAME");

  TLegend *leg = new TLegend(0.18,0.26,0.38,0.46);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);
  leg->AddEntry(hNMuonsPerChannel, "Number of Stopped Muons from SiL", "l");
  leg->AddEntry(hNMuonsPerChannel_Ge, "Number of Stopped Muons from GeLoGain", "l");
  leg->Draw();

  c_NMuonsPerChannel->Write();

  TCanvas* c_Ratio = new TCanvas("c_Ratio", "c_Ratio");
  TH1F* hRatio = (TH1F*) hNMuonsPerChannel_Ge->Clone("hRatio");
  hRatio->SetTitle(histtitle_suffix.str().c_str());
  hRatio->SetYTitle("Ge / SiL");
  hRatio->SetStats(false);
  hRatio->Divide(hNMuonsPerChannel);
  hRatio->SetMaximum(1.5);
  hRatio->SetMinimum(0);
  hRatio->Draw("HIST E");
  c_Ratio->Write();

  hRatioPerSiL1EnergyCut->SetBinContent(hRatioPerSiL1EnergyCut->FindBin(min_sil1_muon_energy),hRatio->GetBinContent(hRatio->GetXaxis()->FindBin("SiL1")));
  hRatioPerSiL1EnergyCut->SetBinError(hRatioPerSiL1EnergyCut->FindBin(min_sil1_muon_energy),hRatio->GetBinError(hRatio->GetXaxis()->FindBin("SiL1")));
  }

  out_file->Write();
  out_file->Close();
}
