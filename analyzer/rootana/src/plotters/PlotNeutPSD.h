#ifndef PLOTNEUTPSD_H_
#define PLOTNEUTPSD_H_

#include "BaseModule.h"
class TGloablData;
class TSetupData;
class TH1F;
class TH2F;
namespace modules {class options;}
#include "IdSource.h"


class PlotNeutPSD : public BaseModule{
  struct SourcePlots_t{
    IDs::source src;
    TH1F* ratio, *ratio_zoomed;
    TH2F* full_v_tail, *full_v_ratio;
  };
  typedef std::vector<SourcePlots_t> SourceList_t;

 public:
  PlotNeutPSD(modules::options* opts);
  ~PlotNeutPSD();

 private:
  virtual int ProcessEntry(TGlobalData *gData, const TSetupData *gSetup);

  virtual int BeforeFirstEntry(TGlobalData *gData, const TSetupData *gSetup);

  virtual int AfterLastEntry(TGlobalData *gData, const TSetupData *gSetup);

  SourceList_t fSourcesToPlot;
  IDs::source fSource;
};

#endif
