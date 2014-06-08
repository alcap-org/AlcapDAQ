#ifndef TRIGGERPLOTTER_H_
#define TRIGGERPLOTTER_H_

#include "definitions.h"
#include "FillHistBase.h"
class TGlobalData;
class TSetupData;
namespace modules {class options;}

class TriggerPlotter : public FillHistBase{
  enum  TriggerType {kE, kG, kL, kGE, kLE};

  enum  ParameterType {
  	kAmplitude,
  	kTime,
  	kIntegral,
  	kTPILength
  	};

 public:
  TriggerPlotter(modules::options* opts);
  ~TriggerPlotter();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
  virtual int BeforeFirstEntry(TGlobalData* gData, TSetupData *setup);
  virtual int AfterLastEntry(TGlobalData* gData, TSetupData *setup);

  std::string GetChannel()const{return fChannel.str();};
  int ShouldDraw(const TAnalysedPulse* i_pulse);
  bool ValuePassesTrigger(const double& value);
  double GetParameterValue(const TAnalysedPulse& pulse);

  int ParseTriggerString();
  int SetTriggerType(const std::string& equality);
  int SetTriggerParameter(const std::string& parameter);
  int SetTriggerValue(const std::string& parameter);

 private:
 AnalysedPulseList* fAPList;
  IDs::channel fChannel;
  std::string fTriggerCondition;
  std::string fRequestedChannel;
  double fTriggerValue;
  TriggerType fTriggerType;
  ParameterType fTriggerParameter;
  std::string fParameterString;
  std::string fTypeString;
};

#endif //TRIGGERPLOTTER_H_
