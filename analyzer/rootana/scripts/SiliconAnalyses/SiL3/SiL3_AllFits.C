#include "scripts/SiliconAnalyses/SiL3/fits/SiL3_FlatBkgFits.C"
#include "scripts/SiliconAnalyses/SiL3/fits/SiL3_LifetimeRooFit.C"

#include <sstream>

void SiL3_AllFits() {

  //  std::string infilename = "~/data/results/SiL3/raw_spectra.root";
  std::string infilename = "~/data/results/SiL3/plots.root";
  std::string outfilename = "~/data/results/SiL3/fits.root";
    
  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");
  outfile->Write();
  outfile->Close();
  
  //  SiL3_FlatBkgFits(infilename, outfilename);
  SiL3_LifetimeRooFit(infilename, outfilename);
}
