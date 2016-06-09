/**
We calculate the efficiency of detecting the 344.279 keV Eu 152 line in Ge, when the source is placed on the 
Al100 target.
This is a very good estimate for the acceptance for the Al 2p-1s line at 347 keV.
Start with Eu152 run 9302
Documentation of Eu152 source, ELOG 68
Activity of Eu152 source 47.8 kBq on 01.03.2000
Nov 17, 2015 is day number 321 in 2015.
Therefore the decay time is T=-60/365+15+321/365=15.715 a
the half-life of Eu 152 is 13.537 a
thus we have a remaining Eu 152 activity of
   47.8e3*2^(-15.715/13.537)=21.38e3 Bq
The rate of detection of the 344 keV gamma from EU 152 is 7781 counts in the peak /1510 s=5.15/s
The branching fraction for emission of the 344 keV gamma is 0.2659 per Eu 152 decay
The efficiency for detecting 344 keV photons is therefore 5.15/(0.2659*21.38e3)=9.05e-4

From elog:262, we have 0.85 Hz of 2p-1s xrays at a momentum of 1.035*25.4 MeV/c. The rate of emission of these 
xrays per muon stop is 0.79. Thus, the stopping rate is

(0.85/0.79)/9.05e-4=1190 Hz


(8377,'2015-11-10 18:46:11','Eu-152 calibration, source on Ge face (crate 5 disabled)','AE JQ MW JM AP','C','-','-','-','2015-11-10 18:54:55',5111,2000500000,NULL,NULL)
**/

void R15b_ge_eu152_calib() {
  // The file to run on run R15b 8377
  TFile* f = new TFile("out08377.root", "READ");
  //TH1* spec = (TH1*)f->Get("PlotTAP_Amplitude/hGe-S#FirstComplete#{constant_fraction=0.60}{no_time_shift=true}_Amplitude");
    TH1* spec = (TH1*)f->Get("PlotTAP_Amplitude/hGeLoGain#FirstComplete#{constant_fraction=0.60}{no_time_shift=true}_Amplitude");
//  TH1* spec = (TH1*)f->Get("PlotTAP_Amplitude/hGeLoGain#MaxBinAPGenerator#any_Amplitude");
  spec->Sumw2();

  // Activity
  unsigned int src_date[] = {1, 3, 2000}; // 1 March 2000?
  unsigned int meas_date[] = {17, 11, 2015} // 17 Nov 2015
  Double_t age = 15.8; // Years according to WolframAlpha
  Double_t halflife = 13.537;
  Double_t activity0 = 21380; // RunPSI2015:273
  TTimeStamp t0(src_date[2], src_date[1], src_date[0], 0, 0, 0);
  TTimeStamp t(meas_date[2], meas_date[1], meas_date[0], 0, 0, 0);
  Double_t dt = (t.GetSec() - t0.GetSec())/60./60./24./365.2425;
  Double_t activity = activity0*TMath::Power(0.5,dt/halflife);
  Double_t livetime = 524; // 8min 44sec
  Double_t events = activity*livetime;

  // Spectrum
  const unsigned int NPeaks = 10;
  const unsigned int Peak511Index = 5, Peak122Index = 0;
  Double_t Energy[NPeaks]    = {
	244.6975,
	344.2785,
	411.1163, 
	443.965, 
	778.904, 
	867.378, 
	964.079,
	1085.9,
	1112.1,
	1408.0
	};
  Double_t Intensity[NPeaks] = { 
	0.0758,
	0.265,
	0.0223,   
	0.031,
	0.1294, 
	0.0425, 
	0.146,
	0.102,
	0.136,
	0.21};
  Double_t LogEnergy[NPeaks];        for (unsigned int i = 0; i < NPeaks; ++i)   LogEnergy[i]      = TMath::Log(Energy[i]);
  Double_t ExpectedCounts[NPeaks]; for (unsigned int i = 0; i < NPeaks; ++i) ExpectedCounts[i] = events*Intensity[i];
  Double_t ADC[NPeaks]       = {  
	626.15,  
	878.52,  
	1047.6,  
	1131.09, 
	1980.39, 
	2204.9, 
	2450.11, 
	2759.74,
	2825.81,
	3576.92};
  Double_t ADC_Meas[NPeaks];
  const char* Functions[NPeaks]       = {
					  "gaus(0)+[3]*x+[4]",
					  "gaus(0)+[3]*x+[4]",
					  "gaus(0)+[3]*x+[4]",
					  "gaus(0)+[3]*x+[4]",
					  "gaus(0)+[3]*x+[4]",
					  "gaus(0)+[3]*x+[4]",
					  "gaus(0)+[3]*x+[4]",
					  "gaus(0)+[3]*x+[4]",
					  "gaus(0)+[3]*x+[4]",
					  "gaus(0)+[3]*x+[4]",
					  };
  const unsigned int NParam[NPeaks]   = { 5, 5, 5, 5, 5, 5, 5, 5, 5, 5};
  const Double_t FitParam[NPeaks][11] = { 
					  {4829.69, ADC[0],  3.720,   -3.50,  3048.72, 0., 0., 0., 0., 0., 0.},
					  {11933.9, ADC[1],  3.998,    2.60, -1450.83, 0., 0., 0., 0., 0., 0.},
					  {683.522, ADC[2],  4.102,  0.2218,  653.992, 0., 0., 0., 0., 0., 0.},
					  {952.212, ADC[3],  4.059,   -0.11,  532.142, 0., 0., 0., 0., 0., 0.},
					  {2011.72, ADC[4],  4.824,   -1.24,   2825.1, 0., 0., 0., 0., 0., 0.},
					  {508.042, ADC[5],  5.220, -0.2697,  896.204, 0., 0., 0., 0., 0., 0.},
					  {1748.53, ADC[6],  5.595,   -0.70,   1929.5, 0., 0., 0., 0., 0., 0.},
					  {1258.16, ADC[7],  6.545, 0.13061,  -178.53, 0., 0., 0., 0., 0., 0.},
					  {1403.01, ADC[8],  5.771, -0.0916,  456.596, 0., 0., 0., 0., 0., 0.},
					  {1508.03, ADC[9],  7.227,  -0.033,  139.652, 0., 0., 0., 0., 0., 0.},
					};
  Double_t FitParamRes[NPeaks][11];
  Double_t FitParamErr[NPeaks][11];
  Double_t Counts[NPeaks];
  Double_t Efficiency[NPeaks];
//  Double_t LogEfficiency[NPeaks];

  // Errors
  Double_t Chi2Ndf[NPeaks];
  Double_t Error_ADC[NPeaks];
  Double_t Error_Counts[NPeaks];
  Double_t Error_Eff[NPeaks];
//  Double_t Error_LogEff[NPeaks];

  // Count the events (integral of fit gaussian)
  const Double_t rt2pi = TMath::Sqrt(2.*TMath::Pi());
  for (unsigned int i = 0; i < NPeaks; ++i) {
    TF1* fit = new TF1("fit", Functions[i]);
    const Double_t fit_window = 100.; //size of the fitting range/window
    spec->GetXaxis()->SetRangeUser(ADC[i] - fit_window, ADC[i] + fit_window);
    fit->SetParameters(FitParam[i]);
    TFitResultPtr res = spec->Fit(fit,"SMEQ+");
    Chi2Ndf[i] = res->Chi2()/(Double_t)res->Ndf();
    for (unsigned int j = 0; j < NParam[i]; ++j) {
      FitParamRes[i][j] = res->Value(j); //retrieve the value for the parameter j
      FitParamErr[i][j] = res->ParError(j); //retrieve the error for parameter j
    }
  }
  TCanvas* c_spec = new TCanvas("c_spec", "c_spec");
  spec->GetXaxis()->SetRangeUser(0, 4000);
  c_spec->SetLogy();
  spec->Draw("");
  for (unsigned int i = 0; i < NPeaks; ++i) {
    // Get energy calibration
    ADC_Meas[i] = FitParamRes[i][1];
    Error_ADC[i] = FitParamErr[i][1];
//  When you fit a TH1F histogram (for example named "hist") that has N events with a Gaussian:
//  hist->Fit("gaus");
//  the Gaussian function is normalized such that it will give you the number of events inside the histogram.
//  Doing some math, if G is a Gaussian with some constant term (c), mean (m) and sigma (s) then you have:
//  G(x) = c * exp ( -0.5 * [(x-m)/s]^2 )
//  and Integral(-infinity -> infinity) [G(x)] = c * s * sqrt(2*pi) = N
    Counts[i] = FitParamRes[i][0]*FitParamRes[i][2]*rt2pi; //rt2pi = squareroot(2*PI)
    Error_Counts[i] = Counts[i]*TMath::Sqrt(TMath::Power(FitParamErr[i][0]/FitParamRes[i][0],2.)+TMath::Power(FitParamErr[i][2]/FitParamRes[i][2],2.));
    Efficiency[i] = Counts[i] / ExpectedCounts[i];
    Error_Eff[i] = Error_Counts[i] / ExpectedCounts[i];
  }

  // Draw
/**
 * Energy calibration curve
 *
 *
**/
  Double_t en_par[2], en_par_err[2];
  Double_t eff_par[2], eff_par_err[2], Chi2[2];
  unsigned int Ndf[2];
  /* TGraphErrors* e_cal   = new TGraphErrors(NPeaks, Energy, ADC_Meas, 0, Error_ADC); */
  TGraphErrors* e_cal   = new TGraphErrors(NPeaks, ADC_Meas, Energy, Error_ADC, 0);
  /* TGraphErrors* eff_cal = new TGraphErrors(NPeaks-1, LogEnergy+1, LogEfficiency+1, 0, Error_LogEff+1); */
  TGraphErrors* eff_cal = new TGraphErrors(NPeaks, Energy, Efficiency, 0, Error_Eff);
  e_cal->SetTitle("Ge Energy Calibration"); e_cal->GetXaxis()->SetTitle("ADC Value [ADC]"); e_cal->GetYaxis()->SetTitle("Energy [keV]");
  eff_cal->SetTitle("Ge Efficiency Calibration"); eff_cal->GetXaxis()->SetTitle("Energy [keV]"); eff_cal->GetYaxis()->SetTitle("Counts / Expected Counts");
  TCanvas* c_en = new TCanvas("c_en");
  TF1* fit = new TF1("fit", "[0] + [1]*x");
  fit->SetParameter(0, 0);
  fit->SetParameter(1, 100);
  fit->SetParName(0, "Offset");
  fit->SetParName(1, "Gradient");
  TFitResultPtr res = e_cal->Fit(fit,"SME"); e_cal->Draw("A*");
  TPaveText* text_en = new TPaveText(0.2, 0.65, 0.5, 0.75, "nb NDC"); text_en->SetBorderSize(0); text_en->SetTextSize(0.04); text_en->SetFillColor(kWhite); text_en->AddText("E = p_{0} + p_{1}*ADC"); text_en->Draw();
  Chi2[0] = res->Chi2(); Ndf[0] = res->Ndf();
  en_par[0] = res->Value(0); en_par[1] = res->Value(1);
  en_par_err[0] = res->ParError(0); en_par_err[1] = res->ParError(1);
/**
 * Efficiency curve
 *
 *
**/
  TCanvas* c_eff = new TCanvas("c_eff");
  //  gStyle->SetOptFit(111);
  TF1* logfit = new TF1("func", "[0]*(x**[1])");
//  TF1* logfit = new TF1("func", "[0]*x+[1]+[2]/x+[3]/x^2+[4]/x^3+[5]/x^4");
  logfit->SetParName(0, "Coefficient");
  logfit->SetParName(1, "Exponent");
  logfit->SetParameters(TMath::Exp(-2.6), -0.84);
  res = eff_cal->Fit(logfit, "SME"); eff_cal->Draw("A*");
  TPaveText* text_eff = new TPaveText(0.5, 0.5, 0.65, 0.6, "nb NDC"); text_eff->SetBorderSize(0); text_eff->SetTextSize(0.05); text_eff->AddText("#varepsilon = p_{0} E^{p_{1}}"); text_eff->SetFillColor(kWhite); text_eff->Draw();
  /* res = eff_cal->Fit("pol1", "SME"); eff_cal->Draw("A*"); */
  Chi2[1] = res->Chi2(); Ndf[1] = res->Ndf();
  eff_par[0] = res->Value(0); eff_par[1] = res->Value(1);
  eff_par_err[0] = res->ParError(0); eff_par_err[1] = res->ParError(1);

/**
 * Print out
 *
 *
**/
  printf("\n");
  printf("    %-10s | %-8s | %-5s | %-10s | %-5s | %-7s\n", "Energy", "ADC", "+/-", "Eff", "+/-", "Chi2");
  for (unsigned int i = 0; i < NPeaks; ++i) {
    int adc_exp = TMath::Floor(TMath::Log10(Error_ADC[i]));
    int eff_exp = TMath::Floor(TMath::Log10(Error_Eff[i]));
    printf("    %-10.10g | %-8.10g | %-5.1g | %-10.10g | %-5.1g | %-7.3g\n",
	   Energy[i],
	   TMath::Floor(ADC_Meas[i] / TMath::Power(10., adc_exp)) * TMath::Power(10., adc_exp),
	   Error_ADC[i],
	   TMath::Floor(Efficiency[i] / TMath::Power(10., eff_exp)) * TMath::Power(10., eff_exp),
	   Error_Eff[i],
	   Chi2Ndf[i]);
  }
  printf("\n");
  printf("Energy = %g(%g) * ADC + %g(%g)    (%g/%u)\n", en_par[1], en_par_err[1], en_par[0], en_par_err[0], Chi2[0], Ndf[0]);
  printf("Eff = %g(%g)*Energy^[%g(%g)]    (%g/%u)\n", eff_par[0], eff_par_err[0], eff_par[1], eff_par_err[1], Chi2[1], Ndf[1]);

  //  c_en->SaveAs("~/plots/ThesisPlots/ge-calibration-curve.pdf");
  //  c_eff->SaveAs("~/plots/ThesisPlots/ge-efficiency-curve.pdf");

  double si_peak = 400.177;
  double al_peak = 346.828;
  std::cout << "Eff(" << si_peak << ") = " << logfit->Eval(si_peak) << std::endl;
  std::cout << "Eff(" << al_peak << ") = " << logfit->Eval(al_peak) << std::endl;
}

/*
Peaks:

Energy     ADC      Sigma
121.7817   1000.75  7.0
244.6975   2015.48  7.7
344.2785   2831.37  7.1
411.1163   3379.41  7.1
443.965    3648.90  4.9
510.999    4196.38  9.5
778.9040   6392.91  7.5
867.378    7118.00  8.5
964.079    7909.78  7.9
1085.869   8907.53  8.0
1089.737   8941.08  7.7
1112.074   9121.85  8.6
1212.948   9948.85  5.8
1299.140   10654.3  7.2
1408.006   11547.5  8.8

E = 0.12202 +/- 0.00002* ADC - 1.0 +/- 0.1
ln(Eff) = -0.84 +/- 0.05 * ln(E) - 2.6 +/- 0.3
Eff=exp(-2.6)*E^(-0.84)
*/
