#include "scripts/SiliconAnalyses/Utils/Unfold_FlatBackground.C"

void Si16b_Unfold_FlatBackground(std::string infilename, std::string outfilename) {

  Unfold_FlatBackgroundArgs args;
  args.infilename = infilename;
  std::string indirname = "SiL3_TimeSlice1000_5000";
  args.inhistname = indirname + "/hRawSpectrum";
  args.intreename = indirname + "/cuttree";
    
  args.corrfilename = args.infilename;
  args.corrhistname = "RawSpectrum_fromEnergyTime_FlatBkg/hRawSpectrum";
  args.corrtreename = "RawSpectrum_fromEnergyTime_FlatBkg/cuttree";

  args.outfilename = outfilename;
  args.outdirname = "FlatBackground";
  Unfold_FlatBackground(args);
}
