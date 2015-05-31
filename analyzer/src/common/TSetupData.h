#ifndef TSetupData_h__
#define TSetupData_h__

#include <vector>
#include <map>
#include <set>
#include <string>
#include <iostream>

#include <TObject.h>

////////////////////////////////////////////////////////////////////////////////
/// \brief
/// Holds data relevent to each run.
/// \ingroup alcapana
/// \ingroup rootana
///
/// \details
/// %TSetupData (TSD) holds information about each run. This consists mainly
/// of data from the ODB WireMap, most importantly a mapping between each
/// digitizer channel and the physical detector it had plugged in.
/// Other hardware information such as the bit precision of each digitizer
/// channel, is also contained here.
////////////////////////////////////////////////////////////////////////////////
class TSetupData : public TObject{
 private:
 public:
  /// (Quasi-)Singleton interface
  static TSetupData* Instance();

  /// \name Maps
  /// All maps have, as their keys the MIDAS bank (digitizer channel) name.
  /// These are all from the ODB.
  //@{
  /// The physical detector name.
  std::map<std::string, std::string> fBankToDetectorMap;
  /// The length, in nanoseconds, of each sample clock tick.
  std::map<std::string, double> fBankToClockTickMap;
  /// The number of bits (precision) each ADC sample has.
  std::map<std::string, int> fBankToBitMap;
  /// Deprecated
  std::map<std::string, double> fBankToADCSlopeCalibMap;
  /// Deprecated
  std::map<std::string, double> fBankToADCOffsetCalibMap;
  /// The timing offset from the muSc.
  std::map<std::string, double> fBankToTimeShift;
  /// The pulse polarity (positive = 1, negative = -1).
  std::map<std::string, int> fBankToPolarityMap;
  /// The voltage offet in units of ADC.
  std::map<std::string, int> fBankToPedestalMap;
  /// Whether or not the channel was enabled.
  std::map<std::string, bool> fBankToEnableBitMap;
  //@}

  /// @return Which bank this detector was hooked up to, or "" if not hooked up.
  std::string GetBankName(std::string DetectorName) const;
  /// @return Which detector was hooked up to this bank, or "blank" if passed the bank "blank".
  /// Undefined behavior if passed a bank other than "blank" or what was used.
  std::string GetDetectorName(std::string BankName) const {
    std::map<std::string, std::string>::const_iterator it = fBankToDetectorMap.find(BankName);
    if (it == fBankToDetectorMap.end())
      return BankName;
    if (!it->second.empty() && it->second !=  "blank")
      return fBankToDetectorMap.find(BankName)->second;
    else
      return BankName;
  };

  /// @param[out] detectors Map with detectors as keys and banks as the mapped value, omitting detectors name "blank".
  void GetAllDetectors(std::map<std::string,std::string>& detectors) const;
  /// @param[out] detectors Vector of all detectors, omitting detectors named "blank".
  void GetAllDetectors(std::vector<std::string>& detectors) const;

  /// \name Getters
  /// @param[in] BankName Name of the MIDAS bank we're querying about.
  //@{
  /// @return Length of sample clock ticks in nanoseconds.
  double GetClockTick(const std::string& BankName) const {
    return GetValue(fBankToClockTickMap, BankName);
  }
  int GetNBits(const std::string& BankName) const;
  /// Deprecated
  double GetADCSlopeCalib(const std::string& BankName) const {
    return GetValue(fBankToADCSlopeCalibMap, BankName);
  }
  /// Deprecated
  double GetADCOffsetCalib(const std::string& BankName) const {
    return GetValue(fBankToADCOffsetCalibMap, BankName);
  }
  /// Timing offset of bank from muSc.
  double GetTimeShift(const std::string& BankName) const {
    return GetValue(fBankToTimeShift, BankName);
  }
  /// Pulse polarity
  int GetTriggerPolarity(const std::string& BankName) const {
    return GetValue(fBankToPolarityMap, BankName);
  }
  int GetPedestal(const std::string& BankName) const {
    return GetValue(fBankToPedestalMap, BankName);
  }
  bool GetEnableBit(const std::string& BankName) const {
    return GetValue(fBankToEnableBitMap, BankName);
  }
  //@}

  /// \name Setters
  /// @param[in] BankName Name of bank information is being set about.
  /// @param[out] value The value to saved.
  // Setters with check, return true if inserted
  //@{
  bool SetDetectorName(std::string BankName, std::string value) {
    std::map< std::string, std::string >::iterator it;
    for (it = fBankToDetectorMap.begin(); it != fBankToDetectorMap.end(); ++it)
      if (it->second == value)
       return false;
    fBankToDetectorMap[BankName] = value;
    return true;
  }
  void SetClockTick(std::string BankName, double value) {
    fBankToClockTickMap[BankName] = value;
  }
  void SetNBits(std::string BankName, int value) {
    fBankToBitMap[BankName] = value;
  }
  void SetTimeShift(std::string BankName, double value) {
    fBankToTimeShift[BankName] = value;
  }
  void SetTriggerPolarity(std::string BankName, int value) {
    fBankToPolarityMap[BankName] = value;
  }
  void SetPedestal(std::string BankName, int value) {
    fBankToPedestalMap[BankName] = value;
  }
  void SetADCSlopeCalib(std::string BankName, double value) {
    fBankToADCSlopeCalibMap[BankName] = value;
  }
  void SetADCOffsetCalib(std::string BankName, double value) {
    fBankToADCOffsetCalibMap[BankName] = value;
  };
  void SetEnableBit(std::string BankName, bool value){
    fBankToEnableBitMap[BankName] = value;
  };
  //@}

  static bool IsFADC(const std::string& BankName) {
    return BankName[0] == 'N';
  }
  static bool IsHoustonCAEN(const std::string& BankName) {
    return BankName.substr(2,2) == "UH";
  }
  static bool IsBostonCAEN(const std::string& BankName) {
    return BankName.substr(2,2)  == "BU";
  }
  static bool IsSlow(const std::string& BankName) {
    return (*BankName.end() -1 ) == 'S';
  }
  static bool IsFast(const std::string& BankName) {
    return (*(BankName.end() -1) == 'F' || BankName.substr(0,2) == "Sc" );
  }
  static bool IsWFD(const std::string& BankName) {
    return BankName.at(0) == 'D';
  }
  static bool IsTDC(const std::string& BankName) {
    return BankName.at(0) == 'T';
  }

  bool IsEnable(const std::string& BankName){return GetEnableBit(BankName);}

private:
  // A small helper function to save us copying this about the place
  // all the time
  double GetValue(const std::map<std::string,double>& map,
                  const std::string& BankName)const{
    std::map<std::string, double>::const_iterator it=map.find(BankName);
    if(it!=map.end()) return it->second;
    return 0.;
  }

  int GetValue(const std::map<std::string,int>& map,
               const std::string& BankName)const{
    std::map<std::string, int>::const_iterator it=map.find(BankName);
    if(it!=map.end()) return it->second;
    return 0;
  }

  bool GetValue(const std::map<std::string,bool>& map,
		const std::string& BankName)const{
    std::map<std::string, bool>::const_iterator it=map.find(BankName);
    if(it!=map.end()) return it->second;
    return false;
  }

  ClassDef(TSetupData, 3)

};
#endif
