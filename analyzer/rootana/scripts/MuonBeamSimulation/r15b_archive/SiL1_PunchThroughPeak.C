#include "SiL1_PunchThroughPeak_Parameters.h"

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
  factory_cmd << "Gaussian::bulge_signal_" << suffix << "(energy[" << energy_low << ", " << energy_high << "], bulge_mean_" << suffix << "[" << bulge_mean_low << ", " << bulge_mean_high << "], bulge_sigma_" << suffix << "[" << bulge_sigma_low << ", " << bulge_sigma_high << "])";
  std::cout << "AE (" << suffix << ") = " << factory_cmd.str().c_str() << std::endl;
  ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");
  sum_factory_cmd << ", nsig_" << suffix << "[" << nsig_low << ", " << nsig_high << "]*bulge_signal_" << suffix;
  n_fit_params += 3; // bulge_mean, bulge_sigma, nsig
  
      
  // Now create the SUM pdf
  sum_factory_cmd << ")";
  std::cout << "AE (" << suffix << ") = " << sum_factory_cmd.str().c_str() << std::endl;
  ws->factory(sum_factory_cmd.str().c_str()); sum_factory_cmd.str("");
}

void FitHistogram(RooWorkspace* ws, TH1F* hist, std::string suffix) {
  // Import the histogram into a RooFit data hist
  std::string dataname = "data_" + suffix;
  RooDataHist data(dataname.c_str(), "SiL1 Energy", (ws->argSet("energy")), hist);
  ws->import(data);
  std::string pdfname = "sum_" + suffix;
  (ws->pdf(pdfname.c_str()))->fitTo(data);
}

void DrawPlot(RooWorkspace* ws, TCanvas* c1, std::string suffix, int run_number, float scale_factor) {
  // Draw the fit
  RooPlot* Eframe = (ws->var("energy"))->frame();
  std::string dataname = "data_" + suffix;
  RooDataHist* data = (RooDataHist*) ws->data(dataname.c_str());
  data->plotOn(Eframe, RooFit::MarkerSize(0.05), RooFit::Name("hist"));
  std::string sumpdfname = "sum_" + suffix;
  std::string bulgepdfname = "bulge_signal_" + suffix;
  std::string landaupdfname = "landau_bkg_" + suffix;
  (ws->pdf(sumpdfname.c_str()))->plotOn(Eframe, RooFit::LineWidth(1), RooFit::Name("fit")); // give these names for the chiSquare call later
  (ws->pdf(sumpdfname.c_str()))->plotOn(Eframe, RooFit::LineWidth(1), RooFit::Components(ws->argSet(bulgepdfname.c_str())), RooFit::LineColor(kRed));
  (ws->pdf(sumpdfname.c_str()))->plotOn(Eframe, RooFit::LineWidth(1), RooFit::Components(ws->argSet(landaupdfname.c_str())), RooFit::LineColor(kBlue), RooFit::LineStyle(kDashed));

  Eframe->Draw();
  histtitle.str("");
  histtitle << "SiL1 Energy (" << suffix << ", Run " << run_number << ", SF=" << scale_factor << ")";
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
  nsig = nsig_low = nsig_high = ws->var(this_nsig_var.c_str())->getVal();
  nsig_error = ws->var(this_nsig_var.c_str())->getError();
  nbkg = nbkg_low = nbkg_high = ws->var(this_nbkg_var.c_str())->getVal();
  nbkg_error = ws->var(this_nbkg_var.c_str())->getError();
}

void DrawText(RooWorkspace* ws, TH1F* hist) {
  double stopping_rate = nsig / (nsig + nbkg);
  std::cout << "Stopping Rate = N_sig / (N_sig + N_bkg) = " << stopping_rate << std::endl;

  std::stringstream text;
  text << "#splitline{N_{sig} = " << nsig << ", N_{bkg} = " << nbkg << "}{Stopping Rate = " << stopping_rate << "}";
  double x_text = 1600;
  double y_text = hist->GetBinContent(hist->GetMaximumBin());
  //  std::cout << "x, y = " << x_text << ", " << y_text << std::endl;
  TLatex* latex = new TLatex(x_text, y_text, text.str().c_str());
  latex->SetTextSize(0.04);
  latex->SetTextAlign(22);
  latex->Draw();

  // Mean and sigma of stop peak
  double y_step = hist->GetBinContent(hist->GetMaximumBin())*0.2;
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

void SiL1_PunchThroughPeak(int run_number, float scale_factor, bool save_pdfs=false) {

  TCanvas* c1 = new TCanvas("c1", "c1");
  //  c1->Divide(1,2);

  filename.str("");
  filename << "stopped-muon-discrepancy-plots_run" << std::setw(5) << std::setfill('0') << run_number << "_eff09685.root";
  //  filename << "/gpfs/home/edmonds_a/data/out/v31/out" << std::setw(5) << std::setfill('0') << run_number << ".root";
  //  filename << "/gpfs/home/edmonds_a/data/out/v31/Si16b.root";
  //  filename << "/gpfs/home/edmonds_a/data/out/v30/out" << std::setw(5) << std::setfill('0') << run_number << ".root";
  TFile* file = new TFile(filename.str().c_str(), "READ");
  if (file->IsZombie()) {
    std::cout << "Problem opening file " << filename.str() << std::endl;
    return;
  }

  histname.str("");
  //  histname << "hSiL1_Energy_NoCuts";
  histname << "hSiL1_Energy_wNoSiL3Coincidence";
  //  histname << "PlotTAP_Energy/hSiL1-14-S#FirstComplete_RoughSyncPulseCut#{constant_fraction=0.20}{no_time_shift= true}_Energy;1";
  TH1F* hEnergy = (TH1F*) file->Get(histname.str().c_str());
  if (!hEnergy) {
    std::cout << "Problem finding histogram " << histname.str() << std::endl;
  }
  hEnergy->Rebin(2);
      
      
  // Create the workspace and functions for fitting and create the summed pdf as we go
  // Fit to the hsitogram that has an anticoincidence with siL3 to get some of the pdf parameters correct
  RooWorkspace* ws = new RooWorkspace("ws", kTRUE);
  std::string this_suffix = "wNoSiL3Coinc";
  //  std::string this_suffix = "SiL1_14_S";
  CreateSummedPdf(ws, this_suffix);
  FitHistogram(ws, hEnergy, this_suffix);
  //  c1->cd(1);
  DrawPlot(ws, c1, this_suffix, run_number, scale_factor);
  UpdateParameters(ws, this_suffix);
  DrawText(ws, hEnergy);

  std::cout << "Bulge Mean = " << bulge_mean_low << " +/- " << bulge_mean_error << std::endl;
  std::cout << "Bulge Sigma = " << bulge_sigma_low << " +/- " << bulge_sigma_error << std::endl;
  std::cout << "chi^2 = " << goodness_of_fit << std::endl;
  std::cout << "Bulge Area (N_sig) = " << nsig << " +/- " << nsig_error << std::endl;
  std::cout << "N_bkg = " << nbkg << " +/- " << nbkg_error << std::endl;
  
  histname.str("");
  histname << "hSiL1_Energy_NoCuts";
  TH1F* hEnergy_NoCuts = (TH1F*) file->Get(histname.str().c_str());
  if (!hEnergy_NoCuts) {
    std::cout << "Problem finding histogram " << histname.str() << std::endl;
  }
  hEnergy_NoCuts->Rebin(2);
      
  this_suffix = "NoCuts";
  // Allow the landau sigma parameter to float
  landau_mean_low -=100;
  landau_mean_high +=100;
  landau_sigma_low = 10;
  landau_sigma_high = 200;
  gauss_sigma_low = 10;
  gauss_sigma_high = 200;
  //  bulge_mean_low -= 100;
  //  bulge_mean_high += 100;
  //  bulge_sigma_low -= 50;
  //  bulge_sigma_high += 50;
  nbkg_high = 1000000;
  nsig_high = 1000000;
  CreateSummedPdf(ws, this_suffix);
  FitHistogram(ws, hEnergy_NoCuts, this_suffix);
  
  TCanvas* c2 = new TCanvas("c2", "c2");
  //  c1->cd(2);
  //  DrawPlot(ws, c1, this_suffix, run_number, scale_factor);
  DrawPlot(ws, c2, this_suffix, run_number, scale_factor);
  UpdateParameters(ws, this_suffix);
  DrawText(ws, hEnergy_NoCuts);
  

  //  RooPlot* Eframe_zoomin = (RooPlot*) Eframe->Clone("Eframe_zoomin");
  //  Eframe_zoomin->GetXaxis()->SetRangeUser(300, 500);
  //  Eframe_zoomin->Draw();
  ws->Print();

  std::stringstream pdfname;
  if (save_pdfs) {
    pdfname.str("");
    pdfname << "/gpfs/home/edmonds_a/plots/2017-06-28/" << histname.str() << "_Run" << run_number << "_SF" << std::setw(4) << std::setfill('0') << scale_factor*1000 << ".pdf";
    c1->SaveAs(pdfname.str().c_str());
  }

  //  RooHist* hresid = Eframe->residHist();
  //  c1->cd(2);
  //  RooHist* hresid = Eframe->pullHist("hist", "pdf");
  //  hresid->GetXaxis()->SetRangeUser(energy_low, energy_high);
  //  hresid->Draw();
}
