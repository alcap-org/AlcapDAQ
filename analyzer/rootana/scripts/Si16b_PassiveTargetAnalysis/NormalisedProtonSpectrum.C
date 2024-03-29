void NormalisedProtonSpectrum() {

  ////////////////////////
  // User parameters
  std::string unfolded_filename = "~/data/results/Si16b_passive/unfolded-wVeto_NewCuts.root";
  std::string unfolded_histname = "hUnfoldedSpectrum";
  std::string mustop_filename = "~/data/results/Si16b_passive/CountStoppedMuons_GeLoGain_fromTMEs_NewCuts.root";
  std::string mustop_treename = "mustops";
  std::string outfilename = "~/data/results/Si16b_passive/final-plots_NewCuts.root";
  int rebin_factor = 2;

  TFile* unfolded_file = new TFile(unfolded_filename.c_str(), "READ");
  TH1F* hUnfoldedSpectrum = (TH1F*) unfolded_file->Get(unfolded_histname.c_str());

  TFile* mustop_file = new TFile(mustop_filename.c_str(), "READ");
  TTree* mustop_tree = (TTree*) mustop_file->Get(mustop_treename.c_str());

  double capture_fraction = 0.658;
  double n_stopped_muons = 0;
  double n_stopped_muons_error = 0;
  mustop_tree->SetBranchAddress("n_stopped_muons", &n_stopped_muons);
  mustop_tree->SetBranchAddress("n_stopped_muons_error", &n_stopped_muons_error);
  mustop_tree->GetEntry(0);

  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");

  TH1F* hNormalisedSpectrum = (TH1F*) hUnfoldedSpectrum->Clone("hNormalisedSpectrum");
  hNormalisedSpectrum->Scale(1.0 / (capture_fraction*n_stopped_muons));
  hNormalisedSpectrum->SetYTitle("Protons / capture / keV");
  hNormalisedSpectrum->Rebin(rebin_factor);
  hNormalisedSpectrum->Scale(1.0 / hNormalisedSpectrum->GetXaxis()->GetBinWidth(1)); // bin_width

  hNormalisedSpectrum->GetXaxis()->SetRangeUser(0, 15000);
  hNormalisedSpectrum->Draw("HIST E");

  double integral_min = 2000;
  double integral_max = 15000;
  int integral_bin_min = hNormalisedSpectrum->FindBin(integral_min);
  int integral_bin_max = hNormalisedSpectrum->FindBin(integral_max);
  double integral, error;
  integral = hNormalisedSpectrum->IntegralAndError(integral_bin_min, integral_bin_max, error, "width");
  std::cout << "Integral (" << integral_min << " -- " << integral_max << " keV): " << integral << " +- " << error << std::endl;
  
  integral_min = 3000;
  integral_max = 10000;
  integral_bin_min = hNormalisedSpectrum->FindBin(integral_min);
  integral_bin_max = hNormalisedSpectrum->FindBin(integral_max);
  integral = hNormalisedSpectrum->IntegralAndError(integral_bin_min, integral_bin_max, error, "width");
  std::cout << "Integral (" << integral_min << " -- " << integral_max << " keV): " << integral << " +- " << error << std::endl;
  
  integral_min = 4000;
  integral_max = 8000;
  integral_bin_min = hNormalisedSpectrum->FindBin(integral_min);
  integral_bin_max = hNormalisedSpectrum->FindBin(integral_max);
  integral = hNormalisedSpectrum->IntegralAndError(integral_bin_min, integral_bin_max, error, "width");
  std::cout << "Integral (" << integral_min << " -- " << integral_max << " keV): " << integral << " +- " << error << std::endl;
  
 

  /*  TF1* proton_spec_fit = new TF1("proton_spec_fit", "[0]*(1 - [1]/x)^[2] * TMath::Exp(-x/[3])", 2000, 15000);
  proton_spec_fit->SetParameters(30e-6, 2000, 0.1, 5000);
  proton_spec_fit->SetParLimits(1, 1000, 2000);
  proton_spec_fit->SetParLimits(2, -1, 1);
  hNormalisedSpectrum->Fit(proton_spec_fit, "R");
  proton_spec_fit->Draw("LSAME");
  
  std::cout << proton_spec_fit->Integral(2000, 10000) << std::endl;
  */
  hNormalisedSpectrum->Write();
  outfile->Write();
  //  outfile->Close();		    
}
