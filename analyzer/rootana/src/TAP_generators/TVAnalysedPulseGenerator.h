#ifndef TVANALYSEDPULSEGENERATOR_H__
#define TVANALYSEDPULSEGENERATOR_H__

#include "TSetupData.h"
#include <vector>
#include <string>
#include "definitions.h"

class TPulseIsland;
class TAnalysedPulse;

class TVAnalysedPulseGenerator {
 protected:

 public:
  TVAnalysedPulseGenerator(){};
  virtual ~TVAnalysedPulseGenerator(){};

 public:
  virtual void ProcessPulses(const TSetupData*, const PulseIslandList&, AnalysedPulseList&)=0;
  void SetDetector(const std::string& det){fDetector=det;};
  const char* GetDetector(){return fDetector.c_str();};

 private:
  std::string fDetector;
};

#endif // TVANALYSEDPULSEGENERATOR_H__
