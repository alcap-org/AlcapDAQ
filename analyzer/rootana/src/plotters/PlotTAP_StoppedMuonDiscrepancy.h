#ifndef PlotTAP_StoppedMuonDiscrepancy_h__
#define PlotTAP_StoppedMuonDiscrepancy_h__

#include "BaseModule.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "IdChannel.h"
#include "TH2.h"

#include "TAnalysedPulse.h"

class PlotTAP_StoppedMuonDiscrepancy : public BaseModule{
 public:
  PlotTAP_StoppedMuonDiscrepancy(modules::options* opts);
  ~PlotTAP_StoppedMuonDiscrepancy();

 private:
  virtual int BeforeFirstEntry(TGlobalData *gData, const TSetupData *gSetup);
  virtual int AfterLastEntry(TGlobalData* gData,const TSetupData* setup){return 0;};
  virtual int ProcessEntry(TGlobalData *gData, const TSetupData *gSetup);

  typedef std::vector<IDs::channel> DetectorList;

  DetectorList fSiL1;
  IDs::channel* fSiL3;
  IDs::channel* fGeLoGain;

  TH1F* fCuts; // store the cuts so I can use them in a ROOT macro later
  TH1F* fSiL1_EnergyPlot_NoCuts;
  TH1F* fSiL3_EnergyPlot_NoCuts;
  TH1F* fGeLoGain_EnergyPlot_NoCuts;
  TH1F* fSiL3_EnergyPlot_wSiL1Coincidence;
  TH1F* fSiL1_EnergyPlot_wNoSiL3Coincidence;
  TH2F* fSiL3_GeLoGain_CoincidentEnergies;
  TH2F* fSiL1_GeLoGain_CoincidentEnergies;
  TH1F* fSiL3_SiL1_TDiff;
  TH1F* fSiL3_GeLoGain_TDiff;
  TH1F* fSiL1_GeLoGain_TDiff;

  void FindCoincidentPulses(const AnalysedPulseList* pulse_list_1, std::vector<const AnalysedPulseList*> multichannel_pulse_lists, AnalysedPulseList& coincident_pulses, double coinc_time_low, double coinc_time_high, bool anti_coincidence, TH1F* hTDiffToFill);
  void FindCoincidentPulses(const AnalysedPulseList* pulse_list_1, const AnalysedPulseList* pulse_list_2, AnalysedPulseList& coincident_pulses, double coinc_time_low, double coinc_time_high, bool anti_coincidence, TH1F* hTDiffToFill);

  double fCoincidenceTimeLow_SiL1_GeLoGain;
  double fCoincidenceTimeHigh_SiL1_GeLoGain;

  double fCoincidenceTimeLow_SiL3_GeLoGain;
  double fCoincidenceTimeHigh_SiL3_GeLoGain;

  double fCoincidenceTimeLow_SiL3_SiL1;
  double fCoincidenceTimeHigh_SiL3_SiL1;
};

#endif

