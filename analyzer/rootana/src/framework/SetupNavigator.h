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
  /// Gets the error on the pedestal from the SQLite database
  double GetPedestalError(std::string bankname);

  /// \brief
  /// Gets the pedestal from the SQLite database
  double GetPedestal(std::string bankname);

 private:
  static SetupNavigator* fThis;
  ARGUMENTS fCommandLineArgs;

  /// \brief
  /// The SQLite filename and server
  std::string fSQLiteFilename;
  TSQLiteServer* fServer;
};

inline SetupNavigator* SetupNavigator::Instance(){
  if(!fThis) fThis=new SetupNavigator();
  return fThis;
}

#endif //SETUPNAVIGATOR_H_
