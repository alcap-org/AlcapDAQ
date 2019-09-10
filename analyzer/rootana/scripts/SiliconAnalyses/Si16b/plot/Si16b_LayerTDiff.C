#include "scripts/SiliconAnalyses/Utils/LayerTDiff.C"

void Si16b_LayerTDiff(std::string infilename, std::string outfilename) {

  // For a possible active target analysis
  LayerTDiffArgs args;
  args.infilename = infilename;
  args.outfilename = outfilename;
  args.treename = "siBlockTree_SiR";
  args.layer1name = "thin";
  args.layer2name = "thick";

  args.min_tdiff = -2000;
  args.max_tdiff = 2000;
  args.tdiff_width = 10;
  
  args.outdirname = "SiR_LayerTDiff";
  LayerTDiff(args);
}
