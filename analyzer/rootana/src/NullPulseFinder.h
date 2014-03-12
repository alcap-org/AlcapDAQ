#ifndef NullPulseFinder_H_
#define NullPulseFinder_H_

#include "TVPulseFinder.h"

class NullPulseFinder : public TVPulseFinder {

 public:
  NullPulseFinder():TVPulseFinder(){};
  virtual ~NullPulseFinder(){};

 private:
  virtual int CalculateTestValue();
  virtual bool PassesStartCondition(int val);
  virtual bool PassesStopCondition(int val);

};

#endif
