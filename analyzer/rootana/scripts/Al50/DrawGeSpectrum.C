void DrawGeSpectrum() {

  TCanvas* c1 = new TCanvas("c1", "c1");

  std::string version = "v79";
  std::string savelocation = "~/data/out/"+version+"/plots";

  std::string filename = "~/data/out/"+version+"/total.root";
  TFile* file = new TFile(filename.c_str(), "READ");
  TH1* hGeEnergy = (TH1*)file->Get("GeSpectrum/hEnergy");
  hGeEnergy->SetStats(0);
  hGeEnergy->SetTitle("Photon Spectrum (Al50)");
  hGeEnergy->GetXaxis()->SetTitle("Energy [keV]"); hGeEnergy->GetYaxis()->SetTitle("Counts");
  hGeEnergy->SetLineWidth(2);

  int rebin_factor = 2;
  double low_energy_limit = 340;
  double high_energy_limit = 360;

  hGeEnergy->SetLineColor(kBlack);
  hGeEnergy->Rebin(rebin_factor);
  hGeEnergy->GetXaxis()->SetRangeUser(low_energy_limit, high_energy_limit);
  hGeEnergy->GetYaxis()->SetRangeUser(0, 12500);
  c1->SetLogy(0);
  hGeEnergy->Draw();


  TH1* hGeEnergyOOT = (TH1*)file->Get("GeSpectrum/hEnergyOOT");
  hGeEnergyOOT->SetLineColor(kSpring);
  hGeEnergyOOT->SetLineWidth(2);
  hGeEnergyOOT->Rebin(rebin_factor);
  hGeEnergyOOT->GetXaxis()->SetRangeUser(low_energy_limit, high_energy_limit);
  hGeEnergyOOT->Draw("SAME");

  TH1* hGeEnergyFarOOT = (TH1*)file->Get("GeSpectrum/hEnergyFarOOT");
  hGeEnergyFarOOT->SetLineColor(kRed);
  hGeEnergyFarOOT->SetLineWidth(2);
  hGeEnergyFarOOT->Rebin(rebin_factor);
  hGeEnergyFarOOT->GetXaxis()->SetRangeUser(low_energy_limit, high_energy_limit);
  hGeEnergyFarOOT->Draw("SAME");

  TH1* hGeEnergyPrompt = ((TH2*)file->Get("GeSpectrum/hTimeEnergy"))->ProjectionY();
  hGeEnergyPrompt->SetLineColor(kBlue);
  hGeEnergyPrompt->SetLineWidth(2);
  hGeEnergyPrompt->Rebin(rebin_factor);
  hGeEnergyPrompt->GetXaxis()->SetRangeUser(low_energy_limit, high_energy_limit);
  hGeEnergyPrompt->Draw("SAME");

  TLegend *leg = new TLegend(0.18,0.66,0.38,0.86);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(kWhite);
  leg->AddEntry(hGeEnergy, "All", "l");
  leg->AddEntry(hGeEnergyPrompt, "Prompt (|#Deltat| < 500ns)", "l");
  leg->AddEntry(hGeEnergyOOT, "Out of Time (500ns < |#Deltat| < 5#mus)", "l");
  leg->AddEntry(hGeEnergyFarOOT, "Far Out of Time (|#Deltat| > 5#mus)", "l");
  leg->Draw();

  //  c1->Print("~/plots/ThesisPlots/ge-spectrum-time-bins.pdf");
  
  hGeEnergyPrompt->GetXaxis()->SetRangeUser(low_energy_limit, 356);
  TF1* double_gaussian_fit = new TF1("double_gaus", "[0]*TMath::Gaus(x, [1], [2]) + [3]*TMath::Gaus(x, [4], [5]) + [6]*x + [7]", low_energy_limit, 356);
  double_gaussian_fit->SetParameter(0, 100);
  double_gaussian_fit->SetParameter(1, 347);
  double_gaussian_fit->SetParameter(2, 2);
  double_gaussian_fit->SetParameter(3, 100);
  double_gaussian_fit->SetParameter(4, 352);
  double_gaussian_fit->SetParameter(5, 2);
  double_gaussian_fit->SetParameter(6, -0.10);
  double_gaussian_fit->SetParameter(7, 1000);
  hGeEnergyPrompt->SetStats(false);
  //gStyle->SetOptFit(11111);
  hGeEnergyPrompt->Fit("double_gaus", "R");
  c1->Print("~/plots/ThesisPlots/x-ray-fit.pdf");

  /*  hGeEnergy->Rebin(8);
  hGeEnergy->GetXaxis()->SetRangeUser(200, 1800);
  c1->SetLogy(1);
  hGeEnergy->Draw();
  c1->Print("~/plots/ThesisPlots/full-ge-spectrum.pdf");
  */

  /*  TSpectrum* spectrum = new TSpectrum;
  TH1* hGeBackground = spectrum->Background(hGeEnergy, 200);
  hGeBackground->SetLineColor(kRed);
  hGeBackground->Draw("SAME");

  TH1F* hGeSignal = hGeEnergy->Clone();
  //  hGeSignal->Add(hGeBackground, -1);
  c1->SetLogy(0);
  hGeSignal->Draw();

  TSpectrum spectrum_2;
  spectrum_2.Search(hGeSignal);
  Double_t* energies = spectrum_2.GetPositionX();
  int n_peaks = spectrum_2.GetNPeaks();
  TF1* peak_fn = new TF1("peak_fn", "[0]*TMath::Gaus(x, [1], [2])");
  for (int i_peak = 0; i_peak < 9; ++i_peak) {
    // Try and fit a Gaussian to each peak
    double low_edge = *(spectrum_2.GetPositionX()+i_peak)-5;
    double high_edge = *(spectrum_2.GetPositionX()+i_peak)+5;
    std::cout << low_edge << ", " << high_edge << std::endl;
    peak_fn->SetRange(low_edge, high_edge);
    peak_fn->SetParameter(0, *(spectrum_2.GetPositionY()+i_peak));
    peak_fn->SetParameter(1, *(spectrum_2.GetPositionX()+i_peak));
    peak_fn->SetParameter(2, 50);
    hGeSignal->Fit(peak_fn, "R");

    std::cout << "Peak #" << i_peak+1 << " = " << *(spectrum_2.GetPositionX()+i_peak) << ", Integral = " << peak_fn->Integral(low_edge, high_edge) << std::endl;
  }
  */
}
