void UnfoldProtonSpectrum() {

  gROOT->ProcessLine(".L scripts/Unfolding/bayesian-unfolding/libRooUnfold.so");

  /////////////////////////////////////
  // User parameters
  std::string data_filename = "~/data/results/Si16b_passive/EvdEPlots.root";
  std::string histname = "hEvdE_Si16b_Proton_Veto";
  std::string response_filename = "~/data/mc/Si16b_passive/R15b_Si16b_full-dir-response-matrix.root";
  std::string outfilename = "~/data/results/Si16b_passive/unfolded.root";

  TCanvas* c1 = new TCanvas("c1", "c1");
  //  c1->Divide(2);

  TFile* response_file = new TFile(response_filename.c_str(), "READ");
  TFile* data_file = new TFile(data_filename.c_str(), "READ");
    
  const int n_arms = 1;
  std::string arm_names[n_arms] = {"SiR"};
  int rebin_factor = 5;

  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  
  for (int i_arm = 0; i_arm < n_arms; ++i_arm) {
    c1->cd(i_arm+1);
    //    c1->GetPad(i_arm+1)->SetLogy();

    std::string arm_name = arm_names[i_arm];
    
    std::string response_name = arm_name + "_response";
    RooUnfoldResponse* response = (RooUnfoldResponse*) response_file->Get(response_name.c_str()); // what I'm trying to reconstruct


    TH2F* hEvdE_proton = (TH2F*) data_file->Get(histname.c_str());
    TH1D* folded_spectrum = hEvdE_proton->ProjectionX();
    folded_spectrum->SetName("hFoldedSpectrum");
    folded_spectrum->Rebin(10);

    TH2D* response_matrix = response->Hresponse();
    
    RooUnfoldBayes unfold (response, folded_spectrum);

    TH1D* unfolded_spectrum = (TH1D*) unfold.Hreco();
    unfolded_spectrum->SetName("hUnfoldedSpectrum");
    unfolded_spectrum->Draw("HIST E SAMES");
    unfolded_spectrum->SetLineColor(kBlue);
    unfolded_spectrum->SetLineWidth(2);
    folded_spectrum->SetLineColor(kRed);
    folded_spectrum->SetLineWidth(2);
    folded_spectrum->Draw("HIST E SAMES");
    unfold.PrintTable(cout);

    double integral_min = 2000;
    double integral_max = 15000;
    int integral_bin_min = unfolded_spectrum->FindBin(integral_min);
    int integral_bin_max = unfolded_spectrum->FindBin(integral_max);
    double integral, error;
    integral = unfolded_spectrum->IntegralAndError(integral_bin_min, integral_bin_max, error);
    std::cout << "Integral (" << integral_min << " -- " << integral_max << " keV): " << integral << " +- " << error << std::endl;

    integral_min = 3000;
    integral_max = 10000;
    integral_bin_min = unfolded_spectrum->FindBin(integral_min);
    integral_bin_max = unfolded_spectrum->FindBin(integral_max);
    integral = unfolded_spectrum->IntegralAndError(integral_bin_min, integral_bin_max, error);
    std::cout << "Integral (" << integral_min << " -- " << integral_max << " keV): " << integral << " +- " << error << std::endl;

    integral_min = 4000;
    integral_max = 8000;
    integral_bin_min = unfolded_spectrum->FindBin(integral_min);
    integral_bin_max = unfolded_spectrum->FindBin(integral_max);
    integral = unfolded_spectrum->IntegralAndError(integral_bin_min, integral_bin_max, error);
    std::cout << "Integral (" << integral_min << " -- " << integral_max << " keV): " << integral << " +- " << error << std::endl;


    outfile->cd();
    response->Write();
    hEvdE_proton->Write();
    folded_spectrum->Write();
    unfolded_spectrum->Write();
  }

  outfile->Write();
  outfile->Close();
}
