#ifndef TVANALYSEDPULSEGENERATOR_H__
#define TVANALYSEDPULSEGENERATOR_H__

#include <vector>
#include <string>
#include <stdexcept>
#include "definitions.h"
#include "TAPGeneratorOptions.h"
#include "TAnalysedPulse.h"

class TPulseIsland;

class TVAnalysedPulseGenerator {

 public:
  TVAnalysedPulseGenerator(const char* name, TAPGeneratorOptions* opts){
    if(opts){
      fDebug=opts->HasOption("debug");
    }
  };
  virtual ~TVAnalysedPulseGenerator(){};

 public:
  virtual int ProcessPulses(const PulseIslandList&, AnalysedPulseList&)=0;
  virtual bool MayDivideTPIs()=0;

  void SetPulseList(PulseIslandList* list){fPulseList=list;};
  TAnalysedPulse* MakeNewTAP(int parent_index)const;

  bool Debug()const{return fDebug;};

  void SetChannel(const std::string& det){fSource.Channel()=det;};
  IDs::channel GetChannel()const {return fSource.Channel();};
  IDs::source GetSource()const {return fSource;};

 private:
  IDs::source fSource;
  bool fDebug;
  PulseIslandList* fPulseList;
};

inline TAnalysedPulse* TVAnalysedPulseGenerator::MakeNewTAP(int parent_index)const{
  TAnalysedPulse* pulse=NULL;
  try{
      TPulseIsland* parent = fPulseList->at(parent_index);
      pulse=new TAnalysedPulse(GetSource(),parent_index,parent);
  } catch (std::out_of_range){}
  return pulse;
}

#endif // TVANALYSEDPULSEGENERATOR_H__
