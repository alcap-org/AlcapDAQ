#ifndef TDPGENERATOROPTIONS_H_
#define TDPGENERATOROPTIONS_H_

#include "ModulesOptions.h"

/// Class to handle options for a TDP generator
/// Inherite from the options class for modules to share the implementation and
/// interface.
class TDPGeneratorOptions:public modules::options{
    public:
	TDPGeneratorOptions(const std::string& name):
		modules::options(name){};
	~TDPGeneratorOptions(){};

    TDPGeneratorOptions(const std::string& name,const modules::options* opts):
        modules::options(name){
            modules::options::operator=(*opts);
        }

  void AddArgument(const int& number, const std::string& option);

    TDPGeneratorOptions& operator=(const modules::options& rhs){
        modules::options::operator=(rhs);
        return *this;
    }
};

#endif // TDPGENERATOROPTIONS_H_
