#include <sstream>

void TrendPlot() {

  gROOT->Reset();
  gROOT->SetStyle("Plain");
  gStyle->SetCanvasBorderMode(0); // turn off canvas borders

  TCanvas* c1 = new TCanvas("c1", "c1");

  const int n_runs = 3;
  const int first_run = 2600;

  TFile* files[n_runs];

  for (int iRun = 0; iRun < n_runs; ++iRun) {

    // Open up the file for this run
    std::stringstream filename;
    filename << "test0" << first_run + iRun << ".root";
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
	    std::cout << "Histogram: " << dirKey->ReadObj()->GetName() << std::endl;
	    histogram_names.push_back(dirKey->ReadObj()->GetName());
	  }
	}
      }

    }
  }


  // Now go through the histograms and create a trend plot for each one
  for (int iHist = 0; iHist < histogram_names.size(); ++iHist) {


    std::string trendplotname = histogram_names.at(iHist) + "_TrendPlot";
    std::string trendplottitle = "Trend plot of " + histogram_names.at(iHist);

    TH2F* trend_plot = new TH2F(trendplotname.c_str(), trendplottitle.c_str(), n_runs, first_run, first_run+n_runs, 1200,0,100e6);

    for (int iRun = 0; iRun < n_runs; ++iRun) {

      std::string histname = "DataQuality_LowLevel/" + histogram_names.at(iHist);
      TH1F* hist = (TH1F*) files[iRun]->Get(histname.c_str());
    
      for (int iBin = 1; iBin <= hist->GetNbinsX(); ++iBin) {
	trend_plot->Fill(first_run + iRun, hist->GetBinCenter(iBin), hist->GetBinContent(iBin)); // (x = run #, y = time stamp, z = N_TPI)
      }

    }

    trend_plot->Draw("COLZ");

    c1->Update();

    std::string trendplotfile = trendplotname + ".pdf";
    c1->Print(trendplotfile.c_str());
  }


}
