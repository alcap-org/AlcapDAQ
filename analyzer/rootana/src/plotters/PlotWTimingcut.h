#ifndef PlotWTimingcut_h__
#define PlotWTimingcut_h__

#include "BaseModule.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"

#include "TH2.h"

class PlotWTimingcut : public BaseModule{
 public:
  PlotWTimingcut(char *HistogramDirectoryName, std::string det_name_a, std::string det_name_b);
  PlotWTimingcut(modules::options* opts);
  ~PlotWTimingcut();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);

  TH2F* amp_vs_tdiff_plot_coarse;
  TH2F* amp_vs_tdiff_plot_fine;

  // Checking DetB against DetA
  std::string fDetNameA;
  std::string fDetNameB;
};

#endif

