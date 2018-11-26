
void Si16a_FinalPlot_BeamEnergy() {
  gROOT->ProcessLine(".L scripts/MuonBeamSimulation/RooGaussExp.cxx++");
  
  const int n_runs = 9;
  int run_numbers[n_runs] = {9735, 9736, 9737, 9739, 9740, 9741, 9742, 9743, -1}; // want to do all runs individually
  double scale_factors[n_runs] = {1.035, 0.99, 0.98, 1.01, 1.02, 1.03, 1.04, 1.05, 0.0};
  double stopping_fractions[n_runs] = {0};

  double punch_gauss_means[n_runs] = {0};
  double punch_gauss_sigmas[n_runs] = {0};
  double punch_landau_means[n_runs] = {0};
  double punch_landau_sigmas[n_runs] = {0};
  double stop_edeps[n_runs] = {0};
  double stop_sigmas[n_runs] = {0};
  double stop_alphas[n_runs] = {0};
  double n_punches[n_runs] = {0};
  double n_stops[n_runs] = {0};

  for (int i_run = 0; i_run < n_runs; ++i_run) {
    TCanvas* c = new TCanvas();
    
    std::stringstream run_str;
    run_str << "0" << run_numbers[i_run];

    std::stringstream sf_str;
    sf_str << scale_factors[i_run];

    std::string infilename;
    std::string wsname;
    if (run_numbers[i_run]>0) {
      infilename = "~/data/results/Si16a/fits" + run_str.str() + ".root";

      //      wsname = "PunchthroughAndStopFit_Thin_wNoThick/ws";
      //      wsname = "PunchthroughAndStopFit_Thin_All/ws";
      wsname = "PunchthroughAndStopFit_SiT_All/ws";
    }
    else {
      infilename = "~/data/mc/Si16a/MuonBeamSim_AllPlots_Fits.root";
      //      wsname = "SiL1/ws";
      wsname = "SiT/ws";
    }
    TFile* infile = new TFile(infilename.c_str(), "READ");
    RooWorkspace* ws = (RooWorkspace*) infile->Get(wsname.c_str());

    RooPlot* Eframe = (ws->var("edep"))->frame();
    std::string histtitle = "Si16a Dataset, Run " + run_str.str() + ", SF = " + sf_str.str();
    Eframe->SetTitle(histtitle.c_str());
    Eframe->GetXaxis()->SetTitle("Energy [keV]");
    //  Eframe->SetMinimum(3000);
    (ws->data("data"))->plotOn(Eframe);
    (ws->pdf("full_pdf"))->plotOn(Eframe);
    if (ws->pdf("punch_pdf") && ws->pdf("stop_pdf")) {
      (ws->pdf("full_pdf"))->plotOn(Eframe, RooFit::Components("punch_pdf"), RooFit::LineStyle(kDashed), RooFit::LineColor(kGreen));
      (ws->pdf("full_pdf"))->plotOn(Eframe, RooFit::Components("stop_pdf"), RooFit::LineStyle(kDashed), RooFit::LineColor(kRed));
    }
    
    std::stringstream axislabel;
    axislabel << "Counts / " << Eframe->getHist("h_data")->getNominalBinWidth() << " keV";
    Eframe->GetYaxis()->SetTitle(axislabel.str().c_str());

    Eframe->Draw();
    ws->Print();
    if (ws->pdf("punch_pdf") && ws->pdf("stop_pdf")) {
      RooRealVar* n_stop_var = ws->var("n_stop");
      double n_stop = n_stop_var->getValV();
      RooRealVar* n_punch_var = ws->var("n_punch");
      double n_punch = n_punch_var->getValV();
      double stopping_fraction = n_stop / (n_punch + n_stop);

      n_punches[i_run] = n_punch;
      n_stops[i_run] = n_stop;
      stopping_fractions[i_run] = stopping_fraction;
    }

    if (ws->var("landau_punch_mean")) {
      punch_landau_means[i_run] = ws->var("landau_punch_mean")->getValV();
      punch_landau_sigmas[i_run] = ws->var("landau_punch_sigma")->getValV();
    }
    punch_gauss_means[i_run] = ws->var("resolution_mean")->getValV();
    punch_gauss_sigmas[i_run] = ws->var("resolution_sigma")->getValV();

    if (ws->var("stop_edep")) {
      stop_edeps[i_run] = ws->var("stop_edep")->getValV();
      stop_sigmas[i_run] = ws->var("stop_sigma")->getValV();
      stop_alphas[i_run] = ws->var("stop_alpha")->getValV();
    }
    //    infile->Close();
  }

  TCanvas* c2 = new TCanvas();
  TGraph* gr = new TGraph(n_runs-1,scale_factors,stopping_fractions);
  gr->Draw("A*E");

  std::cout << "Run\tSF\tPLMean\tPLSigma\tResMean\tResSigm\tSEdep\tSSigma\tSAlpha\tNPunch\tNStop\tSFrac" << std::endl;
  for (int i_run = 0; i_run < n_runs; ++i_run) {
    std::cout << run_numbers[i_run] << "\t" << scale_factors[i_run] << "\t" << punch_landau_means[i_run] << "\t" << punch_landau_sigmas[i_run] << "\t" << punch_gauss_means[i_run] << "\t" << punch_gauss_sigmas[i_run] << "\t" << stop_edeps[i_run] << "\t" << stop_sigmas[i_run] << "\t" << stop_alphas[i_run] << "\t" << n_punches[i_run] << "\t" << n_stops[i_run] << "\t" << stopping_fractions[i_run] << std::endl;
  }
}
