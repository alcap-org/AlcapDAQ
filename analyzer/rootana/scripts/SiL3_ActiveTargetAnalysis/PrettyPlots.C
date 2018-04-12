// Make all the pretty plots for this analysis
#include "scripts/SiL3_ActiveTargetAnalysis/Pretty_CountStoppedMuons_GeLoGain.C";

void PrettyPlots() {

  TCanvas* c_mustops_gelogain = Pretty_CountStoppedMuons_GeLoGain();

  
  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  c_mustops_gelogain->Write();
  outfile->Write();
  outfile->Close();
}
