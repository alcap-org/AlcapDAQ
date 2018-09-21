void UnfoldAttempts() {

  gROOT->ProcessLine(".L scripts/Unfolding/bayesian-unfolding/libRooUnfold.so");
  gROOT->ProcessLine(".L scripts/SiL3_ActiveTargetAnalysis/RooProtonESpectrum.cxx+");
  
  //  std::string filename = "scripts/SiL3_ActiveTargetAnalysis/hist_files/PlotsFromTMETree_Run9040.root";
  //  std::string filename = "scripts/SiL3_ActiveTargetAnalysis/hist_files/PlotsFromTMETree_TwoRuns.root";
  //  std::string filename = "scripts/SiL3_ActiveTargetAnalysis/hist_files/PlotsFromTMETree_Run9041_test.root";
  //  std::string filename = "scripts/SiL3_ActiveTargetAnalysis/hist_files/PlotsFromTMETree_Run9041_noTemplates.root";
  //  std::string filename = "scripts/SiL3_ActiveTargetAnalysis/hist_files/PlotsFromTMETree_Run9041_Large.root";
  //  std::string filename = "scripts/SiL3_ActiveTargetAnalysis/hist_files/PlotsFromTMETree_Run9041_Small.root";
  //  std::string filename = "scripts/SiL3_ActiveTargetAnalysis/hist_files/PlotsFromTMETree_Run9041_Smaller.root";
  //  std::string filename = "scripts/SiL3_ActiveTargetAnalysis/hist_files/PlotsFromTMETree_Run9041_Smaller-Rebin.root";
  //  std::string filename = "scripts/SiL3_ActiveTargetAnalysis/hist_files/PlotsFromTMETree_TwoRuns_Range-0-10MeV_Binning-100keV.root";
  //  std::string filename = "scripts/SiL3_ActiveTargetAnalysis/hist_files/PlotsFromTMETree_TwoRuns_Range-0-100MeV_Binning-100keV.root";
  //  std::string filename = "scripts/SiL3_ActiveTargetAnalysis/hist_files/PlotsFromTMETree_TwoRuns_Range-0-20MeV_Binning-100keV.root";
  //  std::string filename = "scripts/SiL3_ActiveTargetAnalysis/hist_files/PlotsFromTMETree_TwoRuns_Range-0-100MeV_Binning-500keV.root";
  std::string filename = "scripts/SiL3_ActiveTargetAnalysis/hist_files/PlotsFromTMETree_Run9040_Range-0-100MeV_Binning-100keV.root";
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
  double min_time = 2000;
  double max_time = 4000;

  TF1* lifetime_fit = new TF1("lifetime_fit", "[0]*TMath::Exp(-x/[1]) + [2]", 1000,15000);
  lifetime_fit->SetParameters(1.33523e+04, 7.71474e+02, 1.45094e+00);
  lifetime_fit->SetParError(0, 2.05129e+02);
  lifetime_fit->SetParError(1, 4.79208e+00);
  lifetime_fit->SetParError(2, 1.81347e-01);

  double fraction_alive_after_min_time = lifetime_fit->Eval(min_time) / lifetime_fit->GetParameter(0);
  double fraction_alive_after_max_time = lifetime_fit->Eval(max_time) / lifetime_fit->GetParameter(0);
  double fraction_lost_in_gate = fraction_alive_after_min_time - fraction_alive_after_max_time;
  int n_captured_muons_in_gate = fraction_lost_in_gate*n_captured_muons;

  //  std::string corr_filename = "scripts/SiL3_ActiveTargetAnalysis/hist_files/R15b_SiL3-ActiveTargetAnalysis_MC_Corrections.root";
  //  std::string corr_filename = "scripts/SiL3_ActiveTargetAnalysis/hist_files/R15b_SiL3-ActiveTargetAnalysis_MC-expo_Corrections.root";
  //  std::string corr_filename = "scripts/SiL3_ActiveTargetAnalysis/hist_files/R15b_SiL3-ActiveTargetAnalysis_MC-large_Corrections.root";
  //  std::string corr_filename = "scripts/SiL3_ActiveTargetAnalysis/hist_files/R15b_SiL3-ActiveTargetAnalysis_MC-small_Corrections.root";
  //  std::string corr_filename = "scripts/SiL3_ActiveTargetAnalysis/hist_files/R15b_SiL3-ActiveTargetAnalysis_MC-smaller_Corrections.root";
  //  std::string corr_filename = "scripts/SiL3_ActiveTargetAnalysis/hist_files/R15b_SiL3-ActiveTargetAnalysis_MC-expo-smaller_Corrections.root";
  //  std::string corr_filename = "scripts/SiL3_ActiveTargetAnalysis/hist_files/R15b_SiL3-ActiveTargetAnalysis_MC-smaller-rebin_Corrections.root";
  //  std::string corr_filename = "scripts/SiL3_ActiveTargetAnalysis/hist_files/R15b_SiL3-ActiveTargetAnalysis_MC-large-rebin_Corrections.root";
  //  std::string corr_filename = "scripts/SiL3_ActiveTargetAnalysis/hist_files/R15b_SiL3-ActiveTargetAnalysis_MC-Protons-100MeV_Range-0-10-MeV_Binning-100keV.root";
  //  std::string corr_filename = "scripts/SiL3_ActiveTargetAnalysis/hist_files/R15b_SiL3-ActiveTargetAnalysis_MC-Protons-100-MeV_Range-0-100-MeV_Binning-100keV.root";
  //  std::string corr_filename = "scripts/SiL3_ActiveTargetAnalysis/hist_files/R15b_SiL3-ActiveTargetAnalysis_MC-Protons-100-MeV_Range-0-20-MeV_Binning-100keV.root";
  //  std::string corr_filename = "scripts/SiL3_ActiveTargetAnalysis/hist_files/R15b_SiL3-ActiveTargetAnalysis_MC-Protons-100-MeV-3mm_Range-0-100-MeV_Binning-100keV.root";
  //  std::string corr_filename = "scripts/SiL3_ActiveTargetAnalysis/hist_files/R15b_SiL3-ActiveTargetAnalysis_MC-Protons-100-MeV_Range-0-100-MeV_Binning-500keV.root";
  //  std::string corr_filename = "scripts/SiL3_ActiveTargetAnalysis/hist_files/R15b_SiL3-ActiveTargetAnalysis_MC-Protons-100-MeV_Range-0-50-MeV_Binning-100keV.root";
  std::string corr_filename = "scripts/SiL3_ActiveTargetAnalysis/hist_files/R15b_SiL3-ActiveTargetAnalysis_MC-Protons-100-MeV_Range-0-100-MeV_Binning-100keV.root";
  //  std::string corr_filename = "scripts/SiL3_ActiveTargetAnalysis/hist_files/R15b_SiL3-ActiveTargetAnalysis_MC-Protons-100-MeV-Expo_Range-0-50-MeV_Binning-100keV.root";
  //    std::string corr_filename = "scripts/SiL3_ActiveTargetAnalysis/hist_files/R15b_SiL3-ActiveTargetAnalysis_MC-Protons-100-MeV-Large-Expo_Range-0-100-MeV_Binning-100keV.root";
  TFile* corr_file = new TFile(corr_filename.c_str(), "READ");
  TH1F* hEDep_muplus = (TH1F*) corr_file->Get("hEDep_muplus");
  hEDep_muplus->Rebin(rebin_factor);
  hEDep_muplus->Scale( (1-capture_fraction)*fraction_lost_in_gate*n_stopped_muons );
  
  int min_time_bin = hTimeEnergy->GetXaxis()->FindBin(min_time);
  int max_time_bin = hTimeEnergy->GetXaxis()->FindBin(max_time);
  TH1D* hEnergy = hTimeEnergy->ProjectionY("hEnergy", min_time_bin, max_time_bin);

  double min_time_flat_bkg = 10000+min_time;
  double max_time_flat_bkg = 10000+max_time;
  int min_time_flat_bkg_bin = hTimeEnergy->GetXaxis()->FindBin(min_time_flat_bkg);
  int max_time_flat_bkg_bin = hTimeEnergy->GetXaxis()->FindBin(max_time_flat_bkg);

  TH1D* hFlatBackground = hTimeEnergy->ProjectionY("hFlatBkg", min_time_flat_bkg_bin, max_time_flat_bkg_bin);
  hFlatBackground->Rebin(rebin_factor);
  
  TH1D* hEnergyCorrected = (TH1D*) hEnergy->Clone("hEnergyCorrected");
  hEnergyCorrected->SetLineColor(kMagenta);
  hEnergyCorrected->Add(hEDep_muplus, -1);
  hEnergyCorrected->Add(hFlatBackground, -1);
  hEnergy->Draw("HIST");
  hEDep_muplus->Draw("HIST SAME");
  hFlatBackground->Draw("HIST SAME");
  hEnergyCorrected->Draw("HIST SAME");

  double min_energy_check = 400;
  double max_energy_check = 3000;
  int min_energy_check_bin = hEnergy->FindBin(min_energy_check);
  int max_energy_check_bin = hEnergy->FindBin(max_energy_check);
  std::cout << "AE: muplus / total (E<3 MeV) = " << hEDep_muplus->Integral(min_energy_check_bin, max_energy_check_bin) << " / " << hEnergy->Integral(min_energy_check_bin, max_energy_check_bin) << " = " << hEDep_muplus->Integral(min_energy_check_bin, max_energy_check_bin) / hEnergy->Integral(min_energy_check_bin, max_energy_check_bin) << std::endl;
  std::cout << "AE: muplus / total (E>3 MeV) = " << hEDep_muplus->Integral(max_energy_check_bin, hEnergy->GetNbinsX()) << " / " << hEnergy->Integral(max_energy_check_bin, hEnergy->GetNbinsX()) << " = " << hEDep_muplus->Integral(max_energy_check_bin, hEnergy->GetNbinsX()) / hEnergy->Integral(max_energy_check_bin, hEnergy->GetNbinsX()) << std::endl;


  
  TCanvas* c_proton_attempt = new TCanvas("c_proton_attempt", "c_proton_attempt");
  c_proton_attempt->SetLogy();
  hEnergyCorrected->Draw("HIST E SAME");

  TH1F* hStoppingFraction = (TH1F*) corr_file->Get("hStoppingFraction_True");
  /*  TH1F* hTestSpectrum = (TH1F*) hEnergyCorrected->Clone("hTestSpectrum");
  hTestSpectrum->Divide(hStoppingFraction);
  hTestSpectrum->SetLineColor(kCyan);
  hTestSpectrum->Draw("HIST E SAME");

  double n_moved = hTestSpectrum->Integral() - hEnergyCorrected->Integral();
  std::cout << "AE: n_moved = " << n_moved << std::endl;
  TH1F* hSubSpectrum = (TH1F*) hEnergyCorrected->Clone("hSubSpectrum");
  hSubSpectrum->Reset();
  hSubSpectrum->Fill(1500, n_moved);
  hTestSpectrum->Add(hSubSpectrum, -1);
  */ 
  /*  TH1D* hEnergyStopped = (TH1D*) hEnergyCorrected->Clone("hEnergyStopped");
  hEnergyStopped->SetLineColor(kCyan);
  hEnergyStopped->Draw("HIST SAME");

  TH1D* hEnergyNotStopped = (TH1D*) hEnergyCorrected->Clone("hEnergyNotStopped");
  hEnergyNotStopped->Add(hEnergyStopped, -1);
  hEnergyNotStopped->SetLineColor(kGray);
  hEnergyNotStopped->Draw("HIST SAME");
  */
  RooUnfoldResponse* response = (RooUnfoldResponse*) corr_file->Get("SiL3_stop_response");
  //  RooUnfoldBayes unfold (response, hEnergyNotStopped);
  RooUnfoldBayes unfold (response, hEnergyCorrected, 10);
  //  unfold.SetVerbose(5);
  TH1D* hEnergyUnfolded = (TH1D*) unfold.Hreco(RooUnfold::kCovariance);
  unfold.PrintTable(std::cout, 0, RooUnfold::kCovariance);
  hEnergyUnfolded->SetLineColor(kRed);
  hEnergyUnfolded->Draw("HIST E SAME");

  std::cout << "AE: Corrected Integral = " << hEnergyCorrected->Integral() << std::endl;
  std::cout << "AE: Unfolded Integral = " << hEnergyUnfolded->Integral() << std::endl;
  //  hEnergyUnfolded->Rebin(2);
  
  /*  TF1* proton_fit = new TF1("proton_fit", "[0]*(1 - [1]/x)^[2] * TMath::Exp(-x/[3])", 1000, 10000);
  //  TF1* proton_fit = new TF1("proton_fit", "[0]*TMath::Exp(-x/[1])", 8000, 10000);
  proton_fit->SetParameters(hEnergyUnfolded->GetMaximum(), 1400, 1.1, 2000);
  hEnergyUnfolded->Fit(proton_fit, "R");
  proton_fit->Draw("LSAME");
  */
  TCanvas* c_fit = new TCanvas("c_fit", "c_fit");
  RooWorkspace w("w");
  w.factory("E[20000,25000]");
  w.factory("ProtonESpectrum::spectrum(E, T0[500,2500], k[1.1,1.0,20], Q[4000,1000,8000])");
  RooDataHist hist("SiL3Energy", "SiL3 Energy", (w.argSet("E")), hEnergyCorrected);
  w.import(hist);
  //  w.pdf("spectrum")->fitTo(hist, RooFit::SumW2Error(kTRUE));

  RooPlot* plot = w.var("E")->frame(RooFit::Title("SiL3 Energy"));
  w.data("SiL3Energy")->plotOn(plot, RooFit::MarkerColor(kBlack), RooFit::DataError(RooAbsData::Poisson));
  //  w.pdf("spectrum")->plotOn(plot, RooFit::LineColor(kRed));
  plot->Draw();
  

  //  double total_integral = hEnergyUnfolded->Integral();
  //  std::cout << "Total Integral = " << total_integral << std::endl;
  //  std::cout << "Total Rate = " << total_integral / n_captured_muons_in_gate << std::endl;


  // My guess at a method
  TCanvas* c_new = new TCanvas("c_new", "c_new");
  c_new->Divide(2,2);
  //TH1F* hRawSpectrum = w.data("SiL3Energy")->createHistogram("E");//(TH1F*) hEnergyCorrected->Clone("hRawSpectrum");
  TH1F* hRawSpectrum = (TH1F*) hEnergyCorrected->Clone("hRawSpectrum");
  c_new->cd(1);
  c_new->GetPad(1)->SetLogy();
  hRawSpectrum->Draw("HIST");
  TF1* raw_fit = new TF1("raw_fit", "[0]*TMath::Exp(-x/[1])", 10000, 50000);
  raw_fit->SetParameters(100, 5000);
  hRawSpectrum->Fit(raw_fit, "R");
  
  TH1F* hNewSpectrum = (TH1F*) hRawSpectrum->Clone("hNewSpectrum");
  TH1F* hAddSpectrum = (TH1F*) hNewSpectrum->Clone("hAddSpectrum");
  hAddSpectrum->Reset();

  TH2F* hResponseMatrix = response->Hresponse();
  int counter = 0;
  std::cout << " E [keV]    N_stop (raw)    N_stop (new)    Stop Frac.     N_total      N_not_stopped   " << std::endl;
  for (int i_bin = hNewSpectrum->GetNbinsX(); i_bin >= 1; --i_bin) {
    hAddSpectrum->Reset();
    
    double bin_center = hNewSpectrum->GetXaxis()->GetBinCenter(i_bin);
    double n_stopped = hNewSpectrum->GetBinContent(i_bin);
    if (n_stopped < 1) {
      //      n_stopped = 1;//raw_fit->Eval(bin_center);
      continue;
    }
    double n_stopped_error = hNewSpectrum->GetBinError(i_bin);
    double n_raw_stopped = hRawSpectrum->GetBinContent(i_bin);
    double n_raw_stopped_error = hRawSpectrum->GetBinError(i_bin);

    double stopping_fraction = hStoppingFraction->GetBinContent(i_bin);
    double stopping_fraction_error = hStoppingFraction->GetBinError(i_bin);
    if (stopping_fraction > 0.95) {
      stopping_fraction = 1.0;
    }
    else if (stopping_fraction <= 0) {
      continue;
    }
    double n_total = n_stopped / stopping_fraction;
    double n_total_error = n_total * (std::sqrt( (n_stopped_error/n_stopped)*(n_stopped_error/n_stopped) + (stopping_fraction_error/stopping_fraction)*(stopping_fraction_error/stopping_fraction) ));
    double n_not_stopped = n_total - n_stopped;
    std::cout << bin_center << "  " << n_raw_stopped << " +/- " << n_raw_stopped_error << "  " << n_stopped << " +/- " << n_stopped_error << "  " << stopping_fraction << " +/- " << stopping_fraction_error << "   " << n_total << " +/- " << n_total_error << "    " << n_not_stopped << "     ";

    int min_proj_bin = hResponseMatrix->GetXaxis()->FindBin(bin_center);
    int max_proj_bin = min_proj_bin;
    TH1D* hEDepDistribution = hResponseMatrix->ProjectionX("_py", min_proj_bin, max_proj_bin);
    //    TF1* mpv_fit = new TF1("mpv_fit", "[0]*TMath::Gaus(x, [1], [2])");
    //    mpv_fit->SetParameters(hEDepDistribution->GetMaximum(), hEDepDistribution->GetBinCenter(hEDepDistribution->GetMaximumBin()), 5);
    //    hEDepDistribution->Fit(mpv_fit, "Q");
    //    double mpv = mpv_fit->GetParameter(1);
    //    double mpv = hEDepDistribution->GetBinCenter(hEDepDistribution->GetMaximumBin());
    //    std::cout << mpv << " keV";
    //    TH1D* hEDep_MPV = (TH1D*) hEDepDistribution->Clone("hEDep_MPV");
    //    hEDep_MPV->Reset();
    //    hEDep_MPV->Fill(mpv, n_not_stopped);
    if (hEDepDistribution->Integral()>0) {
      hEDepDistribution->Scale(1.0/hEDepDistribution->Integral());
      hEDepDistribution->Scale(n_not_stopped);
    }
    //    for (int i_not_stopped = 0; i_not_stopped < n_not_stopped; ++i_not_stopped) {
    //      double edep = hEDepDistribution->GetRandom();
    //      hAddSpectrum->Fill(edep);
    //      //      std::cout << edep << std::endl;
    //    }
    //    hNewSpectrum->Add(hAddSpectrum, -1);
    //    hAddSpectrum->SetBinContent(i_bin, n_not_stopped);
    c_new->cd(3);
    //    hEDepDistribution->Draw("HIST");
    hAddSpectrum->Draw("HIST");
    hNewSpectrum->Add(hEDepDistribution, -1);
    //    hNewSpectrum->Add(hEDep_MPV, -1);
    std::cout << std::endl;
  }
  //  hNewSpectrum->Add(hAddSpectrum, -1);
  c_new->cd(2);
  hStoppingFraction->Draw("HIST");
  c_new->cd(4);
  hNewSpectrum->Draw("HIST");
}
