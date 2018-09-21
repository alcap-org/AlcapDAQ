#include "scripts/SiliconAnalyses/Utils/RemoveFlatBkg.C"

void SiL3_RemoveFlatBkg(std::string infilename, std::string outfilename) {

  RemoveFlatBkgArgs args;
  args.infilename = outfilename; // this will be doing the subtraction in histograms that are already in the output file
  args.outfilename = outfilename;
  args.outdirname = "Target_wFlatBkgRemoval";
  args.inhistname = "Target/hEnergyTime";
  args.inflatbkgname = "FlatBkg/hEnergyTime";
  RemoveFlatBkg(args);
}
