#include "scripts/SiliconAnalyses/Utils/SiTEnergy.C"

void Si16b_SiTEnergy(std::string infilename, std::string outfilename) {

  SiTEnergyArgs args;
  args.infilename = infilename;

  args.min_energy = 0;
  args.max_energy = 20000;
  args.energy_width = 10;

  args.treename = "siBlockTree_SiT";
  args.outfilename = outfilename;
  args.outhistname = "hSiTEnergy";
  args.outdirname = "SiTEnergy";
  SiTEnergy(args);
}
