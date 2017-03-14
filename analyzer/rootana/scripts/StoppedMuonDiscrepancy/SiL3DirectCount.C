#include "TFile.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TSpectrum.h"
#include "TF1.h"
#include "TFitResult.h"
#include "TGraphErrors.h"

#include "RooWorkspace.h"
#include "RooPlot.h"
#include "RooDataHist.h"
#include "RooAbsPdf.h"
#include "RooRealVar.h"

#include <sstream>

const int n_si_channels = 16;

struct FitParams {
  double landau_mean;
  double landau_mean_error;
  double landau_sigma;
  double landau_sigma_error;
  double gauss_sigma;
  double gauss_sigma_error;
  double nsig;
  double nsig_error;
  /*  double bulge_mean;
  double bulge_mean_error;
  double bulge_sigma;
  double bulge_sigma_error;
  double bulge_area;
  double bulge_area_error;
  */
  double goodness_of_fit;
};


void SiL3DirectCount(TFile* file, TFile* out_file, TH1F* hHistToFill) {

  double zoom_in_ymax = 300;

  FitParams SiL3_fit_pars;
  
  TCanvas* c1;
  TCanvas* c_zoomin;
  std::stringstream canvasname;
  canvasname.str("");
  canvasname << "c_SiL3";
  c1 = new TCanvas(canvasname.str().c_str(), canvasname.str().c_str());

  canvasname.str("");
  canvasname << "c_SiL3_ZoomIn";
  c_zoomin = new TCanvas(canvasname.str().c_str(), canvasname.str().c_str());

  // Create the workspace and functions for fitting and create the summed pdf as we go
  RooWorkspace* ws = new RooWorkspace("ws", kTRUE);

  std::stringstream histname, histtitle;
  
  histname << "PlotTAP_StoppedMuonDiscrepancy/hSiL3_Energy_wSiL1Coincidence";
  TH1F* hEnergy_SiL3Stop = (TH1F*) (file->Get(histname.str().c_str())->Clone());
  hEnergy_SiL3Stop->SetDirectory(0);
  if (!hEnergy_SiL3Stop) {
    std::cout << "Problem finding histogram " << histname.str() << std::endl;
    return;
  }
  hEnergy_SiL3Stop->Rebin(100);
        
  // Keep track of the number of parameters
  int n_fit_params = 0;
    
    
  std::stringstream factory_cmd, sum_factory_cmd;
  sum_factory_cmd << "SUM::sum(";
    
  double energy_low = 100;
  double energy_high = 18000;
  // First, the Landau convoluted with a Gaussian
  factory_cmd << "Landau::landau_pdf(energy[" << energy_low << ", " << energy_high << "], landau_mean[600, 1000], landau_sigma[10, 175])";
  ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");
  factory_cmd << "Gaussian::gauss_pdf(energy[" << energy_low << ", " << energy_high << "], gauss_mean[0], gauss_sigma[20, 400])";
  ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");
  factory_cmd << "FCONV::landau_x_gauss(energy, landau_pdf, gauss_pdf)";
  ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");
  sum_factory_cmd << "nsig[0, 100000]*landau_x_gauss";
  n_fit_params += 4; // landau_mean, landau_sigma, gauss_sigma, nsig
  
  // Now the bulge of interest
  /*  factory_cmd << "Gaussian::bulge_pdf(energy[" << energy_low << ", " << energy_high << "], bulge_mean[975, 1200], bulge_sigma[10, 90])";
  ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");
  sum_factory_cmd << ", bulge_area[0,10000]*bulge_pdf";
  n_fit_params += 3; // bulge_mean, bulge_sigma, bulge_area
  */
    
  // Now create the SUM pdf
  sum_factory_cmd << ")";
  ws->factory(sum_factory_cmd.str().c_str()); sum_factory_cmd.str("");
  
  // Import the histogram into a RooFit data hist
  RooDataHist data("data", "Energy (Not Punch Through)", (ws->argSet("energy")), hEnergy_SiL3Stop);
  (ws->pdf("sum"))->fitTo(data);
    
  // Draw the fit
  c1->cd();
  RooPlot* Eframe = (ws->var("energy"))->frame();
  data.plotOn(Eframe, RooFit::MarkerSize(0.05));
  (ws->pdf("sum"))->plotOn(Eframe, RooFit::LineWidth(1));
  //  (ws->pdf("sum"))->plotOn(Eframe, RooFit::LineWidth(1), RooFit::Components(ws->argSet("bulge_pdf")), RooFit::LineColor(kRed));
  //  (ws->pdf("sum"))->plotOn(Eframe, RooFit::LineWidth(1), RooFit::Components(ws->argSet("landau_pdf")), RooFit::LineColor(kBlue), RooFit::LineStyle(kDashed));
  Eframe->Draw();
  
    
  histtitle.str("");
  histtitle << "Run 9294 (SiL3-S)";
  Eframe->SetTitle(histtitle.str().c_str());
  Eframe->SetXTitle("Energy [keV]");

  SiL3_fit_pars.landau_mean = ws->var("landau_mean")->getVal();
  SiL3_fit_pars.landau_mean_error = ws->var("landau_mean")->getError();
  SiL3_fit_pars.landau_sigma = ws->var("landau_sigma")->getVal();
  SiL3_fit_pars.landau_sigma_error = ws->var("landau_sigma")->getError();
  SiL3_fit_pars.gauss_sigma = ws->var("gauss_sigma")->getVal();
  SiL3_fit_pars.gauss_sigma_error = ws->var("gauss_sigma")->getError();
  SiL3_fit_pars.nsig = ws->var("nsig")->getVal();
  SiL3_fit_pars.nsig_error = ws->var("nsig")->getError();
  /*  SiL3_fit_pars.bulge_mean = ws->var("bulge_mean")->getVal();
  SiL3_fit_pars.bulge_mean_error = ws->var("bulge_mean")->getError();
  SiL3_fit_pars.bulge_sigma = ws->var("bulge_sigma")->getVal();
  SiL3_fit_pars.bulge_sigma_error = ws->var("bulge_sigma")->getError();
  SiL3_fit_pars.bulge_area = ws->var("bulge_area")->getVal();
  SiL3_fit_pars.bulge_area_error = ws->var("bulge_area")->getError();
  */
  SiL3_fit_pars.goodness_of_fit = Eframe->chiSquare(n_fit_params);
  
  
  c_zoomin->cd();
  RooPlot* Eframe_zoomin = (ws->var("energy"))->frame();
  data.plotOn(Eframe_zoomin, RooFit::MarkerSize(0.05));
  (ws->pdf("sum"))->plotOn(Eframe_zoomin, RooFit::LineWidth(1));
  (ws->pdf("sum"))->plotOn(Eframe_zoomin, RooFit::LineWidth(1), RooFit::Components(ws->argSet("bulge_pdf")), RooFit::LineColor(kRed));
  (ws->pdf("sum"))->plotOn(Eframe_zoomin, RooFit::LineWidth(1), RooFit::Components(ws->argSet("landau_pdf")), RooFit::LineColor(kBlue), RooFit::LineStyle(kDashed));
  Eframe_zoomin->GetXaxis()->SetRangeUser(500, 2000);
  Eframe_zoomin->GetYaxis()->SetRangeUser(0, zoom_in_ymax);
  Eframe_zoomin->Draw();
    

  std::cout << "SiL3-S:" << std::endl;
  std::cout << "Bkg Landau Mean = " << SiL3_fit_pars.landau_mean << " +/- " << SiL3_fit_pars.landau_mean_error << std::endl;
  std::cout << "Bkg Landau Sigma = " << SiL3_fit_pars.landau_sigma << " +/- " << SiL3_fit_pars.landau_sigma_error << std::endl;
  std::cout << "Bkg Gauss Sigma = " << SiL3_fit_pars.gauss_sigma << " +/- " << SiL3_fit_pars.gauss_sigma_error << std::endl;
  std::cout << "Nsig = " << SiL3_fit_pars.nsig << " +/- " << SiL3_fit_pars.nsig_error << std::endl;
  //  std::cout << "Signal Mean = " << SiL3_fit_pars.bulge_mean << " +/- " << SiL3_fit_pars.bulge_mean_error << std::endl;
  //  std::cout << "Signal Sigma = " << SiL3_fit_pars.bulge_sigma << " +/- " << SiL3_fit_pars.bulge_sigma_error << std::endl;
  //  std::cout << "Area = " << SiL3_fit_pars.bulge_area << " +/- " << SiL3_fit_pars.bulge_area_error << std::endl;
  hHistToFill->SetBinContent(17, SiL3_fit_pars.nsig);
  hHistToFill->SetBinError(17, SiL3_fit_pars.nsig_error);

  ws->Write();
  c1->Write();
  c_zoomin->Write();
  //  out_file->Write();
}
