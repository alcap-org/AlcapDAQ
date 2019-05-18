#ifndef TSIMPLEMUONEVENT_H_71S0XYEI
#define TSIMPLEMUONEVENT_H_71S0XYEI

#include "BaseModule.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "definitions.h"

#include "TH2.h"
#include "TTree.h"

#define Prepare_Detector(det)\
  std::vector<double> fE_##det;\
  std::vector<double> ft_##det;


class TAnalysedPulse;
class SimpleMuonEvent : public BaseModule{
 public:
  SimpleMuonEvent(modules::options* opts);
  ~SimpleMuonEvent();

 private:
  virtual int ProcessEntry(TGlobalData *gData, const TSetupData *gSetup);
  virtual int BeforeFirstEntry(TGlobalData *gData, const TSetupData * gSetup);
  virtual int AfterLastEntry(TGlobalData* gData, const TSetupData* setup){
    return 0;
  }

  AnalysedPulseList GetDetectorPulse(std::string detname);

  TTree *mutree;
  bool IsMuonHit(const TAnalysedPulse * TScPulse);
  double fTimeWindow;
  double fCoinWindow;
  double fPileupProtectionWindow;
  double fThreshold_TSc;
  bool fDebug;

  static std::map<std::string, std::string> fDetNames;

  double ft_TSc;
  double fE_TSc;

  Prepare_Detector(LaBr3)
  Prepare_Detector(GeCHEL)
  Prepare_Detector(GeCHEH)
};

std::map<std::string, std::string> SimpleMuonEvent::fDetNames;
#undef Prepare_Detector
#endif /* end of include guard: TSIMPLEMUONEVENT_H_71S0XYEI */
