#ifndef TRIGGERPLOTTER_H_
#define TRIGGERPLOTTER_H_

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

  std::string GetChannel()const{return fChannel;};

 private:
 AnalysedPulseList* fAPList;
  ID::channel fChannel;
  std::string fTriggerCondition;
  double fTriggerValue;
  TriggerType fTriggerType;
  ParameterType fTriggerParameter;
};

#endif //TRIGGERPLOTTER_H_
