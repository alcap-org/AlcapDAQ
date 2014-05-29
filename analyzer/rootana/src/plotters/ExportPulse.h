#ifndef ExportPulse_H__
#define ExportPulse_H__

#include <string>
#include <set>
#include <map>
#include <iostream>

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
  typedef int EventID_t;
  typedef std::set<PulseID_t> PulseIDList_t;
  typedef std::map<std::string,PulseIDList_t> ChannelPulseIDs_t;
  typedef std::map<EventID_t,PulseIDList_t> EventPulseIDList_t;
  typedef std::map<std::string,EventPulseIDList_t> EventChannelPulseIDs_t;

  struct PulseInfo_t{
	  int ID;
	  std::string bankname;
	  std::string detname;
  };

 public:
  ExportPulse(modules::options* opts);
  ~ExportPulse();

  void AddToExportList(const std::string& detector,PulseID_t pulse_id);
  static ExportPulse* Instance();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
  virtual int BeforeFirstEntry(TGlobalData* gData,TSetupData *setup);
  //virtual int AfterLastEntry(TGlobalData* gData){return 0;};

  int MakePlot(const TPulseIsland* pulse)const;
  PulseIslandList_t* GetPulsesFromDetector(std::string bank="");

  void SetCurrentPulseID(const PulseID_t& id){fPulseInfo.ID=id;};
  void SetCurrentEventNumber(const Long64_t& num){fEventNumber=num;};
  void SetCurrentDetectorName(const std::string& detector);

  PulseID_t GetCurrentPulseID()const{return fPulseInfo.ID;};
  Long64_t GetCurrentEventNumber()const{return fEventNumber;};
  std::string GetCurrentDetectorName()const{return fPulseInfo.detname;};
  std::string GetCurrentBankName()const{return fPulseInfo.bankname;};
  Long64_t GetTotalNumberOfEvents()const{return fTotalEvents;};
  void ClearPulsesToExport();

  void AddToConfigRequestList( EventID_t event_id, const std::string& detector,PulseID_t pulse_id);
  bool ParseEventRequest(std::string input, std::vector<EventID_t>& event_list);
  void ShowGuidance();
  bool ParsePulseRequest(std::string input, std::vector<EventID_t>& list);
  bool ParseRequest(std::string input, std::vector<EventID_t>& list,
		  const std::string& type, Long64_t lower_limit, Long64_t upper_limit);
  void LoadPulsesRequestedByConfig();

 private:

  bool fGuidanceShown;
  Long64_t fEventNumber,fTotalEvents;
  ChannelPulseIDs_t fPulsesToPlot;
  EventChannelPulseIDs_t fRequestedByConfig;
  PulseInfo_t fPulseInfo;
  TSetupData* fSetup;
  modules::options* fOptions;

  TGlobalData* fGlobalData; // To be removed once Phill finishes the event navigator

};

inline ExportPulse* ExportPulse::Instance() {
  return modules::navigator::Instance()->GetModule<ExportPulse>("ExportPulse");
}

inline void ExportPulse::AddToExportList(const std::string& detector,PulseID_t pulse_id) {
	std::cout<<"Adding pulse for detector '"<<detector<<"' pulse no. '"<<pulse_id<<"'"<<std::endl;
  fPulsesToPlot[detector].insert(pulse_id);
}

inline void ExportPulse::AddToConfigRequestList(EventID_t event_id, const std::string& detector,PulseID_t pulse_id) {
  fRequestedByConfig[detector][event_id].insert(pulse_id);
}

inline void ExportPulse::SetCurrentDetectorName(const std::string& detector){
	fPulseInfo.detname=detector;
        fPulseInfo.bankname=fSetup->GetBankName(detector);
}

#endif // ExportPulse_H__