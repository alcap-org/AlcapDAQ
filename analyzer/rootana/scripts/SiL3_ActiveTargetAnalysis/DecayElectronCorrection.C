void DecayElectronCorrection() {

  std::string filename = "scripts/SiL3_ActiveTargetAnalysis/hist_files/PlotsFromTMETree_Run9040_Range-0-100MeV_Binning-100keV_NewCalib.root";
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
  std::cout << "Fraction alive after " << min_time << " ns = " << fraction_alive_after_min_time << std::endl;
  std::cout << "Fraction alive after " << max_time << " ns = " << fraction_alive_after_max_time << std::endl;
  std::cout << "Fraction lost in gate = " << fraction_lost_in_gate << std::endl;

  //  std::string corr_filename = "scripts/SiL3_ActiveTargetAnalysis/hist_files/R15b_SiL3-ActiveTargetAnalysis_MC-Protons-100-MeV_Range-0-100-MeV_Binning-100keV.root";
    std::string corr_filename = "scripts/SiL3_ActiveTargetAnalysis/hist_files/test.root";
  TFile* corr_file = new TFile(corr_filename.c_str(), "READ");
  TH1F* hEDep_muplus = (TH1F*) corr_file->Get("hEDep_muplus");
  hEDep_muplus->Rebin(rebin_factor);
  hEDep_muplus->Scale( (1-capture_fraction)*fraction_lost_in_gate*n_stopped_muons );
  
  int min_time_bin = hTimeEnergy->GetXaxis()->FindBin(min_time);
  int max_time_bin = hTimeEnergy->GetXaxis()->FindBin(max_time);
  TH1D* hEnergy = hTimeEnergy->ProjectionY("hEnergy", min_time_bin, max_time_bin);

  TH1D* hEnergyCorrected = (TH1D*) hEnergy->Clone("hEnergyCorrected");
  hEnergyCorrected->SetLineColor(kMagenta);
  hEnergyCorrected->Add(hEDep_muplus, -1);
  hEnergy->Draw("HIST");
  hEDep_muplus->Draw("HIST SAME");
  hEnergyCorrected->Draw("HIST SAME");

  double min_energy_check = 300;
  double max_energy_check = 3000;
  int min_energy_check_bin = hEnergy->FindBin(min_energy_check);
  int max_energy_check_bin = hEnergy->FindBin(max_energy_check);
  std::cout << "AE: muplus / total (E<3 MeV) = " << hEDep_muplus->Integral(min_energy_check_bin, max_energy_check_bin) << " / " << hEnergy->Integral(min_energy_check_bin, max_energy_check_bin) << " = " << hEDep_muplus->Integral(min_energy_check_bin, max_energy_check_bin) / hEnergy->Integral(min_energy_check_bin, max_energy_check_bin) << std::endl;
  std::cout << "AE: muplus / total (E>3 MeV) = " << hEDep_muplus->Integral(max_energy_check_bin, hEnergy->GetNbinsX()) << " / " << hEnergy->Integral(max_energy_check_bin, hEnergy->GetNbinsX()) << " = " << hEDep_muplus->Integral(max_energy_check_bin, hEnergy->GetNbinsX()) / hEnergy->Integral(max_energy_check_bin, hEnergy->GetNbinsX()) << std::endl;
}
