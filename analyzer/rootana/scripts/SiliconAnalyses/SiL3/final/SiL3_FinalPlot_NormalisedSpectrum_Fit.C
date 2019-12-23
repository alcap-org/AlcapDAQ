void SiL3_FinalPlot_NormalisedSpectrum_Fit() {

  RooWorkspace ws("ws", kTRUE);
  
  std::string filename = "~/data/results/SiL3/unfold_geq2TgtPulse_newPP20us.root";
  TFile* file = new TFile(filename.c_str(), "READ");
  
  int rebin_factor = 5;
  std::string histname = "FinalNormalisation_TimeSlice2000_4000_allRecoil/hNormalisedSpectrum";
  TH1F* spectrum = (TH1F*) file->Get(histname.c_str());
  if (!spectrum) {
    std::cout << "Error: Problem getting spectrum " << histname << std::endl;
    return;
  }
  spectrum->Sumw2();
  spectrum->Rebin(rebin_factor);
  spectrum->Scale(1.0/rebin_factor);
  spectrum->SetStats(false);

  ws.factory("energy[1400,30000]");
  RooArgSet vars;
  vars.add(*ws.var("energy"));
  ws.import(*(new RooDataHist("data", "data", vars, RooFit::Import(*spectrum))));

  TCanvas* c = new TCanvas();
  //  c->SetLogy();
  
  RooPlot* plot = ws.var("energy")->frame(RooFit::Range(1400, 25000));

  ws.var("energy")->setRange("fit", 1400, 25000);
  //  ws.factory("N0[1e-6, 0,1e-5]");
  ws.factory("Tth0[1400,1000,2000]");
  ws.factory("alpha0[1.328,1,5]");
  ws.factory("T0[4500,100,5000]");
  ws.factory("EXPR::SpectrumA('(1 - Tth0/energy)^alpha0*(exp(-energy/T0))', Tth0, energy, alpha0, T0)");

  ws.factory("Tth1[3000,2000,4000]");
  ws.factory("alpha1[1.328,1,5]");
  ws.factory("T1[3500,100,5000]");
  ws.factory("EXPR::SpectrumB('(1 - Tth1/energy)^alpha1*(exp(-energy/T1))', Tth1, energy, alpha1, T1)");

  ws.factory("SUM::model(N0[0, 1e-4]*SpectrumA, N1[0,1e-4]*SpectrumB)");
  
  //  auto model = ws.pdf("model");
  auto model = ws.pdf("SpectrumA");
  model->fitTo(*ws.data("data"), RooFit::Range("fit"));

  ws.data("data")->plotOn(plot);
  model->plotOn(plot, RooFit::Range("fit"), RooFit::LineColor(kRed));
  model->plotOn(plot, RooFit::Range("fit"), RooFit::LineColor(kBlue), RooFit::Components("SpectrumA"), RooFit::LineStyle(kDashed));
  model->plotOn(plot, RooFit::Range("fit"), RooFit::LineColor(kCyan), RooFit::Components("SpectrumB"), RooFit::LineStyle(kDashed));
  plot->Draw();
  ws.Print();
  /*
    TF1* spectral_fit = new TF1("spectral_fit","[0]*(1 - [1]/x)^[2]*(exp(-x/[3]))",1400,30000);//27000);
    spectral_fit->SetParName(0, "N_0");
    spectral_fit->SetParName(1, "T_{th,0}");
    spectral_fit->SetParName(2, "#alpha_{0}");
    spectral_fit->SetParName(3, "T_{0}");
    spectral_fit->SetParameter(0, 1e-6);
    spectral_fit->SetParameter(1, 1400);
    spectral_fit->SetParLimits(1, 0, 5000);
    spectral_fit->SetParameter(2, 1.328);
    spectral_fit->SetParLimits(2, 0, 5);
    spectral_fit->SetParameter(3, 4600);
    TFitResultPtr fit_result = spectrum->Fit(spectral_fit, "RMES");
    spectral_fit->SetLineWidth(2);
    spectral_fit->SetLineColor(kCyan);
    c_log->cd();
    spectral_fit->Draw("LSAME");
    c_lin->cd();
    spectral_fit->Draw("LSAME");
    
    TF1* spectral_fit2 = new TF1("spectral_fit2","[0]*(1 - [1]/x)^[2]*(exp(-x/[3])+[4]*(1-[5]/x)^[6]*exp(-x/[7]))",1000,27000);
    spectral_fit2->FixParameter(0, spectral_fit->GetParameter(0));
    spectral_fit2->FixParameter(1, spectral_fit->GetParameter(1));
    spectral_fit2->FixParameter(2, spectral_fit->GetParameter(2));
    spectral_fit2->FixParameter(3, spectral_fit->GetParameter(3));
    spectral_fit2->SetParName(4, "N_1");
    spectral_fit2->SetParName(5, "T_{th,1}");
    spectral_fit2->SetParName(6, "#alpha_{1}");
    spectral_fit2->SetParName(7, "T_{1}");
    spectral_fit2->SetParameter(4, 1e-7);
    spectral_fit2->SetParLimits(4, 1e-10, 1e-6);
    spectral_fit2->FixParameter(5, 3000);
    //    spectral_fit2->SetParLimits(5, 2500, 5000);
    spectral_fit2->SetParameter(6, 1.328);
    spectral_fit2->SetParLimits(6, 0, 10);
    spectral_fit2->SetParameter(7, 3500);
    TFitResultPtr fit_result2 = spectrum->Fit(spectral_fit2, "RMES");
    //  if ( (Int_t) fit_result != 0) {
    //    std::cout << "Unfolding FinalNormalisation: Fit Failed! Returning..." << std::endl;
    //    return;
    //  }
    spectral_fit2->SetLineWidth(2);
    spectral_fit2->SetLineColor(kSpring);
    c_log->cd();
    spectral_fit2->Draw("LSAME");
    c_lin->cd();
    spectral_fit2->Draw("LSAME");
    */
}
