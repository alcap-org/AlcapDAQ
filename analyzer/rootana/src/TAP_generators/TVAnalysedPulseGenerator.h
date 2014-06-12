#ifndef TVANALYSEDPULSEGENERATOR_H__
#define TVANALYSEDPULSEGENERATOR_H__

#include "TSetupData.h"
#include <vector>
#include <string>
#include "definitions.h"
#include "TAPGeneratorOptions.h"

class TPulseIsland;
class TAnalysedPulse;

class TVAnalysedPulseGenerator {

 public:
  TVAnalysedPulseGenerator(TAPGeneratorOptions* opts){
    if(opts){
      fDebug=opts->HasOption("debug");
    }
  };

  virtual ~TVAnalysedPulseGenerator(){};

 public:
  virtual int ProcessPulses(const PulseIslandList&, AnalysedPulseList&)=0;
  virtual bool MayDivideTPIs()=0;

  void SetChannel(const std::string& det){fDetector=det;};
  const char* GetChannel(){return fDetector.c_str();};
  bool Debug()const{return fDebug;};

 private:
  std::string fDetector;
  bool fDebug;

};

#endif // TVANALYSEDPULSEGENERATOR_H__
