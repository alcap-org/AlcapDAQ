void FindDetectorSmear() {

  TFile* data_file = new TFile("result_Al50_data.root", "READ");
  TFile* mc_file = new TFile("result_Al50_mc.root", "READ");

  TH1F* SiL_profile_data = (TH1F*) data_file->Get("hProfile_SiL_data");
  TH1F* SiL_profile_mc = (TH1F*) mc_file->Get("hProfile_SiL_mc");

  int n_bins = SiL_profile_data->GetNbinsX();

  for (int i_bin = 1; i_bin <= n_bins; ++i_bin) {
    double energy = SiL_profile_data->GetXaxis()->GetBinLowEdge(i_bin);
    double data_rms = SiL_profile_data->GetBinError(i_bin);
    double mc_rms = SiL_profile_mc->GetBinError(i_bin);

    if (data_rms != 0 && mc_rms != 0 && data_rms > mc_rms) {
      double det_rms = std::sqrt(data_rms*data_rms - mc_rms*mc_rms);
      std::cout << energy << ": data_rms = " << data_rms << ", mc_rms = " << mc_rms << ", det_rms = " << det_rms << std::endl;
    }
  }
}
