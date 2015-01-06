#ifndef PlotAmpVsTDiff_h__
#define PlotAmpVsTDiff_h__

#include "FillHistBase.h"
#include "TGlobalData.h"
#include "TSetupData.h"

#include "TH2.h"
#include "TH1.h"

class PlotAmpVsTDiff : public FillHistBase{
 public:
  PlotAmpVsTDiff(char *HistogramDirectoryName, std::string det_name_a, std::string det_name_b);
  ~PlotAmpVsTDiff();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);

  TH2F* amp_vs_tdiff_plot_coarse;
  TH2F* amp_vs_tdiff_plot_fine;
  TH1F* oned_tdiff_plot;

  // Checking DetB against DetA
  std::string fDetNameA;
  std::string fDetNameB;
};

#endif

