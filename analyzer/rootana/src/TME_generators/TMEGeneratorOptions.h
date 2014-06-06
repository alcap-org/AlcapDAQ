#ifndef TMEGENERATOROPTIONS_H_
#define TMEGENERATOROPTIONS_H_

#include "ModulesOptions.h"

/// Class to handle options for a TME generator
/// Inherite from the options class for modules to share the implementation and
/// interface.
class TMEGeneratorOptions:public modules::options{
    public:
	TMEGeneratorOptions(const std::string& name):
		modules::options(name){};
	~TMEGeneratorOptions(){};
};

#endif // TMEGENERATOROPTIONS_H_
