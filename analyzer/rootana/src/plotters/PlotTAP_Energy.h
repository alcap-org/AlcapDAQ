#ifndef PlotTAP_energy_h__
#define PlotTAP_energy_h__

#include "BaseModule.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"

class PlotTAP_Energy : public BaseModule{
 public:
  PlotTAP_Energy(modules::options* opts);
  ~PlotTAP_Energy();

 private:
  virtual int BeforeFirstEntry(TGlobalData *gData, const TSetupData *gSetup);
  virtual int AfterLastEntry(TGlobalData* gData,const TSetupData* setup){return 0;};
  virtual int ProcessEntry(TGlobalData *gData, const TSetupData *gSetup);

  std::map<std::string, TH1F*> fEnergyPlots;

  int fNBins;
  double fMinEnergy;
  double fMaxEnergy;
};

#endif

