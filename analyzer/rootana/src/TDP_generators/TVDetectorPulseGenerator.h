#ifndef TVDETECTORPULSEGENERATOR_H__
#define TVDETECTORPULSEGENERATOR_H__

#include "TSetupData.h"
#include "TAnalysedPulse.h"
#include "TDetectorPulse.h"
#include "TDPGeneratorOptions.h"
#include "definitions.h"

class TVDetectorPulseGenerator {
 public:

 public:
  TVDetectorPulseGenerator(const char* name, TDPGeneratorOptions* opts):
      fDebug(false),fFastPulses(NULL),fSlowPulses(NULL){
            fSource.Generator()=IDs::generator(name);//,opts->ToOneString());
      }
  virtual ~TVDetectorPulseGenerator(){}

 public:
  virtual int ProcessPulses(const TSetupData* setup,const std::string& detector, 
		  const AnalysedPulseList* fast, const AnalysedPulseList* slow,
		  DetectorPulseList& output)=0;
  virtual bool ChecksForPileUp()const =0;

  void SetPulseSources(const IDs::source& fast, const IDs::source& slow){
      fFastSource=fast;
      fSlowSource=slow;
  }
  void SetPulseLists(AnalysedPulseList* fast, AnalysedPulseList* slow){
      fFastPulses=fast;
      fSlowPulses=slow;
  }

  void SetDebug(const bool& val=true){fDebug=val;};
  bool Debug(){return fDebug;};

  TDetectorPulse* MakeTDP(int fast_parent, int slow_parent)const{
      TDetectorPulse* pulse=NULL;
      TAnalysedPulse* fast=fFastPulses?fFastPulses->at(fast_parent):NULL;
      TAnalysedPulse* slow=fSlowPulses?fSlowPulses->at(slow_parent):NULL;
      if(fast || slow) {
          pulse=new TDetectorPulse(GetSource(),fast_parent,fast,slow_parent,slow);
      }
      return pulse;
  }
  IDs::source GetSource()const {return fSource;};
  void SetChannel(const std::string& det){fSource.Channel()=det;};

 private:
  bool fDebug;
  IDs::source fFastSource, fSlowSource, fSource;
  AnalysedPulseList *fFastPulses, *fSlowPulses;
};

#endif // TVDETECTORPULSEGENERATOR_H__
