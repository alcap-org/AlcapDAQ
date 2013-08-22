void AlCapFADCAnalysis() {

  gROOT->ProcessLine(".L libDataClasses.so");
  gROOT->ProcessLine(".L CalibrateFADC.C+");
  gROOT->ProcessLine(".L RemovePedestal.C+");
  gROOT->ProcessLine(".L Resolution.C+");
}
