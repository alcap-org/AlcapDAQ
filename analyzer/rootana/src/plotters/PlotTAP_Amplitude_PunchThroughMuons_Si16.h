#ifndef PlotTAP_Amplitude_PunchThroughMuons_Si16_h__
#define PlotTAP_Amplitude_PunchThroughMuons_Si16_h__

#include "BaseModule.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "IdChannel.h"

class PlotTAP_Amplitude_PunchThroughMuons_Si16 : public BaseModule{
 public:
  PlotTAP_Amplitude_PunchThroughMuons_Si16(modules::options* opts);
  ~PlotTAP_Amplitude_PunchThroughMuons_Si16();

 private:
  virtual int BeforeFirstEntry(TGlobalData *gData, const TSetupData *gSetup);
  virtual int AfterLastEntry(TGlobalData* gData,const TSetupData* setup){return 0;};
  virtual int ProcessEntry(TGlobalData *gData, const TSetupData *gSetup);

  std::map<std::string, TH1F*> fAmplitudePlots_PunchThrough;
  std::map<std::string, TH1F*> fAmplitudePlots_NotPunchThrough;

  typedef std::vector<IDs::channel> DetectorList;
  DetectorList fSiL1;
  IDs::channel* fSiL3;
};

#endif

