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

void SiL1Calib() {

  gStyle->SetOptFit(1);

  const int n_si_channels = 16;

  double punch_through_adcs[n_si_channels] = {-1, 397.892, 370.482, 377.435, 385.76, 392.143, 382.358, 398.644, 384.729, 389.826, 379.125, 373.895, 368.695, 393.713, 386.974, 0};
  double punch_through_adc_errors[n_si_channels] = {-1, 1.36927, 1.0991, 0.968719, 0.905911, 0.842081, 0.848423, 0.844272, 0.851763, 0.880642, 0.824741, 0.92053, 0.956623, 1.02479, 1.13229, -1};

  const int n_calib_points = 2;
  std::string calib_labels[n_calib_points] = { "Am241", "Punch-through" };//"#splitline{50 mV Pulser}{@ 15 MeV/V}", "#splitline{150 mV Pulser}{@ 15 MeV/V}" };
  const int Am241_element = 0; // still needed for pulser only plots
  double convert_to_keV = 1000;

  double calib_energies[n_calib_points] = 
    { 5.486*convert_to_keV, // Am241
      1.0*convert_to_keV // punch-through muons
    };

  double calib_energy_errors[n_calib_points] = 
    { 0.05*convert_to_keV, 
      0.05*convert_to_keV
    };
  int calib_run_numbers[n_calib_points] = 
    { 10483, 
      -1 // signify we will do our own thing here
    };
  double calib_adcs[n_si_channels][n_calib_points];
  double calib_adc_errors[n_si_channels][n_calib_points];
  double calib_gain[n_si_channels];
  double calib_gain_error[n_si_channels];
  double calib_offset[n_si_channels];
  double calib_offset_error[n_si_channels];
  
  double calib_adc_hist_range_low[n_calib_points] = 
    { 1800, 
      1000, 
    }; // used to zoom in on the histogram for drawing purposes
  double calib_adc_hist_range_high[n_calib_points] = 
    { 2500, 
      1400, 
    };
  int calib_rebin_factors[n_calib_points] = 
    { 8, 
      4, 
    }; // want to rebin each calibration point slightly differently to get nice fits
  double calib_fit_range_low[n_calib_points] = 
    { -5, 
      -20,
    }; // want to fit in slightly different ranges around the estimate
  double calib_fit_range_high[n_calib_points] = 
    { 40, 
      20, 
    };
  TCanvas* si_channel_canvases[n_si_channels];

  std::stringstream canvasname;
  for (int i_si_channel = 0; i_si_channel < n_si_channels; ++i_si_channel) {
    canvasname.str("");
    canvasname << "c_SiL1-" << i_si_channel+1 << "-S";
    si_channel_canvases[i_si_channel] = new TCanvas(canvasname.str().c_str(), canvasname.str().c_str());
    si_channel_canvases[i_si_channel]->Divide(2, 2);
  }

  std::stringstream filename;
  std::stringstream histname;
  for (int i_calib_point = 0; i_calib_point < n_calib_points; ++i_calib_point) {

    if (calib_run_numbers[i_calib_point] == 0) {
      calib_energies[i_calib_point] = 0;
      calib_energy_errors[i_calib_point] = 0.005*convert_to_keV;
      for (int i_si_channel = 0; i_si_channel < n_si_channels; ++i_si_channel) {
	calib_adcs[i_si_channel][i_calib_point] = 0;
	calib_adc_errors[i_si_channel][i_calib_point] = 0;
      }
      continue;
    }

    if (calib_run_numbers[i_calib_point] == -1) {
      for (int i_si_channel = 0; i_si_channel < n_si_channels; ++i_si_channel) {
	calib_adcs[i_si_channel][i_calib_point] = punch_through_adcs[i_si_channel];
	calib_adc_errors[i_si_channel][i_calib_point] = punch_through_adc_errors[i_si_channel];
      }
      continue;
    }

    filename.str("");
    filename << "out" << std::setw(5) << std::setfill('0') << calib_run_numbers[i_calib_point] << ".root";
    TFile* file = new TFile(filename.str().c_str(), "READ");

    canvasname.str("");
    canvasname << "c" << i_calib_point;
    for (int i_si_channel = 0; i_si_channel < n_si_channels; ++i_si_channel) {

      si_channel_canvases[i_si_channel]->cd(i_calib_point+1);

      histname.str("");
      histname << "PlotTAP_Amplitude_wTimeCuts/hSiL1-" << i_si_channel+1 << "-S#MaxBinAPGenerator#{no_time_shift=true}_Amplitude_wTimeCuts";
      TH1F* hist = (TH1F*) file->Get(histname.str().c_str());
      if (!hist) {
	std::cout << "No histogram called " << histname.str() << " in " << filename.str() << std::endl;
	continue;
      }
      hist->SetDirectory(0);
      hist->Rebin(calib_rebin_factors[i_calib_point]);
      hist->GetXaxis()->SetRangeUser(calib_adc_hist_range_low[i_calib_point], calib_adc_hist_range_high[i_calib_point]);

      TSpectrum spec;
      double threshold = 0.005; // want a lower threshold than the default 0.05 because the noise peak can be really big
      //      if (i_calib_point == Am241_element) {
      //	threshold = 0.01;
      //      }
      spec.Search(hist, 2, "", threshold); 
      double estimate = 0; // find the right most peak
      for (int i_peak = 0; i_peak < spec.GetNPeaks(); ++i_peak) {
	double position_x = *(spec.GetPositionX() + i_peak);
	//	std::cout << "Peak #" << i_peak+1 << ": Position = " << position_x << std::endl;
	if (position_x > estimate) {
	  estimate = position_x;
	}
      }

      /* not needed with cleaned up Am241 spectrum
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
      */

      TF1* gaussian = new TF1("gaussian", "[0]*TMath::Gaus(x, [1], [2])", estimate+calib_fit_range_low[i_calib_point], estimate+calib_fit_range_high[i_calib_point]);
      gaussian->SetParameters(100, estimate, 1);
      hist->Draw("HIST E");
      TFitResultPtr result = hist->Fit("gaussian", "SRQ");
      if ( (Int_t) result == 0) {
	calib_adcs[i_si_channel][i_calib_point] = result->Parameter(1);
	calib_adc_errors[i_si_channel][i_calib_point] = result->ParError(1);
	std::cout << "SiL1-" << i_si_channel+1 << "-S (" << calib_energies[i_calib_point] << " +/- " << calib_energy_errors[i_calib_point] << " keV): " << calib_adcs[i_si_channel][i_calib_point] << " +/- " << calib_adc_errors[i_si_channel][i_calib_point] << " ADC (Esimate was " << estimate << ", NPeaks = " << spec.GetNPeaks() << ")" << std::endl;

      }
      else {
	std::cout << "SiL1-" << i_si_channel+1 << "-S, Calib Point #" << i_calib_point << " (" << calib_energies[i_calib_point] << " keV) fit failed" << std::endl;
      }
      hist->GetFunction("gaussian")->SetLineWidth(1);

      hist->SetTitle(calib_labels[i_calib_point].c_str());
    }
    file->Close();
  }
  std::cout << "Here2" << std::endl;
 
  std::stringstream text;
  // Now loop through the Si channels and get the calibration
  for (int i_si_channel = 0; i_si_channel < n_si_channels; ++i_si_channel) {
    si_channel_canvases[i_si_channel]->cd(4);
    //    TCanvas* c1 = new TCanvas("c1", "c1");

    TGraphErrors* gr = new TGraphErrors(n_calib_points, calib_adcs[i_si_channel], calib_energies, calib_adc_errors[i_si_channel], calib_energy_errors);
    //    gr->SetMarkerStyle(kDot);
    //    gr->SetMarkerSize(0.125);
    gr->SetMarkerColor(kBlack);
    gr->Draw("APE");
    gr->GetXaxis()->SetTitle("Peak Position [ADC]");
    gr->GetYaxis()->SetTitle("Energy [keV]");
    gr->GetXaxis()->SetLimits(-500, 4000);
    gr->GetYaxis()->SetRangeUser(-500, 11*convert_to_keV);

    for (int i_calib_point = 0; i_calib_point < n_calib_points; ++i_calib_point) {
      TLatex* calib_text = 
	new TLatex( calib_adcs[i_si_channel][i_calib_point],
		    calib_energies[i_calib_point]-1000, 
		    calib_labels[i_calib_point].c_str());
      calib_text->SetTextSize(0.03);
      calib_text->SetTextAlign(22);
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
      
      std::cout << "SiL1-" << i_si_channel+1 << "-S: E [keV] = " << calib_gain[i_si_channel] << " (+/- " << calib_gain_error[i_si_channel] << ") * A [ADC] + " << calib_offset[i_si_channel] << " (+/- " << calib_offset_error[i_si_channel] << ")\t(chi^2/ndf = " << result->Chi2() << " / " << result->Ndf() << " = " << result->Chi2() / result->Ndf() << ")" << std::endl;
      //      result->PrintCovMatrix(std::cout);

      text.str("");
      text << "E [keV] = p0 * A [ADC] + p1";
      TLatex* latex = new TLatex(1100, 9.5*convert_to_keV, text.str().c_str());
      latex->SetTextSize(0.04);
      latex->Draw();
      
      text.str("");
      text << std::fixed << std::setprecision(5) << "p0 = (" << calib_gain[i_si_channel] << " #pm " << calib_gain_error[i_si_channel] << ")";
      TLatex* latex2 = new TLatex(1100, 9.0*convert_to_keV, text.str().c_str());
      latex2->SetTextSize(0.04);
      latex2->Draw();
      
      text.str("");
      text << std::fixed << std::setprecision(2) << "p1 = (" << calib_offset[i_si_channel] << " #pm " << calib_offset_error[i_si_channel] << ")";
      TLatex* latex3 = new TLatex(1100, 8.5*convert_to_keV, text.str().c_str());
      latex3->SetTextSize(0.04);
      latex3->Draw();
      
      text.str("");
      text << std::fixed << std::setprecision(2) << "#chi^{2} / ndf = " << result->Chi2() << " / " << result->Ndf();
      TLatex* latex4 = new TLatex(1100, 7.5*convert_to_keV, text.str().c_str());
      latex4->SetTextSize(0.04);
      latex4->Draw();
    }
    else {
      std::cout << "Calibration curve fit failed for SiL1-" << i_si_channel+1 << "-S" << std::endl;
    }
    //    c1->SaveAs("/gpfs/home/edmonds_a/plots/2016-11-09/SiL1-3-S_EnergyCalibrationCurve.pdf");
  }
  //  c_gr->SaveAs("/gpfs/home/edmonds_a/plots/2016-11-07/SiL1-3-S_EnergyCalibrationCurve_Test.pdf");

  for (int i_si_channel = 0; i_si_channel < n_si_channels; ++i_si_channel) {
    std::string canvasname = si_channel_canvases[i_si_channel]->GetName();
    std::string savename = "~/plots/2016-12-19/" + canvasname + ".pdf";
    si_channel_canvases[i_si_channel]->SaveAs(savename.c_str());
  }

  // Now write out the calibration constants
  std::ofstream calib_script;
  calib_script.open("scripts/r15b_insert_calib_sil1.sh", std::ofstream::out);
  calib_script << "#! /bin/bash" << std::endl;
  calib_script << std::endl;
  calib_script << "Channels=( SiL1-{1.." << n_si_channels << "}-S )" << std::endl;
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
  calib_script << "  for i_channel in {0.." << n_si_channels << "};do" << std::endl;
  calib_script << "    echo \"INSERT into Energy  (run, channel, gain, offset) values ( $run, \\\"${Channels[$i_channel]}\\\", ${Gains[$i_channel]}, ${Offsets[$i_channel]}) ;\"" << std::endl;
  calib_script << "  done" << std::endl;
  calib_script << "done" << std::endl;
  calib_script.close();

}
