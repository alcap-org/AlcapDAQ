// LoadAnalysisFunctions.C
// -- Load the functions you want to use

void LoadAnalysisFunctions() {

  gROOT->ProcessLine(".L libDataClasses.so");

  gROOT->ProcessLine(".L GetPulseParameters.C+");
  gROOT->ProcessLine(".L Calibrate.C+");
  gROOT->ProcessLine(".L RemovePedestal.C+");

  gROOT->ProcessLine(".L Resolution.C+");
}
