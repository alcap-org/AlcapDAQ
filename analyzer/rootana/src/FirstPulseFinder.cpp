#include "FirstPulseFinder.h"

#include <iostream>
#include <cmath>
#include <sstream>

// CalculateTestValue()
// -- Calculates the test value that we will use to check against the start/stop conditions
// -- Returns the test value
int FirstPulseFinder::CalculateTestValue() {

  // Return the current height (taking into account pulse polarity)
  return fTriggerPolarity*(fCurrentSample - fPedestal);
}


// PassesStartCondition()
// -- Checks to see if the value given has passed the start condition
// -- Returns true if it does pass and false if it doesn't
bool FirstPulseFinder::PassesStartCondition(int test_val) {

  int RMS = 20; // hard-coded for the time being

  if (test_val > 0 + RMS)
    return true;
  else
    return false;
}


// PassesStopCondition()
// -- Checks to see if the value given has passed the stop condition
// -- Returns true if it does pass and false if it doesn't
bool FirstPulseFinder::PassesStopCondition(int test_val) {

  if (test_val < 0)
    return true;
  else
    return false;
}
