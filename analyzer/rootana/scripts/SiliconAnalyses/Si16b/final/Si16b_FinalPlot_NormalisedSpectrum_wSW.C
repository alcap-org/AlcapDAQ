void Si16b_FinalPlot_NormalisedSpectrum_wSW(std::string savedir = "") {

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetLogy();

  const int n_ranges = 8;
  double min_energies[n_ranges] = {0,    1400,  10000, 5000,   3500, 4000, 3500, 14000};
  double max_energies[n_ranges] = {26000,26000, 26000, 10000, 10000, 8000, 14000, 15000};
  double SW_rate[n_ranges] = {0};
    
  //  std::string SW_filename = "~/data/results/SiL3/SW-plots.root";
  std::string SW_filename = "~/data/results/SiL3/SW-plots_betterErrors.root";
  //  std::string SW_filename = "~/data/results/Si16b/SW-plots-above1400keV.root";
  TFile* SW_file = new TFile(SW_filename.c_str(), "READ");
  TGraphErrors* SW_gre = (TGraphErrors*) SW_file->Get("Graph");
  SW_gre->Draw("APE");
  SW_gre->SetTitle("Charged Particle Emission");
  SW_gre->GetXaxis()->SetRangeUser(0,20000);
  SW_gre->GetXaxis()->SetTitle("Energy [keV]");
  SW_gre->GetYaxis()->SetTitle("Rate of Charged Particle Emission per Muon Capture per keV");

  TLegend* leg = new TLegend(0.50,0.55,0.85,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);
  leg->AddEntry(SW_gre, "Sobottka-Wills", "pl");
  SW_gre->GetFunction("tdr_fit")->SetBit(TF1::kNotDraw);
  //  SW_gre->GetFunction("tdr_fit")->SetLineColor(kBlack);
  //  leg->AddEntry(SW_gre->GetFunction("tdr_fit"), "S-W Fit", "l");

  for (int i_range = 0; i_range < n_ranges; ++i_range) {
    double min_energy = min_energies[i_range];
    double max_energy = max_energies[i_range];
    bool start_integral = false;
    double SW_integral = 0;
    double SW_error = 0;
    for (int i_element = 0; i_element < SW_gre->GetN(); ++i_element) {
      if (*(SW_gre->GetX()+i_element) > min_energy) {
        start_integral = true;
      }
      if (start_integral) {
        double previous_x = *(SW_gre->GetX()+i_element-1);
        double this_x = *(SW_gre->GetX()+i_element);
        double width_x = this_x - previous_x;
	if (width_x == 0) {
	  continue;
	}
	double this_y = *(SW_gre->GetY()+i_element);
	double this_err = *(SW_gre->GetEY()+i_element);

	double this_integral = this_y * width_x;
	double this_integral_error = (this_err/this_y)*this_integral;
	SW_integral += this_integral;
	SW_error += (this_integral_error * this_integral_error);

	if (i_range == n_ranges-1) {
	  std::cout << "AE; x = " << this_x << ", w = " << width_x << ", y = " << this_y << " +/- " << this_err
		    << ", integral = " << this_integral << " +/- " << this_integral_error
		    << ", (total err)^2 so far = " << SW_error
		    << ", total err so far = " << std::sqrt(SW_error) << std::endl;
	}
      }
      if (*(SW_gre->GetX()+i_element) > max_energy) {
        break;
      }
    }
    SW_error = std::sqrt(SW_error);
    SW_rate[i_range] = SW_integral;
    std::cout << "S-W Integral (" << min_energy / 1000 << " MeV -- " << max_energy / 1000 << " MeV) = " << SW_integral << " +/- " << SW_error << std::endl;
  }

  //  std::string filename = "~/data/results/Si16b/unfold_newPP.root";
  //  std::string filename = "~/data/results/Si16b/unfold_newPP_geq1TgtPulse.root";
  //  std::string filename = "~/data/results/Si16b/unfold_newPP_geq1TgtPulse_SiL1-2--6.root";
  //  std::string filename = "~/data/results/Si16b/unfold_newPP_geq1TgtPulse_SiL1-7--10.root";
  //  std::string filename = "~/data/results/Si16b/unfold_newPP_geq1TgtPulse_SiL1-11--15.root";
  //  std::string filename = "~/data/results/Si16b/unfold_newPP_geq1TgtPulse_SiL1-2--14.root";
  std::string filename = "~/data/results/Si16b/systematics_newPP_geq1TgtPulse_3sigma.root";
  TFile* file = new TFile(filename.c_str(), "READ");
  
  const int n_settings = 1;//5;
  std::string particle_names[n_settings] = {"Sum"};//proton", "deuteron", "triton", "alpha", "SiL3"};
  Int_t colours[n_settings] = {kBlue};//kRed, kCyan, kMagenta, kSpring, kGray};
  std::string leglabels[n_settings] = {"AlCap (sum)"};//, "AlCap (deuterons)", "AlCap (tritons)", "AlCap (alphas)", "AlCap (SiL3 inc.)"};
  double rates[n_settings][n_ranges] = {0};
  double rate_errs[n_settings][n_ranges] = {0};

  int rebin_factor = 1;
  THStack* hStack = new THStack("hStack", "");
  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {

    std::string i_particle_name = particle_names[i_setting];
    Int_t i_colour = colours[i_setting];

    std::string dirname = "FinalSystPlot_" + i_particle_name + "_TCutG";
    std::string i_histname = dirname + "/hFinalStatSyst";

    TGraphAsymmErrors* spectrum = (TGraphAsymmErrors*) file->Get(i_histname.c_str());
    if (!spectrum) {
      std::cout << "Error: Problem getting spectrum " << i_histname << std::endl;
      //      return;
      continue;
    }
    //    spectrum->SetStats(false);
    spectrum->SetLineColor(i_colour);
    spectrum->SetFillStyle(0);
    spectrum->SetFillColor(0);
    spectrum->SetLineWidth(2);
    //    spectrum->SetLineColor(kBlack);
    //    spectrum->SetFillColor(i_colour);
    spectrum->Draw("PE SAMEs");


    leg->AddEntry(spectrum, leglabels[i_setting].c_str(), "l");

    /*    std::string fitname = dirname + "/spectral_fit";
    TF1* fit = (TF1*) file->Get(fitname.c_str());
    if (fit) {
      fit->SetLineWidth(2);
      fit->SetLineColor(kCyan);
      fit->Draw("LSAME");
    }
    */

    // alcaphistogram(spectrum);
    // if (i_setting == 0) {
    //   alcapPreliminary(spectrum);
    // }
  }

  SW_gre->Draw("PE SAME");
  leg->Draw();

  gPad->RedrawAxis();

  if (savedir != "") {
    std::string savename = savedir + "AlCapData_Si16bDataset_NormalisedSpectrum_wSW";

    std::string pdfname = savename + ".pdf";
    c1->SaveAs(pdfname.c_str());
    std::string pngname = savename + ".png";
    c1->SaveAs(pngname.c_str());
  }
}
