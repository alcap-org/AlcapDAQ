#ifndef TSetupData_h__
#define TSetupData_h__

#include <vector>
#include <map>
#include <string>

#include <TObject.h>
#include "TPulseIsland.h"

/** This class holds all of the setup information
  */
class TSetupData : public TObject{
 public:
  // Map of detector to bank names.
  std::map<std::string, std::string> fBankToDetectorMap;

  ClassDef(TSetupData, 2)
};
#endif
