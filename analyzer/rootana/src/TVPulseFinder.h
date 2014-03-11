#ifndef TVPULSEFINDER_H__
#define TVPULSEFINDER_H__

#include "TSetupData.h"
#include <vector>
#include <string>

class TPulseIsland;
class TAnalysedPulse;

class TVPulseFinder {
 protected:
  typedef std::vector<TPulseIsland*> PulseIslandList_t;
  typedef std::vector<TAnalysedPulse*> AnalysedPulseList_t;
  typedef std::map<std::string, PulseIslandList_t > BankPulseList_t;

 public:
  TVPulseFinder(){};
  virtual ~TVPulseFinder(){};

 public:
  virtual PulseIslandList_t FindPulses(const PulseIslandList_t&)=0;
};

#endif // TVPULSEFINDER_H__
