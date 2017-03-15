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
  double nbkg;
  double nbkg_error;
  double bulge_mean;
  double bulge_mean_error;
  double bulge_sigma;
  double bulge_sigma_error;
  double bulge_area;
  double bulge_area_error;
  double goodness_of_fit;
};


void SiL1DirectCount(TFile* file, TFile* out_file, TH1F* hHistToFill) {

  double zoom_in_ymax = 300;

  FitParams SiL1_fit_pars[n_si_channels];
  
  TCanvas* c1;
  TCanvas* c_zoomin;
  std::stringstream canvasname;
  canvasname.str("");
  canvasname << "c_SiL1";
  c1 = new TCanvas(canvasname.str().c_str(), canvasname.str().c_str());
  c1->Divide(4,4);

  canvasname.str("");
  canvasname << "c_SiL1_ZoomIn";
  c_zoomin = new TCanvas(canvasname.str().c_str(), canvasname.str().c_str());
  c_zoomin->Divide(4,4);

  // Create the workspace and functions for fitting and create the summed pdf as we go
  RooWorkspace* ws = new RooWorkspace("ws", kTRUE);

  std::stringstream histname, histtitle;
  
  for (int i_si_channel = 0; i_si_channel < n_si_channels; ++i_si_channel) {
    c1->cd(i_si_channel+1);
    histname.str("");
    histname << "PlotTAP_StoppedMuonDiscrepancy/hSiL1-" << i_si_channel+1 << "-S_Energy_wNoSiL3Coincidence";
    TH1F* hEnergy_NotPunchThrough = (TH1F*) (file->Get(histname.str().c_str())->Clone());
    hEnergy_NotPunchThrough->SetDirectory(0);
    if (!hEnergy_NotPunchThrough) {
      std::cout << "Problem finding histogram " << histname.str() << std::endl;
      continue;
    }
    hEnergy_NotPunchThrough->Rebin(25);
        
    // Keep track of the number of parameters
    int n_fit_params = 0;
    
    
    std::stringstream factory_cmd, sum_factory_cmd;
    sum_factory_cmd << "SUM::sum(";
    
    double energy_low = 350;
    double energy_high = 5000;
    // First, the Landau convoluted with a Gaussian
    factory_cmd << "Landau::landau_pdf(energy[" << energy_low << ", " << energy_high << "], landau_mean[450, 650], landau_sigma[0.1, 75])";
    ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");
    //    factory_cmd << "Gaussian::gauss_pdf(energy[" << energy_low << ", " << energy_high << "], gauss_mean[0], gauss_sigma[20, 400])";
    //    ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");
    //    factory_cmd << "FCONV::landau_x_gauss(energy, landau_pdf, gauss_pdf)";
    //    ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");
    sum_factory_cmd << "nbkg[0, 10000]*landau_pdf";//x_gauss";
    n_fit_params += 3; // landau_mean, landau_sigma, gauss_sigma, nbkg
    
    // Now the bulge of interest
    factory_cmd << "Gaussian::bulge_pdf(energy[" << energy_low << ", " << energy_high << "], bulge_mean[975, 1200], bulge_sigma[10, 90])";
    ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");
    sum_factory_cmd << ", bulge_area[0,10000]*bulge_pdf";
    n_fit_params += 3; // bulge_mean, bulge_sigma, bulge_area
    
    
    // Now create the SUM pdf
    sum_factory_cmd << ")";
    ws->factory(sum_factory_cmd.str().c_str()); sum_factory_cmd.str("");
    
    // Import the histogram into a RooFit data hist
    RooDataHist data("data", "Energy (Not Punch Through)", (ws->argSet("energy")), hEnergy_NotPunchThrough);
    (ws->pdf("sum"))->fitTo(data);
    
    // Draw the fit
    RooPlot* Eframe = (ws->var("energy"))->frame();
    data.plotOn(Eframe, RooFit::MarkerSize(0.05));
    (ws->pdf("sum"))->plotOn(Eframe, RooFit::LineWidth(1));
    (ws->pdf("sum"))->plotOn(Eframe, RooFit::LineWidth(1), RooFit::Components(ws->argSet("bulge_pdf")), RooFit::LineColor(kRed));
    (ws->pdf("sum"))->plotOn(Eframe, RooFit::LineWidth(1), RooFit::Components(ws->argSet("landau_pdf")), RooFit::LineColor(kBlue), RooFit::LineStyle(kDashed));
    Eframe->Draw();
    
    
    histtitle.str("");
    histtitle << "Run 9298 (SiL1-" << i_si_channel+1 << "-S)";
    Eframe->SetTitle(histtitle.str().c_str());
    Eframe->SetXTitle("Energy [keV]");

    SiL1_fit_pars[i_si_channel].landau_mean = ws->var("landau_mean")->getVal();
    SiL1_fit_pars[i_si_channel].landau_mean_error = ws->var("landau_mean")->getError();
    SiL1_fit_pars[i_si_channel].landau_sigma = ws->var("landau_sigma")->getVal();
    SiL1_fit_pars[i_si_channel].landau_sigma_error = ws->var("landau_sigma")->getError();
    //    SiL1_fit_pars[i_si_channel].gauss_sigma = ws->var("gauss_sigma")->getVal();
    //    SiL1_fit_pars[i_si_channel].gauss_sigma_error = ws->var("gauss_sigma")->getError();
    SiL1_fit_pars[i_si_channel].nbkg = ws->var("nbkg")->getVal();
    SiL1_fit_pars[i_si_channel].nbkg_error = ws->var("nbkg")->getError();
    SiL1_fit_pars[i_si_channel].bulge_mean = ws->var("bulge_mean")->getVal();
    SiL1_fit_pars[i_si_channel].bulge_mean_error = ws->var("bulge_mean")->getError();
    SiL1_fit_pars[i_si_channel].bulge_sigma = ws->var("bulge_sigma")->getVal();
    SiL1_fit_pars[i_si_channel].bulge_sigma_error = ws->var("bulge_sigma")->getError();
    SiL1_fit_pars[i_si_channel].bulge_area = ws->var("bulge_area")->getVal();
    SiL1_fit_pars[i_si_channel].bulge_area_error = ws->var("bulge_area")->getError();
    SiL1_fit_pars[i_si_channel].goodness_of_fit = Eframe->chiSquare(n_fit_params);
    

    c_zoomin->cd(i_si_channel+1);
    RooPlot* Eframe_zoomin = (ws->var("energy"))->frame();
    data.plotOn(Eframe_zoomin, RooFit::MarkerSize(0.05));
    (ws->pdf("sum"))->plotOn(Eframe_zoomin, RooFit::LineWidth(1));
    (ws->pdf("sum"))->plotOn(Eframe_zoomin, RooFit::LineWidth(1), RooFit::Components(ws->argSet("bulge_pdf")), RooFit::LineColor(kRed));
    (ws->pdf("sum"))->plotOn(Eframe_zoomin, RooFit::LineWidth(1), RooFit::Components(ws->argSet("landau_pdf")), RooFit::LineColor(kBlue), RooFit::LineStyle(kDashed));
    Eframe_zoomin->GetXaxis()->SetRangeUser(500, 2000);
    Eframe_zoomin->GetYaxis()->SetRangeUser(0, zoom_in_ymax);
    Eframe_zoomin->Draw();
    //    ws->Print();
    
  }

  for (int i_si_channel = 1; i_si_channel < n_si_channels-1; ++i_si_channel) { // ignoring channel 1 and 16
    std::cout << "SiL1-" << i_si_channel+1 << "-S:" << std::endl;
    std::cout << "Bkg Landau Mean = " << SiL1_fit_pars[i_si_channel].landau_mean << " +/- " << SiL1_fit_pars[i_si_channel].landau_mean_error << std::endl;
    std::cout << "Bkg Landau Sigma = " << SiL1_fit_pars[i_si_channel].landau_sigma << " +/- " << SiL1_fit_pars[i_si_channel].landau_sigma_error << std::endl;
    //    std::cout << "Bkg Gauss Sigma = " << SiL1_fit_pars[i_si_channel].gauss_sigma << " +/- " << SiL1_fit_pars[i_si_channel].gauss_sigma_error << std::endl;
    std::cout << "NBkg = " << SiL1_fit_pars[i_si_channel].nbkg << " +/- " << SiL1_fit_pars[i_si_channel].nbkg_error << std::endl;
    std::cout << "Signal Mean = " << SiL1_fit_pars[i_si_channel].bulge_mean << " +/- " << SiL1_fit_pars[i_si_channel].bulge_mean_error << std::endl;
    std::cout << "Signal Sigma = " << SiL1_fit_pars[i_si_channel].bulge_sigma << " +/- " << SiL1_fit_pars[i_si_channel].bulge_sigma_error << std::endl;
    std::cout << "Area = " << SiL1_fit_pars[i_si_channel].bulge_area << " +/- " << SiL1_fit_pars[i_si_channel].bulge_area_error << std::endl;
    hHistToFill->SetBinContent(i_si_channel+1, SiL1_fit_pars[i_si_channel].bulge_area);
    hHistToFill->SetBinError(i_si_channel+1, SiL1_fit_pars[i_si_channel].bulge_area_error);
  }

  ws->Write();
  c1->Write();
  c_zoomin->Write();
  //  out_file->Write();
}
