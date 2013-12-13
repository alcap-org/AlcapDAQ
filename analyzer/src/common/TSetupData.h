#ifndef TSetupData_h__
#define TSetupData_h__

#include <vector>
#include <map>
#include <string>

#include <TObject.h>

/** This class holds all of the setup data for each run.
  */
class TSetupData : public TObject{
 private:
  /*         struct DetectorInfo{
                 int polarity;
                 int pedestal;
         };
  std::map<std::string, DetectorInfo> fBankToDetectorConfigs;
  */
 public:
  // Map of bank names to detector names
  std::map<std::string, std::string> fBankToDetectorMap;
  std::map<std::string, double> fBankToClockTickMap;
  std::map<std::string, int> fBankToBitMap;
  std::map<std::string, double> fBankToADCValueMap;
  std::map<std::string, double> fBankToTimeShift;
  std::map<std::string, int> fBankToPolarityMap;
  std::map<std::string, int> fBankToPedestalMap;

  std::string GetBankName(std::string DetectorName);
  
  std::string GetDetectorName(std::string BankName) { 
    if (fBankToDetectorMap[BankName] != "blank")
      return fBankToDetectorMap[BankName]; 
    else
      return BankName;
  };
  double GetClockTick(std::string BankName) { return fBankToClockTickMap[BankName]; };
  int GetNBits(std::string BankName) { return fBankToBitMap[BankName]; };
  double GetADCValue(std::string BankName) { return fBankToADCValueMap[BankName]; };
  double GetTimeShift(std::string BankName) { return fBankToTimeShift[BankName]; };
  int GetTriggerPolarity(std::string BankName){return fBankToPolarityMap[BankName];};
  int GetPedestal(std::string BankName){return fBankToPedestalMap[BankName];};

  void SetDetectorName(std::string BankName, std::string value) { fBankToDetectorMap[BankName]=value; };
  void SetClockTick(std::string BankName, double value) { fBankToClockTickMap[BankName]=value; };
  void SetNBits(std::string BankName, int value) { fBankToBitMap[BankName]=value; };
  void SetADCValue(std::string BankName, double value) { fBankToADCValueMap[BankName]=value; };
  void SetTimeShift(std::string BankName, double value) {fBankToTimeShift[BankName]=value; };
  void SetTriggerPolarity(std::string BankName, int value){fBankToPolarityMap[BankName]=value;};
  void SetPedestal(std::string BankName, int value){fBankToPedestalMap[BankName]=value;};

  static bool IsFADC(std::string BankName) { return BankName[0] == 'N'; } // if the first letter is N then the bank name is for a FADC
  static bool IsHoustonCAEN(std::string BankName) { return BankName.substr(2,2) == "UH"; } // if the first letter is C then the bank name is for a CAEN
  static bool IsBostonCAEN(std::string BankName) { return BankName.substr(2,2)  == "BU"; } // if the first letter is C then the bank name is for a CAEN

  ClassDef(TSetupData, 2)
};
#endif
