#ifndef TAPGENERATOROPTIONS_H_
#define TAPGENERATOROPTIONS_H_

#include <string>

#include "ModulesOptions.h"

/// Class to handle options for a TAP generator
/// Inherite from the options class for modules to share the implementation and
/// interface.
class TAPGeneratorOptions:public modules::options{
 public:
 TAPGeneratorOptions(const std::string& name):
  modules::options(name){};
  ~TAPGeneratorOptions(){};
  
  void AddArgument(const int& number, const std::string& option);
  
  std::string GetChannel()const {
    return fChannel;
  }
  void SetChannel(const std::string& ch){ 
    fChannel = ch;
  }

 private:
  std::string fChannel;

};

#endif // TAPGENERATOROPTIONS_H_
