#include <TROOT.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <TKey.h>

#include <iostream>
#include <sstream>

std::vector<std::string> OrderHistogramNames(std::vector<std::string> hist_names);

bool WantPlot(std::string histname);
bool WantAsTrendPlot(std::string histname);
bool WantLogZ(std::string histname);

void ZoomTrendPlot(TH2F* trend_plot);
void ZoomIndividualPlot(TH1* individual_plot);
void ZoomAxis(TAxis* axis, int max_bin);


void CreatePictureBooks(const char* data_dir, int first_run, const int n_runs) {

  gROOT->Reset();
  gROOT->SetStyle("Plain");
  gStyle->SetCanvasBorderMode(0); // turn off canvas borders



  // Create the canvases
  TCanvas* trend_canvas = new TCanvas("trend_canvas", "trend_canvas");

  TFile* files[n_runs];
  TCanvas* individual_canvases[n_runs];

  // Names for opening, closing and saving to PDFs
  std::stringstream individual_basepdfname[n_runs];
  std::string individual_openpdfname[n_runs];
  std::string individual_closepdfname[n_runs];

  for (int iRun = 0; iRun < n_runs; ++iRun) {

    int run_number = first_run + iRun;

    // Open up the file for this run
    std::stringstream filename;
    filename << data_dir << "/hist/hist0" << run_number << ".root";
    files[iRun] = new TFile(filename.str().c_str(), "READ");

    // Create an individual canvas for this run
    std::stringstream canvasname;
    canvasname << "individual_canvas_run0" << run_number;
    individual_canvases[iRun] = new TCanvas(canvasname.str().c_str(),canvasname.str().c_str());

    // Open a PDF for each run
    individual_basepdfname[iRun].str("");
    individual_basepdfname[iRun] << "DQ_LowLevel_RunPlots_Run0" << run_number << ".pdf";
    
    individual_openpdfname[iRun] =  individual_basepdfname[iRun].str() + "[";
    individual_closepdfname[iRun] = individual_basepdfname[iRun].str() + "]";
    individual_canvases[iRun]->Print(individual_openpdfname[iRun].c_str());
  }



  // Get the names of all the histograms
  std::vector<std::string> histogram_names;

  TIter nextFileKey(files[0]->GetListOfKeys()); // get the list of keys for the top level in the first file (all files should be the same)
  TKey *fileKey;
        
  while (fileKey = (TKey*)nextFileKey()) {

    // Get the directory for low-level data quality plots first
    if (strcmp(fileKey->ReadObj()->ClassName(), "TDirectoryFile") == 0) {
      // Check that the directory has the correct name
      if (strcmp(fileKey->ReadObj()->GetName(), "DataQuality_LowLevel") == 0) {

	TDirectoryFile* dir = (TDirectoryFile*) fileKey->ReadObj();
	
	TIter nextDirKey(dir->GetListOfKeys()); // get the list of keys in the directory (all histograms should be in this folder)
	TKey *dirKey;
        
	while (dirKey = (TKey*)nextDirKey()) {

	  // Get the names of all the histograms and count them up
	  if (strcmp(dirKey->ReadObj()->ClassName(), "TH1F") == 0 ||
	      strcmp(dirKey->ReadObj()->ClassName(), "TH2F") == 0) {

	    histogram_names.push_back(dirKey->ReadObj()->GetName());
	  } // end if (class is TH1F)

	} // end while loop (through keys in the directory)
      } // end if (directory is DataQuality_LowLevel)
    } // end if (class is TDirectoryFile)
  } // end while loop (through keys in file)

  // Reorder the histogram names so that channels from the same board are next to each other (might make it easier to print 4 plots on the same page)
  std::vector<std::string> ordered_histogram_names = OrderHistogramNames(histogram_names);

  std::stringstream trend_basepdfname;
  trend_basepdfname << "DQ_LowLevel_TrendPlots_Runs0" << first_run << "-" << first_run + n_runs << ".pdf";
  std::string trend_openpdfname = trend_basepdfname.str() + "[";
  std::string trend_closepdfname = trend_basepdfname.str() + "]";

  trend_canvas->Print(trend_openpdfname.c_str());



  // Now we have the histogram names, so loop though them
  for (int iHist = 0; iHist < ordered_histogram_names.size(); ++iHist) {

    std::string histname = ordered_histogram_names.at(iHist);
    std::string fullhistname = "DataQuality_LowLevel/" + histname;

    // Check to see if we actually want this plot
    if ( !WantPlot(histname)) {
      continue;
    }


    // Create the trend plot
    std::string trendplotname = histname + "_TrendPlot";
    std::string trendplottitle = "Trend plot of " + histname;

    // Get some useful information from the histogram in the first run file (for the y-axis range of the trend plot)
    TH1F* hist = (TH1F*) files[0]->Get(fullhistname.c_str());

    TH2F* trend_plot = new TH2F(trendplotname.c_str(), trendplottitle.c_str(), n_runs, first_run, first_run+n_runs, hist->GetNbinsX(), hist->GetXaxis()->GetXmin(), hist->GetXaxis()->GetXmax());

    trend_plot->GetYaxis()->SetTitle(hist->GetXaxis()->GetTitle());
    trend_plot->GetYaxis()->SetLabelSize(0.03);

    trend_plot->GetXaxis()->SetTitle("Run Number");
    trend_plot->GetXaxis()->SetLabelSize(0.03);

    trend_plot->GetZaxis()->SetTitle(hist->GetYaxis()->GetTitle());
    trend_plot->GetZaxis()->SetLabelSize(0.03);
    trend_plot->GetZaxis()->SetTitleOffset(0.85);
    trend_plot->GetZaxis()->SetTitleSize(0.03);
    trend_plot->SetStats(false);

    // Set the fraction trend plots to have a maximum of 1 so that red = bad
    if (histname.find("Fraction") != std::string::npos) {
      trend_plot->GetZaxis()->SetRangeUser(0,1);
    }
    // Copy the axis labels so that they show the detector and channel names
    else if (histname.find("hDQ_IslandRate") != std::string::npos) {
      for (int jBin = 1; jBin < hist->GetNbinsX(); ++jBin) {
	trend_plot->GetYaxis()->SetBinLabel(jBin, hist->GetXaxis()->GetBinLabel(jBin));
      }
      trend_plot->GetYaxis()->SetTitle(false);
      trend_plot->GetYaxis()->SetLabelSize(0.02);
      trend_plot->GetZaxis()->SetLabelSize(0.02);
    }

    // Check to see if we want this histogram as an individual or a trend plot
    bool want_trend_plot = WantAsTrendPlot(histname);

    // Loop through the runs
    for (int iRun = 0; iRun < n_runs; ++iRun) {
      
      // Make sure this file exists (there may be some missing in our golden run ranges)
      if (files[iRun]->IsZombie())
	continue;

      // Get the histogram
      TH1* hist = (TH1*) files[iRun]->Get(fullhistname.c_str());
      
      if (want_trend_plot == false) {

	// Print the histogram out directly to the individual picture book
	individual_canvases[iRun]->cd();
	ZoomIndividualPlot(hist);
	hist->Draw("COLZ");
	individual_canvases[iRun]->SetLogz(WantLogZ(histname)); // change to a log-z scale if we want to
	individual_canvases[iRun]->Update();
	individual_canvases[iRun]->Print(individual_basepdfname[iRun].str().c_str());
      
      }
      else {
	// Fill this histogram into the trend plot
	for (int iBin = 1; iBin <= hist->GetNbinsX(); ++iBin) {
	  trend_plot->Fill(first_run + iRun, hist->GetBinCenter(iBin), hist->GetBinContent(iBin)); // (x = run #, y = time stamp, z = N_TPI)
	} // end for loop (filling trend plot)
      } // end if (trend plot)
    } // end for loop (through runs)

    // Now export the trend plot to PDF (if applicable)
    if (want_trend_plot) {
      trend_canvas->cd();
      ZoomTrendPlot(trend_plot);
      trend_plot->Draw("COLZ");
      trend_canvas->Print(trend_basepdfname.str().c_str());
    }
  }


  // Now go through and clean up
  for (int iRun = 0; iRun < n_runs; ++iRun) {
    individual_canvases[iRun]->Print(individual_closepdfname[iRun].c_str());
  }
  trend_canvas->Print(trend_closepdfname.c_str());
}

// std::vector<std::string> OrderHistogramNames(std::vector<std::string> hist_names)
// -- Reorders the histogram names so that plots from the same board are next to each other
std::vector<std::string> OrderHistogramNames(std::vector<std::string> hist_names) {
  
  std::vector<std::string> output_names;
  output_names.resize(hist_names.size());
  int current_location = 0;

  for (int iHist = 0; iHist < hist_names.size(); ++iHist) {

    std::string histname = hist_names.at(iHist);
    std::cout << histname << " " << current_location << std::endl;

    if (histname.find("CaBU") != std::string::npos) {
      output_names[current_location] = histname;
      ++current_location;

      // Get the base histogram name (without the channel and detector names)
      histname.erase(histname.rfind("_"), histname.size());
      histname.erase(histname.rfind("_"), histname.size());
      std::cout << histname << std::endl;

      // Search for the other three channels and add them in the elements after this one
      for (int jHist = 0; jHist < hist_names.size(); ++jHist) {
	
	std::string histname_b = hist_names.at(jHist);
	if (histname_b.find(histname) != std::string::npos) {
	  if (histname_b.find("CbBU") != std::string::npos) {
	    output_names[current_location] = histname_b;
	    ++current_location;
	  }
	  else if (histname_b.find("CcBU") != std::string::npos) {
	    output_names[current_location] = histname_b;
	    ++current_location;
	  }
	  else if (histname_b.find("CdBU") != std::string::npos) {
	    output_names[current_location] = histname_b;
	    ++current_location;
	  }
	}
      }
    } // end if CaBU

    // Don't want to print these twice
    else if ( histname.find("CbBU") != std::string::npos ||
              histname.find("CcBU") != std::string::npos ||
              histname.find("CdBU") != std::string::npos) {
      // Do nothing

    }

    // Anything that's not channel specific (e.g. run time)
    else {
      output_names[current_location] = histname;
      ++current_location;
    }
  }

  return output_names;
}

// bool WantPlot(std::string histname)
// -- returns true if we want this plot
bool WantPlot(std::string histname) {
  
  // There's only a few histograms we don't need
  if (histname.find("hDQ_FADCBufferOverflow_Total") != std::string::npos ||
      histname.find("hDQ_FADCPacketLoss_Total") != std::string::npos || 
      histname.find("hDQ_TDCCheck_muSc") != std::string::npos ||
      histname.find("hDQ_TDCCheck_muSc_time") != std::string::npos ||
      histname.find("hDQ_TDCCheck_muScA") != std::string::npos ||
      histname.find("hDQ_TDCCheck_muPC") != std::string::npos) {

    return false;
  }
  else {
    return true;
  }
}

// bool WantAsTrendPlot(std::string histname)
// -- returns true if we want this histogram to be exported as a trend plot
bool WantAsTrendPlot(std::string histname) {

  if (histname.find("hDQ_Amplitude") != std::string::npos ||
      histname.find("hDQ_PulseShapes") != std::string::npos || 
      histname.find("hDQ_muScTDiff") != std::string::npos) {

    return false;
  }
  else {
    return true;
  }
}

// bool WantLogZ(std::string histname)
// -- returns true if this plot wants to have the z-axis as log
bool WantLogZ(std::string histname) {

  if (histname.find("hDQ_PulseShapes") != std::string::npos) {
    return true;
  }
  else {
    return false;
  }
}

// void ZoomTrendPlot(TH2F* trend_plot)
// -- Finds the range that we want on the y-axis for the trend plot
void ZoomTrendPlot(TH2F* trend_plot) {

  int max_bin = 0;

  for (int jBin = trend_plot->GetNbinsY(); jBin > 0; --jBin) {
	  
    // Loop through the runs as well since the highest occupied y-bin might be in one of these
    for (int iBin = trend_plot->GetNbinsX(); iBin > 0; --iBin) {

      if (trend_plot->GetBinContent(iBin, jBin) >= 1) {
	      
	// See if this y-bin is higher than the previous
	if (jBin > max_bin) {
	  max_bin = jBin;
	  break;
	}
      }
    }
  }

  ZoomAxis(trend_plot->GetYaxis(), max_bin);
}

// void ZoomIndividualPlot(TH1* individual_plot)
// -- Finds the range that we want on the x-axis for the individual plot
void ZoomIndividualPlot(TH1* individual_plot) {

  int max_bin = 0;

  for (int iBin = individual_plot->GetNbinsX(); iBin > 0; --iBin) {
	  
    if (individual_plot->GetBinContent(iBin) >= 1) {
	      
      // See if this bin is higher than the previous
      if (iBin > max_bin) {
	max_bin = iBin;
	break;
      }
    }
  }

  ZoomAxis(individual_plot->GetXaxis(), max_bin);
}


// int ZoomAxis(TAxis* axis, int max_bin)
// -- Sets the range of the axis so that it is the smallest possible
void ZoomAxis(TAxis* axis, int max_bin) {

  axis->SetRange(1, max_bin); // set the range based on bin number
}
