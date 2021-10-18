#include "../../Utils/IntegrateRates.C"

void Ti50_FinalPlot_ResponseMatrixFit(std::string savedir = "", std::ostream& numbers_file = std::cout) {

  bool save_plots = true;
  std::string basename = "~/plots/2021-06-23";

  const int n_particles = 4;
  std::string particles[n_particles] = {"proton", "deuteron", "triton", "alpha"};
  double min_Es[n_particles] = {4, 5, 6, 15};
  double max_Es[n_particles] = {20, 18, 17, 20};
  double min_integral_energies[n_particles] = {3, 5, 5, 15};
  double max_integral_energies[n_particles] = {17, 17, 17, 20};
  double latex_xs[n_particles] = {0.68, 0.3, 0.3, 0.35};
  double latex_ys[n_particles] = {0.65, 0.55, 0.5, 0.55};
  double leg_x1s[n_particles] = {0.45, 0.45, 0.45, 0.15};
  double leg_y1s[n_particles] = {0.65, 0.65, 0.65, 0.65};
  double leg_x2s[n_particles] = {0.80, 0.80, 0.80, 0.40};
  double leg_y2s[n_particles] = {0.85, 0.85, 0.85, 0.85};
  
  std::string filename = "~/data/results/Ti50/fits_fromMarkHists.root";
  TFile* file = new TFile(filename.c_str(), "READ");

  std::string arm = "SiR";
  for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
    TCanvas* c1 = new TCanvas();

    std::string particle = particles[i_particle];
    double min_E = min_Es[i_particle];
    double max_E = max_Es[i_particle];
    
    TLatex* latex = new TLatex();
    //  latex->DrawLatexNDC(0.17, 0.75, "AlCap #bf{#it{Preliminary}}");
    std::string dirname = "FinalFit_" + arm + "_" + particle;
    std::string histname = dirname + "/hInputSpectrum";
    TGraphAsymmErrors* raw_spectrum = (TGraphAsymmErrors*) file->Get(histname.c_str());
    if (!raw_spectrum) {
      std::cout << "Error: Problem getting raw_spectrum " << histname << std::endl;
      return;
    }
    raw_spectrum->SetLineColor(kBlack);
    raw_spectrum->SetMarkerColor(kBlack);
  
    const int n_fits = 2;//4;
    std::string fits[n_fits] = {"Single", "Double"};//{"Single", "Double", "SingleThresh", "DoubleThresh"};
    TF1* fit_fns[n_fits] = {0};
    for (int i_fit = 0; i_fit < n_fits; ++i_fit) {

      std::string fit = fits[i_fit];

      std::string fnname = dirname + "/" + fit;
      TF1* fitted_fn = (TF1*) file->Get(fnname.c_str());

      TF1* true_fn = 0;
      std::string resultname = dirname + "/TFitResult-hInputSpectrum-" + fit;
      TFitResult* result = (TFitResult*) file->Get(resultname.c_str());
      if (fit == "Single") {
	true_fn = new TF1(fit.c_str(), "[0]*(TMath::Exp(-x/[1]))", min_E, max_E);
	true_fn->SetParName(0, "N0");
	true_fn->SetParName(1, "T0");
	true_fn->SetParameters(fitted_fn->GetParameter(0), fitted_fn->GetParameter(1));
	std::cout << "N0 = " << fitted_fn->GetParameter(0) << " +/- " << fitted_fn->GetParError(0) << std::endl;
	std::cout << "T0 = " << fitted_fn->GetParameter(1) << " +/- " << fitted_fn->GetParError(1) << std::endl;
      }
      else if (fit == "Double") {
	//    min_E = 5;
	true_fn = new TF1(fit.c_str(), "[0]*(TMath::Exp(-x/[1]) + [2]*TMath::Exp(-x/[3]))", min_E, max_E);
	true_fn->SetParName(0, "N0");
	true_fn->SetParName(1, "T0");
	true_fn->SetParLimits(1, 0, 5);
	true_fn->SetParName(2, "r");
	true_fn->SetParLimits(2, 0, 1);
	true_fn->SetParName(3, "T1");
	true_fn->SetParLimits(3, 5, 30);
	true_fn->SetParameters(fitted_fn->GetParameter(0), fitted_fn->GetParameter(1), fitted_fn->GetParameter(2), fitted_fn->GetParameter(3));
	std::cout << "N0 = " << fitted_fn->GetParameter(0) << " +/- " << fitted_fn->GetParError(0) << std::endl;
	std::cout << "T0 = " << fitted_fn->GetParameter(1) << " +/- " << fitted_fn->GetParError(1) << std::endl;
	std::cout << "r = " << fitted_fn->GetParameter(2) << " +/- " << fitted_fn->GetParError(2) << std::endl;
	std::cout << "T1 = " << fitted_fn->GetParameter(3) << " +/- " << fitted_fn->GetParError(3) << std::endl;
      }
      // else if (fit == "Triple") {
      //   true_fn = new TF1(fit.c_str(), model, min_E, max_E, 6);
      //   true_fn->SetParName(0, "N0");
      //   true_fn->SetParName(1, "T0");
      //   true_fn->SetParName(2, "r");
      //   true_fn->SetParLimits(2, 0, 1);
      //   true_fn->SetParName(3, "T1");
      //   true_fn->SetParName(4, "r2");
      //   true_fn->SetParLimits(4, 0, 1);
      //   true_fn->SetParName(5, "T2");
      //   true_fn->SetParameters(0.1, 4.8, 0.5, 7.8, 0.1, 5.0);
      // }
      else if (fit == "SingleThresh") {
	true_fn = new TF1(fit.c_str(), "[0]*TMath::Power( (1 - ([2]/x)), [3])*(TMath::Exp(-x/[1]))", min_E, max_E);
	true_fn->SetParName(0, "N0");
	true_fn->SetParName(1, "T0");
	true_fn->SetParLimits(0, 0.01, 1);
	true_fn->SetParLimits(1, 1, 10);
	true_fn->SetParName(2, "Tth");
	true_fn->SetParLimits(2, 0, 5.0);
	true_fn->SetParName(3, "#alpha");
	true_fn->SetParLimits(3, 0, 100);
	true_fn->SetParameters(fitted_fn->GetParameter(0), fitted_fn->GetParameter(1), fitted_fn->GetParameter(2), fitted_fn->GetParameter(3));
	std::cout << "N0 = " << fitted_fn->GetParameter(0) << " +/- " << fitted_fn->GetParError(0) << std::endl;
	std::cout << "T0 = " << fitted_fn->GetParameter(1) << " +/- " << fitted_fn->GetParError(1) << std::endl;
	std::cout << "Tth = " << fitted_fn->GetParameter(2) << " +/- " << fitted_fn->GetParError(2) << std::endl;
	std::cout << "alpha = " << fitted_fn->GetParameter(3) << " +/- " << fitted_fn->GetParError(3) << std::endl;
      }
      else if (fit == "DoubleThresh") {
	true_fn = new TF1(fit.c_str(), "[0]*TMath::Power( (1 - ([2]/x)), [3])*(TMath::Exp(-x/[1]) + [4]*TMath::Exp(-x/[5]))",min_E, max_E);
	true_fn->SetParName(0, "N0");
	true_fn->SetParName(1, "T0");
	true_fn->SetParLimits(0, 0.01, 1);
	true_fn->SetParLimits(1, 1, 10);
	true_fn->SetParName(2, "Tth");
	true_fn->SetParLimits(2, 0, 2);
	true_fn->SetParName(3, "#alpha");
	true_fn->SetParLimits(3, 0, 100);
	true_fn->SetParName(4, "r");
	true_fn->SetParLimits(4, 0, 1.0);
	true_fn->SetParName(5, "T1");
	true_fn->SetParLimits(5, 5, 30);
	true_fn->SetParameters(0.1, 4.8, 1.4, 0.2, 0.1, 10);
	true_fn->SetParameters(fitted_fn->GetParameter(0), fitted_fn->GetParameter(1), fitted_fn->GetParameter(2), fitted_fn->GetParameter(3), fitted_fn->GetParameter(4), fitted_fn->GetParameter(5));
	std::cout << "N0 = " << fitted_fn->GetParameter(0) << " +/- " << fitted_fn->GetParError(0) << std::endl;
	std::cout << "T0 = " << fitted_fn->GetParameter(1) << " +/- " << fitted_fn->GetParError(1) << std::endl;
	std::cout << "Tth = " << fitted_fn->GetParameter(2) << " +/- " << fitted_fn->GetParError(2) << std::endl;
	std::cout << "alpha = " << fitted_fn->GetParameter(3) << " +/- " << fitted_fn->GetParError(3) << std::endl;
	std::cout << "r = " << fitted_fn->GetParameter(4) << " +/- " << fitted_fn->GetParError(4) << std::endl;
	std::cout << "T1 = " << fitted_fn->GetParameter(5) << " +/- " << fitted_fn->GetParError(5) << std::endl;
      }

      double min_integral_energy = min_integral_energies[i_particle];
      double max_integral_energy = max_integral_energies[i_particle];
      //      int min_integral_bin = raw_spectrum->GetXaxis()->FindBin(min_integral_energy);
      //      int max_integral_bin = raw_spectrum->GetXaxis()->FindBin(max_integral_energy)-1;
      //      double hist_integral_err = 0;
      //      double hist_integral = raw_spectrum->Integral(min_integral_bin, max_integral_bin);
      double fit_integral = true_fn->Integral(min_integral_energy, max_integral_energy);
      std::cout << "Cov(0,0) = " << (result->GetCovarianceMatrix())(0,0) << std::endl;
      double fit_integral_error = true_fn->IntegralError(min_integral_energy, max_integral_energy, result->GetParams(), result->GetCovarianceMatrix().GetMatrixArray());
      double chi2 = result->Chi2();
      int ndf = result->Ndf();
      std::cout << "Chi^2 / ndf = " << chi2 << " / " << ndf << " = " << chi2/ndf << std::endl;
      //      std::cout << "Hist Integral (" << min_integral_energy << " MeV -- " << max_integral_energy << " MeV) = " << hist_integral << " +/- " << hist_integral_err << std::endl;
      std::cout << "Fit Integral (" << min_integral_energy << " MeV -- " << max_integral_energy << " MeV) = (" << fit_integral*1e3 << " +/- " << fit_integral_error*1e3 << ") x 10^{-3} " << particle << "s per muon capture" << std::endl;

      fitted_fn->SetLineColor(kRed);
      true_fn->SetLineColor(kMagenta);

      raw_spectrum->Draw("APE");
      raw_spectrum->GetFunction("Single")->SetBit(TF1::kNotDraw);
      raw_spectrum->GetFunction("Double")->SetBit(TF1::kNotDraw);
      raw_spectrum->GetFunction("DoubleThresh")->SetBit(TF1::kNotDraw);
      raw_spectrum->GetFunction("SingleThresh")->SetBit(TF1::kNotDraw);
      raw_spectrum->GetXaxis()->SetTitle("Energy [MeV]");
      raw_spectrum->GetYaxis()->SetTitle("Yield / MeV");
      true_fn->Draw("LSAME");
      fitted_fn->Draw("LSAME");

      TLegend* leg = new TLegend(leg_x1s[i_particle], leg_y1s[i_particle], leg_x2s[i_particle], leg_y2s[i_particle]);
      leg->SetBorderSize(0);
      leg->SetTextSize(0.04);
      leg->SetFillColor(kWhite);
      std::string header = "AlCap Ti - " + particle;
      leg->SetHeader(header.c_str());
      leg->AddEntry(raw_spectrum, "raw spectrum (normalised)", "lp");
      std::string leglabel = "truth model (" + fit + ")";
      leg->AddEntry(true_fn, leglabel.c_str(), "lp");
      leg->AddEntry(fitted_fn, "folded truth", "lp");
      leg->Draw();

      TLatex* latex = new TLatex();
      latex->SetTextAlign(22);
      latex->SetTextSize(0.04);
      std::stringstream text;
      double latex_x = latex_xs[i_particle];
      double latex_y = latex_ys[i_particle];
      double latex_y_step = 0.05;

      text.str("");
      latex_y -= latex_y_step;
      text << "N_{0} exp(-T/T_{0})";
      latex->SetTextSize(0.045);
      latex->DrawLatexNDC(latex_x, latex_y, text.str().c_str());

      latex->SetTextSize(0.04);
      text.str("");
      latex_y -= latex_y_step;
      text << std::fixed << std::setprecision(2) << "N_{0} = (" << fitted_fn->GetParameter(0)*1e3 << " #pm " << fitted_fn->GetParError(0)*1e3 << ") #times 10^{-3}";
      latex->DrawLatexNDC(latex_x, latex_y, text.str().c_str());

      text.str("");
      latex_y -= latex_y_step;
      text << "T_{0} = " << std::setprecision(2) << fitted_fn->GetParameter(1) << " #pm " << fitted_fn->GetParError(1) << " MeV";
      latex->DrawLatexNDC(latex_x, latex_y, text.str().c_str());

            if (fit == "Double") {
	text.str("");
	latex_y -= latex_y_step;
	text << "r = " << std::setprecision(2) << fitted_fn->GetParameter(2) << " #pm " << fitted_fn->GetParError(2) << "";
	latex->DrawLatexNDC(latex_x, latex_y, text.str().c_str());

	text.str("");
	latex_y -= latex_y_step;
	text << "T_{1} = " << std::setprecision(2) << fitted_fn->GetParameter(3) << " #pm " << fitted_fn->GetParError(3) << " MeV";
	latex->DrawLatexNDC(latex_x, latex_y, text.str().c_str());	
      }

      text.str("");
      latex_y -= latex_y_step;
      text << "#chi^2 / ndf = " << chi2 << " / " << ndf << " = " << chi2/ndf;
      latex->DrawLatexNDC(latex_x, latex_y, text.str().c_str());

      text.str("");
      latex_y -= latex_y_step;
      text << std::setprecision(0) << "Fit Range: " << min_E << " MeV - " << max_E << " MeV"; 
      latex->DrawLatexNDC(latex_x, latex_y, text.str().c_str());

      text.str("");
      latex_y -= latex_y_step;
      text << std::setprecision(0) << "Truth Integral (" << min_integral_energy << " MeV - " << max_integral_energy << " MeV):"; 
      latex->DrawLatexNDC(latex_x, latex_y, text.str().c_str());

      text.str("");
      latex_y -= latex_y_step;
      text << std::setprecision(2) << "(" << fit_integral*1e3 << " #pm " << fit_integral_error*1e3 << ") #times 10^{-3}";
      latex->DrawLatexNDC(latex_x, latex_y, text.str().c_str());

      if (save_plots) {
	std::string pngname = basename + "/AlCapData_Ti50Dataset_ResponseMatrixFit_" + fit + "Exp_" + particle + "_" + arm + ".png";
	std::cout << pngname << std::endl;
	c1->SaveAs(pngname.c_str());
      }
    }
  }  
}
