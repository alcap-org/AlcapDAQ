#include "TFile.h"
#include "TH1.h"
#include "TH2.h"

#include "scripts/XRayAnalysis/XRayUtils.h"

int XRaySpectrum() {

  //////////////////
  // User Parameters
  std::string channel = "GeLoGain";
  std::string dataset = "Si16b_passive";
  //  std::string filename = "/home/edmonds/data/out/local/Si16b_v11_ge-spectrum.root"; // must contain a 2D histogram of energy vs time
  //  std::string histname = "PlotTAP_EnergyTime/h" + channel + "#FirstComplete#{constant_fraction=0.20}{no_time_shift= true}_EnergyTime;1";
  //  std::string outfilename = "/home/edmonds/data/results/" + dataset + "/XRaySpectrum_" + channel + ".root";
  //  int rebin_factor = 1;
  //  std::string filename = "/home/edmonds/data/results/Si16b_passive/EvdEPlots.root"; // must contain a 2D histogram of energy vs time
  std::string filename = "/home/edmonds/data/results/Si16b_passive/EvdEPlots_NewCuts.root"; // must contain a 2D histogram of energy vs timea
  std::string histname = "h" + channel + "_EnergyTime";
  std::string outfilename = "/home/edmonds/data/results/" + dataset + "/XRaySpectrum_" + channel + "_fromTMEs_NewCuts.root";
  int rebin_factor = 4;
  //  double min_time_cut = -200;
  //  double max_time_cut = 200;
  
  ///////////////////////////////////
  // Now produce plots  
  TFile* file = new TFile(filename.c_str(), "READ");
  if (file->IsZombie()) {
    std::cout << "Trouble opening file " << filename << std::endl;
    return 1;
  }
  
  TH2F* hGe_2DPlot = (TH2F*) file->Get(histname.c_str());
  if (!hGe_2DPlot) {
    std::cout << "Trouble finding 2D histogram " << histname << std::endl;
    return 1;
  }

  //  int min_time_cut_bin = hGe_2DPlot->GetYaxis()->FindBin(min_time_cut);
  //  int max_time_cut_bin = hGe_2DPlot->GetYaxis()->FindBin(max_time_cut)-1;
  TH1D* hGe_FullSpectrum = hGe_2DPlot->ProjectionX("hGe_FullSpectrum");//, min_time_cut_bin, max_time_cut_bin);
  hGe_FullSpectrum->Rebin(rebin_factor);

  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");

  std::stringstream histtitle;
  histtitle << "X-Ray Spectrum (" << channel << ")";
  hGe_FullSpectrum->SetStats(false);
  hGe_FullSpectrum->SetXTitle("Energy [keV]");
  hGe_FullSpectrum->SetYTitle("Raw Count");
  hGe_FullSpectrum->GetYaxis()->SetTitleOffset(1.3);
  hGe_FullSpectrum->SetTitle(histtitle.str().c_str());
  hGe_FullSpectrum->GetXaxis()->SetRangeUser(50, 1200);
  hGe_FullSpectrum->SetMaximum(hGe_FullSpectrum->GetMaximum()*1.50);
  hGe_FullSpectrum->Write();
  
  outfile->Write();
  outfile->Close();
}
