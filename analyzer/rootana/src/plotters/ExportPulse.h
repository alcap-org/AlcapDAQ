#ifndef ExportPulse_H__
#define ExportPulse_H__

#include <string>
#include <set>
#include <map>
#include <iostream>

#include "definitions.h"
#include "BaseModule.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "ModulesNavigator.h"
#include "PulseCandidateFinder_TSpectrum.h"

class TDirectory;
class TVAnalysedPulseGenerator;
class TPulseIsland;
#include "TAnalysedPulse.h"

/// Class to handle drawing of a given pulse.
/// @see https://github.com/alcap-org/AlcapDAQ/wiki/rootana_module_ExportPulses
class ExportPulse : public BaseModule{
  typedef int TPulseIslandID;
  typedef int EventID_t;
  typedef std::set<TPulseIslandID> PulseIDList_t;
  typedef std::vector< std::pair<std::string,TPulseIslandID> > ChannelPulseIDs_t;
  typedef std::map<EventID_t,PulseIDList_t> EventPulseIDList_t;
  typedef std::map<std::string,EventPulseIDList_t> EventChannelPulseIDs_t;
  typedef std::set<const TAnalysedPulse*> TAPList_t;
  typedef std::vector< std::pair<std::string,const TAnalysedPulse*> > ChannelTAPs_t;

  /// Information needed to make one plot.
  /// In future this may be moved elsewhere to be used by other modules
  struct PulseInfo_t{
    int pulseID;
    int subPulseID;
    Long64_t event;
    std::string bankname;
    std::string detname;
    /// Function to convert the details in this struct
    /// into a string used for naming plots
    std::string MakeTPIName()const;
    std::string MakeTAPName()const;
  };

  enum XAxis {kXAxisError=-1, kSampleNumber=0, kPulseTime=1, kBlockTime=2};
  enum YAxis {kYAxisError=-1, kNotPedestalSubtracted=0, kPedestalSubtracted=1};

 public:
  ExportPulse(modules::options* opts);
  ~ExportPulse();

  /// @brief Log a request to draw a given TPI
  ///
  /// @param channel Name of channel to draw TPI from
  /// @param pulse_id TPI location in list
  void AddToExportList(const std::string& channel,TPulseIslandID pulse_id);

  /// @brief Add TAP to draw.  Also draws the TAPs parent TPI.
  ///
  /// @param channel Name of channel that produced this pulse
  /// @param pulse Pulse to be drawn
  void AddToExportList(const TAnalysedPulse* pulse);

  /// Static method to get the instance of ExportPulse. 
  /// @warning This method returns NULL if the ExportPulse module is not
  /// requested by the user through the modules file.  Always check the returned
  /// pointer is not NULL before using it:
  /// @code
  /// if(ExportPulse::Instance()){
  ///    // now we know it's safe, you can do something with this module
  //     ExportPulse::Instance()->AddToExportList(channel,0);
  /// }
  /// @endcode
  /// @return NULL if the module was not requested in the modules file
  static ExportPulse* Instance();

 private:
  /// ExportPulse uses this method to process the config 
  /// file for any specifically requested pulses
  virtual int BeforeFirstEntry(TGlobalData* gData, const TSetupData* setup);

  /// @brief Plot all pulses that we were asked to draw for this event.
  /// @details First loads pulses requested by the config file, then draws all
  /// TPIs, then draw all TAPs.
  virtual int ProcessEntry(TGlobalData *gData, const TSetupData* gSetup);
  virtual int AfterLastEntry(TGlobalData* gData,const TSetupData* setup){return 0;};

  /// Draw all TPIs requested for this event
  int DrawTPIs();

  /// Draw all TAPs requested for this event
  int DrawTAPs();

  /// Plot a single TPI
  int PlotTPI(const TPulseIsland* pulse, const PulseInfo_t& info);

  TH1F* MakeHistTPI(const TPulseIsland* pulse, const std::string& name, const PulseInfo_t& info, int shift=0, int samples=0)const;

  /// Plot a single TAP
  int PlotTAP(const TAnalysedPulse* pulse, const PulseInfo_t& info)const;

  TH1F* MakeHistTAP(TH1F* tpi_hist, const TAnalysedPulse* pulse, const PulseInfo_t& info)const;
  /// Get a pointer to the list of TPIs for a given detector
  PulseIslandList* GetTPIsFromDetector(std::string bank="");

  void SetCurrentPulseID(const TPulseIslandID& id){fPulseInfo.pulseID=id;};
  void SetCurrentSubPulseID(const int& id){fPulseInfo.subPulseID=id;};
  void SetCurrentEventNumber(const Long64_t& num){fPulseInfo.event=num;};
  void SetCurrentDetectorName(const std::string& detector);

  Long64_t GetCurrentEventNumber()const{return fPulseInfo.event;};
  std::string GetCurrentBankName()const{return fPulseInfo.bankname;};
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
  double fClockTick;
  double fTimeShift;
  ChannelPulseIDs_t fTPIsToPlot;
  ChannelTAPs_t fTAPsToPlot;
  EventChannelPulseIDs_t fRequestedByConfig;
  PulseInfo_t fPulseInfo;
  const TSetupData* fSetup;
  modules::options* fOptions;
  bool fUsePCF;
  PulseCandidateFinder_TSpectrum* fPulseFinder_TSpectrum;
  PulseIslandList fSubPulses;
  TDirectory* fTPIDirectory;
  TDirectory* fTAPDirectory;

  TGlobalData* fGlobalData; // To be removed once Phill finishes the event navigator

  XAxis fXAxis;
  YAxis fYAxis;

};

inline ExportPulse* ExportPulse::Instance() {
  return modules::navigator::Instance()->GetModule<ExportPulse>("ExportPulse");
}

inline void ExportPulse::AddToExportList(const std::string& detector,TPulseIslandID pulse_id) {
  std::pair<std::string, TPulseIslandID> new_pair(detector, pulse_id);

  // Check we haven't already added this
  bool already_added = false;
  for (ChannelPulseIDs_t::const_iterator i_tpi_to_plot = fTPIsToPlot.begin(); i_tpi_to_plot != fTPIsToPlot.end(); ++i_tpi_to_plot) {
    if (i_tpi_to_plot->first == new_pair.first && i_tpi_to_plot->second == new_pair.second) {
      already_added = true;
      break;
    }
  }
  if (!already_added) {
    fTPIsToPlot.push_back(new_pair);
  }
}

inline void ExportPulse::AddToExportList(const TAnalysedPulse* pulse){
  std::string channel=pulse->GetSource().Channel().str();
  //  std::cout<<"ExportPulse: Asked to draw a TAP for "<<channel<<std::endl;

  std::pair<std::string, const TAnalysedPulse*> new_pair(channel, pulse);
  fTAPsToPlot.push_back(new_pair);
  AddToExportList(channel,pulse->GetParentID());
}

inline void ExportPulse::AddToConfigRequestList(EventID_t event_id, const std::string& detector,TPulseIslandID pulse_id) {
  fRequestedByConfig[detector][event_id].insert(pulse_id);
}

inline void ExportPulse::SetCurrentDetectorName(const std::string& detector){
  fPulseInfo.detname=detector;
  fPulseInfo.bankname=fSetup->GetBankName(detector);
  fClockTick = TSetupData::Instance()->GetClockTick(fPulseInfo.bankname);
  fTimeShift = TSetupData::Instance()->GetTimeShift(fPulseInfo.bankname);
  if(fPulseFinder_TSpectrum) fPulseFinder_TSpectrum->SetChannel(detector);
}

#endif // ExportPulse_H__
