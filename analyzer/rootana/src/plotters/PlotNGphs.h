#ifndef PLOTNGPHS_H_
#define PLOTNGPHS_H_

#include "BaseModule.h"
class TGloablData;
class TSetupData;
class TH1F;
namespace modules {class options;}
#include "IdSource.h"

class PlotNGphs : public BaseModule {
  struct SourcePlots_t{
    IDs::source src;
    TH1F* Neut, NeutE, Gam, GamE;
  };
  typedef std::vector<SourcePlots_t> SourceList_t;

 public:
  PlotNGphs(modules::options*opts);
  ~PlotNGphs();

 private:

  double ftCutMin;
  double ftCutMax;

  virtual int ProcessEntry(TGlobalData *gData, const TSetupData *gSetup);
  virtual int BeforeFirstEntry(TGlobalData *gData, const TSetupData *gSetup);
  virtual int AfterLastEntry(TGlobalData *gData, const TSetupData *gSetup);

  SourceList_t fSourcesToPlot;
  IDs::source fSource;
};

#endif
