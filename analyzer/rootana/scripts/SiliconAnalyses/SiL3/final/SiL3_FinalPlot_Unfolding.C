void SiL3_FinalPlot_Unfolding() {

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetLogy();

  const int n_unfolding_steps = 3;
  std::string unfolding_steps[n_unfolding_steps] = {"FlatBkg", "TimeCut", "GeomAcceptance"};
  
  TLegend* leg = new TLegend(0.50,0.55,0.90,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);
  //  leg->AddEntry(SW_gre, "Sobottka-Wills", "pl");
  SW_gre->GetFunction("tdr_fit")->SetLineColor(kBlack);
  //  leg->AddEntry(SW_gre->GetFunction("tdr_fit"), "S-W Fit", "l");

  std::string norm_filename = "~/data/results/SiL3/normalisation.root";
  //  std::string norm_filename = "~/data/results/SiL3/normalisation_wMuScCut_3000-3500ADC.root";
  TFile* norm_file = new TFile(norm_filename.c_str(), "READ");
  TTree* counttree = (TTree*) norm_file->Get("XRaySpectrum_GeLoGain_wTimeCut/counttree");
  //  TTree* counttree = (TTree*) norm_file->Get("GeLoGain_wSiL3Coinc_MuonSlice3000_6000_2p-1s/counttree");
  double n_stopped_muons = 0;
  counttree->SetBranchAddress("n_stopped_muons", &n_stopped_muons);
  counttree->GetEntry(0);
  double capture_fraction = 0.658;
  double n_captured_muons = n_stopped_muons * capture_fraction;
  std::cout << "AE: n_stopped_muons = " << n_stopped_muons << std::endl;
  std::cout << "AE: n_captured_muons = " << n_captured_muons << std::endl;

  //  std::string filename = "~/data/results/SiL3/unfold.root";
  //  std::string filename = "~/data/results/SiL3/unfold_wMuScCut_3000-3500ADC.root";
  std::string filename = "~/data/results/SiL3/unfold_special.root";
  TFile* file = new TFile(filename.c_str(), "READ");

  /*
  const int n_slices = 5;
  double min_time_slices[n_slices] = {2000, 2000, 2500, 3000, 3500};
  double max_time_slices[n_slices] = {4000, 2500, 3000, 3500, 4000};
  Int_t colours[n_slices] = {kRed, kBlue, kMagenta, kSpring, kGray};//, kGray, kGreen+2};
  */
  
  const int n_slices = 1;
  double min_time_slices[n_slices] = {2000};
  double max_time_slices[n_slices] = {4000};
  Int_t colours[n_slices] = {kRed};
  
  
  std::stringstream time_slice_str;
  TH1F* hFinalSpectrum = 0;
  for (int i_slice = 0; i_slice < n_slices; ++i_slice) {
    double i_min_time_slice = min_time_slices[i_slice];
    double i_max_time_slice = max_time_slices[i_slice];

    time_slice_str.str("");
    time_slice_str << "TimeSlice" << i_min_time_slice << "_" << i_max_time_slice;

    Int_t i_colour = colours[i_slice];

    std::string i_histname = "TimeCut_" + time_slice_str.str() + "/hCorrectedSpectrum";
    //    std::string i_histname = "ProtonEscapeCorrection_" + time_slice_str.str() + "/hUnfoldedSpectrum";

    TH1F* spectrum = (TH1F*) file->Get(i_histname.c_str());
    spectrum->Sumw2();
    if (!spectrum) {
      std::cout << "Error: Problem getting spectrum " << i_histname << std::endl;
      return;
    }
    //    spectrum->Sumw2();
    spectrum->Scale(1.0 / (spectrum->GetXaxis()->GetBinWidth(1)) ); // per keV
    int rebin_factor = 2;
    spectrum->Rebin(rebin_factor);
    spectrum->Scale(1.0/rebin_factor);
    spectrum->Scale(1.0/n_captured_muons);
    spectrum->SetStats(false);
    //    spectrum->GetXaxis()->SetRangeUser(0,10000);
    spectrum->SetLineColor(i_colour);
    spectrum->Draw("HIST E SAMES");
    hFinalSpectrum = spectrum;
    //    leg->AddEntry(spectrum, time_slice_str.str().c_str(), "l");

    const int n_ranges = 5;
    double min_energies[n_ranges] = {1400,  5000,  5000,  3500, 4000};
    double max_energies[n_ranges] = {26000, 26000, 8000, 10000, 8000};

    for (int i_range = 0; i_range < n_ranges; ++i_range) {
      double min_energy = min_energies[i_range];
      double max_energy = max_energies[i_range];
      bool start_integral = false;
      double SW_integral = 0;
      for (int i_element = 0; i_element < SW_gre->GetN(); ++i_element) {
	if (*(SW_gre->GetX()+i_element) > min_energy) {
	  start_integral = true;
	}
	if (start_integral) {
	  double previous_x = *(SW_gre->GetX()+i_element-1);
	  double this_x = *(SW_gre->GetX()+i_element);
	  double width_x = this_x - previous_x;
	  SW_integral += *(SW_gre->GetY()+i_element) * width_x;
	}
	if (*(SW_gre->GetX()+i_element) > max_energy) {
	  break;
	}
      }
      std::cout << "S-W Integral (" << min_energy / 1000 << " MeV -- " << max_energy / 1000 << " MeV) = " << SW_integral << std::endl;

      int min_energy_bin = spectrum->GetXaxis()->FindBin(min_energy);
      int max_energy_bin = spectrum->GetXaxis()->FindBin(max_energy) - 1;
      std::cout << "AlCap Integral (" << i_min_time_slice << "ns -- " << i_max_time_slice << "ns, " << min_energy / 1000 << " MeV -- " << max_energy / 1000 << " MeV) = " << spectrum->Integral(min_energy_bin, max_energy_bin, "width") << std::endl;
    }
  }

  
  // Add MC truth and measured S-W spectrum
  //  TFile* mcfile = new TFile("~/data/mc/SiL3/R15b_SiL3_response-matrix_500keV-bins_protons-SWSpec.root", "READ");
  TFile* mcfile = new TFile("~/data/mc/SiL3/R15b_SiL3_response-matrix_100keV-bins_protons-SWSpec.root", "READ");
  //    TFile* mcfile = new TFile("~/data/mc/SiL3/R15b_SiL3_response-matrix_100keV-bins_protons-SWSpec_3mm.root", "READ");
  RooUnfoldResponse* response = (RooUnfoldResponse*) mcfile->Get("Target_two_layer_response");
  double n_generated_events = 1e6;
  TH1F* hTruth = response->Htruth();
  hTruth->Scale(1.0 / n_generated_events);
  hTruth->Scale(1.0 / hTruth->GetXaxis()->GetBinWidth(1));
  //  hTruth->Scale(0.658); // capture rate
  hTruth->Scale(0.15);
  hTruth->SetLineColor(kBlue);
  TH1F* hMeas = response->Hmeasured();
  hMeas->Scale(1.0 / n_generated_events);
  hMeas->Scale(1.0 / hMeas->GetXaxis()->GetBinWidth(1));
  //  hMeas->Scale(0.658); // capture rate
  hMeas->Scale(0.15);
  hMeas->SetLineColor(kMagenta);

  for (int i_range = 0; i_range < n_ranges; ++i_range) {
    double min_energy = min_energies[i_range];
    double max_energy = max_energies[i_range];
    int min_energy_bin = hTruth->GetXaxis()->FindBin(min_energy);
    int max_energy_bin = hTruth->GetXaxis()->FindBin(max_energy) - 1;
    std::cout << "MC Turth Integral (" << i_min_time_slice << "ns -- " << i_max_time_slice << "ns, " << min_energy / 1000 << " MeV -- " << max_energy / 1000 << " MeV) = " << hTruth->Integral(min_energy_bin, max_energy_bin, "width") << std::endl;
  }

  hTruth->SetXTitle("Energy [keV]");
  hTruth->SetYTitle("Rate of Charged Particle Emission per Muon Capture per keV");
  hTruth->Draw("HIST SAME");
  hMeas->Draw("HIST SAME");

  leg->AddEntry(hTruth, "MC Truth", "l");
  leg->AddEntry(hMeas, "MC Measured", "l");
  /*
  TF1* tdr_fit = new TF1("tdr_fit","[0]*(1 - [1]/x)^[2]*(exp(-x/[3]))",1400,27000);
  tdr_fit->SetParameter(0, 0.1);
  tdr_fit->FixParameter(1, 1431);
  tdr_fit->FixParameter(2, 0.3264);
  tdr_fit->FixParameter(3, 4581);
  */
  //  hTruth->Fit(tdr_fit);
  //  SW_gre->Fit(tdr_fit);
  //  tdr_fit->Draw("LSAME");

  //  hTruth->Draw("HIST");
  //  hMeas->Draw("HIST SAMES");
  leg->Draw();
}
