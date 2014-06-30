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
class TSimpleMuonEvent : public BaseModule{
 public:
  TSimpleMuonEvent(modules::options* opts);
  ~TSimpleMuonEvent();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
  virtual int BeforeFirstEntry(TGlobalData *gData, TSetupData * gSetup);

  AnalysedPulseList GetDetectorPulse(std::string detname);

  TTree *mutree;
  bool IsMuonHit(TAnalysedPulse * muScPulse);
  double fTimeWindow;
  double fCoinWindow;
  double fPileupProtectionWindow;
  double fThreshold_muSc;

  static std::map<std::string, std::string> fDetNames;

  double ft_muSc;
  double fE_muSc;
  Prepare_Detector(SiR2S)
  Prepare_Detector(SiL2S)

  Prepare_Detector(GeS)
  Prepare_Detector(SiR11S)
  Prepare_Detector(SiR12S)
  Prepare_Detector(SiR13S)
  Prepare_Detector(SiR14S)

  Prepare_Detector(SiL11S)
  Prepare_Detector(SiL12S)
  Prepare_Detector(SiL13S)
  Prepare_Detector(SiL14S)
};

std::map<std::string, std::string> TSimpleMuonEvent::fDetNames;
#undef Prepare_Detector
#endif /* end of include guard: TSIMPLEMUONEVENT_H_71S0XYEI */
