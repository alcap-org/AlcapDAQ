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

    TMEGeneratorOptions(const std::string& name,const modules::options* opts):
        modules::options(name){
            modules::options::operator=(*opts);
        }

    TMEGeneratorOptions& operator=(const modules::options& rhs){
        modules::options::operator=(rhs);
        return *this;
    }
};

#endif // TMEGENERATOROPTIONS_H_
