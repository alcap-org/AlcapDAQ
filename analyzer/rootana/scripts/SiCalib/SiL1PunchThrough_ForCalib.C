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

struct RangeCurveRun {
  double scale_factor;
  int run_number; // only have one run number per SF here
  double SiL1_bulge_pos[n_si_channels];
  double SiL1_bulge_pos_error[n_si_channels];
  double SiL1_bulge_sigma[n_si_channels];
  double SiL1_bulge_sigma_error[n_si_channels];
  double goodness_of_fit[n_si_channels];
  double zoom_in_ymax;
};


void SiL1PunchThrough_ForCalib() {

  
  std::vector<RangeCurveRun*> scale_factors;
  
  RangeCurveRun* sf_0_83 = new RangeCurveRun;
  sf_0_83->scale_factor = 0.83;
  sf_0_83->run_number = 9298;
  sf_0_83->zoom_in_ymax = 500;
  scale_factors.push_back(sf_0_83);

  RangeCurveRun* sf_0_84 = new RangeCurveRun;
  sf_0_84->scale_factor = 0.84;
  sf_0_84->run_number = 9297;
  sf_0_84->zoom_in_ymax = 500;
  scale_factors.push_back(sf_0_84);

  RangeCurveRun* sf_0_85 = new RangeCurveRun;
  sf_0_85->scale_factor = 0.85;
  sf_0_85->run_number = 9294;
  sf_0_85->zoom_in_ymax = 300;
  scale_factors.push_back(sf_0_85);

  RangeCurveRun* sf_0_86 = new RangeCurveRun;
  sf_0_86->scale_factor = 0.86;
  sf_0_86->run_number = 9291;
  sf_0_86->zoom_in_ymax = 300;
  scale_factors.push_back(sf_0_86);
  
  //  RangeCurveRun* sf_0_87 = new RangeCurveRun;
  //  sf_0_87->scale_factor = 0.87;
  //  sf_0_87->run_number = 9290;
  //  sf_0_87->zoom_in_ymax = 100;
  //  scale_factors.push_back(sf_0_87);

  TCanvas* c1[n_si_channels];
  TCanvas* c_zoomin[n_si_channels];
  std::stringstream canvasname;
  for (int i_si_channel = 0; i_si_channel < n_si_channels; ++i_si_channel) {
    canvasname.str("");
    canvasname << "c_SiL1-" << i_si_channel+1 << "-S";
    c1[i_si_channel] = new TCanvas(canvasname.str().c_str(), canvasname.str().c_str());
    c1[i_si_channel]->Divide(2,2);

    canvasname.str("");
    canvasname << "c_SiL1-" << i_si_channel+1 << "-S_ZoomIn";
    c_zoomin[i_si_channel] = new TCanvas(canvasname.str().c_str(), canvasname.str().c_str());
    c_zoomin[i_si_channel]->Divide(2,2);
  }

  std::stringstream filename, histname, histtitle;
  for (std::vector<RangeCurveRun*>::iterator i_scale_factor = scale_factors.begin(); i_scale_factor != scale_factors.end(); ++i_scale_factor) {
    int i_sf = (i_scale_factor - scale_factors.begin());

    filename.str("");
    filename << "/gpfs/home/edmonds_a/AlcapDAQ/analyzer/rootana/out0" << (*i_scale_factor)->run_number << ".root";
    TFile* file = new TFile(filename.str().c_str(), "READ");
    if (file->IsZombie()) {
      std::cout << "Problem opening file " << filename.str() << std::endl;
      continue;
    }

    for (int i_si_channel = 0; i_si_channel < n_si_channels; ++i_si_channel) {
      c1[i_si_channel]->cd(i_sf+1);
      //    c1->GetPad(i_si_channel+1)->SetLogy();
      /*    histname.str("");
	    histname << "PlotTAP_Amplitude/hSiL1-" << i_si_channel+1 << "-S#MaxBinAPGenerator#any_Amplitude";
	    TH1F* hAmplitude_All = (TH1F*) file->Get(histname.str().c_str());
	    
	    histname.str("");
	    histname << "PlotTAP_Amplitude_PunchThroughMuons_SiL1/hSiL1-" << i_si_channel+1 << "-S#MaxBinAPGenerator#any_Amplitude_PunchThroughMuons_SiL1";
	    TH1F* hAmplitude_PunchThrough = (TH1F*) file->Get(histname.str().c_str());
      */
      histname.str("");
      histname << "PlotTAP_Amplitude_PunchThroughMuons_SiL1/hSiL1-" << i_si_channel+1 << "-S#MaxBinAPGenerator#any_Amplitude_NotPunchThroughMuons_SiL1";    
      TH1F* hAmplitude_NotPunchThrough = (TH1F*) file->Get(histname.str().c_str());
      hAmplitude_NotPunchThrough->Rebin(16);
      
      // Keep track of the number of parameters
      int n_fit_params = 0;
      
      // Create the workspace and functions for fitting and create the summed pdf as we go
      RooWorkspace* ws = new RooWorkspace("ws", kTRUE);
      std::stringstream factory_cmd, sum_factory_cmd;
      sum_factory_cmd << "SUM::sum(";
      
      double adc_low = 150;
      double adc_high = 1000;
      // First, the Landau convoluted with a Gaussian
      factory_cmd << "Landau::landau_pdf(adc[" << adc_low << ", " << adc_high << "], landau_mean[150, 300], landau_sigma[0.1, 20])";
      ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");
      factory_cmd << "Gaussian::gauss_pdf(adc[" << adc_low << ", " << adc_high << "], gauss_mean[0], gauss_sigma[1, 50])";
      ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");
      factory_cmd << "FCONV::landau_x_gauss(adc, landau_pdf, gauss_pdf)";
      ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");
      sum_factory_cmd << "nbkg[0, 10000]*landau_x_gauss";
      n_fit_params += 4; // landau_mean, landau_sigma, gauss_sigma, nbkg
      
      // Now the bulge of interest
      factory_cmd << "Gaussian::bulge_pdf(adc[" << adc_low << ", " << adc_high << "], bulge_mean[300, 500], bulge_sigma[10, 75])";
      ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");
      sum_factory_cmd << ", bulge_area[0,1000]*bulge_pdf";
      n_fit_params += 3; // bulge_mean, bulge_sigma, bulge_area
      
      
      // Now create the SUM pdf
      sum_factory_cmd << ")";
      ws->factory(sum_factory_cmd.str().c_str()); sum_factory_cmd.str("");
      
      // Import the histogram into a RooFit data hist
      RooDataHist data("data", "Amplitude (Not Punch Through)", (ws->argSet("adc")), hAmplitude_NotPunchThrough);
      (ws->pdf("sum"))->fitTo(data);
      
      // Draw the fit
      RooPlot* Eframe = (ws->var("adc"))->frame();
      data.plotOn(Eframe, RooFit::MarkerSize(0.05));
      (ws->pdf("sum"))->plotOn(Eframe, RooFit::LineWidth(1));
      (ws->pdf("sum"))->plotOn(Eframe, RooFit::LineWidth(1), RooFit::Components(ws->argSet("bulge_pdf")), RooFit::LineColor(kRed));
      (ws->pdf("sum"))->plotOn(Eframe, RooFit::LineWidth(1), RooFit::Components(ws->argSet("landau_pdf")), RooFit::LineColor(kBlue), RooFit::LineStyle(kDashed));
      Eframe->Draw();
      
      histtitle.str("");
      histtitle << "SF = " << (*i_scale_factor)->scale_factor << ", Run " << (*i_scale_factor)->run_number << " (SiL1-" << i_si_channel+1 << "-S)";
      Eframe->SetTitle(histtitle.str().c_str());
      Eframe->SetXTitle("Pulse Amplitude [ADC]");
      
      (*i_scale_factor)->SiL1_bulge_pos[i_si_channel] = ws->var("bulge_mean")->getVal();
      (*i_scale_factor)->SiL1_bulge_pos_error[i_si_channel] = ws->var("bulge_mean")->getError();
      (*i_scale_factor)->SiL1_bulge_sigma[i_si_channel] = ws->var("bulge_sigma")->getVal();
      (*i_scale_factor)->SiL1_bulge_sigma_error[i_si_channel] = ws->var("bulge_sigma")->getError();
      (*i_scale_factor)->goodness_of_fit[i_si_channel] = Eframe->chiSquare(n_fit_params);
      c_zoomin[i_si_channel]->cd(i_sf+1);
      RooPlot* Eframe_zoomin = (RooPlot*) Eframe->Clone("Eframe_zoomin");
      Eframe_zoomin->GetXaxis()->SetRangeUser(300, 500);
      Eframe_zoomin->GetYaxis()->SetRangeUser(0, (*i_scale_factor)->zoom_in_ymax);
      Eframe_zoomin->Draw();
      //    ws->Print();

      std::cout << "SiL1-" << i_si_channel+1 << "-S, SF=" << (*i_scale_factor)->scale_factor << " done" << std::endl;
    }
  }

  TCanvas* c_peak_pos = new TCanvas("c_peak_pos", "c_peak_pos");
  c_peak_pos->Divide(4, 4);

  std::stringstream graphtitle;
  double final_bulge_pos[n_si_channels];
  double final_bulge_pos_error[n_si_channels];
  for (int i_si_channel = 0; i_si_channel < n_si_channels; ++i_si_channel) {
    c_peak_pos->cd(i_si_channel+1);
    double scale_factor_vals[scale_factors.size()];
    double peak_pos[scale_factors.size()];
    double peak_pos_error[scale_factors.size()];

    for (std::vector<RangeCurveRun*>::iterator i_scale_factor = scale_factors.begin(); i_scale_factor != scale_factors.end(); ++i_scale_factor) {
      int i_sf = (i_scale_factor - scale_factors.begin());

      scale_factor_vals[i_sf] = (*i_scale_factor)->scale_factor;
      peak_pos[i_sf] = (*i_scale_factor)->SiL1_bulge_pos[i_si_channel];
      peak_pos_error[i_sf] = (*i_scale_factor)->SiL1_bulge_pos_error[i_si_channel];

      std::cout << "SF = " << scale_factor_vals[i_sf] << ", Run " << (*i_scale_factor)->run_number << ", SiL1-" << i_si_channel+1 << "-S: Peak Position = " << peak_pos[i_sf] << " +/- " << peak_pos_error[i_sf] << ", Peak Sigma = " << (*i_scale_factor)->SiL1_bulge_sigma[i_si_channel] << " +/- " << (*i_scale_factor)->SiL1_bulge_sigma_error[i_si_channel] << ", chi^2 / ndf = " << (*i_scale_factor)->goodness_of_fit[i_si_channel] << std::endl;
    }

    TGraphErrors* gr = new TGraphErrors(scale_factors.size(), scale_factor_vals, peak_pos, NULL, peak_pos_error);
    graphtitle.str("");
    graphtitle << "SiL1-" << i_si_channel+1 << "-S" << std::endl;
    gr->Draw("A*");
    //    gr->SetMarkerStyle(kFullTriangleUp);
    gr->SetMarkerSize(0.05);
    gr->SetTitle(graphtitle.str().c_str());
    gr->GetXaxis()->SetTitle("Scale Factor");
    gr->GetYaxis()->SetTitle("Stopped Muon Peak Position [ADC]");
    gr->SetMinimum(300);
    gr->SetMaximum(500);

    // Fit a line
    TF1* line = new TF1("line", "[0]", 0.83, 0.87);
    TFitResultPtr fit_result = gr->Fit(line, "QRS");
    if ( (Int_t) fit_result == 0) {
      final_bulge_pos[i_si_channel] = fit_result->Parameter(0);
      final_bulge_pos_error[i_si_channel] = fit_result->ParError(0);
    }
  }

  std::stringstream pdfname;
  std::string basename = "~/plots/2016-12-16/PunchThroughMuonFits_SiL1-";
  for (int i_si_channel = 0; i_si_channel < n_si_channels; ++i_si_channel) {
    pdfname.str("");
    pdfname << basename << i_si_channel+1 << "-S_LandauXGauss.pdf";
    c1[i_si_channel]->SaveAs(pdfname.str().c_str());

    pdfname.str("");
    pdfname << basename << i_si_channel+1 << "-S_ZoomIn_LandauXGauss.pdf";
    c_zoomin[i_si_channel]->SaveAs(pdfname.str().c_str());
  }
  pdfname.str("");
  pdfname << "~/plots/2016-12-16/PunchThroughMuonFits_PeakPositions_LandauXGauss.pdf";
  c_peak_pos->SaveAs(pdfname.str().c_str());

  for (int i_si_channel = 0; i_si_channel < n_si_channels; ++i_si_channel) {
    std::cout << "SiL1-" << i_si_channel+1 << "-S: Final Bulge Position (from fit) = " << final_bulge_pos[i_si_channel] << " +/- " << final_bulge_pos_error[i_si_channel] << std::endl;
  }
}
