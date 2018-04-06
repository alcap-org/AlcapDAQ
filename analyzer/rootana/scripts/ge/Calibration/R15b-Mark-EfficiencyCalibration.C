/**
    First ten points from Eu-125, last two from Y-88
    Efficiency numbers are from John's run_calib.C code
    Elog: https://muon.npl.washington.edu/elog/mu2e/Analysis+R2015b/139
**/
void R15b-Mark-EfficiencyCalibration() {
  gStyle->SetOptFit();
  const Int_t n = 12;
  //GeHiGain
  Double_t x[] = {244.697, 344.279, 411.116, 443.965, 778.904, 867.378, 964.079, 1085.9, 1112.1, 1408, 898.042, 1836.06};
  Double_t y[] = {0.00106402, 0.000780241, 0.000633102, 0.000605201, 0.000350621, 0.000319256, 0.000304784, 0.000297027, 0.000263328, 0.000202848, 0.000318147, 0.000148357};
  Double_t yerr[] = {2.71131e-05, 1.09875e-05, 4.28003e-05, 3.40975e-05, 1.09366e-05, 2.3462e-05, 9.30609e-06, 1.22715e-05, 8.8586e-06, 5.53307e-06, 9.16206e-06, 6.39162e-06  };
  //GeLoGain
  Double_t xl[] = {244.697, 344.279, 411.116, 443.965, 778.904, 867.378, 964.079, 1085.9, 1112.1, 1408, 898.042, 1836.06};
  Double_t yl[] = {0.00103431, 0.000775405, 0.000618343, 0.000577133, 0.000343959, 0.000312045, 0.00030333, 0.000293683, 0.000260709, 0.000207614, 0.00032314, 0.000147642};
  Double_t yerrl[] = {2.79282e-05, 1.11342e-05, 4.44276e-05, 3.60912e-05, 1.11514e-05, 2.4106e-05, 9.66199e-06, 1.23666e-05, 8.95328e-06, 5.47804e-06, 9.71151e-06, 5.93912e-06};


  TF1 *fit = new TF1("fit", "[0]*x^[1]");
  fit->SetParameter(0, 0.2);
  fit->SetParameter(1, -1);
  TLatex label;
  TCanvas *c = new TCanvas("GeHiGain", "GeHiGain");
  TGraphErrors *gerr = new TGraphErrors(n, x, y, 0, yerr);
  gerr->SetTitle("GeHiGain efficiency with Eu-152 and Y-88");
  gerr->GetYaxis()->SetTitle("Efficiency");
  gerr->GetXaxis()->SetTitle("Energy [keV]");
  gerr->SetLineWidth(2);
  gerr->Draw("AP");
  gerr->Fit("fit", "SME");
  std::cout << "Correlation: " << gerr->GetCorrelationFactor() << std::endl;
  label.DrawLatex(1500, 0.0005, "Eff=p0*E^p1");
  c->Draw();

  TLatex labell;
  TCanvas *cl = new TCanvas("GeLoGain", "GeLoGain");
  TGraphErrors *gerrl= new TGraphErrors(n, xl, yl, 0, yerrl);
  gerrl->SetTitle("GeLoGain efficiency with Eu-152 and Y-88");
  gerrl->GetYaxis()->SetTitle("Efficiency");
  gerrl->GetXaxis()->SetTitle("Energy [keV]");
  gerrl->SetLineWidth(2);
  gerrl->Draw("AP");
  gerrl->Fit("fit", "SME");
  std::cout << "Correlation: " << gerrl->GetCorrelationFactor() << std::endl;
  labell.DrawLatex(1500, 0.0005, "Eff=p0*E^p1");
  c->Draw();
  
}
