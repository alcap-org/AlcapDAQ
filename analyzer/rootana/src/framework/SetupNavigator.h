#ifndef SETUPNAVIGATOR_H_
#define SETUPNAVIGATOR_H_

#include "CommandLine.h"
#include "IdSource.h"
#include "AlcapExcept.h"

#include <TSQLiteServer.h>

#include <iostream>
#include <vector>
#include <utility>
#include <map>

MAKE_EXCEPTION(SetupNavigator, Base)
MAKE_EXCEPTION(NoCalibDB, SetupNavigator)
MAKE_EXCEPTION(MissingTable, SetupNavigator)
MAKE_EXCEPTION(UncalibratedRun, SetupNavigator)
MAKE_EXCEPTION(InvalidDetector, SetupNavigator)

class SetupNavigator{
  SetupNavigator();
  /// \brief
  /// Closes the database if open or saves
  ~SetupNavigator();
 public:
  static SetupNavigator* Instance();

  void Close();

  /// \brief
  /// Closes the database if this is a calibration run.
  void SetCommandLineArgs(const ARGUMENTS& args){fCommandLineArgs = args;}

  int GetRunNumber()const{return fCommandLineArgs.run;};
  std::string GetBank(const IDs::channel&)const;

  /// \brief
  /// Caches the information stored in the calib DB
  void CacheCalibDB();

  /// \brief
  /// Gets the pedestal from the SQLite database
  double GetPedestal(const IDs::channel& channel) const;
  /// \brief
  /// Gets the error on the pedestal from the SQLite database
  double GetNoise(const IDs::channel& channel) const;
  double GetCoarseTimeOffset( IDs::source src) const;
  /// \brief
  /// Gets the energy calibration constants
  std::pair<double,double> GetEnergyCalibrationConstants(const IDs::channel&) const;

  double GetAdcToEnergyGain(const IDs::channel& ch) const{return GetEnergyCalibrationConstants(ch).first;}
  double GetAdcToEnergyConstant(const IDs::channel& ch) const{return GetEnergyCalibrationConstants(ch).second;}

  void SetPedestalAndNoise(const IDs::channel& channel, double pedestal, double noise);
  void SetCoarseTimeOffset(const IDs::source& src, double dt);

private:

  /// \brief
  /// Reads the pedestal and pedestal error values.
  bool ReadPedestalAndNoiseValues();
  /// \brief
  /// Reads the gross time offset values.
  bool ReadCoarseTimeOffsetValues();
  /// \brief
  /// Read in gross time offset columns determining what TAP generators we're ready for.
  std::vector<std::string> GetCoarseTimeOffsetColumns();
  /// \brief
  /// Read in energy calibration constants.
  bool ReadEnergyCalibrationConstants();
  void OutputCalibCSV();

  bool IsCalibRun() const {return fCommandLineArgs.calib;}

 private:
  static SetupNavigator* fThis;
  ARGUMENTS fCommandLineArgs;

  /// \brief
  /// The SQLite filename, the SQLite server object and the current table name
  const std::string fSQLiteFilename;
  TSQLiteServer* fServer;
  const std::string fPedestalNoiseTableName;
  const std::string fCoarseTimeOffsetTableName;
  const std::string fEnergyCalibrationConstantsTableName;

  /// \brief
  /// The map that stores the pedestal values that we get from the SQLite database
  static std::map<IDs::channel, double> fPedestalValues;
  /// \brief
  /// The map that stores the pedestal error values (i.e. noise) that we get from the SQLite database
  static std::map<IDs::channel, double> fNoiseValues;
  /// \brief
  /// The map that stores the gross time offset values from SQLite database.
  static std::map<IDs::source, double> fCoarseTimeOffset;
  /// \brief
  /// The map that stores energy calibration constants as <Gain,Pedestal> pairs.
  typedef std::pair<double,double> EnergyCalibRow_t;
  static std::map< IDs::channel, EnergyCalibRow_t > fEnergyCalibrationConstants;

};

inline SetupNavigator* SetupNavigator::Instance(){
  if(!fThis) fThis=new SetupNavigator();
  return fThis;
}

#endif //SETUPNAVIGATOR_H_
