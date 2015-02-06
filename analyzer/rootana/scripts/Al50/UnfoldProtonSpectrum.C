void UnfoldProtonSpectrum() {

  TFile* response_file = new TFile("Al50_1.07_target-hist_2015-01-15_response_stopped-only_10M-thnsparse_500keV-bins.root", "READ");
  TFile* data_file = new TFile("result_Al50.root", "READ");
  TFile* out_file = new TFile("unfolded.root", "RECREATE");

  const int n_arms = 2;
  std::string arm_names[n_arms] = {"SiL", "SiR"};
  int rebin_factor = 5;

  for (int i_arm = 0; i_arm < n_arms; ++i_arm) {
    std::string arm_name = arm_names[i_arm];
    
    std::string response_name = arm_name + "_response";
    RooUnfoldResponse* response = (RooUnfoldResponse*) response_file->Get(response_name.c_str());

    std::string folded_spectrum_name = arm_name + "_EvdE_px";
    TH1D* folded_spectrum = (TH1D*) data_file->Get(folded_spectrum_name.c_str());
    folded_spectrum->Rebin(rebin_factor);
    //    response->Hresponse()->Draw("COLZ");


    
    double low_energy_cutoff = 0; // keV
    double high_energy_cutoff = 20000; // keV

    // set all bins outside of this range to 0
    for (int i_bin = 0; i_bin < folded_spectrum->GetNbinsX(); ++i_bin) {
      double bin_centre = folded_spectrum->GetXaxis()->GetBinCenter(i_bin);
      if (bin_centre < low_energy_cutoff || bin_centre > high_energy_cutoff) {
	folded_spectrum->SetBinContent(i_bin, 0);
      }
    }
    for (int i_bin = 0; i_bin < response->Hresponse()->GetNbinsX(); ++i_bin) {
      double bin_centre_x = response->Hresponse()->GetXaxis()->GetBinCenter(i_bin);

      for (int j_bin = 0; j_bin < response->Hresponse()->GetNbinsY(); ++j_bin) {
	double bin_centre_y = response->Hresponse()->GetYaxis()->GetBinCenter(j_bin);
	if ( (bin_centre_x < low_energy_cutoff || bin_centre_x > high_energy_cutoff)){//
	  //	     (bin_centre_y < low_energy_cutoff || bin_centre_y > high_energy_cutoff)) {
	  response->Hresponse()->SetBinContent(i_bin, j_bin, 0);
	}
      }
    }
    

    //folded_spectrum->GetXaxis()->SetRangeUser(low_energy_cutoff, high_energy_cutoff);
    //response->Hresponse()->GetXaxis()->SetRangeUser(low_energy_cutoff, high_energy_cutoff);

    RooUnfoldBayes unfold (response, folded_spectrum);
    //    response->Hresponse()->Draw("COLZ");
    TH1D* unfolded_spectrum = (TH1D*) unfold.Hreco();
    unfolded_spectrum->GetXaxis()->SetRangeUser(low_energy_cutoff, high_energy_cutoff);
    unfolded_spectrum->Draw("HIST E");
    folded_spectrum->SetLineColor(kRed);
    folded_spectrum->Draw("HIST E SAME");
    unfold.PrintTable(cout);

    double energy_range_low = 4000;
    double energy_range_high = 8000;
    int bin_low = unfolded_spectrum->FindBin(energy_range_low);
    int bin_high = unfolded_spectrum->FindBin(energy_range_high);
    double error;
    std::cout << arm_names[i_arm] << " Proton Integral (" << energy_range_low << " - " << energy_range_high << " keV) = " << unfolded_spectrum->IntegralAndError(bin_low, bin_high, error) << " +- " << error << std::endl;

    response->Write();
    unfold.Write();
    unfolded_spectrum->Write();
  }
  out_file->Close();
}
