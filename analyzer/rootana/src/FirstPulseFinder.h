#ifndef FIRSTPULSEFINDER_H_
#define FIRSTPULSEFINDER_H_

#include "TVPulseFinder.h"

class FirstPulseFinder : public TVPulseFinder {

 public:
  FirstPulseFinder():TVPulseFinder(){};
  virtual ~FirstPulseFinder(){};

  virtual PulseIslandList_t FindPulses(const PulseIslandList_t&);
};

#endif
