#include "scripts/SiliconAnalyses/Utils/RawSpectrum_fromEvdE.C"

void Si16b_RawSpectrum_fromEvdE(std::string infilename, std::string outfilename) {

  RawSpectrum_fromEvdEArgs args;
  args.datafilename = infilename;
  args.outfilename = outfilename;

  args.outdirname = "proton_combined";
  args.datacuttreename = "proton_SiR_timecut/cuttree";
  args.datahistnames.push_back("proton_SiR_timecut/hEvdE_TwoLayer_12not3");
  args.scale_ratios.push_back(1);
  args.datahistnames.push_back("proton_SiR_timecut/hEvdE_ThreeLayer_123");
  args.scale_ratios.push_back(1.0 / 0.64);
  RawSpectrum_fromEvdE(args);

  args.datahistnames.clear();
  args.scale_ratios.clear();
  args.outdirname = "proton";
  args.datacuttreename = "proton_SiR_timecut/cuttree";
  args.datahistnames.push_back("proton_SiR_timecut/hEvdE_TwoLayer_12not3");
  args.scale_ratios.push_back(1);
  RawSpectrum_fromEvdE(args);

  args.datahistnames.clear();
  args.scale_ratios.clear();
  args.outdirname = "proton_ThreeLayer";
  args.datacuttreename = "proton_SiR_timecut/cuttree";
  args.datahistnames.push_back("proton_SiR_timecut/hEvdE_ThreeLayer_123");
  args.scale_ratios.push_back(1);
  RawSpectrum_fromEvdE(args);
  
  args.datahistnames.clear();
  args.scale_ratios.clear();
  args.outdirname = "deuteron";
  args.datacuttreename = "deuteron_SiR_timecut/cuttree";
  args.datahistnames.push_back("deuteron_SiR_timecut/hEvdE_TwoLayer_12not3");
  args.scale_ratios.push_back(1);
  RawSpectrum_fromEvdE(args);

  args.datahistnames.clear();
  args.scale_ratios.clear();
  args.outdirname = "triton";
  args.datacuttreename = "triton_SiR_timecut/cuttree";
  args.datahistnames.push_back("triton_SiR_timecut/hEvdE_TwoLayer_12not3");
  args.scale_ratios.push_back(1);
  RawSpectrum_fromEvdE(args);

  args.datahistnames.clear();
  args.scale_ratios.clear();
  args.outdirname = "alpha";
  args.datacuttreename = "alpha_SiR_timecut/cuttree";
  args.datahistnames.push_back("alpha_SiR_timecut/hEvdE_TwoLayer_12not3");
  args.scale_ratios.push_back(1);
  RawSpectrum_fromEvdE(args);
}
