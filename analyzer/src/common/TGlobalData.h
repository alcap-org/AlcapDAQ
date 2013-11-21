#ifndef TGlobalData_h__
#define TGlobalData_h__

#include <vector>
#include <map>
#include <string>

#include <TObject.h>

#include "TPulseIsland.h"

/** This class holds all of the data created by analysis modules.
  */
class TGlobalData : public TObject{
 public:
  // Map of bank names to vectors of islands.
  std::map<std::string, std::vector<TPulseIsland*> >
    fPulseIslandToChannelMap;

  // Map of bank names to detector names
  std::map<std::string, std::string> fBankToDetectorMap;

  //std::string GetBankName(std::string DetectorName){ };
  //std::string GetDetectorName(std::string BankName){ };

  ClassDef(TGlobalData, 2)
};
#endif
