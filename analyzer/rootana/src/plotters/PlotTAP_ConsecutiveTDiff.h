#ifndef PlotTAP_ConsecutiveTDiff_h__
#define PlotTAP_ConsecutiveTDiff_h__

#include "BaseModule.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "TH2.h"

class PlotTAP_ConsecutiveTDiff : public BaseModule{
 public:
  PlotTAP_ConsecutiveTDiff(modules::options* opts);
  ~PlotTAP_ConsecutiveTDiff();

 private:
  virtual int ProcessEntry(TGlobalData *gData, const TSetupData* gSetup);
  virtual int AfterLastEntry(TGlobalData* gData,const TSetupData* setup){return 0;};
  virtual int BeforeFirstEntry(TGlobalData* gData,const TSetupData* setup){return 0;};
  std::map<std::string, TH1F*> fConsecutiveTDiffPlots;
  std::map<std::string, TH1F*> fConsecutiveTDiffZoomPlots;
  //  std::map<std::string, double> fLastTimes;
    
  std::map<std::string, TH1F*> fConsecutiveTDiffWAmpCutPlots;
  std::map<std::string, TH1F*> fConsecutiveTDiffZoomWAmpCutPlots;
  //  std::map<std::string, double> fLastTimesWAmpCut;
  std::map<std::string, TH2F*> fScatterPlots;
};

#endif

