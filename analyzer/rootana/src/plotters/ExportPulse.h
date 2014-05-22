#ifndef ExportPulse_H__
#define ExportPulse_H__

#include <string>
#include <set>
#include <map>

#include "FillHistBase.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "ModulesNavigator.h"

class TVAnalysedPulseGenerator;
class TPulseIsland;
class TAnalysedPulse;

class ExportPulse : public FillHistBase{
  typedef std::vector<TPulseIsland*> PulseIslandList_t;
  typedef std::map<std::string, PulseIslandList_t > BankPulseList_t;
  typedef int PulseID_t;
  typedef std::set<PulseID_t> PulseIDList_t;
  typedef std::map<std::string,PulseIDList_t> ChannelPulseIDs_t;

  struct PulseInfo_t{
	  int ID;
	  std::string bankname;
	  std::string detname;
  };

 public:
  ExportPulse(modules::options* opts);
  ~ExportPulse();

  void AddToExportList(const std::string& detector,int pulse_id);
  static ExportPulse* Instance();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
  virtual int BeforeFirstEntry(TGlobalData* gData,TSetupData *setup);
  //virtual int AfterLastEntry(TGlobalData* gData){return 0;};

  int MakePlot(const TPulseIsland* pulse)const;
  PulseIslandList_t* GetPulsesFromDetector(std::string bank="");

  void SetCurrentPulseID(const PulseID_t& id){fPulseInfo.ID=id;};
  void SetCurrentDetectorName(const std::string& detector);

  PulseID_t GetCurrentPulseID()const{return fPulseInfo.ID;};
  std::string GetCurrentDetectorName()const{return fPulseInfo.detname;};
  std::string GetCurrentBankName()const{return fPulseInfo.bankname;};

 private:

  ChannelPulseIDs_t fPulsesToPlot;
  PulseInfo_t fPulseInfo;
  TSetupData* fSetup;

  TGlobalData* fGlobalData; // To be removed once Phill finishes the event navigator

};

inline ExportPulse* ExportPulse::Instance() {
  return modules::navigator::Instance()->GetModule<ExportPulse>("ExportPulse");
}

inline void ExportPulse::AddToExportList(const std::string& detector,int pulse_id) {
  fPulsesToPlot[detector].insert(pulse_id);
}

inline void ExportPulse::SetCurrentDetectorName(const std::string& detector){
	fPulseInfo.detname=detector;
        fPulseInfo.bankname=fSetup->GetBankName(detector);
}


#endif // ExportPulse_H__
