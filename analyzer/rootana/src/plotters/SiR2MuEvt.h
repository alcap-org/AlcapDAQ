#ifndef SiR2MuEvt_h__
#define SiR2MuEvt_h__

#include "BaseModule.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"

#include "TH2.h"
#include "TTree.h"

class TAnalysedPulse;
class SiR2MuEvt : public BaseModule{
 public:
  SiR2MuEvt(char *HistogramDirectoryName);
  SiR2MuEvt(modules::options* opts);
  ~SiR2MuEvt();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);

  TTree *mutree;
  double fE_muSc;
  double ft_muSc;
  std::vector<double> fE_SiR2_S;
  std::vector<double> ft_SiR2_S;
  std::vector<double> fE_SiR2_F;
  std::vector<double> ft_SiR2_F;

  bool IsMuonHit(TAnalysedPulse * muScPulse);
  double fCoinWindow;
  double fPileupProtectionTWindow;
  double fThreshold_muSc;
  double fTimeMargin;
};

#endif

