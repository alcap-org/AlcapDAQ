#ifndef TSetupData_h__
#define TSetupData_h__

#include <vector>
#include <map>
#include <set>
#include <string>

#include <TObject.h>

/** This class holds all of the setup data for each run.
  */
class TSetupData : public TObject{
 private:
  /*         struct DetectorInfo{
                 int polarity;
                 int pedestal;
	   double adcSlopeCalib;
	   double adcOffsetCalib;
	   bool isFast; // true if this is a fast pulse
         };
  std::map<std::string, DetectorInfo> fBankToDetectorConfigs;
  */
 public:
  //(Quasi-)Singleton interface
  static TSetupData* Instance();

  // Map of bank names to detector names
  std::map<std::string, std::string> fBankToDetectorMap;
  std::map<std::string, double> fBankToClockTickMap;
  std::map<std::string, int> fBankToBitMap;
  std::map<std::string, double> fBankToADCSlopeCalibMap;
  std::map<std::string, double> fBankToADCOffsetCalibMap;
  std::map<std::string, double> fBankToTimeShift;
  std::map<std::string, int> fBankToPolarityMap;
  std::map<std::string, int> fBankToPedestalMap;
	std::map<std::string, bool> fBankToEnableBitMap;

  std::string GetBankName(std::string DetectorName);

  std::string GetDetectorName(std::string BankName)const { 
    if (fBankToDetectorMap.find(BankName)->second!= "blank")
      return fBankToDetectorMap.find(BankName)->second; 
    else
      return BankName;
  };

  // Fill a map of all detectors that were used to their banks
  void GetAllDetectors(std::map<std::string,std::string>& detectors)const;

  // Getters
  double GetClockTick(const std::string& BankName) const{ return GetValue(fBankToClockTickMap,BankName);}
  int GetNBits(const std::string& BankName) const{ return GetValue(fBankToBitMap,BankName);}
  double GetADCSlopeCalib(const std::string& BankName) const{ return GetValue(fBankToADCSlopeCalibMap,BankName); };
  double GetADCOffsetCalib(const std::string& BankName) const{ return GetValue(fBankToADCOffsetCalibMap,BankName); };
  double GetTimeShift(const std::string& BankName) const{ return GetValue(fBankToTimeShift,BankName); };
  int GetTriggerPolarity(const std::string& BankName)const{return GetValue(fBankToPolarityMap,BankName);};
  int GetPedestal(const std::string& BankName)const{return GetValue(fBankToPedestalMap,BankName);};
	bool GetEnableBit(const std::string& BankName)const {return GetValue(fBankToEnableBitMap, BankName);};

  // Setters with check, return true if inserted
  bool SetDetectorName(std::string BankName, std::string value) {
    std::map< std::string, std::string >::iterator it;
    for (it = fBankToDetectorMap.begin(); it != fBankToDetectorMap.end(); ++it)
      if (it->second == value)
	return false;
    fBankToDetectorMap[BankName] = value;
    return true;
  }
  void SetClockTick(std::string BankName, double value) { fBankToClockTickMap[BankName]=value; }
  void SetNBits(std::string BankName, int value) { fBankToBitMap[BankName]=value; }
  void SetTimeShift(std::string BankName, double value) { fBankToTimeShift[BankName]=value; }
  void SetTriggerPolarity(std::string BankName, int value) { fBankToPolarityMap[BankName]=value; }
  void SetPedestal(std::string BankName, int value) { fBankToPedestalMap[BankName]=value; }
  void SetADCSlopeCalib(std::string BankName, double value) { fBankToADCSlopeCalibMap[BankName] = value; }
  void SetADCOffsetCalib(std::string BankName, double value) { fBankToADCOffsetCalibMap[BankName] = value; };
	void SetEnableBit(std::string BankName, bool value){fBankToEnableBitMap[BankName] = value;};

  static bool IsFADC(const std::string& BankName) { return BankName[0] == 'N'; } // if the first letter is N then the bank name is for a FADC
  static bool IsHoustonCAEN(const std::string& BankName) { return BankName.substr(2,2) == "UH"; } // if the first letter is C then the bank name is for a CAEN
  static bool IsBostonCAEN(const std::string& BankName) { return BankName.substr(2,2)  == "BU"; } // if the first letter is C then the bank name is for a CAEN
  static bool IsSlow(const std::string& BankName) { return (*BankName.end() -1 ) == 'S'; } // if the last letter is S then the bank name is for a Slow pulse
  static bool IsFast(const std::string& BankName) { return (*(BankName.end() -1)  == 'F' || BankName.substr(0,2) == "Sc" ); } // if the last letter is F then the bank name is for a Fast pulse

	bool IsEnable(const std::string& BankName){return GetEnableBit(BankName);}
private:
  // A small helper function to save us copying this about the place all the time
  double GetValue(const std::map<std::string,double>& map,const std::string& BankName)const{
          std::map<std::string, double>::const_iterator it=map.find(BankName); 
          if(it!=map.end()) return it->second;
          return 0.;
  }

  int GetValue(const std::map<std::string,int>& map,const std::string& BankName)const{
          std::map<std::string, int>::const_iterator it=map.find(BankName); 
          if(it!=map.end()) return it->second;
          return 0;
  }

	bool GetValue(const std::map<std::string,bool>& map,const std::string& BankName)const{
		std::map<std::string, bool>::const_iterator it=map.find(BankName); 
		if(it!=map.end()) 
			return it->second;
		else
			return false;
	}
  ClassDef(TSetupData, 2)

};
#endif
