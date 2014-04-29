#include <TROOT.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <TKey.h>

#include <iostream>
#include <sstream>

bool WantPlot(std::string histname);
bool WantAsTrendPlot(std::string histname);
bool WantLogZ(std::string histname);

void ZoomAxes();


void CreatePictureBooks(const char* data_dir, int first_run, const int n_runs) {

  gROOT->Reset();
  gROOT->SetStyle("Plain");
  gStyle->SetCanvasBorderMode(0); // turn off canvas borders

  // Create the canvases
  //  TCanvas* trend_canvas = new TCanvas("trend_canvas", "trend_canvas");

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


  // Now we have the histogram names, so loop though them
  for (int iHist = 0; iHist < histogram_names.size(); ++iHist) {

    std::string histname = histogram_names.at(iHist);

    // Check to see if we actually want this plot
    if ( !WantPlot(histname)) {
      continue;
    }

    // Check to see if we want this histogram as an individual plot or a trend plot
    bool trend_plot = WantAsTrendPlot(histname);

    // Loop through the runs
    for (int iRun = 0; iRun < n_runs; ++iRun) {
      
      // Make sure this file exists (there may be some missing in our golden run ranges)
      if (files[iRun]->IsZombie())
	continue;

      // Get the histogram
      std::string fullhistname = "DataQuality_LowLevel/" + histname;      
      TH1* hist = (TH1*) files[iRun]->Get(fullhistname.c_str());
      
      if (trend_plot == false) {

	// Print the histogram out directly to the individual picture book
	individual_canvases[iRun]->cd();
	hist->Draw("COLZ");
	individual_canvases[iRun]->SetLogz(WantLogZ(histname)); // change to a log-z scale if we want to
	individual_canvases[iRun]->Update();
	individual_canvases[iRun]->Print(individual_basepdfname[iRun].str().c_str());
      
      }
      else {
	// Fill this histogram into the trend plot
      }
    }
  }


  // Now go through and clean up
  for (int iRun = 0; iRun < n_runs; ++iRun) {
    individual_canvases[iRun]->Print(individual_closepdfname[iRun].c_str());
  }
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

// bool WantLogZ()
// -- returns true if this plot wants to have the z-axis as log
bool WantLogZ() {

  if (histname.find("hDQ_PulseShapes") != std::string::npos) {
    return true;
  }
  else {
    return false;
  }
}


void ZoomAxes() {
  std::cout << "Implement Zooming Axes" << std::endl;
}
