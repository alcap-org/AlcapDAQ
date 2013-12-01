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
  std::map<std::string, double> fBankToClockTickMap;

  //std::string GetBankName(std::string DetectorName){ };
  std::string GetDetectorName(std::string BankName) { return fBankToDetectorMap[BankName]; };
  double GetClockTick(std::string BankName) { return fBankToClockTickMap[BankName]; };

  static bool IsFADC(std::string BankName) { return BankName[0] == 'N'; } // if the first letter is N then the bank name is for a FADC
  static bool IsCAEN(std::string BankName) { return BankName[0] == 'C'; } // if the first letter is C then the bank name is for a CAEN

  ClassDef(TSetupData, 1)
};
#endif
