#ifndef PULSEVIEWER_H_
#define PULSEVIEWER_H_

#include "definitions.h"
#include "BaseModule.h"
class TGlobalData;
class TSetupData;
namespace modules {class options;}
class TFormula;

/// @brief Module to plot pulses meeting a certain criteria
/// @see https://github.com/alcap-org/AlcapDAQ/wiki/rootana-module-PulseViewer
class PulseViewer : public BaseModule{
  typedef int TPulseIslandID;
  typedef int TAnalysedPulseID;
  typedef int EventID_t;
  typedef std::map<TPulseIslandID,int> PulseIDList_t;
  typedef std::map<EventID_t,PulseIDList_t> EventPulseIDList_t;

  enum  TriggerType {kE, kG, kL, kGE, kLE};

  enum  ParameterType {
    kAmplitude,
    kTime,
    kIntegral,
    kTPILength,
    kEnergy,
    kPedestal,
    kTriggerTime
  };

 public:
  PulseViewer(modules::options* opts);
  ~PulseViewer();

 private:
  virtual int ProcessEntry(TGlobalData *gData, const TSetupData* gSetup);
  virtual int BeforeFirstEntry(TGlobalData* gData, const TSetupData* setup);
  virtual int AfterLastEntry(TGlobalData* gData, const TSetupData* setup);

  /// Get the channel as a string
  IDs::source GetSource()const{return fSource;};

  /// Ask ExportPulse to draw this pulse if it matches the trigger criteria
  /// @return 0 on success, non-zero otherwise
  int ConsiderDrawing(const TAnalysedPulseID& id, const TAnalysedPulse* i_pulse);

  /// Get the value of interest from pulse
  double GetParameterValue(const TAnalysedPulse& pulse,const ParameterType& parameter);

  /// Parse a trigger condition and set up the values needed to handle it
  /// @return 0 on success, non-zero otherwise
  int ParseTriggerString(const std::string& trigger_condition);

  /// Dump a summary of what was done
  bool SummarisePlots(){return fSummarize;};

 private:
  IDs::source fSource;
  std::string fTriggerCondition;
  std::string fRequestedSource;
  long int fTotalPlotted;
  long int fMaxToPlot;
  bool fSummarize, fStopAtMax;
  EventPulseIDList_t fPulsesPlotted;
  TFormula* fFormula;

  typedef std::map<std::string,ParameterType> ParameterKeys;
  static ParameterKeys fAvailableParams;
};

#endif //PULSEVIEWER_H_
