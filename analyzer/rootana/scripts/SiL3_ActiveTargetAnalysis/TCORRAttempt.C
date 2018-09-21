void TCORRAttempt() {

  std::string filename = "scripts/SiL3_ActiveTargetAnalysis/hist_files/PlotsFromTMETree_Run9040_Range-0-50MeV_Binning-100keV.root";
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

  std::string corr_filename = "scripts/SiL3_ActiveTargetAnalysis/hist_files/R15b_SiL3-ActiveTargetAnalysis_MC-Protons-100-MeV_Range-0-50-MeV_Binning-100keV.root";
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


  //  RooUnfoldResponse* response = (RooUnfoldResponse*) corr_file->Get("SiL3_stop_response");
  //  TH2F* hResponseMatrix = response->Hresponse();
  TH1F* hStoppingFraction = (TH1F*) corr_file->Get("hStoppingFraction_True");

  TH1F* hPEi = (TH1F*) hStoppingFraction->Clone("hPEi");
  hPEi->Reset();

  double energy_cutoff = 1000;
  double max_energy = 30000;
  double energy_step = 100;
  double min_energy = energy_cutoff + energy_step;
  //  double thickness = 1500*1e-4; // cm
  double thickness = 334*1e-4; // cm

  TTree* tree = new TTree();
  tree->ReadFile("pstar_Si.csv", "Ek/F:S/F", '|');

  tree->Draw("2.3290*S:1000*Ek", "(Ek>1 && Ek<30)", "");
  TF1* fit = new TF1("fit", "expo(0)+expo(2)");
  Graph->Fit(fit);

  double range_cutoff = 1.0/fit->Eval(energy_cutoff);

  std::cout << "   E [keV]    " << "    S [MeV/cm]   " << std::endl;
  for (int i_bin = 1; i_bin <= hPEi->GetNbinsX(); ++i_bin) {

    double i_energy = hPEi->GetBinCenter(i_bin);
    double range = 0;
    for (int j_bin = 1; j_bin <= i_bin; ++j_bin) {
      double j_energy = hPEi->GetBinCenter(j_bin);
      range += 1.0/fit->Eval(j_energy);
    }
    std::cout << "    " << i_energy << "     " << fit->Eval(i_energy) << "     " << range << std::endl;
    if (range < thickness) {
      hPEi->Fill(i_energy, thickness / (range - range_cutoff) );
    }
    else {
      hPEi->Fill(i_energy, 1.0);
    }
  }
  hPEi->Draw("HIST");

  /*  TH2F* hResponse = new TH2F("hResponse", "", 500,0,50000, 500,0,50000);
  for (int i_bin = 1; i_bin < hResponse->GetXaxis()->GetNbins(); ++i_bin) {
    double i_energy = hResponse->GetXaxis()->GetBinCenter(i_bin);
    double bin_width = hResponse->GetXaxis()->GetBinWidth(i_bin);
    double e_i_plus = i_energy + 0.5*bin_width;
    double e_i_minus = i_energy - 0.5*bin_width;

    for (int j_bin = 1; j_bin < hResponse->GetYaxis()->GetNbins(); ++j_bin) {
      double j_energy = hResponse->GetYaxis()->GetBinCenter(j_bin);
	
      double range = 0;
      double k_energy = j_energy;
      while (range < thickness) {
	range += 1.0/fit->Eval(k_energy);
	k_energy += energy_step;
      }
      double delta_e = k_energy - j_energy;
      
      double delta_e_max = max_energy - j_energy;
      double k = delta_e / energy_step;
      
      //      std::cout << i_energy << ", " << j_energy << ", " << e_i_plus << ", " << e_i_minus << ", " << k << std::endl;
      if (e_i_plus < j_energy || e_i_plus > j_energy+delta_e) {
	hResponse->Fill(i_energy, j_energy, 0);
	//	std::cout << "Branch1" << std::endl;
      }
      else if ( (e_i_minus < j_energy && j_energy < e_i_plus) || (e_i_minus < j_energy+delta_e_max && j_energy+delta_e_max < e_i_plus)){
	hResponse->Fill(i_energy, j_energy, (1.0/k) * ( (abs(i_energy - j_energy) / energy_step)) );
	std::cout << j_energy << ", " << i_energy << ", " << (std::fabs(i_energy - j_energy) ) << std::endl;
      }
      else {
	hResponse->Fill(i_energy, j_energy, 1.0/k);
      }
    }
  }

  hResponse->Draw("COLZ");
  */
}
