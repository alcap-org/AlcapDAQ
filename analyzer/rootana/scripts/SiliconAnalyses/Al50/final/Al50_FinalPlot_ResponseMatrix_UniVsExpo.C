void Al50_FinalPlot_ResponseMatrix_UniVsExpo() {

  const int n_settings = 2;
  std::string filenames[n_settings] = {"~/g4sim/alcap/respMatrix_100M_Al50_proton_100keVBins.root", "~/g4sim/alcap/respMatrix_100M_Al50_proton_100keVBins_expo.root"};

  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
    TFile* file = new TFile(filenames[i_setting].c_str(), "READ");
    RooUnfoldResponse* response = (RooUnfoldResponse*) file->Get("SiR_three_layer_response");
    response->Hmeasured()->Rebin(5)->Draw("HIST SAME");
  }
  
}
