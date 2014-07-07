#ifndef SETUPNAVIGATOR_H_
#define SETUPNAVIGATOR_H_

#include "CommandLine.h"

class SetupNavigator{
  SetupNavigator(){};
  ~SetupNavigator(){ if (fThis) delete fThis;};
 public:
  static SetupNavigator* Instance();

  void SetCommandLineArgs(const ARGUMENTS& args){fCommandLineArgs=args;}

  int GetRunNumber(){return fCommandLineArgs.run;};

 private:
  static SetupNavigator* fThis;
  ARGUMENTS fCommandLineArgs;
};

inline SetupNavigator* SetupNavigator::Instance(){
  if(!fThis) fThis=new SetupNavigator();
  return fThis;
}

#endif //SETUPNAVIGATOR_H_