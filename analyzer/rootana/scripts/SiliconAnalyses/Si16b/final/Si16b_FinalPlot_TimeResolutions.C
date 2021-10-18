void Si16b_FinalPlot_TimeResolutions() {

  std::string filename = "~/data/out/v10/Si16b.root";
  std::string treename = "TMETree/TMETree";
  const int n_central_channels = 4;
  std::string central_channels[n_central_channels] = {"SiT-1-S", "SiT-2-S", "SiT-3-S", "SiT-4-S"};
  std::string central_detnames[n_central_channels] = {"SiT_1", "SiT_2", "SiT_3", "SiT_4"};

  const int n_other_detnames = 9;
  std::string other_detnames[n_other_detnames] = {"SiR2", "SiR3", "SiR1_1", "SiR1_2", "SiR1_3", "SiR1_4", "GeHiGain", "GeLoGain", "SiL3"};
  const int n_other2_detnames = 2;
  std::string other2_detnames[n_other2_detnames] = {"SiL1_8", "SiL1_9"};

  TFile* file = new TFile(filename.c_str(), "READ");
  TTree* tree = (TTree*) file->Get(treename.c_str());

  for (int i_central_channel = 0; i_central_channel < n_central_channels; ++i_central_channel) {
    std::string central_channel = central_channels[i_central_channel];
    std::string central_detname = central_detnames[i_central_channel];

    for (int i_other_detname = 0; i_other_detname < n_other_detnames; ++i_other_detname) {
      std::string other_detname = other_detnames[i_other_detname];

      for (int i_other2_detname = 0; i_other2_detname < n_other2_detnames; ++i_other2_detname) {
	std::string other2_detname = other2_detnames[i_other2_detname];

      
	const int n_pairs = 3;
	std::string det1names[n_pairs] = {other_detname, central_detname, other2_detname};
	std::string det2names[n_pairs] = {central_detname, other2_detname, other_detname};
	// double min_fits[n_pairs] = {-125, -25, -50};
	// double max_fits[n_pairs] = {25, 125, 125};
	double min_fits[n_pairs] = {-100, -50, -50};
	double max_fits[n_pairs] = {50, 100, 50};


  // std::string det1names[n_pairs] = {"SiR2", "SiT_3", "SiR3"};
  // std::string det2names[n_pairs] = {"SiT_3", "SiR3", "SiR2"};
  // double min_fits[n_pairs] = {-125, -25, -50};
  // double max_fits[n_pairs] = {25, 125, 125};

  // std::string det1names[n_pairs] = {"SiR2", "SiT_3", "SiR1_3"};
  // std::string det2names[n_pairs] = {"SiT_3", "SiR1_3", "SiR2"};
  // double min_fits[n_pairs] = {-200, -25, -50};
  // double max_fits[n_pairs] = {25, 125, 50};

	double sigmas[n_pairs] = {0};
	double sigma_errs[n_pairs] = {0};


	TCanvas* c = new TCanvas();
	c->Divide(2,2);
	for (int i_pair = 0; i_pair < n_pairs; ++i_pair) {
	  c->cd(i_pair+1);
	  std::string det1name = det1names[i_pair];
	  std::string det2name = det2names[i_pair];
	
	  double min_tdiff = -500;
	  double max_tdiff = 500;
	  double tdiff_width = 20;//10;
	  int n_tdiff_bins = (max_tdiff - min_tdiff) / tdiff_width;
	  std::string histname = "hTDiff_" + det1name + "_" + det2name;
	  std::string histtitle = det1name + " - " + det2name;
	  TH1F* hTDiff = new TH1F(histname.c_str(), histtitle.c_str(), n_tdiff_bins,min_tdiff,max_tdiff);
	  hTDiff->SetXTitle(histtitle.c_str());
  
	  std::string drawcmd = det1name + ".tblock-" + det2name + ".tblock>>" + histname;
	  std::string cutcmd = "(centralMuonChannel==\"" + central_channel + "\" && @" + det2name + ".size()==1)";
	  int n_entries = tree->GetEntries();
	  tree->Draw(drawcmd.c_str(), cutcmd.c_str(), "", n_entries);

	  double min_fit = hTDiff->GetBinCenter(hTDiff->GetMaximumBin()) + min_fits[i_pair];
	  double max_fit = hTDiff->GetBinCenter(hTDiff->GetMaximumBin()) + max_fits[i_pair];
	  TF1* fit_fn = new TF1("fit_fn", "[0]*TMath::Gaus(x, [1], [2])", min_fit, max_fit);
	  fit_fn->SetParameters(hTDiff->GetMaximum(), 0, 100);
	  hTDiff->Fit(fit_fn, "RQ");

	  sigmas[i_pair] = std::fabs(fit_fn->GetParameter(2));
	  sigma_errs[i_pair] = fit_fn->GetParError(2);

	  TLatex* latex = new TLatex();
	  latex->SetTextAlign(22);
	  latex->SetTextSize(0.05);
	  std::stringstream text;
	  text << std::fixed << std::setprecision(1) << "#sigma = " << sigmas[i_pair] << " #pm " << sigma_errs[i_pair] << " ns";
	  latex->DrawLatexNDC(0.7, 0.7, text.str().c_str());
	}

	for (int i_pair = 0; i_pair < n_pairs; ++i_pair) {
	  std::cout << det1names[i_pair] << " - " << det2names[i_pair] << ": sigma = " << sigmas[i_pair] << " +/- " << sigma_errs[i_pair] << " ns" << std::endl;
	}
	// a^2 + b^2 = X^2 (1)
	// b^2 + c^2 = Y^2 (2)
	// c^2 + a^2 = Z^2 (3)
	// (1)-(2): a^2 - c^2 = X^2-Y^2 (4)
	// (4) + (3): 2a^2 = X^2-Y^2+Z^2 => a^2 = 0.5(X^2-Y^2+Z^2)
	// => b^2 = X^2 - 0.5(X^2-Y^2+Z^2)
	// => c^2 = Z^2 - 0.5(X^2-Y^2+Z^2)
	double det1_sig = std::sqrt(0.5*(sigmas[0]*sigmas[0]-sigmas[1]*sigmas[1]+sigmas[2]*sigmas[2]));
	double det2_sig = std::sqrt(sigmas[0]*sigmas[0] - 0.5*(sigmas[0]*sigmas[0]-sigmas[1]*sigmas[1]+sigmas[2]*sigmas[2]));
	double det3_sig = std::sqrt(sigmas[2]*sigmas[2] - 0.5*(sigmas[0]*sigmas[0]-sigmas[1]*sigmas[1]+sigmas[2]*sigmas[2]));
	std::cout << det1names[0] << ": sigma =  "<< det1_sig << std::endl;
	std::cout << det1names[1] << ": sigma =  "<< det2_sig << std::endl;
	std::cout << det1names[2] << ": sigma =  "<< det3_sig << std::endl;

	c->cd(4);
	TLatex* latex = new TLatex();
	latex->SetTextAlign(22);
	latex->SetTextSize(0.05);
	std::stringstream text;
	text.str("");
	text << std::fixed << std::setprecision(1) << det1names[0] << ": #sigma = " << det1_sig << " ns";
	latex->DrawLatexNDC(0.5, 0.7, text.str().c_str());

	text.str("");
	text << std::fixed << std::setprecision(1) << det1names[1] << ": #sigma = " << det2_sig << " ns";
	latex->DrawLatexNDC(0.5, 0.5, text.str().c_str());

	text.str("");
	text << std::fixed << std::setprecision(1) << det1names[2] << ": #sigma = " << det3_sig << " ns";
	latex->DrawLatexNDC(0.5, 0.3, text.str().c_str());

	std::string basename = "~/plots/2021-04-29/AlCapData_Si16bDataset_TimeResolution_" + det1names[0] + "_" + det1names[1] + "_" + det1names[2] + ".png";
	c->SaveAs(basename.c_str());
      }
    }
  }
}
