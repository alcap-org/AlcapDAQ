{
  gROOT->ProcessLine(".include ../src/common");
  gROOT->ProcessLine(".L ../src/common/TSetupData.cpp+");
  gROOT->ProcessLine(".L ../src/common/TPulseIsland.cpp+");
  gROOT->ProcessLine(".L ../src/common/TGlobalData.cpp+");
  gROOT->ProcessLine(".L pulse_viewer.C+");
}
