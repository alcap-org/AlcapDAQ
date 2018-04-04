void ActiveTargetAnalysis() {
  std::string filename = "scripts/Si16b_ActiveTargetAnalysis/hist_files/PlotsFromTMETree_Run10404_DetArmVeto.root";
  TFile* file = new TFile(filename.c_str(), "READ");

  TH2F* hTimeEnergy = (TH2F*) file->Get("hTimeEnergy");
  TTree* cuttree = (TTree*) file->Get("cuts");

  int n_stopped_muons = 0;
  double min_muon_energy = 0;
  double max_muon_energy = 0;
  double muon_time_cut = 0;
  int i_stopped_muons = 0;
  cuttree->SetBranchAddress("n_stopped_muons", &i_stopped_muons);
  cuttree->SetBranchAddress("min_muon_energy", &min_muon_energy);
  cuttree->SetBranchAddress("max_muon_energy", &max_muon_energy);
  cuttree->SetBranchAddress("muon_time_cut", &muon_time_cut);
  for (int i_entry = 0; i_entry < cuttree->GetEntries(); ++i_entry) {
    cuttree->GetEntry(i_entry);
    n_stopped_muons += i_stopped_muons;
  }

  int rebin_factor = 1;
  
  double capture_fraction = 0.658;
  int n_captured_muons = capture_fraction*n_stopped_muons;

  /*  std::string corr_filename = "scripts/SiL3_ActiveTargetAnalysis/hist_files/R15b_SiL3-ActiveTargetAnalysis_MC-expo_Corrections.root";
  TFile* corr_file = new TFile(corr_filename.c_str(), "READ");
  TH1F* hStoppingFraction = (TH1F*) corr_file->Get("hStoppingFraction");
  hStoppingFraction->Rebin(rebin_factor);
  hStoppingFraction->Scale(1.0/rebin_factor);
  TH1F* hEDep_muplus = (TH1F*) corr_file->Get("hEDep_muplus");
  hEDep_muplus->Rebin(rebin_factor);
  
  double min_time_flat_bkg = 11000;
  double max_time_flat_bkg = 14000;
  int min_time_flat_bkg_bin = hTimeEnergy->GetXaxis()->FindBin(min_time_flat_bkg);
  int max_time_flat_bkg_bin = hTimeEnergy->GetXaxis()->FindBin(max_time_flat_bkg);

  TH1D* hFlatBackground = hTimeEnergy->ProjectionY("hFlatBkg", min_time_flat_bkg_bin, max_time_flat_bkg_bin);
  hFlatBackground->Rebin(rebin_factor);
  */

  TCanvas* c_minus1 = new TCanvas("c_minus1", "c_minus1");
  c_minus1->SetLogz();
  hTimeEnergy->GetYaxis()->SetRangeUser(0,15000);
  hTimeEnergy->Draw("COLZ");
  
  TCanvas* c_muon = new TCanvas("c_muon", "c_muon");
  c_muon->Divide(2, 1);
  c_muon->cd(1);
  c_muon->GetPad(1)->SetLogz();
  TH2F* hTimeEnergy_Zoom = (TH2F*) hTimeEnergy->Clone("hTimeEnergy_Zoom");
  hTimeEnergy_Zoom->GetXaxis()->SetRangeUser(-5000,5000);
  hTimeEnergy_Zoom->Draw("COLZ");

  TLine* min_muon_time_line = new TLine(-muon_time_cut, hTimeEnergy->GetYaxis()->GetXmin(), -muon_time_cut, hTimeEnergy->GetYaxis()->GetXmax());
  min_muon_time_line->SetLineColor(kRed);
  min_muon_time_line->SetLineWidth(2);
  min_muon_time_line->Draw("LSAME");
  TLine* max_muon_time_line = new TLine(muon_time_cut, hTimeEnergy->GetYaxis()->GetXmin(), muon_time_cut, hTimeEnergy->GetYaxis()->GetXmax());
  max_muon_time_line->SetLineColor(kRed);
  max_muon_time_line->SetLineWidth(2);
  max_muon_time_line->Draw("LSAME");
  
  
  c_muon->cd(2);
  int min_muon_time_cut_bin = hTimeEnergy->GetXaxis()->FindBin(-muon_time_cut);
  int max_muon_time_cut_bin = hTimeEnergy->GetXaxis()->FindBin(muon_time_cut);
  TH1D* hMuonEnergy = hTimeEnergy->ProjectionY("hMuonEnergy", min_muon_time_cut_bin, max_muon_time_cut_bin);
  hMuonEnergy->Draw();
  hMuonEnergy->GetXaxis()->SetRangeUser(0,7000);
  TLine* min_muon_energy_line = new TLine(min_muon_energy, 0, min_muon_energy, hMuonEnergy->GetMaximum());
  min_muon_energy_line->SetLineColor(kRed);
  min_muon_energy_line->SetLineWidth(2);
  min_muon_energy_line->Draw("LSAME");
  TLine* max_muon_energy_line = new TLine(max_muon_energy, 0, max_muon_energy, hMuonEnergy->GetMaximum());
  max_muon_energy_line->SetLineColor(kRed);
  max_muon_energy_line->SetLineWidth(2);
  max_muon_energy_line->Draw("LSAME");
  


  TCanvas* c_time = new TCanvas("c_time", "c_time");
  c_time->Divide(2,1);
  c_time->GetPad(1)->SetLogz();
  c_time->cd(1);
  hTimeEnergy->Draw("COLZ");

  double min_energy = 3000;
  double max_energy = 7000;
  int min_energy_bin = hTimeEnergy->GetYaxis()->FindBin(min_energy);
  int max_energy_bin = hTimeEnergy->GetYaxis()->FindBin(max_energy);

  TLine* lifetime_line_min = new TLine(hTimeEnergy->GetXaxis()->GetXmin(), min_energy, hTimeEnergy->GetXaxis()->GetXmax(), min_energy);
  lifetime_line_min->SetLineWidth(2);
  lifetime_line_min->SetLineColor(kRed);
  lifetime_line_min->Draw("LSAME");
  TLine* lifetime_line_max = new TLine(hTimeEnergy->GetXaxis()->GetXmin(), max_energy, hTimeEnergy->GetXaxis()->GetXmax(), max_energy);
  lifetime_line_max->SetLineWidth(2);
  lifetime_line_max->SetLineColor(kRed);
  lifetime_line_max->Draw("LSAME");

  c_time->cd(2);
  c_time->GetPad(2)->SetLogy(1);
  TH1D* hTime = hTimeEnergy->ProjectionX("hTime", min_energy_bin, max_energy_bin);
  hTime->Rebin(rebin_factor);
  hTime->GetXaxis()->SetRangeUser(-1000, 20000);
  hTime->Draw("HIST E");

  double min_time_fit = 200;
  double max_time_fit = 15000;
  TF1* lifetime_fit = new TF1("lifetime_fit", "[0]*TMath::Exp(-x/[1]) + [2]", min_time_fit, max_time_fit);
  lifetime_fit->SetParameters(hTime->GetMaximum(), 500, 10);
  hTime->Fit(lifetime_fit, "R");
  lifetime_fit->Draw("LSAME");

  std::stringstream text;
  text << "#tau = " << std::setprecision(4) << lifetime_fit->GetParameter(1) << " #pm " << std::setprecision(2) << lifetime_fit->GetParError(1) << " ns";
  TLatex* lifetime_text = new TLatex(12500, hTime->GetMaximum()/1000, text.str().c_str());
  lifetime_text->SetTextAlign(22);
  lifetime_text->Draw();

  double min_time = 1000;
  double max_time = 4000;
  int min_time_bin = hTimeEnergy->GetXaxis()->FindBin(min_time);
  int max_time_bin = hTimeEnergy->GetXaxis()->FindBin(max_time);
  
  double fraction_alive_after_min_time = lifetime_fit->Eval(min_time) / lifetime_fit->GetParameter(0);
  double fraction_alive_after_max_time = lifetime_fit->Eval(max_time) / lifetime_fit->GetParameter(0);
  double fraction_lost_in_gate = fraction_alive_after_min_time - fraction_alive_after_max_time;
  int n_captured_muons_in_gate = fraction_lost_in_gate*n_captured_muons;

  TCanvas* c_energy = new TCanvas("c_energy", "c_energy");
  c_energy->Divide(2,1);
  c_energy->cd(1);
  c_energy->GetPad(1)->SetLogz();
  hTimeEnergy->Draw("COLZ");

  TLine* time_line_min = new TLine(min_time, hTimeEnergy->GetYaxis()->GetXmin(), min_time, hTimeEnergy->GetYaxis()->GetXmax());
  time_line_min->SetLineWidth(2);
  time_line_min->SetLineColor(kRed);
  time_line_min->Draw("LSAME");
  TLine* time_line_max = new TLine(max_time, hTimeEnergy->GetYaxis()->GetXmin(), max_time, hTimeEnergy->GetYaxis()->GetXmax());
  time_line_max->SetLineWidth(2);
  time_line_max->SetLineColor(kRed);
  time_line_max->Draw("LSAME");


  c_energy->cd(2);
  c_energy->GetPad(2)->SetLogy();
  TH1D* hEnergy = hTimeEnergy->ProjectionY("hEnergy", min_time_bin, max_time_bin);
  hEnergy->SetLineColor(kBlack);
  hEnergy->Rebin(rebin_factor);
  hEnergy->Draw("HIST E");

  TCanvas* c_spectrum = new TCanvas("c_spectrum", "c_spectrum");
  c_spectrum->SetLogy();
  TH1F* hEnergy_SWZoom = (TH1F*) hEnergy->Clone("hEnergy_SWZoom");
  hEnergy_SWZoom->Draw("HIST E");
  hEnergy_SWZoom->GetXaxis()->SetRangeUser(0, 26000);


  
  /*  TCanvas* c_muplus_spectrum = new TCanvas("c_muplus_spectrum", "c_muplus_spectrum");
  c_muplus_spectrum->SetLogy();
  hEnergy->Draw("HIST E");
  hEDep_muplus->SetLineColor(kRed);
  hEDep_muplus->Scale( (1-capture_fraction)*fraction_lost_in_gate*n_stopped_muons );
  hEDep_muplus->Draw("HIST E SAME");
  TH1F* hEnergyCorrected_muplusOnly = (TH1F*) hEnergy->Clone("hEnergyCorrected_muplusOnly");
  hEnergyCorrected_muplusOnly->SetLineColor(kRed);
  hEnergyCorrected_muplusOnly->Add(hEDep_muplus, -1);
  hEnergyCorrected_muplusOnly->Draw("HIST E SAME");
  */
  /*  double min_energy_check = 400;
  double max_energy_check = 3000;
  int min_energy_check_bin = hEnergy->FindBin(min_energy_check);
  int max_energy_check_bin = hEnergy->FindBin(max_energy_check);
  std::cout << "AE: muplus / total (E<3 MeV) = " << hEDep_muplus->Integral(min_energy_check_bin, max_energy_check_bin) << " / " << hEnergy->Integral(min_energy_check_bin, max_energy_check_bin) << " = " << hEDep_muplus->Integral(min_energy_check_bin, max_energy_check_bin) / hEnergy->Integral(min_energy_check_bin, max_energy_check_bin) << std::endl;
  std::cout << "AE: muplus / total (E>3 MeV) = " << hEDep_muplus->Integral(max_energy_check_bin, hEnergy->GetNbinsX()) << " / " << hEnergy->Integral(max_energy_check_bin, hEnergy->GetNbinsX()) << " = " << hEDep_muplus->Integral(max_energy_check_bin, hEnergy->GetNbinsX()) / hEnergy->Integral(max_energy_check_bin, hEnergy->GetNbinsX()) << std::endl;

  std::cout << "AE: FlatBackground / total (E<3 MeV) = " << hFlatBackground->Integral(min_energy_check_bin, max_energy_check_bin) << " / " << hEnergy->Integral(min_energy_check_bin, max_energy_check_bin) << " = " << hFlatBackground->Integral(min_energy_check_bin, max_energy_check_bin) / hEnergy->Integral(min_energy_check_bin, max_energy_check_bin) << std::endl;
  std::cout << "AE: FlatBackground / total (E>3 MeV) = " << hFlatBackground->Integral(max_energy_check_bin, hEnergy->GetNbinsX()) << " / " << hEnergy->Integral(max_energy_check_bin, hEnergy->GetNbinsX()) << " = " << hFlatBackground->Integral(max_energy_check_bin, hEnergy->GetNbinsX()) / hEnergy->Integral(max_energy_check_bin, hEnergy->GetNbinsX()) << std::endl;
  */
  /*
  TCanvas* c_flat_bkg = new TCanvas("c_flat_bkg", "c_flat_bkg");
  c_flat_bkg->Divide(2,1);
  c_flat_bkg->cd(1);
  c_flat_bkg->GetPad(1)->SetLogz();
  hTimeEnergy->Draw("COLZ");
  TLine* flat_bkg_line_min = new TLine(min_time_flat_bkg, hTimeEnergy->GetYaxis()->GetXmin(), min_time_flat_bkg, hTimeEnergy->GetYaxis()->GetXmax());
  flat_bkg_line_min->SetLineWidth(2);
  flat_bkg_line_min->SetLineColor(kRed);
  flat_bkg_line_min->Draw("LSAME");
  TLine* flat_bkg_line_max = new TLine(max_time_flat_bkg, hTimeEnergy->GetYaxis()->GetXmin(), max_time_flat_bkg, hTimeEnergy->GetYaxis()->GetXmax());
  flat_bkg_line_max->SetLineWidth(2);
  flat_bkg_line_max->SetLineColor(kRed);
  flat_bkg_line_max->Draw("LSAME");

  c_flat_bkg->cd(2);
  c_flat_bkg->GetPad(2)->SetLogy();
  hFlatBackground->Draw("HIST E");
  

  TCanvas* c_final_spectrum = new TCanvas("c_final_spectrum", "c_final_spectrum");
  c_final_spectrum->SetLogy();
  hEnergy->Draw("HIST E");
  hEDep_muplus->Draw("HIST SAME");
  hFlatBackground->Draw("HIST SAME");
  
  TH1D* hEnergyCorrected = (TH1D*) hEnergy->Clone("hEnergyCorrected");
  hEnergyCorrected->SetLineColor(kMagenta);
  hEnergyCorrected->Add(hEDep_muplus, -1);
  hEnergyCorrected->Add(hFlatBackground, -1);
  hEnergyCorrected->Draw("HIST SAME");

  TLegend* leg = new TLegend(0.64,0.54,0.79,0.78);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);
  leg->AddEntry(hEnergy, "Raw Spectrum", "l");
  leg->AddEntry(hEDep_muplus, "Decay Electron Bkg", "l");
  leg->AddEntry(hFlatBackground, "Flat Bkg", "l");
  leg->AddEntry(hEnergyCorrected, "Corrected Energy Spectrum", "l");
  leg->Draw();


  
  TCanvas* c_proton_attempt = new TCanvas("c_proton_attempt", "c_proton_attempt");
  c_proton_attempt->SetLogy();
  hEnergyCorrected->Draw("HIST SAME");
  */
  /*  TH1D* hEnergyStopped = (TH1D*) hEnergyCorrected->Clone("hEnergyStopped");
  hEnergyStopped->Multiply(hStoppingFraction);
  hEnergyStopped->SetLineColor(kCyan);
  hEnergyStopped->Draw("HIST SAME");

  TH1D* hEnergyNotStopped = (TH1D*) hEnergyCorrected->Clone("hEnergyNotStopped");
  hEnergyNotStopped->Add(hEnergyStopped, -1);
  hEnergyNotStopped->SetLineColor(kGray);
  hEnergyNotStopped->Draw("HIST SAME");
  */
  /*  RooUnfoldResponse* response = (RooUnfoldResponse*) corr_file->Get("SiL3_response");
  //  RooUnfoldBayes unfold (response, hEnergyNotStopped);
  RooUnfoldBayes unfold (response, hEnergyCorrected, 3);
  TH1D* hEnergyUnfolded = (TH1D*) unfold.Hreco();
  hEnergyUnfolded->SetLineColor(kRed);
  hEnergyUnfolded->Draw("HIST SAME");
  */
  /*  TH1D* hEnergyTotal = (TH1D*) hEnergyStopped->Clone("hEnergyTotal");
  hEnergyTotal->Add(hEnergyUnfolded);
  hEnergyTotal->SetLineColor(kSpring);
  hEnergyTotal->Draw("HIST SAME");
  */
  /*  TLegend* leg2 = new TLegend(0.64,0.54,0.79,0.78);
  leg2->SetBorderSize(0);
  leg2->SetTextSize(0.03);
  leg2->SetFillColor(kWhite);
  leg2->AddEntry(hEnergyCorrected, "hEnergyCorrected", "l");
  //  leg2->AddEntry(hEnergyStopped, "hEnergyStopped (i.e. hEnergyCorrected #times stopping fraction)", "l");
  //  leg2->AddEntry(hEnergyNotStopped, "hEnergyNotStopped (i.e. hEnergyCorrected - hEnergyStopped)", "l");
  leg2->AddEntry(hEnergyUnfolded, "hEnergyUnfolded (i.e. hEnergyNotStopped unfolded)", "l");
  //  leg2->AddEntry(hEnergyTotal, "hEnergyTotal (i.e. hEnergyStopped + hEnergyUnfolded)", "l");
  leg2->Draw();
  */
  /*  TH1D* hEnergyUnfolded = (TH1D*) hEnergyCorrected->Clone("hEnergyUnfolded");
  hEnergyUnfolded->Reset();
  hEnergyUnfolded->SetLineColor(kMagenta);
  TH2D* response_matrix = response->Hresponse();
  for (int i_bin = 1; i_bin <= response_matrix->GetNbinsX(); ++i_bin) {
    TH1D* slice = response_matrix->ProjectionY("_py", i_bin, i_bin);
    for (int i_entry = 0; i_entry < hEnergyCorrected->GetBinContent(i_bin); ++i_entry) {
      hEnergyUnfolded->Fill(slice->GetRandom());
    }
  }
  */
  

  /*  TCanvas* c_result = new TCanvas("c_result", "c_result");
  c_result->SetLogy();
  TH1D* hEnergyResult = (TH1D*) hEnergyCorrected->Clone("hEnergyResult");
  hEnergyResult->SetLineColor(kBlack);
  hEnergyResult->GetXaxis()->SetRangeUser(0,26000);
  //  hEnergyResult->Rebin(2);
  hEnergyResult->Draw("HIST E");
  */
  TF1* spectrum_fit = new TF1("spectrum_fit", "[0]*TMath::Exp(-x/[1])", min_energy, max_energy);
  spectrum_fit->SetParameters(hEnergy->GetMaximum(), 1000);
  hEnergy->Fit(spectrum_fit, "R");
  spectrum_fit->Draw("LSAME");

  text.str("");
  text << "decay constant = " << std::setprecision(4) << spectrum_fit->GetParameter(1) << " #pm " << std::setprecision(2) << spectrum_fit->GetParError(1) << " keV";
  std::cout << text.str() << std::endl;
  TLatex* spectrum_text = new TLatex(18000, 1000, text.str().c_str());
  spectrum_text->SetTextAlign(22);
  spectrum_text->Draw();

  double error;
  double n_protons = hEnergy->IntegralAndError(min_energy_bin, max_energy_bin, error);

  std::cout << "N Stopped Muons = " << n_stopped_muons << std::endl;
  std::cout << "Fraction Captured = " << capture_fraction << std::endl;
  std::cout << "N Captured Muons = " << n_captured_muons << std::endl;
  std::cout << "Fraction Lost In Gate = " << fraction_lost_in_gate << std::endl;
  std::cout << "N Captured Muons In Gate = " << n_captured_muons_in_gate << std::endl;
  std::cout << "N Protons = " << n_protons << " +/- " << error << std::endl;
  double R = n_protons / n_captured_muons_in_gate;
  double error_R = R*(error/n_protons);
  std::cout << "=> R = " << R << " +/- " << error_R << std::endl;
  
  text.str("");
  text << "#splitline{N_{protons, 3-15 MeV} = " << std::setprecision(5) << n_protons << " #pm " << error << "}{Rate = " << R << " #pm " << error_R << "}";
																		      std::cout << text.str() << std::endl;
  TLatex* rate_text = new TLatex(18000, 400, text.str().c_str());
  rate_text->SetTextAlign(22);
  rate_text->Draw();

}
