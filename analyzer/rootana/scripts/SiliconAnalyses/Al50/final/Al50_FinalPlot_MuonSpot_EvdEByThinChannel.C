void Al50_FinalPlot_MuonSpot_EvdEByThinChannel() {

  // using John's data  
  std::string infilename = "~jrquirk/data/R15b/Al50/psel.3layer.pidpow2.root";
  TFile* infile = new TFile(infilename.c_str(), "READ");

  const int n_arms = 2;
  std::string arms[n_arms] = {"R", "L"};
  std::string histtitles[n_arms] = {"Al50 Dataset, Right Arm (SiR1 && SiR2 && !SiR3)",
				    "Al50 Dataset, Left Arm (SiL1 && SiL3)"};
  const int n_thin_channels[n_arms] = {4, 16};

  const int n_both_points = 5;
  double both_x[n_both_points] = {825, 1700, 2300, 1500, 825};
  double both_y[n_both_points] = {625, 1500, 950, 300, 625};
  TCutG* both = new TCutG("both", n_both_points, both_x, both_y);
  both->SetLineColor(kBlack);
  both->SetLineWidth(2);
  both->SetFillStyle(0);

  const int n_lower_points = 5;
  double lower_x[n_lower_points] = {both_x[0], 1100, 1700, both_x[3], both_x[4]};
  double lower_y[n_lower_points] = {both_y[0], 880, 440, both_y[3], both_y[4]};
  TCutG* lower = new TCutG("lower", n_lower_points, lower_x, lower_y);
  lower->SetLineColor(kRed);
  lower->SetLineWidth(2);
  lower->SetFillStyle(0);

  const int n_upper_points = 5;
  double upper_x[n_upper_points] = {lower_x[1], both_x[1], both_x[2], lower_x[2], lower_x[1]};
  double upper_y[n_upper_points] = {lower_y[1], both_y[1], both_y[2], lower_y[2], lower_y[1]};
  TCutG* upper = new TCutG("upper", n_upper_points, upper_x, upper_y);
  upper->SetLineColor(kBlue);
  upper->SetLineWidth(2);
  upper->SetFillStyle(0);
  
  const int n_particles = 5;
  std::string particles[n_particles] = {"P", "D", "T", "A", "U"};
  
  double min_x = 0;
  double max_x = 25000;
  double x_width = 10;
  int n_x_bins = (max_x - min_x) / x_width;

  double min_y = 0;
  double max_y = 25000;
  double y_width = 10;
  int n_y_bins = (max_y - min_y) / y_width;

  std::string draw = "e1:e1+e2";
  std::string cut = "e3<=0 && abs(t1-t2)<200";

  for (int i_arm = 0; i_arm < n_arms; ++i_arm) {
    std::string arm = arms[i_arm];
    std::string histtitle = histtitles[i_arm];
    int n_channels = n_thin_channels[i_arm];

    std::stringstream thin_channel;
    for (int i_thin_channel = 0; i_thin_channel < n_channels; ++i_thin_channel) {
      thin_channel.str("");
      thin_channel << i_thin_channel+1;
      
      std::string canvasname = "c_EvdE_Si" + arm + "-" + thin_channel.str();
      TCanvas* c_EvdE = new TCanvas(canvasname.c_str(), canvasname.c_str());

      std::string histname = "hEvdE_Si" + arm + "_" + thin_channel.str();
      TH2F* hEvdE = new TH2F(histname.c_str(), histtitle.c_str(), n_x_bins,min_x,max_x, n_y_bins,min_y,max_y);
      hEvdE->SetXTitle("E_{1} + E_{2} [keV]");
      hEvdE->SetYTitle("E_{1} [keV]");

      for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
	std::string particle = particles[i_particle];
	std::string intreename = "PID_" + arm + particle;
	
	TTree* tree = (TTree*) infile->Get(intreename.c_str());
	
	std::string drawcmd = draw + ">>+" + histname;
	std::string cutcmd = "(" + cut + " && seg==" + thin_channel.str() + ")";
	tree->Draw(drawcmd.c_str(), cutcmd.c_str(), "goff");
      }
      
      hEvdE->SetStats(false);
      hEvdE->GetXaxis()->SetRangeUser(0, 10000);
      hEvdE->GetYaxis()->SetRangeUser(0, 2000);
      //  hEvdE->GetYaxis()->SetRangeUser(0, 10000);
      hEvdE->Draw("COLZ");

      double integral_both = 0;
      double integral_lower = 0;
      double integral_upper = 0;
      for (int i_bin = 1; i_bin <= hEvdE->GetXaxis()->GetNbins(); ++i_bin) {
	for (int j_bin = 1; j_bin <= hEvdE->GetYaxis()->GetNbins(); ++j_bin) {

	  double bin_x = hEvdE->GetXaxis()->GetBinCenter(i_bin);
	  double bin_y = hEvdE->GetXaxis()->GetBinCenter(j_bin);
	  double bin_content = hEvdE->GetBinContent(i_bin, j_bin);
	
	  // Test if this bin is within the bounds of the TCutG
	  if (both->IsInside(bin_x, bin_y)) {
	    integral_both += bin_content;
	  }
	  if (lower->IsInside(bin_x, bin_y)) {
	    integral_lower += bin_content;
	  }
	  if (upper->IsInside(bin_x, bin_y)) {
	    integral_upper += bin_content;
	  }
	}
      }

      std::cout << "Si" << arm << "-" << thin_channel.str() << ": both = " << integral_both
		<< ", lower = " << integral_lower << " (" << (integral_lower / integral_both)*100 << "%)"
		<< ", upper = " << integral_upper << " (" << (integral_upper / integral_both)*100 << "%)"
		<< ", lower / upper = " << integral_lower / integral_upper
		<< std::endl;

      both->Draw("LSAME");
      lower->Draw("LSAME");
      upper->Draw("LSAME");
      
      TLatex* latex = new TLatex();
      latex->DrawLatexNDC(0.55, 0.65, "AlCap Preliminary");
    }
  }
}
