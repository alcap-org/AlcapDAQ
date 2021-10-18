void Al50_FinalPlot_ResponseMatrix(std::string savedir = "") {

  const int n_settings = 3;
  std::string settings[n_settings] = {"", "_midLowE", "_midHighE"};
  std::string labels[n_settings] = {"nominal", "closer to front", "closer to back"};
  
  const int n_particles = 1;
  std::string particles[n_particles] = {"proton"};//, "deuteron", "triton", "alpha"};

  const int n_arms = 2;
  std::string armnames[n_arms] = {"SiR", "SiL"};

  for (int i_arm = 0; i_arm < n_arms;  ++i_arm) {
    std::string armname = armnames[i_arm];

    for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
      std::string setting = settings[i_setting];
      
      for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
	std::string particle = particles[i_particle];
	std::string infilename = "/data/R15bMC/respMatrix_100M_Al50_" + particle + "_100keVBins" + setting + ".root";

	std::string responsename = armname + "_";
	if (particle == "proton" && armname == "SiR") {
	  responsename += "three";
	}
	else {
	  responsename += "two";
	}
	responsename += "_layer_response";
  
	TFile* infile = new TFile(infilename.c_str(), "READ");
	RooUnfoldResponse* response = (RooUnfoldResponse*) infile->Get(responsename.c_str());

	TCanvas* c = new TCanvas();
	TH1D* matrix = (TH1D*) response->Hresponse();
	matrix->GetXaxis()->SetRangeUser(1,5);
	matrix->GetYaxis()->SetRangeUser(1,5);
	matrix->Draw("COLZ");

	TLatex* latex = new TLatex();
	std::string text = armname + " (" + labels[i_setting] + ")";
	latex->DrawLatexNDC(0.50, 0.35, text.c_str());
      }
    }
  }

}
