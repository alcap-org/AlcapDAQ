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
            fSource.Generator()=IDs::generator(name,opts->StringDescription());
      }
  virtual ~TVDetectorPulseGenerator(){}

 public:
  virtual int ProcessPulses( DetectorPulseList& output)=0;
  virtual bool ChecksForPileUp()const =0;

  void SetPulseSources(const IDs::source& fast, const IDs::source& slow){
      fFastSource=fast;
      fSlowSource=slow;
  }
  void SetPulseLists(const AnalysedPulseList* fast,const AnalysedPulseList* slow){
      fFastPulses=fast;
      fSlowPulses=slow;
  }

  void SetDebug(const bool& val=true){fDebug=val;};
  bool Debug(){return fDebug;};

  TDetectorPulse* MakeTDP(int fast_parent, int slow_parent)const{
      TDetectorPulse* pulse=NULL;
      const TAnalysedPulse* fast=NULL;
      if(fFastPulses && fast_parent>=0) fast=fFastPulses->at(fast_parent);
      const TAnalysedPulse* slow=NULL;
      if(fSlowPulses && slow_parent>=0) slow=fSlowPulses->at(slow_parent);
      if(fast || slow) {
          pulse=new TDetectorPulse(GetSource(),fast_parent,fast,slow_parent,slow);
      }
      return pulse;
  }
  IDs::source GetSource()const {return fSource;};
  IDs::source GetFastSource()const {return fFastSource;};
  IDs::source GetSlowSource()const {return fSlowSource;};
  void SetChannel(const IDs::channel& ch){fSource.Channel()=ch;};
  IDs::generator GetGeneratorID()const{return fSource.Generator();}

  const AnalysedPulseList* FastPulseList()const{return fFastPulses;}
  const AnalysedPulseList* SlowPulseList()const{return fSlowPulses;}
  const AnalysedPulseList* PulseList(bool getFast)const{return getFast?fFastPulses:fSlowPulses;}

 private:
  bool fDebug;
  IDs::source fFastSource, fSlowSource, fSource;
  const AnalysedPulseList *fFastPulses, *fSlowPulses;
};

#endif // TVDETECTORPULSEGENERATOR_H__
