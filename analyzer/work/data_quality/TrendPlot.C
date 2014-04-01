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

    std::stringstream filename;
    filename << "test0" << first_run + iRun << ".root";
    std::cout << filename.str() << std::endl;
    files[iRun] = new TFile(filename.str().c_str(), "READ");
  }

  TH2F* trend_plot = new TH2F("TrendPlot", "TrendPlot", n_runs, first_run, first_run+n_runs, 1200,0,100e6);

  for (int iRun = 0; iRun < n_runs; ++iRun) {
    TH1F* hist = (TH1F*) files[iRun]->Get("DataQuality_LowLevel/MDQ_BlockTime/hDQ_BlockTime_muSc");
    
    for (int iBin = 1; iBin <= hist->GetNbinsX(); ++iBin) {
      trend_plot->Fill(first_run + iRun, hist->GetBinCenter(iBin), hist->GetBinContent(iBin)); // (x = run #, y = time stamp, z = N_TPI)
    }
    hist->Draw();

  }

  trend_plot->Draw("COLZ");

  c1->Update();

}
