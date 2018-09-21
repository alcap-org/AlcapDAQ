#include "scripts/SiliconAnalyses/Utils/BandExtractionEfficiency.C"

void Si16b_BandExtractionEfficiency() {
  BandExtractionEfficiencyArgs args;
  args.indatafilename = "~/data/results/Si16b_passive/plots.root";
  args.indatahistname = "all_SiR_timecut/hEvdE_TwoLayer";
  
  //  args.inmcfilenames.push_back("~/data/mc/Si16b_passive/R15b_Si16b_response-matrix_500keV-bins_protons_new.root");
  args.inmcfilenames.push_back("~/data/mc/Si16b_passive/R15b_Si16b_response-matrix_500keV-bins_protons_new_wThinEnergySF.root");
  args.inmchistnames.push_back("SiR_EvdE_stopped");
  args.incutnames.push_back("proton_SiR_timecut/proton_cut_two_layer");

  //  args.inmcfilenames.push_back("~/data/mc/Si16b_passive/R15b_Si16b_response-matrix_500keV-bins_deuterons_new.root");
  //  args.inmchistnames.push_back("SiR_EvdE_stopped");
  //  args.incutnames.push_back("deuteron_SiR_timecut/deuteron_cut_two_layer");

  args.outfilename = "~/data/results/Si16b_passive/band-extraction-efficiency.root";

  TFile* outfile = new TFile(args.outfilename.c_str(), "RECREATE");
  outfile->Write();
  outfile->Close();

  BandExtractionEfficiency(args);
}
