void ProtonBandGraphicalCut() {

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetLogz(1);

  std::string version = "v69";
  std::string savelocation = "~/data/out/"+version+"/plots";
  const int n_arms = 2;

  std::string filename = "~/data/out/"+version+"/total.root";
  TFile* file = new TFile(filename.c_str(), "READ");
  TH2F* SiL_EvdE = (TH2F*) file->Get("TME_EvdE/all_particles/SiL_EvdE");
  TH2F* SiR_EvdE = (TH2F*) file->Get("TME_EvdE/all_particles/SiR_EvdE");

  TH2F* evde_hists[n_arms] = {SiL_EvdE, SiR_EvdE};
  std::string arm_names[n_arms] = {"SiL", "SiR"};

  for (int i_arm = 0; i_arm < n_arms; ++i_arm) {
  
    // straight line cut to remove electrons
    double x_1 = 0, y_1 = 2000, x_2 = 4000, y_2 = 0;
    double gradient = (y_2 - y_1) / (x_2 - x_1);
    double offset = y_1;

    // Cut to remove the remaining deuteron band
    TF1* fit_fn = new TF1("fit_fn", "[0]*TMath::Exp([1]*x) + [2]", 0, 15000);
    fit_fn->SetLineColor(kBlue);
    fit_fn->SetParameter(0, 4500);
    fit_fn->SetParameter(1, -0.0004);
    fit_fn->SetParameter(2, 500);
    //    evde_hists[i_arm]->Fit(fit_fn, "R");

    for (int i_bin = 1; i_bin <= evde_hists[i_arm]->GetNbinsX(); ++i_bin) {
      for (int j_bin = 1; j_bin <= evde_hists[i_arm]->GetNbinsY(); ++j_bin) {
	double x_coord = evde_hists[i_arm]->GetXaxis()->GetBinCenter(i_bin);
	double y_coord = evde_hists[i_arm]->GetYaxis()->GetBinCenter(j_bin);
	
	if (evde_hists[i_arm]->GetBinContent(i_bin, j_bin) < 20 ||
	    y_coord < gradient*x_coord + offset || 
	    y_coord < 300 ||
	    y_coord > fit_fn->Eval(x_coord) ) {
	  evde_hists[i_arm]->SetBinContent(i_bin, j_bin, 0);
	}
      }
    }
    evde_hists[i_arm]->Draw("COLZ");
    fit_fn->Draw("LSAME");
  }
}
