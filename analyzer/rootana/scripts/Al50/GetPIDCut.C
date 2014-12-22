void GetPIDCut() {

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetLogz(1);

  std::string version = "v66";
  std::string savelocation = "~/data/out/"+version+"/plots";
  const int n_arms = 2;

  std::string filename = "~/data/out/"+version+"/total.root";
  TFile* file = new TFile(filename.c_str(), "READ");
  TH2F* SiL_EvdE = (TH2F*) file->Get("TME_EvdE/all_particles/SiL_EvdE");
  TH2F* SiR_EvdE = (TH2F*) file->Get("TME_EvdE/all_particles/SiR_EvdE");

  TH2F* evde_hists[n_arms] = {SiL_EvdE, SiR_EvdE};
  std::string arm_names[n_arms] = {"SiL", "SiR"};

  //  for (int i_arm = 0; i_arm < n_arms; ++i_arm) {

  for (int i_bin = 1; i_bin <= evde_hists[0]->GetNbinsX(); ++i_bin) {
    for (int j_bin = 1; j_bin <= evde_hists[0]->GetNbinsY(); ++j_bin) {
      
      if (evde_hists[0]->GetBinContent(i_bin, j_bin) < 20) {
	evde_hists[0]->SetBinContent(i_bin, j_bin, 0);
      }
    }
  }
    evde_hists[0]->Draw("COLZ");
    //}

}
