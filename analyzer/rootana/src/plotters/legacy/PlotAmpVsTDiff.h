#ifndef PlotAmpVsTDiff_h__
#define PlotAmpVsTDiff_h__

#include "BaseModule.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"

#include "TH2.h"

class PlotAmpVsTDiff : public BaseModule{
 public:
  PlotAmpVsTDiff(char *HistogramDirectoryName, std::string det_name_a, std::string det_name_b);
  PlotAmpVsTDiff(modules::options* opts);
  ~PlotAmpVsTDiff();

 private:
  virtual int ProcessEntry(TGlobalData *gData, const TSetupData* gSetup);
  virtual int AfterLastEntry(TGlobalData* gData,const TSetupData* setup){return 0;};
  virtual int BeforeFirstEntry(TGlobalData* gData,const TSetupData* setup){return 0;};

  TH2F* amp_vs_tdiff_plot_coarse;
  TH2F* amp_vs_tdiff_plot_fine;

  // Checking DetB against DetA
  std::string fDetNameA;
  std::string fDetNameB;
};

#endif

