#include <TROOT.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <TKey.h>
#include <TLegend.h>

#include <iostream>
#include <sstream>

extern void ZoomAxis(TH2* hist);

// This will generate the following PDFs:
// -- hDQ_[RunHistogramName]_TrendPlot.png
void GenerateTrendPlots(std::string data_dir, int first_run, const int n_runs) {

  std::cout << "Generating trend plots..." << std::endl;

  gROOT->Reset();
  gROOT->SetStyle("Plain");
  gStyle->SetCanvasBorderMode(0); // turn off canvas borders

  // Get all the files here first
  TFile* files[n_runs];

  for (int iRun = 0; iRun < n_runs; ++iRun) {

    std::stringstream filename;
    filename << data_dir << "hist/hist0" << first_run+iRun << ".root";
    files[iRun] = new TFile(filename.str().c_str(), "READ");
  }

  // The histograms
  TH2F* hDQ_TrendPlot;
  TH1F *hDQ_RunPlot;
  
  //TH1F *hDQ_MuPCTrendPlot[4];//mean x, rms x, mean y, rms y
  //TH2F *hDQ_MuPCRunPlot;

  // Work out what the dataset is
  std::string dataset = "NULL";
  if (first_run == 2091)
    dataset = "SiR";
  else if (first_run == 2808)
    dataset = "Al100";
  else if (first_run == 3101)
    dataset = "Al50(a)_without-NDet2";
  else if (first_run == 3442)
    dataset = "Al50(a)_with-NDet2";
  else if (first_run == 3474)
    dataset = "Si16P";
  else if (first_run == 3563)
    dataset = "Al50(b)";
  else if (first_run == 3763)
    dataset = "SiR2(3%)";
  else if (first_run == 3771)
    dataset = "SiR2(1%)";
  else if (first_ru == 6179)
    dataset = "Ambient";

  // Loop through the histograms in the first_file and get the island histograms (bank and channel names may differ between runs)
  TDirectoryFile* dir = (TDirectoryFile*) files[0]->Get("DataQuality_LowLevel");

  TIter nextDirKey(dir->GetListOfKeys()); // get the list of keys in the directory (all histograms should be in this folder)
  TKey *dirKey;
  
  while ( (dirKey = (TKey*)nextDirKey()) ) {
    std::string histogram_name = dirKey->ReadObj()->GetName();    
    // Get all the histograms
    if (strcmp(dirKey->ReadObj()->ClassName(), "TH1F") == 0) {


      // Set up the canvases and trend plots from the first file
      TCanvas *c1 = new TCanvas();

      //std::string histogram_name = dirKey->ReadObj()->GetName();

      // Don't want any of these plots as trends
      if (histogram_name.find("Total") != std::string::npos || 
	  histogram_name.find("TDCCheck_muScA") != std::string::npos || 
	  histogram_name.find("TDCCheck_muPC") != std::string::npos || 
	  histogram_name.find("TDCCheck_muSc_time") != std::string::npos) {
	continue;
      }

      std::string histogram_location = "DataQuality_LowLevel/" + histogram_name;
      std::string pngname = "data_quality_figs/" + histogram_name + "_TrendPlot.png";

      // Modify some names so that they go in the correct sections
      if (histogram_name.find("Amplitude") != std::string::npos) {
	pngname.insert(pngname.find("Amplitude"), "Island");
      }
      if (histogram_name.find("Threshold") != std::string::npos) {
	pngname.insert(pngname.find("Threshold"), "DAQ");
      }
      if (histogram_name.find("PulseShapes_Pedestal") != std::string::npos) {
	pngname.insert(pngname.find("PulseShapes"), "DAQ");
      }
      if (histogram_name.find("PulseShapes_Noise") != std::string::npos) {
	pngname.insert(pngname.find("PulseShapes"), "DAQ");
      }


      files[0]->GetObject(histogram_location.c_str(),hDQ_RunPlot);



      std::string trendplotname = histogram_name + "_TrendPlot";
      std::string trendplottitle = "Trend Plot of " + histogram_name + " (" + dataset + ")";

      hDQ_TrendPlot = new TH2F(trendplotname.c_str(), trendplottitle.c_str(), n_runs, first_run, first_run+n_runs, hDQ_RunPlot->GetNbinsX(), hDQ_RunPlot->GetXaxis()->GetXmin(), hDQ_RunPlot->GetXaxis()->GetXmax());

      hDQ_TrendPlot->GetYaxis()->SetTitle(hDQ_RunPlot->GetXaxis()->GetTitle());
      hDQ_TrendPlot->GetYaxis()->SetLabelSize(0.03);

      hDQ_TrendPlot->GetXaxis()->SetTitle("Run Number");
      hDQ_TrendPlot->GetXaxis()->SetLabelSize(0.03);

      hDQ_TrendPlot->GetZaxis()->SetTitle(hDQ_RunPlot->GetYaxis()->GetTitle());
      hDQ_TrendPlot->GetZaxis()->SetLabelSize(0.03);
      hDQ_TrendPlot->GetZaxis()->SetTitleOffset(0.85);
      hDQ_TrendPlot->GetZaxis()->SetTitleSize(0.03);

      gStyle->SetOptStat(0000);
      hDQ_TrendPlot->SetStats(true);

      // Want to copy the bin labels for this plot
      if (histogram_name.find("IslandRate") != std::string::npos) {

	// With hDQ_IslandRate we want the bin labels transferred from the x-axis of the histogram to the y-axis of the trend plot
	for (int jBin = 1; jBin < hDQ_RunPlot->GetNbinsX(); ++jBin) {

	  hDQ_TrendPlot->GetYaxis()->SetBinLabel(jBin, hDQ_RunPlot->GetXaxis()->GetBinLabel(jBin));
	  hDQ_TrendPlot->GetYaxis()->SetTitle("");
	}
      }

      // Want the Fraction plots to go up to 1 so that red = bad
      // also do the same for the DAQ livetime plot
      if (histogram_name.find("Fraction") != std::string::npos ||
	  histogram_name.find("DAQLivetime") != std::string::npos) {
	hDQ_TrendPlot->GetZaxis()->SetRangeUser(0,1);
      }



      // Loop through the runs
      for (int iRun = 0; iRun < n_runs; ++iRun) {

       	if (files[iRun]->IsZombie())
	  continue; 

	files[iRun]->GetObject(histogram_location.c_str(),hDQ_RunPlot);

	if (hDQ_RunPlot == NULL)
	  continue; // possible that this plot might not exist

	// Fill the trend plot
	for (int iBin = 1; iBin <= hDQ_RunPlot->GetNbinsX(); ++iBin) {
	  
	  // Check that the bin labels match before filling the trend plot
	  if (strcmp(hDQ_TrendPlot->GetYaxis()->GetBinLabel(iBin), hDQ_RunPlot->GetXaxis()->GetBinLabel(iBin)) == 0) {
	    hDQ_TrendPlot->Fill(first_run + iRun, hDQ_RunPlot->GetBinCenter(iBin), hDQ_RunPlot->GetBinContent(iBin)); // (x = run #, y = time stamp, z = N_TPI)
	  }
	  else {
	    // loop through the bins again until we see the correct label
	    bool match_found = false;
	    for (int jBin = 1; jBin <= hDQ_TrendPlot->GetNbinsY(); ++jBin) {
	      if (strcmp(hDQ_TrendPlot->GetYaxis()->GetBinLabel(jBin), hDQ_RunPlot->GetXaxis()->GetBinLabel(iBin)) == 0) {
		hDQ_TrendPlot->Fill(first_run + iRun, hDQ_RunPlot->GetBinCenter(jBin), hDQ_RunPlot->GetBinContent(iBin)); // (x = run #, y = time stamp, z = N_TPI)
		match_found = true;
		break;
	      }
	    }

	    if (match_found == false) {
	      // Find the last emtpy y-axis bin, fill it and re-label it
	      for (int jBin = 1; jBin <= hDQ_TrendPlot->GetNbinsY(); ++jBin) {
		if (strcmp(hDQ_TrendPlot->GetYaxis()->GetBinLabel(jBin), "") == 0) {
		  hDQ_TrendPlot->Fill(first_run + iRun, hDQ_RunPlot->GetBinCenter(jBin), hDQ_RunPlot->GetBinContent(iBin));
		  hDQ_TrendPlot->GetYaxis()->SetBinLabel(jBin, hDQ_RunPlot->GetXaxis()->GetBinLabel(iBin));
		  break;
		}
	      }
	    }
	  }
	}

      }

      // Zoom in on some plots
      if (histogram_name.find("IslandCounter") != std::string::npos || 
	  histogram_name.find("IslandLength") != std::string::npos) {
	ZoomAxis(hDQ_TrendPlot);
      }

      // Set a lower limit on the z-axis for the normalised plots
      if (histogram_name.find("normalised") != std::string::npos) {
	hDQ_TrendPlot->SetMinimum(1e-7);
      }

      // For the FADC threshold plots, set the limit 
      if (histogram_name.find("Threshold") != std::string::npos && 
	  histogram_name.find("BU") == std::string::npos && histogram_name.find("UH") == std::string::npos) {
	  hDQ_TrendPlot->SetMinimum(1000); // I don't think there were any thresholds below 1000
      }

      // For the pedestals, change all bin contents so that they are the difference from the mean for that channel
      if (histogram_name.find("Pedestal") != std::string::npos) {
	
	// Loop through the y-axis (channels)
	for (int yBin = 0; yBin < hDQ_TrendPlot->GetNbinsY(); ++yBin) {
	  
	  // Get the mean for this row
	  double mean = 0;
	  int n_runs = 0;
	  for (int xBin = 0; xBin < hDQ_TrendPlot->GetNbinsX(); ++xBin) {
	    double bin_content = hDQ_TrendPlot->GetBinContent(xBin, yBin);
	    if (bin_content != 0) {
	      mean += bin_content;
	      ++n_runs;
	    }
	  }
	  mean /= n_runs;

	  // Now go through and change the contents of the histogram
	  for (int xBin = 0; xBin < hDQ_TrendPlot->GetNbinsX(); ++xBin) {
	    double bin_content = hDQ_TrendPlot->GetBinContent(xBin, yBin);
	    if (bin_content != 0) {
	      bin_content = (bin_content - mean);
	      hDQ_TrendPlot->SetBinContent(xBin, yBin, bin_content);
	    }
	  }
	}

	hDQ_TrendPlot->GetZaxis()->SetTitle("Pedestal - Mean of Pedestal [ADC]");
      }

      // Get a legend ready here
      TLegend* legend = new TLegend(0.3, 0.85, 0.6, 0.75, "");
      legend->SetBorderSize(0);
      legend->SetTextSize(0.04);
      legend->SetFillColor(kWhite);

      // For these plots we want a TH1F* rather than a TH2
      if (histogram_name.find("DAQLivetime") != std::string::npos || // DAQ livetime
	  histogram_name.find("RunTime") != std::string::npos ||
	  histogram_name.find("TDCCheck_muSc") != std::string::npos ) {
	
	TH1D* hDQ_TrendPlot_1D = hDQ_TrendPlot->ProjectionX("_px", 2, 2);
	hDQ_TrendPlot_1D->GetYaxis()->SetTitle(hDQ_TrendPlot->GetZaxis()->GetTitle());
	hDQ_TrendPlot_1D->Draw();
      }
      else if (histogram_name.find("Threshold") != std::string::npos) {


	TH1D* hDQ_TrendPlot_1D = hDQ_TrendPlot->ProjectionX("_px", 1, 1);
	hDQ_TrendPlot_1D->GetYaxis()->SetTitle(hDQ_TrendPlot->GetZaxis()->GetTitle());
	hDQ_TrendPlot_1D->Draw();
      }

      // Set some plots to log-z
      if (histogram_name.find("TDiff") != std::string::npos ||
	  histogram_name.find("Amplitude") != std::string::npos ||
	  histogram_name.find("IslandLength") != std::string::npos ||
	  histogram_name.find("IslandRate") != std::string::npos ||
	  histogram_name.find("IslandCounter") != std::string::npos ||
	  histogram_name.find("Pedestal") != std::string::npos) {
	c1->SetLogz(1);
      }
      else {
	c1->SetLogz(0);
      }
      c1->Print(pngname.c_str());
    }

  }

  // Loop through the runs and delete the files
  for (int iRun = 0; iRun < n_runs; ++iRun) {
    delete files[iRun];
  }
}
