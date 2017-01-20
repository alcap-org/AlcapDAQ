#include "TFile.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TDirectory.h"
#include "TList.h"
#include "TRandom3.h"

#include <iostream>

// This macro draws 4 random TPIs and TAPs that were exported using ExportPulse in rootana
void DrawTPIsAndTAPs_fromPulseViewer(std::string filename, int seed = 0) {
  TFile* file = new TFile(filename.c_str(), "READ");
  if (file->IsZombie()) {
    std::cout << "Problem opening file " << filename << std::endl;
    return;
  }

  int n_pulses_to_draw = 4;
  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->Divide(n_pulses_to_draw/2, n_pulses_to_draw/2);

  TDirectory* tpi_dir = file->GetDirectory("ExportPulse/TPIs");
  TList* tpi_hists = tpi_dir->GetListOfKeys();
  int n_max_hists = tpi_hists->GetSize();

  TRandom3 random(seed);

  for (int i_hist = 0; i_hist < n_pulses_to_draw; ++i_hist) {
    c1->cd(i_hist+1);

    int rndm = (random.Rndm())*n_max_hists;

    std::string pulse_name = tpi_hists->At(rndm)->GetName();
    std::string tpi_histname = "ExportPulse/TPIs/" + pulse_name;
 
    TH1F* hTPI = (TH1F*) file->Get(tpi_histname.c_str());
    if (!hTPI) {
      std::cout << "Problem obtaining TPI histogram " << tpi_histname << std::endl;
      return;
    }
    
    std::string tap_histname = "ExportPulse/TAPs/" + pulse_name + "_AP";
    TH1F* hTAP = (TH1F*) file->Get(tap_histname.c_str());
    if (!hTAP) {
      std::cout << "Problem obtaining TAP histogram " << tap_histname << std::endl;
      return;
    }
    hTAP->SetLineColor(kRed);
    
    hTPI->Draw();
    hTAP->Draw("SAME");
  }
}
