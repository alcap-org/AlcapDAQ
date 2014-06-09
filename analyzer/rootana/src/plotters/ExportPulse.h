#ifndef ExportPulse_H__
#define ExportPulse_H__

#include <string>
#include <set>
#include <map>
#include <iostream>

#include "definitions.h"
#include "FillHistBase.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "ModulesNavigator.h"

class TVAnalysedPulseGenerator;
class TPulseIsland;
#include "TAnalysedPulse.h"

class ExportPulse : public FillHistBase{
  typedef int TPulseIslandID;
  typedef int EventID_t;
  typedef std::set<TPulseIslandID> PulseIDList_t;
  typedef std::map<std::string,PulseIDList_t> ChannelPulseIDs_t;
  typedef std::map<EventID_t,PulseIDList_t> EventPulseIDList_t;
  typedef std::map<std::string,EventPulseIDList_t> EventChannelPulseIDs_t;

  struct PulseInfo_t{
	int pulseID;
	Long64_t event;
	std::string bankname;
	std::string detname;
	std::string MakeTPIName()const;
  };

 public:
  ExportPulse(modules::options* opts);
  ~ExportPulse();

  void AddToExportList(const std::string& detector,TPulseIslandID pulse_id);
  void AddToExportList(const std::string& detector,const TAnalysedPulse*);
  static ExportPulse* Instance();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
  virtual int BeforeFirstEntry(TGlobalData* gData,TSetupData *setup);
  //virtual int AfterLastEntry(TGlobalData* gData){return 0;};

  int DrawTPIs();
  int DrawTAPs();

  int PlotTPI(const TPulseIsland* pulse)const;
  int PlotTAP(const TAnalysedPulse* pulse)const;
  PulseIslandList* GetTPIsFromDetector(std::string bank="");

  void SetCurrentPulseID(const TPulseIslandID& id){fPulseInfo.pulseID=id;};
  void SetCurrentEventNumber(const Long64_t& num){fPulseInfo.event=num;};
  void SetCurrentDetectorName(const std::string& detector);

  TPulseIslandID GetCurrentPulseID()const{return fPulseInfo.pulseID;};
  Long64_t GetCurrentEventNumber()const{return fPulseInfo.event;};
  std::string GetCurrentDetectorName()const{return fPulseInfo.detname;};
  std::string GetCurrentBankName()const{return fPulseInfo.bankname;};
  std::string GetTPIPlotName()const{return fPulseInfo.MakeTPIName();};
  Long64_t GetTotalNumberOfEvents()const{return fTotalEvents;};
  void ClearPulsesToExport();

  void AddToConfigRequestList( EventID_t event_id, const std::string& detector,TPulseIslandID pulse_id);
  bool ParseEventRequest(std::string input, std::vector<EventID_t>& event_list);
  void ShowGuidance();
  bool ParsePulseRequest(std::string input, std::vector<EventID_t>& list);
  bool ParseRequest(std::string input, std::vector<EventID_t>& list,
		  const std::string& type, Long64_t lower_limit, Long64_t upper_limit);
  void LoadPulsesRequestedByConfig();

 private:

  bool fGuidanceShown;
  Long64_t fTotalEvents;
  ChannelPulseIDs_t fTPIsToPlot;
  StringConstAnalPulseMap fTAPsToPlot;
  EventChannelPulseIDs_t fRequestedByConfig;
  PulseInfo_t fPulseInfo;
  TSetupData* fSetup;
  modules::options* fOptions;

  TGlobalData* fGlobalData; // To be removed once Phill finishes the event navigator

};

inline ExportPulse* ExportPulse::Instance() {
  return modules::navigator::Instance()->GetModule<ExportPulse>("ExportPulse");
}

inline void ExportPulse::AddToExportList(const std::string& detector,TPulseIslandID pulse_id) {
  fTPIsToPlot[detector].insert(pulse_id);
}

inline void ExportPulse::AddToExportList(const std::string& detector,const TAnalysedPulse* pulse){
	std::cout<<"ExportPulse: Asked to draw a TAP for "<<detector<<std::endl;
	fTAPsToPlot[detector].push_back(pulse);
	AddToExportList(detector,pulse->GetPulseIslandID());
}

inline void ExportPulse::AddToConfigRequestList(EventID_t event_id, const std::string& detector,TPulseIslandID pulse_id) {
  fRequestedByConfig[detector][event_id].insert(pulse_id);
}

inline void ExportPulse::SetCurrentDetectorName(const std::string& detector){
	fPulseInfo.detname=detector;
        fPulseInfo.bankname=fSetup->GetBankName(detector);
      //fClockTick = fSetup->GetClockTick(bankname);
      //fTimeShift = fSetup->GetTimeShift(bankname);
}

#endif // ExportPulse_H__
