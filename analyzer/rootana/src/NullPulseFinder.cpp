#include "NullPulseFinder.h"

#include <iostream>
#include <cmath>
#include <sstream>

// CalculateTestValue()
// -- Calculates the test value that we will use to check against the start/stop conditions
// -- Returns the test value
int NullPulseFinder::CalculateTestValue() {

  // Return nothing, we're not going to use it anyway
  return 0;
}


// PassesStartCondition()
// -- Checks to see if the value given has passed the start condition
// -- Returns true if it does pass and false if it doesn't
bool NullPulseFinder::PassesStartCondition(int test_val) {

  // just want the whole island, so start straight away
  return true;
}


// PassesStopCondition()
// -- Checks to see if the value given has passed the stop condition
// -- Returns true if it does pass and false if it doesn't
bool NullPulseFinder::PassesStopCondition(int test_val) {

  // Just want the whole island, so don't tell the pulse finder to stop
  return false;
}
