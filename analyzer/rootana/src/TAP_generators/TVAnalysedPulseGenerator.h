#ifndef TVANALYSEDPULSEGENERATOR_H__
#define TVANALYSEDPULSEGENERATOR_H__

#include "TSetupData.h"
#include <vector>
#include <string>

class TPulseIsland;
class TAnalysedPulse;

class TVAnalysedPulseGenerator {
 protected:
  typedef std::vector<TPulseIsland*> PulseIslandList;
  typedef std::vector<TAnalysedPulse*> AnalysedPulseList;
  typedef std::map<std::string, PulseIslandList > StringAnalPulseMap;

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
