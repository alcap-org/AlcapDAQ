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
  virtual void ProcessPulses(const TSetupData*, const PulseIslandList&, AnalysedPulseList&)=0;
  void SetDetector(const std::string& det){fDetector=det;};
  const char* GetDetector(){return fDetector.c_str();};
  bool Debug()const{return fDebug;};

 private:
  std::string fDetector;
  bool fDebug;

};

#endif // TVANALYSEDPULSEGENERATOR_H__
