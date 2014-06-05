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
};

#endif // TDPGENERATOROPTIONS_H_
