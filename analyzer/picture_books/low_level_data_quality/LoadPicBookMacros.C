

// This will load all the picture book macros that you will need
void LoadPicBookMacros() {
  
  // Load the functions individually first
  gROOT->ProcessLine(".L GenerateFADCPlots.C+");

  // Load the master function
  gROOT->ProcessLine(".L GenerateAllPlots.C+");
}
