#ifndef TSetupData_h__
#define TSetupData_h__

#include <vector>
#include <map>
#include <string>

#include <TObject.h>
#include "TPulseIsland.h"

/** This class holds all of the setup data for each run.
  */
class TSetupData : public TObject{
 public:
  // Map of bank names to detector names
  std::map<std::string, std::string> fBankToDetectorMap;

  //std::string GetBankName(std::string DetectorName){ };
  //std::string GetDetectorName(std::string BankName){ };

  ClassDef(TSetupData, 1)
};
#endif
