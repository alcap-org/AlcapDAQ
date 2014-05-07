#include <TROOT.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <TKey.h>

#include <iostream>
#include <sstream>

// This will generate the following PDFs:
// -- hDQ_[RunHistogramName]_TrendPlot.png
void GenerateTrendPlots(std::string data_dir, int first_run, int n_runs) {

  std::cout << "Generating trend plots..." << std::endl;

  gROOT->Reset();
  gROOT->SetStyle("Plain");
  gStyle->SetCanvasBorderMode(0); // turn off canvas borders


  std::stringstream filename;
  filename << data_dir << "hist/hist0" << first_run << ".root";
  TFile* first_file = new TFile(filename.str().c_str(), "READ");

  // The histograms
  TH2F* hDQ_TrendPlot;
  TH1F *hDQ_RunPlot;

  // Loop through the histograms in the first_file and get the island histograms (bank and channel names may differ between runs)
  TDirectoryFile* dir = (TDirectoryFile*) first_file->Get("DataQuality_LowLevel");
  
  TIter nextDirKey(dir->GetListOfKeys()); // get the list of keys in the directory (all histograms should be in this folder)
  TKey *dirKey;
  
  while ( (dirKey = (TKey*)nextDirKey()) ) {
    // Get all the histograms
    if (strcmp(dirKey->ReadObj()->ClassName(), "TH1F") == 0) {


      // Set up the canvases and trend plots from the first file
      TCanvas *c1 = new TCanvas();

      std::string histogram_name = dirKey->ReadObj()->GetName();

      // Don't want any of these plots as trends
      if (histogram_name.find("Total") != std::string::npos || 
	  histogram_name.find("TDCCheck_muSc") != std::string::npos || 
	  histogram_name.find("TDCCheck_muScA") != std::string::npos || 
	  histogram_name.find("TDCCheck_muPC") != std::string::npos || 
	  histogram_name.find("TDCCheck_muSc_time") != std::string::npos) {
	continue;
      }

      std::string histogram_location = "DataQuality_LowLevel/" + histogram_name;
      std::string pngname = "data_quality_figs/" + histogram_name + "_TrendPlot.png";

      if (histogram_name.find("Amplitude") != std::string::npos) {
	pngname.insert(pngname.find("Amplitude"), "Island");
      }

      first_file->GetObject(histogram_location.c_str(),hDQ_RunPlot);



      std::string trendplotname = histogram_name + "_TrendPlot";
      std::string trendplottitle = "Trend Plot of " + histogram_name;

      hDQ_TrendPlot = new TH2F(trendplotname.c_str(), trendplottitle.c_str(), n_runs, first_run, first_run+n_runs, hDQ_RunPlot->GetNbinsX(), hDQ_RunPlot->GetXaxis()->GetXmin(), hDQ_RunPlot->GetXaxis()->GetXmax());

      hDQ_TrendPlot->GetYaxis()->SetTitle(hDQ_RunPlot->GetXaxis()->GetTitle());
      hDQ_TrendPlot->GetYaxis()->SetLabelSize(0.03);

      hDQ_TrendPlot->GetXaxis()->SetTitle("Run Number");
      hDQ_TrendPlot->GetXaxis()->SetLabelSize(0.03);

      hDQ_TrendPlot->GetZaxis()->SetTitle(hDQ_RunPlot->GetYaxis()->GetTitle());
      hDQ_TrendPlot->GetZaxis()->SetLabelSize(0.03);
      hDQ_TrendPlot->GetZaxis()->SetTitleOffset(0.85);
      hDQ_TrendPlot->GetZaxis()->SetTitleSize(0.03);
      hDQ_TrendPlot->SetStats(false);

      // Want to copy the bin labels for this plot
      if (histogram_name.find("IslandRate") != std::string::npos) {

	// With hDQ_IslandRate we want the bin labels transferred from the x-axis of the histogram to the y-axis of the trend plot
	for (int jBin = 1; jBin < hDQ_RunPlot->GetNbinsX(); ++jBin) {

	  hDQ_TrendPlot->GetYaxis()->SetBinLabel(jBin, hDQ_RunPlot->GetXaxis()->GetBinLabel(jBin));
	  hDQ_TrendPlot->GetYaxis()->SetTitle("");
	}
      }

      // Want the Fraction plots to go up to 1 so that red = bad
      if (histogram_name.find("Fraction") != std::string::npos) {
	hDQ_TrendPlot->GetZaxis()->SetRangeUser(0,1);
      }



      // Loop through the runs
      for (int iRun = 0; iRun < n_runs; ++iRun) {

	std::stringstream newfilename;
	newfilename << data_dir << "hist/hist0" << (first_run + iRun) << ".root";

	TFile* newfile = new TFile(newfilename.str().c_str(), "READ");

	if (newfile->IsZombie())
	  continue; 

	newfile->GetObject(histogram_location.c_str(),hDQ_RunPlot);

	// Fill the trend plot
	for (int iBin = 1; iBin <= hDQ_RunPlot->GetNbinsX(); ++iBin) {
	  hDQ_TrendPlot->Fill(first_run + iRun, hDQ_RunPlot->GetBinCenter(iBin), hDQ_RunPlot->GetBinContent(iBin)); // (x = run #, y = time stamp, z = N_TPI)
	}

	newfile->Close();
      }

      hDQ_TrendPlot->Draw("COLZ");

      // Set some plots to log-z
      if (histogram_name.find("TDiff") != std::string::npos ||
	  histogram_name.find("Amplitude") != std::string::npos ||
	  histogram_name.find("IslandLength") != std::string::npos ||
	  histogram_name.find("IslandRate") != std::string::npos) {
	c1->SetLogz(1);
      }
      else {
	c1->SetLogz(0);
      }
      c1->Print(pngname.c_str());
    }
  }
}
