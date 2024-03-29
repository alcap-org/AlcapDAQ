#include "TFile.h"
#include "TH1.h"
#include "TH2.h"

#include "scripts/XRayAnalysis/XRayUtils.h"

int XRaySpectrum() {

  //////////////////
  // User Parameters
  std::string channel = "GeHiGain";
  std::string dataset = "SiL3_active";
  //  std::string filename = "/home/edmonds/data/out/v6/out09040.root"; // must contain a 2D histogram of energy vs time
  //  std::string filename = "/home/edmonds/data/out/v6/SiL3_ge-spectrum.root"; // must contain a 2D histogram of energy vs time
  //  std::string histname = "PlotTAP_EnergyTime/h" + channel + "#MaxBinAPGenerator#{no_time_shift=true}_EnergyTime";
  std::string filename = "~/data/results/" + dataset + "/EventSelection_NewCuts.root"; // must contain a 2D histogram of energy vs time
  std::string histname = "h" + channel + "_EnergyTime";
  std::string outfilename = "/home/edmonds/data/results/" + dataset + "/XRaySpectrum_" + channel + ".root";
  
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

  TH1D* hGe_FullSpectrum = hGe_2DPlot->ProjectionX("hGe_FullSpectrum");

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
