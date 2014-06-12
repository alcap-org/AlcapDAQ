#ifndef PULSEVIEWER_H_
#define PULSEVIEWER_H_

#include "definitions.h"
#include "BaseModule.h"
class TGlobalData;
class TSetupData;
namespace modules {class options;}

/// @brief Module to plot pulses meeting a certain criteria
/// @see https://github.com/alcap-org/AlcapDAQ/wiki/rootana-module-PulseViewer
class PulseViewer : public BaseModule{
  enum  TriggerType {kE, kG, kL, kGE, kLE};

  enum  ParameterType {
    kAmplitude,
    kTime,
    kIntegral,
    kTPILength
  };

 public:
  PulseViewer(modules::options* opts);
  ~PulseViewer();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
  virtual int BeforeFirstEntry(TGlobalData* gData, TSetupData *setup);
  virtual int AfterLastEntry(TGlobalData* gData, TSetupData *setup);

  /* ------------ Methods called by ProcessEntry ---------*/
  /// Get the channel as a string
  std::string GetChannel()const{return fChannel.str();};

  /// Ask ExportPulse to draw this pulse if it matches the trigger criteria
  /// @return 0 on success, non-zero otherwise
  int ConsiderDrawing(const TAnalysedPulse* i_pulse);

  /// Check if value passes the trigger condition
  bool ValuePassesTrigger(const double& value);

  /// Get the value of interest from pulse
  double GetParameterValue(const TAnalysedPulse& pulse);

  /* ------------ Methods to configure this module ---------*/
  /// Parse a trigger condition and set up the values needed to handle it
  /// @return 0 on success, non-zero otherwise
  int ParseTriggerString(const std::string& trigger_condition);

  /// Set the equality test used in the trigger
  /// @return 0 on success, non-zero otherwise
  int SetTriggerType(const std::string& equality);

  /// Set the parameter to trigger against 
  /// @return 0 on success, non-zero otherwise
  int SetTriggerParameter(const std::string& parameter);

  /// Set the value to use as the trigger
  /// @return 0 on success, non-zero otherwise
  int SetTriggerValue(const std::string& parameter);

 private:
  IDs::channel fChannel;
  std::string fTriggerCondition;
  std::string fRequestedChannel;
  double fTriggerValue;
  TriggerType fTriggerType;
  ParameterType fTriggerParameter;
  std::string fParameterString;
  std::string fTypeString;
};

#endif //PULSEVIEWER_H_
