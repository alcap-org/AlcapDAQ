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
  double GetPedestal(IDs::channel channel) { return fPedestalValues[channel]; };
  /// \brief
  /// Gets the error on the pedestal from the SQLite database
  double GetNoise(IDs::channel channel) { return fNoiseValues[channel]; };

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
  /// Reads the pedestal and pedestal error values
  void ReadPedestalAndNoiseValues();
};

inline SetupNavigator* SetupNavigator::Instance(){
  if(!fThis) fThis=new SetupNavigator();
  return fThis;
}

#endif //SETUPNAVIGATOR_H_
