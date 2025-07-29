// CK 07-29-25:
// CURRENTLY BROKEN (after gSetup added to TPulseIsland.cpp)
{
  gROOT->ProcessLine(".include ../src/common");
  gROOT->ProcessLine(".include ../src/Calibration");
  //gROOT->ProcessLine(".include ../../midas/include");
  //gROOT->ProcessLine(".include ../work/full_analysis_first_CF/"); // update alcapana work dir as needed
  gROOT->ProcessLine(".L ../src/common/TSetupData.cpp+");
  //gROOT->ProcessLine(".L ../src/common/analyzer.cpp+");
  gROOT->ProcessLine(".L ../src/common/TPulseIsland.cpp+");
  gROOT->ProcessLine(".L ../src/common/TGlobalData.cpp+");
  gROOT->ProcessLine(".L pulse_viewer.C+");
}
