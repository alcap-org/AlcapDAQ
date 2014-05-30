#ifndef TVANALYSEDPULSEGENERATOR_H__
#define TVANALYSEDPULSEGENERATOR_H__

#include "TSetupData.h"
#include <vector>
#include <string>

class TPulseIsland;
class TAnalysedPulse;

class TVAnalysedPulseGenerator {
 protected:
  typedef std::vector<TPulseIsland*> PulseIslandList_t;
  typedef std::vector<TAnalysedPulse*> AnalysedPulseList_t;
  typedef std::map<std::string, PulseIslandList_t > BankPulseList_t;

 public:
  TVAnalysedPulseGenerator(){};
  virtual ~TVAnalysedPulseGenerator(){};

 public:
  virtual void ProcessPulses(const TSetupData*, const PulseIslandList_t&, AnalysedPulseList_t&)=0;
  void SetDetector(const std::string& det){fDetector=det;};
  const char* GetDetector(){return fDetector.c_str();};

 private:
  std::string fDetector;
};

#endif // TVANALYSEDPULSEGENERATOR_H__
