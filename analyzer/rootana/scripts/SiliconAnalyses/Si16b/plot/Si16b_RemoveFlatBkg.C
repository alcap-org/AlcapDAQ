#include "scripts/SiliconAnalyses/Utils/RemoveFlatBkg.C"

void Si16b_RemoveFlatBkg(std::string infilename, std::string outfilename) {

  RemoveFlatBkgArgs args;
  args.infilename = outfilename; // this will be doing the subtraction in histograms that are already in the output file
  args.outfilename = outfilename;
  args.outdirname = "SiL3_wFlatBkgRemoval";
  args.inhistname = "SiL3/hEnergyTime";
  args.inflatbkgname = "SiL3_FlatBkg/hEnergyTime";
  RemoveFlatBkg(args);
}
