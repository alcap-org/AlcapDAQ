#ifndef FirstPulseFinder_H_
#define FirstPusleFinder_H_

#include "TVPulseFinder.h"

class FirstPulseFinder : public TVPulseFinder {

 public:
  FirstPulseFinder():TVPulseFinder(){};
  virtual ~FirstPulseFinder(){};

 private:
  virtual int CalculateTestValue();
  virtual bool PassesStartCondition(int val);
  virtual bool PassesStopCondition(int val);

};

#endif
