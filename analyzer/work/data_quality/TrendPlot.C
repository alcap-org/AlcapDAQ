#include <TROOT.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <TKey.h>

#include <iostream>
#include <sstream>

void TrendPlot(const char* data_dir, int first_run, const int n_runs) {

  gROOT->Reset();
  gROOT->SetStyle("Plain");
  gStyle->SetCanvasBorderMode(0); // turn off canvas borders

  TCanvas* c1 = new TCanvas("c1", "c1");

  TFile* files[n_runs];

  for (int iRun = 0; iRun < n_runs; ++iRun) {

    // Open up the file for this run
    std::stringstream filename;
    filename << data_dir << "/hist/hist0" << first_run + iRun << ".root";
    files[iRun] = new TFile(filename.str().c_str(), "READ");
  }

  // Ultimately we want a list of histograms
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
	  if (strcmp(dirKey->ReadObj()->ClassName(), "TH1F") == 0) {
	    histogram_names.push_back(dirKey->ReadObj()->GetName());
	  }
	}
      }

    }
  }

  // Open a PDF to save all the trend plots to
  c1->Print("DQ_LowLevel_TrendPlots.pdf[");

  // Now go through the histograms and create a trend plot for each one
  for (int iHist = 0; iHist < histogram_names.size(); ++iHist) {


    std::string trendplotname = histogram_names.at(iHist) + "_TrendPlot";
    std::string trendplottitle = "Trend plot of " + histogram_names.at(iHist);

    // Get some useful information from the histogram in the first run file (for the y-axis range of the trend plot)
    std::string histname = "DataQuality_LowLevel/" + histogram_names.at(iHist);
    TH1F* hist = (TH1F*) files[0]->Get(histname.c_str());

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

    // Now go through all the runs and fill the trend plot
    for (int iRun = 0; iRun < n_runs; ++iRun) {
      
      // Make sure this file exists (in our golden runs, there may be some missing)
      if (files[iRun]->IsZombie())
	continue;
      
      std::string histname = "DataQuality_LowLevel/" + histogram_names.at(iHist);
      TH1F* hist = (TH1F*) files[iRun]->Get(histname.c_str());

      for (int iBin = 1; iBin <= hist->GetNbinsX(); ++iBin) {
	trend_plot->Fill(first_run + iRun, hist->GetBinCenter(iBin), hist->GetBinContent(iBin)); // (x = run #, y = time stamp, z = N_TPI)
      }

      // Do any changes to specific types of histograms here
      if (histname.find("hDQ_IslandCounter") != std::string::npos) {

	// Currently, the trend plots for this module have a large range for some of the channels
	// so find the last bin that has any content and shrink the trend plot to that
	int max_bin = 0;

	for (int jBin = trend_plot->GetNbinsY(); jBin > 0; --jBin) {
	  
	  if (trend_plot->GetBinContent(1, jBin) >= 1) {
	    max_bin = jBin;
	    break;
	  }
	}

	trend_plot->GetYaxis()->SetRange(1, max_bin+5); // set the range based on bin number
      }

    }

    trend_plot->Draw("COLZ");

    c1->Update();

    c1->Print("DQ_LowLevel_TrendPlots.pdf");
  }

  // Close the PDF file
  c1->Print("DQ_LowLevel_TrendPlots.pdf]");


}
