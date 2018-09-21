#include "scripts/SiliconAnalyses/Utils/Unfold_FlatBackground.C"

void Si16b_Unfold_FlatBackground(std::string infilename, std::string outfilename) {

  Unfold_FlatBackgroundArgs args;
  args.infilename = infilename;
  std::string indirname = "SiL3_all_TimeSlice1000_5000";
  args.inhistname = indirname + "/hRawSpectrum";
  args.corrfilename = args.infilename;
  args.corrhistname = indirname + "/hFlatBkg";

  args.outfilename = outfilename;
  args.outdirname = "FlatBackground";
  Unfold_FlatBackground(args);
}
