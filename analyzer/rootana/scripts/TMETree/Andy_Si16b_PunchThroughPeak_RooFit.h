
#include "TFile.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TSpectrum.h"
#include "TF1.h"
#include "TFitResult.h"
#include "TGraphErrors.h"
#include "TLatex.h"

#include "RooWorkspace.h"
#include "RooPlot.h"
#include "RooDataHist.h"
#include "RooAbsPdf.h"
#include "RooRealVar.h"

#include <sstream>
#include <iomanip>

std::stringstream filename, histname, histtitle;

double energy_low = 200;
//double energy_high = 700;
double energy_high = 2000;

double landau_mean_low = 200;
double landau_mean_high = 600;
double landau_mean_error = 0;

double landau_sigma_low = 0.1;
double landau_sigma_high = 200;
double landau_sigma_error = 0;

double gauss_mean_low = 0;
double gauss_mean_high = 0;
double gauss_mean_error = 0;

double gauss_sigma_low = 1;
double gauss_sigma_high = 250;
double gauss_sigma_error = 0;

double nbkg_low = 0;
//double nbkg_high = 1000000;//run9737, 9736
double nbkg_high = 10000000;//run9739
double nbkg = 0;
double nbkg_error = 0;

double bulge_mean_low = 850;
double bulge_mean_high = 1100;
double bulge_mean_error = 0;

double bulge_sigma_low = 10;
double bulge_sigma_high = 250;
double bulge_sigma_error = 0;

double gaus_bulge_mean_low = 0;
double gaus_bulge_mean_high = 0;
double gaus_bulge_mean_error = 0;

double gaus_bulge_sigma_low = 10;
double gaus_bulge_sigma_high = 250;
double gaus_bulge_sigma_error = 0;

double nsig_low = 0;
double nsig_high = 1000000;
double nsig = 0;
double nsig_error = 0;

int n_fit_params = 0;
double goodness_of_fit = 0;

void CreateSummedPdf(RooWorkspace* ws, std::string suffix) {
  n_fit_params = 0;

  std::stringstream factory_cmd, sum_factory_cmd;
  sum_factory_cmd << "SUM::sum_" << suffix << "(";
  
  // First, the Landau convoluted with a Gaussian
  factory_cmd << "Landau::landau_bkg_" << suffix << "(energy[" << energy_low << ", " << energy_high << "], landau_mean_" << suffix << "[" << landau_mean_low << ", " << landau_mean_high << "], landau_sigma_" << suffix << "[" << landau_sigma_low << ", " << landau_sigma_high << "])";
  std::cout << "AE (" << suffix << ") = " << factory_cmd.str().c_str() << std::endl;
  ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");

  factory_cmd << "Gaussian::gauss_bkg_" << suffix << "(energy[" << energy_low << ", " << energy_high << "], gauss_mean_" << suffix << "[" << gauss_mean_low << ", " << gauss_mean_high << "], gauss_sigma_" << suffix << "[" << gauss_sigma_low << ", " << gauss_sigma_high << "])";
  std::cout << "AE (" << suffix << ") = " << factory_cmd.str().c_str() << std::endl;
  ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");

  factory_cmd << "FCONV::landau_x_gauss_" << suffix << "(energy, landau_bkg_" << suffix << ", gauss_bkg_" << suffix << ")";
  std::cout << "AE (" << suffix << ") = " << factory_cmd.str().c_str() << std::endl;
  ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");
  sum_factory_cmd << "nbkg_" << suffix << "[" << nbkg_low << ", " << nbkg_high << "]*landau_x_gauss_" << suffix;
  n_fit_params += 4; // landau_mean, landau_sigma, gauss_sigma, nbkg
      
  // Now the bulge of interest
  /*  factory_cmd << "Gaussian::bulge_signal_" << suffix << "(energy[" << energy_low << ", " << energy_high << "], bulge_mean_" << suffix << "[" << bulge_mean_low << ", " << bulge_mean_high << "], bulge_sigma_" << suffix << "[" << bulge_sigma_low << ", " << bulge_sigma_high << "])";
  std::cout << "AE (" << suffix << ") = " << factory_cmd.str().c_str() << std::endl;
  ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");
  sum_factory_cmd << ", nsig_" << suffix << "[" << nsig_low << ", " << nsig_high << "]*bulge_signal_" << suffix;
  n_fit_params += 3; // bulge_mean, bulge_sigma, nsig
  */

  factory_cmd << "Landau::landau_bulge_" << suffix << "(energy[" << energy_low << ", " << energy_high << "], bulge_mean_" << suffix << "[" << bulge_mean_low << ", " << bulge_mean_high << "], bulge_sigma_" << suffix << "[" << bulge_sigma_low << ", " << bulge_sigma_high << "])";
  std::cout << "AE (" << suffix << ") = " << factory_cmd.str().c_str() << std::endl;
  ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");

  factory_cmd << "Gaussian::gauss_bulge_" << suffix << "(energy[" << energy_low << ", " << energy_high << "], gaus_bulge_mean_" << suffix << "[" << gaus_bulge_mean_low << ", " << gaus_bulge_mean_high << "], gaus_bulge_sigma_" << suffix << "[" << gaus_bulge_sigma_low << ", " << gaus_bulge_sigma_high << "])";
  std::cout << "AE (" << suffix << ") = " << factory_cmd.str().c_str() << std::endl;
  ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");

  factory_cmd << "FCONV::landau_x_gauss_bulge_" << suffix << "(energy, landau_bulge_" << suffix << ", gauss_bulge_" << suffix << ")";
  std::cout << "AE (" << suffix << ") = " << factory_cmd.str().c_str() << std::endl;
  ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");
  sum_factory_cmd << "nsig_" << suffix << "[" << nsig_low << ", " << nsig_high << "]*landau_x_gauss_" << suffix;
  n_fit_params += 4; // landau_mean, landau_sigma, gauss_sigma, nbkg

  // Now create the SUM pdf
  sum_factory_cmd << ")";
  std::cout << "AE (" << suffix << ") = " << sum_factory_cmd.str().c_str() << std::endl;
  ws->factory(sum_factory_cmd.str().c_str()); sum_factory_cmd.str("");
}

void FitHistogram(RooWorkspace* ws, TH1F* hist, std::string suffix) {
  // Import the histogram into a RooFit data hist
  std::string dataname = "data_" + suffix;
  RooDataHist data(dataname.c_str(), "SiT Energy", (ws->argSet("energy")), hist);
  ws->import(data);
  std::string pdfname = "sum_" + suffix;
  //  std::string pdfname = "landau_x_gauss_" + suffix;
  (ws->pdf(pdfname.c_str()))->fitTo(data);
}

void DrawPlot(RooWorkspace* ws, TCanvas* c1, std::string suffix) {
  // Draw the fit
  RooPlot* Eframe = (ws->var("energy"))->frame();
  std::string dataname = "data_" + suffix;
  RooDataHist* data = (RooDataHist*) ws->data(dataname.c_str());
  data->plotOn(Eframe, RooFit::MarkerSize(0.05), RooFit::Name("hist"));
  std::string sumpdfname = "sum_" + suffix;
  std::string bulgepdfname = "bulge_signal_" + suffix;
  //  std::string landaupdfname = "landau_bkg_" + suffix;
  std::string landaupdfname = "landau_x_gauss_" + suffix;
    //    (ws->pdf(landaupdfname.c_str()))->plotOn(Eframe, RooFit::LineWidth(1), RooFit::Name("fit")); // give these names for the chiSquare call later
    (ws->pdf(sumpdfname.c_str()))->plotOn(Eframe, RooFit::LineWidth(1), RooFit::Name("fit")); // give these names for the chiSquare call later
    (ws->pdf(sumpdfname.c_str()))->plotOn(Eframe, RooFit::LineWidth(1), RooFit::Components(ws->argSet(bulgepdfname.c_str())), RooFit::LineColor(kRed));
    (ws->pdf(sumpdfname.c_str()))->plotOn(Eframe, RooFit::LineWidth(1), RooFit::Components(ws->argSet(landaupdfname.c_str())), RooFit::LineColor(kBlue), RooFit::LineStyle(kDashed));

  Eframe->Draw();
  histtitle.str("");
  histtitle << "SiT Energy (" << suffix << ")";
  Eframe->SetTitle(histtitle.str().c_str());
  Eframe->SetXTitle("Energy [keV]");

  goodness_of_fit = Eframe->chiSquare("fit", "hist", n_fit_params);
}

void UpdateParameters(RooWorkspace* ws, std::string suffix) {
  std::string this_landau_mean_var = "landau_mean_" + suffix;
  std::string this_landau_sigma_var = "landau_sigma_" + suffix;
  std::string this_gauss_mean_var = "gauss_mean_" + suffix;
  std::string this_gauss_sigma_var = "gauss_sigma_" + suffix;
  std::string this_bulge_mean_var = "bulge_mean_" + suffix;
  std::string this_bulge_sigma_var = "bulge_sigma_" + suffix;
  std::string this_gaus_bulge_sigma_var = "gaus_bulge_sigma_" + suffix;
  std::string this_nsig_var = "nsig_"+suffix;
  std::string this_nbkg_var = "nbkg_"+suffix;

  landau_mean_low = landau_mean_high = ws->var(this_landau_mean_var.c_str())->getVal();
  landau_mean_error = ws->var(this_landau_mean_var.c_str())->getError();
  landau_sigma_low = landau_sigma_high = ws->var(this_landau_sigma_var.c_str())->getVal();
  landau_sigma_error = ws->var(this_landau_sigma_var.c_str())->getError();
  gauss_mean_low = gauss_mean_high = ws->var(this_gauss_mean_var.c_str())->getVal();
  gauss_mean_error = ws->var(this_gauss_mean_var.c_str())->getError();
  gauss_sigma_low = gauss_sigma_high = ws->var(this_gauss_sigma_var.c_str())->getVal();
  gauss_sigma_error = ws->var(this_gauss_sigma_var.c_str())->getError();
  bulge_mean_low = bulge_mean_high = ws->var(this_bulge_mean_var.c_str())->getVal();
  bulge_mean_error = ws->var(this_bulge_mean_var.c_str())->getError();
  bulge_sigma_low = bulge_sigma_high = ws->var(this_bulge_sigma_var.c_str())->getVal();
  bulge_sigma_error = ws->var(this_bulge_sigma_var.c_str())->getError();
  gaus_bulge_sigma_low = gaus_bulge_sigma_high = ws->var(this_gaus_bulge_sigma_var.c_str())->getVal();
  gaus_bulge_sigma_error = ws->var(this_gaus_bulge_sigma_var.c_str())->getError();
  nsig = nsig_low = nsig_high = ws->var(this_nsig_var.c_str())->getVal();
  nsig_error = ws->var(this_nsig_var.c_str())->getError();
  nbkg = nbkg_low = nbkg_high = ws->var(this_nbkg_var.c_str())->getVal();
  nbkg_error = ws->var(this_nbkg_var.c_str())->getError();
  
}

void DrawText(RooWorkspace* ws, TH1F* hist) {
  std::stringstream text;
  //  double x_text = 1600;
  double x_text = 600;
  double y_text = hist->GetBinContent(hist->GetMaximumBin());
  double y_step = hist->GetBinContent(hist->GetMaximumBin())*0.2;
  TLatex* latex = NULL;
  double stopping_rate = nsig / (nsig + nbkg);
  std::cout << "Stopping Rate = N_sig / (N_sig + N_bkg) = " << stopping_rate << std::endl;

  text << "#splitline{N_{sig} = " << nsig << ", N_{bkg} = " << nbkg << "}{Stopping Rate = " << stopping_rate << "}";
  //  std::cout << "x, y = " << x_text << ", " << y_text << std::endl;
  latex = new TLatex(x_text, y_text, text.str().c_str());
  latex->SetTextSize(0.04);
  latex->SetTextAlign(22);
  latex->Draw();

  // Mean and sigma of stop peak
  text.str("");
  text << "Stopped Muon Peak: #splitline{Mean = (" << bulge_mean_low << " #pm " << bulge_mean_error << ") keV}{#sigma = (" << bulge_sigma_low << " #pm " << bulge_sigma_error << ") keV}";
  latex = new TLatex(x_text, y_text-y_step, text.str().c_str());
  latex->SetTextSize(0.035);
  latex->SetTextAlign(22);
  latex->SetTextColor(kRed);
  latex->Draw();
  
  text.str("");
  text << "Punchthrough Landau: #splitline{Mean = (" << landau_mean_low << " #pm " << landau_mean_error << ") keV}{#sigma = (" << landau_sigma_low << " #pm " << landau_sigma_error << ") keV}";
  latex = new TLatex(x_text, y_text-2*y_step, text.str().c_str());
  latex->SetTextSize(0.035);
  latex->SetTextAlign(22);
  latex->SetTextColor(kBlue);
  latex->Draw();

  text.str("");
  text << "Punchthrough Gauss: #sigma = (" << gauss_sigma_low << " #pm " << gauss_sigma_error << ") keV";
  latex = new TLatex(x_text, y_text-3*y_step, text.str().c_str());
  latex->SetTextSize(0.035);
  latex->SetTextAlign(22);
  latex->SetTextColor(kBlue);
  latex->Draw();
}

void PunchThroughPeak_RooFit(TH1F* hist, std::string this_suffix, bool save_pdfs=false) {

  TCanvas* c1 = new TCanvas("c1", "c1");
  //  c1->Divide(1,2);

  // Create the workspace and functions for fitting and create the summed pdf as we go
  // Fit to the hsitogram that has an anticoincidence with siL3 to get some of the pdf parameters correct
  RooWorkspace* ws = new RooWorkspace("ws", kTRUE);
  //  std::string this_suffix = "";
  CreateSummedPdf(ws, this_suffix);
  FitHistogram(ws, hist, this_suffix);
  DrawPlot(ws, c1, this_suffix);
  UpdateParameters(ws, this_suffix);
  DrawText(ws, hist);

  std::cout << "Bulge Mean = " << bulge_mean_low << " +/- " << bulge_mean_error << std::endl;
  std::cout << "Bulge Sigma = " << bulge_sigma_low << " +/- " << bulge_sigma_error << std::endl;
  std::cout << "chi^2 = " << goodness_of_fit << std::endl;
  std::cout << "Bulge Area (N_sig) = " << nsig << " +/- " << nsig_error << std::endl;
  std::cout << "N_bkg = " << nbkg << " +/- " << nbkg_error << std::endl;
  
  ws->Print();

  std::stringstream pdfname;
  if (save_pdfs) {
    pdfname.str("");
    //    pdfname << "/gpfs/home/edmonds_a/plots/2017-06-28/" << histname.str() << "_Run" << run_number << "_SF" << std::setw(4) << std::setfill('0') << scale_factor*1000 << ".pdf";
    //    c1->SaveAs(pdfname.str().c_str());
  }
}
