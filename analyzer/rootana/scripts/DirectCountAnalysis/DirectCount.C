void GaussianFits() {

  //  std::string filename = "out10352.root";
  //  std::string filename = "out08997_direct-count.root";
  //  std::string filename = "out10404.root";
  std::string filename = "combo.root";
  TFile* file = new TFile(filename.c_str());
  if ( file->IsZombie() ) {
    std::cout << "Problems opening file " << filename << std::endl;
    return;
  }

  std::string histname = "PlotTAP_NMuons_Si16/hNMuonsPerStrip";
  TH1F* hNMuonsPerStrip = (TH1F*) file->Get(histname.c_str());
  if ( !hNMuonsPerStrip ) {
    std::cout << "Problems finding " << histname << std::endl;
    exit(1);
  }
  hNMuonsPerStrip->Draw("HIST E");

  double error = 0;
  double n_muons = hNMuonsPerStrip->IntegralAndError(1, 15, error);
  std::cout << "N_muons (strips 1 to 15) = " << n_muons << " +/- " << error << " (stat.)" << std::endl;
  hNMuonsPerStrip->Fit("gaus", "", "", 1, 16);
  std::cout << "N_muons (strips 1 to 15, from fit) = " << gaus->Integral(1, 16) << std::endl;
  std::cout << "N_muons (strips 1 to 16, from fit) = " << gaus->Integral(1, 17) << std::endl;

  //  TH1F* hSiL3Amplitude = (TH1F*) file->Get("PlotTAP_Amplitude/hSiL3-S#MaxBinAPGenerator#any_Amplitude");
  //  if ( !hSiL3Amplitude ) {
  //    std::cout << "Problems finding SiL3-S amplitude plot" << std::endl;
  //    exit(1);
  //  }
  //  double integral_low = 300;
  //  double integral_high = 4200;
  //  double integral_low_bin = hSiL3Amplitude->FindBin(integral_low);
  //  double integral_high_bin = hSiL3Amplitude->FindBin(integral_high);
  //  double integral = hSiL3Amplitude->Integral(integral_low_bin, integral_high_bin);
  //  std::cout << "SiL3-S: Integral (" << integral_low << " ADC -- " << integral_high << " ADC) = " << integral << std::endl;
  //  hSiL3Amplitude->Draw();
}
