#include "TGraph.h"
#include "TGraphErrors.h"
#include "TFile.h"
#include "TH1.h"
#include "TFitResult.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TSpectrum.h"
#include "TLatex.h"
#include "TLine.h"

#include <iostream>
#include <sstream>
#include <iomanip>

void Si16Calib() {

  const int n_si_channels = 16;

  const int n_pulser_runs = 4;
  double pulser_voltages[n_pulser_runs] = {0.1, 0.05, 0.15, 0.2};

  // 100 mV Pulser + Am241
  const int n_calib_points = 5;
  const int Am241_element = 1;
  double mesytec = 15; // MeV/V (according to datasheet) TODO: How well do we know this?
  double frac_mesytec_uncert = 0; // ready just in case
  double attenuator = 53.2; // MeV/V (see elog:36)
  double attenuator_uncertainty = 1.1; // MeV/V
  double frac_atten_uncert = attenuator_uncertainty / attenuator;
  double pulser_uncertainty = 0.001; // V = 1mV (looked at the scope shot in elog:210)
  double calib_energies[n_calib_points] = {mesytec*pulser_voltages[0], // 100 mV pulser put in the Mesytec test input
					   5.486, // Am241
					   attenuator*pulser_voltages[1], // 50 mV pulser put through attenuator box
					   attenuator*pulser_voltages[2], // 150 mV pulser put through attenuator box
					   attenuator*pulser_voltages[3] }; // 200 mV pulser put through attenuator box
  double calib_energy_errors[n_calib_points] = 
    { calib_energies[0]*std::sqrt( (pulser_uncertainty/pulser_voltages[0])*(pulser_uncertainty/pulser_voltages[0]) + frac_mesytec_uncert*frac_mesytec_uncert), 
      0.05, 
      calib_energies[2]*std::sqrt( (pulser_uncertainty/pulser_voltages[1])*(pulser_uncertainty/pulser_voltages[1]) + frac_atten_uncert*frac_atten_uncert), 
      calib_energies[3]*std::sqrt( (pulser_uncertainty/pulser_voltages[2])*(pulser_uncertainty/pulser_voltages[2]) + frac_atten_uncert*frac_atten_uncert), 
      calib_energies[4]*std::sqrt( (pulser_uncertainty/pulser_voltages[3])*(pulser_uncertainty/pulser_voltages[3]) + frac_atten_uncert*frac_atten_uncert), 
    };
  int calib_run_numbers[n_calib_points] = {8702, 10483, 9176, 9182, 9181};
  double calib_adcs[n_si_channels][n_calib_points];
  double calib_adc_errors[n_si_channels][n_calib_points];
  double calib_gain[n_si_channels];
  double calib_gain_error[n_si_channels];
  double calib_offset[n_si_channels];
  double calib_offset_error[n_si_channels];

  double pulser_energies[n_pulser_runs] = {1.5, attenuator*0.05, attenuator*0.15, attenuator*0.2};
  double pulser_adcs[n_si_channels][n_pulser_runs];
  double pulser_adc_errors[n_si_channels][n_pulser_runs];

  double calib_adc_hist_range_low[n_calib_points] = {0, 2000, 1000, 2500, 3100}; // used to zoom in on the histogram for drawing purposes
  double calib_adc_hist_range_high[n_calib_points] = {500, 2500, 1400, 3250, 3300};
  int calib_rebin_factors[n_calib_points] = {4, 8, 4, 4, 1}; // want to rebin each calibration point slightly differently to get nice fits
  double calib_fit_range_low[n_calib_points] = {-20, -5, -20, -20, -1}; // want to fit in slightly different ranges around the estimate
  double calib_fit_range_high[n_calib_points] = {20, 40, 20, 20, 2};
  TCanvas* si_channel_canvases[n_si_channels];

  std::stringstream canvasname;
  for (int i_si_channel = 0; i_si_channel < n_si_channels; ++i_si_channel) {
    canvasname.str("");
    canvasname << "c_SiL1-" << i_si_channel+1 << "-S";
    si_channel_canvases[i_si_channel] = new TCanvas(canvasname.str().c_str(), canvasname.str().c_str());
    si_channel_canvases[i_si_channel]->Divide(3, 2);
  }

  std::stringstream filename;
  std::stringstream histname;
  for (int i_calib_point = 0; i_calib_point < n_calib_points; ++i_calib_point) {
    filename.str("");
    filename << "out" << std::setw(5) << std::setfill('0') << calib_run_numbers[i_calib_point] << ".root";
    TFile* file = new TFile(filename.str().c_str(), "READ");

    canvasname.str("");
    canvasname << "c" << i_calib_point;
    for (int i_si_channel = 0; i_si_channel < n_si_channels; ++i_si_channel) {
      si_channel_canvases[i_si_channel]->cd(i_calib_point+1);

      histname.str("");
      histname << "PlotTAP_Amplitude/hSiL1-" << i_si_channel+1 << "-S#MaxBinAPGenerator#any_Amplitude";
      TH1F* hist = (TH1F*) file->Get(histname.str().c_str());
      hist->SetDirectory(0);
      hist->Rebin(calib_rebin_factors[i_calib_point]);

      TSpectrum spec;
      double threshold = 0.005; // want a lower threshold than the default 0.05 because the noise peak can be really big
      if (i_calib_point == Am241_element) {
	threshold = 0.01;
      }
      spec.Search(hist, 2, "", threshold); 
      double estimate = 0; // find the right most peak
      for (int i_peak = 0; i_peak < spec.GetNPeaks(); ++i_peak) {
	double position_x = *(spec.GetPositionX() + i_peak);
	//	std::cout << "Peak #" << i_peak+1 << ": Position = " << position_x << std::endl;
	if (position_x > estimate) {
	  estimate = position_x;
	}
      }

      if (i_calib_point == Am241_element) {
	// Do it again because we want the peak that is second from the right
	double second_estimate = 0;
	for (int i_peak = 0; i_peak < spec.GetNPeaks(); ++i_peak) {
	  double position_x = *(spec.GetPositionX() + i_peak);
	  //	std::cout << "Peak #" << i_peak+1 << ": Position = " << position_x << std::endl;
	  if (position_x > second_estimate && position_x < estimate) {
	    second_estimate = position_x;
	  }
	}
	estimate = second_estimate; // 
      }

      TF1* gaussian = new TF1("gaussian", "[0]*TMath::Gaus(x, [1], [2])", estimate+calib_fit_range_low[i_calib_point], estimate+calib_fit_range_high[i_calib_point]);
      gaussian->SetParameters(100, estimate, 1);
      hist->Draw("HIST E");
      TFitResultPtr result = hist->Fit("gaussian", "SRQ");
      if ( (Int_t) result == 0) {
	calib_adcs[i_si_channel][i_calib_point] = result->Parameter(1);
	calib_adc_errors[i_si_channel][i_calib_point] = result->ParError(1);
	std::cout << "SiL1-" << i_si_channel+1 << "-S (" << calib_energies[i_calib_point] << " +/- " << calib_energy_errors[i_calib_point] << " MeV): " << calib_adcs[i_si_channel][i_calib_point] << " +/- " << calib_adc_errors[i_si_channel][i_calib_point] << " ADC (Esimate was " << estimate << ", NPeaks = " << spec.GetNPeaks() << ")" << std::endl;

	if (i_calib_point != Am241_element) {
	  int i_element = i_calib_point;
	  if (i_calib_point > Am241_element) {
	    --i_element;
	  }
	  pulser_adcs[i_si_channel][i_element] = result->Parameter(1);
	  pulser_adc_errors[i_si_channel][i_element] = result->ParError(1);
	}
      }
      else {
	std::cout << "SiL1-" << i_si_channel+1 << "-S, Calib Point #" << i_calib_point << " (" << calib_energies[i_calib_point] << " MeV) fit failed" << std::endl;
      }
      hist->GetXaxis()->SetRangeUser(calib_adc_hist_range_low[i_calib_point], calib_adc_hist_range_high[i_calib_point]);
    }

    file->Close();
  }

 
  std::stringstream text;
  // Now loop through the Si channels and get the calibration
  for (int i_si_channel = 0; i_si_channel < n_si_channels; ++i_si_channel) {
    si_channel_canvases[i_si_channel]->cd(6);

    TGraphErrors* gr = new TGraphErrors(n_calib_points, calib_adcs[i_si_channel], calib_energies, calib_adc_errors[i_si_channel], calib_energy_errors);
    //    gr->SetMarkerStyle(kDot);
    //    gr->SetMarkerSize(0.125);
    gr->SetMarkerColor(kBlack);
    gr->Draw("APE");
    gr->GetXaxis()->SetTitle("Peak Position [ADC]");
    gr->GetYaxis()->SetTitle("Energy [MeV]");
    gr->GetXaxis()->SetRangeUser(0, 4000);
    gr->GetYaxis()->SetRangeUser(0, 11);

    TF1* fit_fn = new TF1("fit_fn", "TMath::Exp([0]*x + [1])");
    //    TF1* fit_fn = new TF1("fit_fn", "pol1");
    TFitResultPtr result = gr->Fit("fit_fn", "SQ");
    if ( (Int_t) result == 0) {
      gr->GetFunction("fit_fn")->SetLineWidth(1);
      calib_offset[i_si_channel] = result->Parameter(1);
      calib_offset_error[i_si_channel] = result->ParError(1);
      calib_gain[i_si_channel] = result->Parameter(0);
      calib_gain_error[i_si_channel] = result->ParError(0);
      
      std::cout << "SiL1-" << i_si_channel+1 << "-S: E [MeV] = exp[" << calib_gain[i_si_channel] << " (+/- " << calib_gain_error[i_si_channel] << ") * A [ADC] + " << calib_offset[i_si_channel] << " (+/- " << calib_offset_error[i_si_channel] << ")]\t(chi^2/ndf = " << result->Chi2() << " / " << result->Ndf() << " = " << result->Chi2() / result->Ndf() << ")" << std::endl;

      text.str("");
      text << "E [MeV] = e^{p0 * A [ADC] + p1}";
      TLatex* latex = new TLatex(500, 8, text.str().c_str());
      latex->SetTextSize(0.04);
      latex->Draw();
      
      text.str("");
      text << std::fixed << std::setprecision(5) << "p0 = (" << calib_gain[i_si_channel] << " #pm " << calib_gain_error[i_si_channel] << ")";
      TLatex* latex2 = new TLatex(500, 7.5, text.str().c_str());
      latex2->SetTextSize(0.04);
      latex2->Draw();
      
      text.str("");
      text << std::fixed << std::setprecision(2) << "p1 = (" << calib_offset[i_si_channel] << " #pm " << calib_offset_error[i_si_channel] << ")";
      TLatex* latex3 = new TLatex(500, 7.0, text.str().c_str());
      latex3->SetTextSize(0.04);
      latex3->Draw();
      
      text.str("");
      text << std::fixed << std::setprecision(2) << "#chi^{2} / ndf = " << result->Chi2() << " / " << result->Ndf();
      TLatex* latex4 = new TLatex(500, 6.0, text.str().c_str());
      latex4->SetTextSize(0.04);
      latex4->Draw();
    }
    else {
      std::cout << "Calibration curve fit failed for SiL1-" << i_si_channel+1 << "-S" << std::endl;
    }
  }
  //  c_gr->SaveAs("/gpfs/home/edmonds_a/plots/2016-11-07/SiL1-3-S_EnergyCalibrationCurve_Test.pdf");

  for (int i_si_channel = 0; i_si_channel < n_si_channels; ++i_si_channel) {
    std::string canvasname = si_channel_canvases[i_si_channel]->GetName();
    std::string savename = "~/plots/2016-11-07/" + canvasname + ".pdf";
    si_channel_canvases[i_si_channel]->SaveAs(savename.c_str());
  }

  TCanvas* c_gr2 = new TCanvas("c_gr2", "c_gr2");
  //  c_gr2->Divide(4, 4);
  // Now loop through the Si channels and get the calibration
  for (int i_si_channel = 0; i_si_channel < 2; ++i_si_channel) {
    //    c_gr2->cd(i_si_channel+1);
    
    TGraphErrors* gr = new TGraphErrors(n_pulser_runs, pulser_energies, pulser_adcs[i_si_channel], NULL, NULL);
    gr->SetMarkerStyle(kFullDotMedium);
    gr->Draw("A*");
    gr->GetXaxis()->SetTitle("Pulser Energy [MeV]");
    gr->GetYaxis()->SetTitle("Peak Position [ADC]");

    //    TLine* line = new TLine(0, 0, pulser_voltages[2], pulser_adcs[i_si_channel][2]);
    //    line->SetLineColor(kRed);
    //    line->SetLineStyle(kDashed);
    //    line->Draw("LSAME");
  }
  c_gr2->SaveAs("/gpfs/home/edmonds_a/plots/2016-11-07/PulserEnergiesVsPeakPosition.pdf");
  

  /*  std::stringstream filename;
  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
    filename.str("");
    filename << "out" << std::setw(5) << std::setfill('0') << n_run_numbers[i_setting] << ".root";
    TFile* file = new TFile(filename.str().c_str(), "READ");
    TH1F* hist = (TH1F*) file->Get("PlotTAP_Amplitude/hSiL1-3-S#MaxBinAPGenerator#any_Amplitude");
    adc_values[i_setting] = hist->GetBinCenter(hist->GetMaximumBin());
    all_adc_values[i_setting] = adc_values[i_setting];
    energies[i_setting] = (4.0/100)*pulser_amplitudes[i_setting];
    file->Close();
  }
  
  TGraph* gr = new TGraph(n_settings, pulser_amplitudes, adc_values);
  gr->Draw("A*");
  gr->GetXaxis()->SetTitle("Pulser Amplitude [mV]");
  gr->GetYaxis()->SetTitle("Peak Position [ADC]");

  filename.str("");
  filename << "out" << all_run_numbers[4] << ".root";
  TFile* file = new TFile(filename.str().c_str(), "READ");
  TH1F* hist = (TH1F*) file->Get("PlotTAP_Amplitude/hSiL1-3-S#MaxBinAPGenerator#any_Amplitude");
  hist->GetXaxis()->SetRangeUser(0, 2200);
  all_adc_values[4] = hist->GetBinCenter(hist->GetMaximumBin());
  energies[4] = 5.486; //MeV

  TGraph* gr2 = new TGraph(n_energies, energies, all_adc_values);
  gr2->Draw("A*");
  gr2->GetXaxis()->SetTitle("Energy [MeV]");
  gr2->GetXaxis()->SetRangeUser(0, 10);
  gr2->GetYaxis()->SetTitle("Peak Position [ADC]");
  gr2->GetYaxis()->SetRangeUser(0, 3500);

  std::cout << "Energy [MeV] | ADC" << std::endl;
  std::cout << "------------------" << std::endl;
  for (int i_energy = 0; i_energy < n_energies; ++i_energy) {
    std::cout << energies[i_energy] << "  |  " << all_adc_values[i_energy] << std::endl;
  }
  */
}
