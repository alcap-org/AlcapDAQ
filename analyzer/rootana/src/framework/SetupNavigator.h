#ifndef SETUPNAVIGATOR_H_
#define SETUPNAVIGATOR_H_

#include "CommandLine.h"
#include "IdSource.h"

#include <TSQLiteServer.h>

class SetupNavigator{
  SetupNavigator();
  ~SetupNavigator();
 public:
  static SetupNavigator* Instance();

  void SetCommandLineArgs(const ARGUMENTS& args){fCommandLineArgs=args;}

  int GetRunNumber()const{return fCommandLineArgs.run;};
  std::string GetBank(const IDs::channel&)const;

  /// \brief
  /// Caches the information stored in the calib DB
  void CacheCalibDB();

  /// \brief
  /// Gets the pedestal from the SQLite database
  double GetPedestal(const IDs::channel& channel) { return fPedestalValues[channel]; }
  /// \brief
  /// Gets the error on the pedestal from the SQLite database
  double GetNoise(const IDs::channel& channel) { return fNoiseValues[channel]; }
  double GetGrossTimeOffset(const IDs::source& src) { return fGrossTimeOffset.at(source); }
  void SetGrossTimeOffset(const IDs::source& src, double dt);

 private:
  static SetupNavigator* fThis;
  ARGUMENTS fCommandLineArgs;

  /// \brief
  /// The SQLite filename, the SQLite server object and the current table name
  std::string fSQLiteFilename;
  TSQLiteServer* fServer;
  std::string fTableName;

  /// \brief
  /// The map that stores the pedestal values that we get from the SQLite database
  static std::map<IDs::channel, double> fPedestalValues;
  /// \brief
  /// The map that stores the pedestal error values (i.e. noise) that we get from the SQLite database
  static std::map<IDs::channel, double> fNoiseValues;
  /// \brief
  /// The map that stores the gross time offset values from SQLite database.
  static std::map<IDs::source, double> fGrossTimeOffset;

  /// \brief
  /// Reads the pedestal and pedestal error values
  void ReadPedestalAndNoiseValues();
  /// \brief
  /// Reads the gross time offset values
  void ReadGrossTimeOffsetValues();
  /// \brief
  /// Read in gross time offset columns determining what TAP generators we're ready for
  std::vector<std::string> GetGrossTimeOffsetColumns();

};

inline SetupNavigator* SetupNavigator::Instance(){
  if(!fThis) fThis=new SetupNavigator();
  return fThis;
}

#endif //SETUPNAVIGATOR_H_
