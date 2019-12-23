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
#include "TStyle.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

extern TStyle* gStyle;

void SiR1Calib() {

  gStyle->SetOptFit(1);

  const int n_si_channels = 4;

  const int n_pulser_runs = 4;
  double pulser_voltages[n_pulser_runs] = {0.1, 0.05, 0.15, 0.2};
  double frac_pulser_uncertainty = 0.02; // estimate based on elog:210 (smallest divisions are 4 mV, can probably eyeball it to half that)
  double pulser_voltage_errors[n_pulser_runs] = {frac_pulser_uncertainty*pulser_voltages[0],
						 frac_pulser_uncertainty*pulser_voltages[1],
						 frac_pulser_uncertainty*pulser_voltages[2],
						 frac_pulser_uncertainty*pulser_voltages[3]};

  // 50, 100, 150 mV Pulser and Am241 (NB Am241 and 100 mV pulser are in the same run)
  const int n_calib_points = 5;
  std::string calib_labels[n_calib_points] = { "Am241", "#splitline{100 mV Pulser}{@ 15 MeV/V}", "#splitline{50 mV Pulser}{@ 15 MeV/V}", "#splitline{150 mV Pulser}{@ 15 MeV/V}", "#splitline{200 mV Pulser}{@ 15 MeV/V}" };
  const int Am241_element = 0; // still needed for pulser only plots
  double convert_to_keV = 1000;
  double mesytec = 15*convert_to_keV; // MeV/V (according to datasheet)
  double frac_mesytec_uncert = 0; // ready just in case

  double calib_energies[n_calib_points] = 
    { 5.486*convert_to_keV, // Am241
      mesytec*pulser_voltages[0], // 100 mV pulser put through mesytec box
      mesytec*pulser_voltages[1], //  50 mV pulser put through mesytec box
      mesytec*pulser_voltages[2],  // 150 mV pulser put through mesytec box
      mesytec*pulser_voltages[3]  // 200 mV pulser put through mesytec box
    };

  double calib_energy_errors[n_calib_points] = 
    { 0.05*convert_to_keV, 
      calib_energies[1]*std::sqrt( frac_pulser_uncertainty*frac_pulser_uncertainty + frac_mesytec_uncert*frac_mesytec_uncert), 
      calib_energies[2]*std::sqrt( frac_pulser_uncertainty*frac_pulser_uncertainty + frac_mesytec_uncert*frac_mesytec_uncert),
      calib_energies[3]*std::sqrt( frac_pulser_uncertainty*frac_pulser_uncertainty + frac_mesytec_uncert*frac_mesytec_uncert),
      calib_energies[4]*std::sqrt( frac_pulser_uncertainty*frac_pulser_uncertainty + frac_mesytec_uncert*frac_mesytec_uncert)
    };
  int calib_run_numbers[n_calib_points] = 
    { 9028,
      9028,
      9178, 
      9190,
      9179
    };
  double calib_adcs[n_si_channels][n_calib_points];
  double calib_adc_errors[n_si_channels][n_calib_points];
  double calib_gain[n_si_channels];
  double calib_gain_error[n_si_channels];
  double calib_offset[n_si_channels];
  double calib_offset_error[n_si_channels];

  double pulser_energies[n_pulser_runs] = {mesytec*0.05, mesytec*0.15};
  double pulser_adcs[n_si_channels][n_pulser_runs];
  double pulser_adc_errors[n_si_channels][n_pulser_runs];
  
  double calib_adc_hist_range_low[n_calib_points] = 
    { 2750, 
      750, 
      200,
      1000,
      1600
    }; // used to zoom in on the histogram for drawing purposes
  double calib_adc_hist_range_high[n_calib_points] = 
    { 3250, 
      1250, 
      600,
      1500,
      1900
    };
  int calib_rebin_factors[n_calib_points] = 
    { 8, 
      4, 
      4,
      4,
      4
    }; // want to rebin each calibration point slightly differently to get nice fits
  double calib_fit_range_low[n_calib_points] = 
    { -10, 
      -20,
      -20,
      -20,
      -20
    }; // want to fit in slightly different ranges around the estimate
  double calib_fit_range_high[n_calib_points] = 
    { 60, 
      20, 
      20,
      20,
      20
    };
  TCanvas* si_channel_canvases[n_si_channels];

  std::stringstream canvasname;
  for (int i_si_channel = 0; i_si_channel < n_si_channels; ++i_si_channel) {
    canvasname.str("");
    canvasname << "c_SiR1-" << i_si_channel+1 << "-S";
    si_channel_canvases[i_si_channel] = new TCanvas(canvasname.str().c_str(), canvasname.str().c_str());
    si_channel_canvases[i_si_channel]->Divide(3, 2);
  }

  std::stringstream filename;
  std::stringstream histname;
  std::string basename = "~/data/out/local/";
  for (int i_calib_point = 0; i_calib_point < n_calib_points; ++i_calib_point) {
    filename.str("");
    filename << basename << "out" << std::setw(5) << std::setfill('0') << calib_run_numbers[i_calib_point] << ".root";
    TFile* file = new TFile(filename.str().c_str(), "READ");

    canvasname.str("");
    canvasname << "c" << i_calib_point;
    for (int i_si_channel = 0; i_si_channel < n_si_channels; ++i_si_channel) {
      si_channel_canvases[i_si_channel]->cd(i_calib_point+1);

      histname.str("");
      histname << "PlotTAP_Amplitude_wTimeCuts/hSiR1-" << i_si_channel+1 << "-S#MaxBinAPGenerator#{no_time_shift=true}_Amplitude_wTimeCuts";
      TH1F* hist = (TH1F*) file->Get(histname.str().c_str());
      hist->SetDirectory(0);
      hist->Rebin(calib_rebin_factors[i_calib_point]);
      hist->GetXaxis()->SetRangeUser(calib_adc_hist_range_low[i_calib_point], calib_adc_hist_range_high[i_calib_point]);

      TSpectrum spec;
      double threshold = 0.005; // want a lower threshold than the default 0.05 because the noise peak can be really big
      spec.Search(hist, 2, "", threshold); 
      double estimate = 0; // find the right most peak
      for (int i_peak = 0; i_peak < spec.GetNPeaks(); ++i_peak) {
	double position_x = *(spec.GetPositionX() + i_peak);
	//	std::cout << "Peak #" << i_peak+1 << ": Position = " << position_x << std::endl;
	if (position_x > estimate) {
	  estimate = position_x;
	}
      }


      TF1* gaussian = new TF1("gaussian", "[0]*TMath::Gaus(x, [1], [2])", estimate+calib_fit_range_low[i_calib_point], estimate+calib_fit_range_high[i_calib_point]);
      gaussian->SetParameters(100, estimate, 1);
      hist->Draw("HIST E");
      TFitResultPtr result = hist->Fit("gaussian", "SRQ");
      if ( (Int_t) result == 0) {
	calib_adcs[i_si_channel][i_calib_point] = result->Parameter(1);
	calib_adc_errors[i_si_channel][i_calib_point] = result->ParError(1);
	std::cout << "SiR1-" << i_si_channel+1 << "-S (" << calib_energies[i_calib_point] << " +/- " << calib_energy_errors[i_calib_point] << " keV): " << calib_adcs[i_si_channel][i_calib_point] << " +/- " << calib_adc_errors[i_si_channel][i_calib_point] << " ADC (Esimate was " << estimate << ", NPeaks = " << spec.GetNPeaks() << ")" << std::endl;

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
	std::cout << "SiR1-" << i_si_channel+1 << "-S, Calib Point #" << i_calib_point << " (" << calib_energies[i_calib_point] << " keV) fit failed" << std::endl;
      }
      hist->GetFunction("gaussian")->SetLineWidth(1);
      hist->GetFunction("gaussian")->Draw("LSAME");

      hist->SetTitle(calib_labels[i_calib_point].c_str());
    }

    file->Close();
  }

 
  std::stringstream text;
  // Now loop through the Si channels and get the calibration
  for (int i_si_channel = 0; i_si_channel < n_si_channels; ++i_si_channel) {
    si_channel_canvases[i_si_channel]->cd(n_calib_points+1);
    //    TCanvas* c1 = new TCanvas("c1", "c1");

    TGraphErrors* gr = new TGraphErrors(n_calib_points, calib_adcs[i_si_channel], calib_energies, calib_adc_errors[i_si_channel], calib_energy_errors);
    //    gr->SetMarkerStyle(kDot);
    //    gr->SetMarkerSize(0.125);
    gr->SetMarkerColor(kBlack);
    gr->Draw("APE");
    gr->GetXaxis()->SetTitle("Peak Position [ADC]");
    gr->GetYaxis()->SetTitle("Energy [keV]");
    gr->GetXaxis()->SetLimits(-1000, 4000);
    gr->GetYaxis()->SetRangeUser(-1000, 8*convert_to_keV);

    for (int i_calib_point = 0; i_calib_point < n_calib_points; ++i_calib_point) {
      TLatex* calib_text = 
	new TLatex( calib_adcs[i_si_channel][i_calib_point]+100,
		    calib_energies[i_calib_point]-500, 
		    calib_labels[i_calib_point].c_str());
      calib_text->SetTextSize(0.03);
      calib_text->Draw();
    }


    if (n_calib_points == 1) {
      // single point calib
      calib_gain[i_si_channel] = calib_energies[0] / calib_adcs[i_si_channel][0];
      calib_gain_error[i_si_channel] = 0;
      calib_offset[i_si_channel] = 0;
      calib_offset_error[i_si_channel] = 0;

      TF1* linear_fit = new TF1("linear_fit", "[0]*x + 1", 0, 4000);
      linear_fit->SetParameters(calib_gain[i_si_channel], calib_offset[i_si_channel]);
      linear_fit->Draw("LSAME");

      continue; // skip to next channel
    }
    //    TF1* fit_fn = new TF1("fit_fn", "TMath::Exp([0]*x + [1])");
    //    fit_fn->SetParameters(0.0007, 10);
    TF1* fit_fn = new TF1("fit_fn", "[0]*x + [1]");
    TFitResultPtr result = gr->Fit("fit_fn", "SQ");
    if ( (Int_t) result == 0) {
      gr->GetFunction("fit_fn")->SetLineWidth(1);
      calib_offset[i_si_channel] = result->Parameter(1);
      calib_offset_error[i_si_channel] = result->ParError(1);
      calib_gain[i_si_channel] = result->Parameter(0);
      calib_gain_error[i_si_channel] = result->ParError(0);
      
      std::cout << "SiR1-" << i_si_channel+1 << "-S: E [keV] = " << calib_gain[i_si_channel] << " (+/- " << calib_gain_error[i_si_channel] << ") * A [ADC] + " << calib_offset[i_si_channel] << " (+/- " << calib_offset_error[i_si_channel] << ")\t(chi^2/ndf = " << result->Chi2() << " / " << result->Ndf() << " = " << result->Chi2() / result->Ndf() << ")" << std::endl;
      //      result->PrintCovMatrix(std::cout);

      text.str("");
      text << "E [keV] = p0 * A [ADC] + p1";
      double first_line_y = 7*convert_to_keV;
      double line_step_y = 0.5*convert_to_keV;
      double first_line_x = -500;
      TLatex* latex = new TLatex(first_line_x, first_line_y, text.str().c_str());
      latex->SetTextSize(0.04);
      latex->Draw();
      
      text.str("");
      text << std::fixed << std::setprecision(5) << "p0 = (" << calib_gain[i_si_channel] << " #pm " << calib_gain_error[i_si_channel] << ")";
      TLatex* latex2 = new TLatex(first_line_x, first_line_y-line_step_y, text.str().c_str());
      latex2->SetTextSize(0.04);
      latex2->Draw();
      
      text.str("");
      text << std::fixed << std::setprecision(2) << "p1 = (" << calib_offset[i_si_channel] << " #pm " << calib_offset_error[i_si_channel] << ")";
      TLatex* latex3 = new TLatex(first_line_x, first_line_y-2*line_step_y, text.str().c_str());
      latex3->SetTextSize(0.04);
      latex3->Draw();
      
      text.str("");
      text << std::fixed << std::setprecision(2) << "#chi^{2} / ndf = " << result->Chi2() << " / " << result->Ndf();
      TLatex* latex4 = new TLatex(first_line_x, first_line_y-4*line_step_y, text.str().c_str());
      latex4->SetTextSize(0.04);
      latex4->Draw();
    }
    else {
      std::cout << "Calibration curve fit failed for SiR1-" << i_si_channel+1 << "-S" << std::endl;
    }
    //    c1->SaveAs("/gpfs/home/edmonds_a/plots/2016-11-09/SiR1-3-S_EnergyCalibrationCurve.pdf");
  }
  //  c_gr->SaveAs("/gpfs/home/edmonds_a/plots/2016-11-07/SiR1-3-S_EnergyCalibrationCurve_Test.pdf");

  for (int i_si_channel = 0; i_si_channel < n_si_channels; ++i_si_channel) {
    std::string canvasname2 = si_channel_canvases[i_si_channel]->GetName();
    std::string savename = "~/plots/2016-12-19/" + canvasname2 + ".pdf";
    //    si_channel_canvases[i_si_channel]->SaveAs(savename.c_str());
  }

  // Now write out the calibration constants
  std::ofstream calib_script;
  calib_script.open("scripts/r15b_insert_calib_sir1.sh", std::ofstream::out);
  calib_script << "#! /bin/bash" << std::endl;
  calib_script << std::endl;
  calib_script << "Channels=( SiR1-{1.." << n_si_channels << "}-S )" << std::endl;
  calib_script << "Gains=(";    
  for (int i_si_channel = 0; i_si_channel < n_si_channels; ++i_si_channel) {
    calib_script << " " << calib_gain[i_si_channel];
  }
  calib_script << ")" << std::endl;
  calib_script << "Offsets=("; 
  for (int i_si_channel = 0; i_si_channel < n_si_channels; ++i_si_channel) {
    calib_script << " " << calib_offset[i_si_channel];
  }
  calib_script << ")" << std::endl;
  calib_script << "for run in $@;do" << std::endl;
  calib_script << "  for i_channel in {0.." << n_si_channels-1 << "};do" << std::endl;
  calib_script << "    echo \"INSERT into Energy  (run, channel, gain, offset) values ( $run, \\\"${Channels[$i_channel]}\\\", ${Gains[$i_channel]}, ${Offsets[$i_channel]}) ;\"" << std::endl;
  calib_script << "  done" << std::endl;
  calib_script << "done" << std::endl;
  calib_script.close();


  TCanvas* c_gr2 = new TCanvas("c_gr2", "c_gr2");
  c_gr2->Divide(4, 4);
  // Now loop through the Si channels and get the calibration
  for (int i_si_channel = 0; i_si_channel < n_si_channels; ++i_si_channel) {
    c_gr2->cd(i_si_channel+1);
    
    TGraphErrors* gr = new TGraphErrors(n_pulser_runs, pulser_voltages, pulser_adcs[i_si_channel], pulser_voltage_errors, pulser_adc_errors[i_si_channel]);
    gr->SetMarkerStyle(kFullDotMedium);
    gr->Draw("APE");
    gr->GetXaxis()->SetTitle("Pulser Amplitude [mV]");
    gr->GetYaxis()->SetTitle("Peak Position [ADC]");
    gr->GetXaxis()->SetLimits(0, 0.25);
    gr->GetYaxis()->SetRangeUser(0, 4000);

    gr->Fit("pol1", "Q");

    TLine* line = new TLine(0, 0, pulser_voltages[1], pulser_adcs[i_si_channel][1]);
    line->SetLineColor(kRed);
    line->SetLineStyle(kDashed);
    line->Draw("LSAME");
  }
  //  c_gr2->SaveAs("/gpfs/home/edmonds_a/plots/2016-11-28/PulserVoltageVsPeakPosition.pdf");
  

  /*  std::stringstream filename;
  for (int i_setting = 0; i_setting < n_settings; ++i_setting) {
    filename.str("");
    filename << "out" << std::setw(5) << std::setfill('0') << n_run_numbers[i_setting] << ".root";
    TFile* file = new TFile(filename.str().c_str(), "READ");
    TH1F* hist = (TH1F*) file->Get("PlotTAP_Amplitude_wTimeCuts/hSiR1-3-S#MaxBinAPGenerator#any_Amplitude_wTimeCuts");
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
  TH1F* hist = (TH1F*) file->Get("PlotTAP_Amplitude_wTimeCuts/hSiR1-3-S#MaxBinAPGenerator#any_Amplitude_wTimeCuts");
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
