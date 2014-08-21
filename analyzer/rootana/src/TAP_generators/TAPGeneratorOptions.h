#ifndef TAPGENERATOROPTIONS_H_
#define TAPGENERATOROPTIONS_H_

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
        return GetString("channel_name");
    }
    void SetChannel(const std::string& ch){ 
        SetOption("channel_name",ch);
    }
};

#endif // TAPGENERATOROPTIONS_H_
