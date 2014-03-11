#include "FirstPulseFinder.h"

#include <iostream>
#include <cmath>
#include <sstream>

#include "TH1.h"

// PassesSanityChecks()
// -- Checks if the pulse island passes the following sanity checks:
//      1. overflowed the digitizer (i.e. any sample is >= 2^n, where n is the number of bits in the digitiser)
// -- Returns: true if the pulse island passes and false if it fails
bool FirstPulseFinder::PassesSanityChecks(const TPulseIsland* island) {

  //////////////////
  // Sanity Check 1
  // See if any samples are above the max digitisation value
  int max_digitised_value = std::pow(2, fNBits);
  std::vector<int> theSamples = island->GetSamples();

  for (std::vector<int>::iterator sampleIter = theSamples.begin(); sampleIter != theSamples.end(); ++sampleIter) {
    if (*sampleIter >= max_digitised_value) {
      std::cout << "Pulse #" << fPulseCounter << ": has a sample with value " << *sampleIter << " which is greater than or equal to the maximum digitised value of " 
		<< max_digitised_value << " and so will be ignored." << std::endl;
      return false;
    }
  }
  // End Sanity Check 1
  ///////////////////////////////////

  return true; // passed everything
}

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
