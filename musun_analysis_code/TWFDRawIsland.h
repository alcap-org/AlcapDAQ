#ifndef TWFDRawIsland_h
#define TWFDRawIsland_h

#include <vector>
#include "Rtypes.h"

/** Timestamp and vector of adc samples. No frills.
  * The weird class name is because a TWfdIsland is already
  * defined, but it's specific for gondolas islands.
  */
class TWFDRawIsland {
  public:
  Int_t fTime;
  std::vector<UChar_t> fADC;
};

#endif
