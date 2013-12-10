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
         struct DetectorInfo{
                 int polarity;
                 int pedestal;
	   bool isFast; // true if this is a fast pulse
         };
  std::map<std::string, DetectorInfo> fBankToDetectorConfigs;
 public:
  //(Quasi-)Singleton interface
  static TSetupData* Instance();

  // Map of bank names to detector names
  std::map<std::string, std::string> fBankToDetectorMap;
  std::map<std::string, double> fBankToClockTickMap;
  std::map<std::string, int> fBankToBitMap;
  std::map<std::string, double> fBankToADCValueMap;

  //std::string GetBankName(std::string DetectorName){ };
  std::string GetDetectorName(std::string BankName) { 
    if (fBankToDetectorMap[BankName] != "blank")
      return fBankToDetectorMap[BankName]; 
    else
      return BankName;
  };
  double GetClockTick(std::string BankName) { return fBankToClockTickMap[BankName]; };
  int GetNBits(std::string BankName) { return fBankToBitMap[BankName]; };
  double GetADCValue(std::string BankName) { return fBankToADCValueMap[BankName]; };
  int GetTriggerPolarity(std::string BankName){return fBankToDetectorConfigs[BankName].polarity;};
  int GetPedestal(std::string BankName){return fBankToDetectorConfigs[BankName].pedestal;};
  bool GetIsFast(std::string BankName) { return fBankToDetectorConfigs[BankName].isFast; }

  void SetDetectorName(std::string BankName, std::string value) { fBankToDetectorMap[BankName]=value; };
  void SetClockTick(std::string BankName, double value) { fBankToClockTickMap[BankName]=value; };
  void SetNBits(std::string BankName, int value) { fBankToBitMap[BankName]=value; };
  void SetADCValue(std::string BankName, double value) { fBankToADCValueMap[BankName]=value; };
  void SetTriggerPolarity(std::string BankName, int value){fBankToDetectorConfigs[BankName].polarity=value;};
  void SetPedestal(std::string BankName, int value){fBankToDetectorConfigs[BankName].pedestal=value;};
  void SetIsFast(std::string BankName, bool value) {fBankToDetectorConfigs[BankName].isFast=value;};

  static bool IsFADC(std::string BankName) { return BankName[0] == 'N'; } // if the first letter is N then the bank name is for a FADC
  static bool IsHoustonCAEN(std::string BankName) { return BankName.substr(2,2) == "UH"; } // if the first letter is C then the bank name is for a CAEN
  static bool IsBostonCAEN(std::string BankName) { return BankName.substr(2,2)  == "BU"; } // if the first letter is C then the bank name is for a CAEN

  ClassDef(TSetupData, 1)
};
#endif
