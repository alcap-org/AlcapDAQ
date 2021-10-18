#ifndef SiL3_Unfold_FinalNormalisation_
#define SiL3_Unfold_FinalNormalisation_

#include "../../Utils/Unfold_FinalNormalisation.C"

void SiL3_Unfold_FinalNormalisation(std::string infilename, std::string inhistname, std::string countfilename, std::string counttreename, std::string outfilename, std::string outdirname) {

  Unfold_FinalNormalisationArgs args;
  args.infilename = infilename;
  args.inhistname = inhistname;


  args.outfilename = outfilename;
  
  args.countfilename = countfilename;
  args.counttreename = counttreename;
  args.capture_fraction = 0.658;

  args.outdirname = outdirname + "_100keVBins";
  args.rebin_factor = 2;
  Unfold_FinalNormalisation(args);

  args.outdirname = outdirname + "_200keVBins";
  args.rebin_factor = 4;
  Unfold_FinalNormalisation(args);

  args.outdirname = outdirname + "_500keVBins";
  args.rebin_factor = 10;
  Unfold_FinalNormalisation(args);
}

#endif
