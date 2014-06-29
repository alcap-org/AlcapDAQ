#ifndef SiR2PlotsWTimecut_h__
#define SiR2PlotsWTimecut_h__

#include "BaseModule.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"

#include "TH2.h"
#include "TList.h"

class SiR2PlotsWTimecut : public BaseModule{
 public:
  SiR2PlotsWTimecut(modules::options* opts);
  ~SiR2PlotsWTimecut();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
  virtual int BeforeFirstEntry(TGlobalData *gData, TSetupData *gSetup);

  TH2F* amp_vs_tdiff_plot_coarse;
  TH2F* amp_vs_tdiff_plot_fine;

  TList *fOutputList;

  // Checking DetB against DetA
  std::string fDetNameA;
  std::string fDetNameB;
  double fTimeShiftCorrection;
  double fTimeMargin;
};

#endif

