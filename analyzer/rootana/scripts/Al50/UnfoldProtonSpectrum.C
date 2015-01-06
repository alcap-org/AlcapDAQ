void UnfoldProtonSpectrum() {

  TFile* response_file = new TFile("base_Al50_1.07_collimated_2014-12-30_response.root", "READ");
  TFile* data_file = new TFile("result.root", "UPDATE");

  const int n_arms = 2;
  std::string arm_names[n_arms] = {"SiL", "SiR"};

  for (int i_arm = 0; i_arm < n_arms; ++i_arm) {
    std::string arm_name = arm_names[i_arm];
    
    std::string response_name = arm_name + "_response";
    RooUnfoldResponse* response = (RooUnfoldResponse*) response_file->Get(response_name.c_str());

    std::string folded_spectrum_name = arm_name + "_EvdE_px";
    TH1D* folded_spectrum = (TH1D*) data_file->Get(folded_spectrum_name.c_str());
  }
}
