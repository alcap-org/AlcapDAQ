#ifndef MODULESFACTORY__HH_
#define MODULESFACTORY__HH_
#include <map>
#include <string>
#include <stdio.h>

//#include "FillHistBase.h"
#include "ModulesOptions.h"
class FillHistBase;

namespace modules{
    // In the future I plan to rename the FillHistBase class, so this will help
   typedef FillHistBase BaseModule;
    class factory;
}

class modules::factory{
    private:
	// Private constructor as this is a singleton class
	factory(){};
	~factory(){
	    fModuleMakers.clear();
	    fModuleOptions.clear();
	}

    public:
	// typdef a function pointer for creating a module
	typedef BaseModule*(*maker)(options*);
	// Add a maker function to the list of makers
	void registerModule(const std::string&, maker);
	// get a pointer to the requested module
	BaseModule* createModule(const std::string&, modules::options*);
	BaseModule* createModule(const std::string& name);
	void addOptions(const std::string& name, modules::options *opts);
	void addArguments(const std::string& all_args);
	void addArgument(const std::string& module,const std::string& argument);
	void addArgument(const std::string& argument);
	std::string GetArgumentName(const std::string& module,const int& argument);

	// Get the instance of this class
	static factory* Instance();

	// Set the factory to output debugging commands
	void SetDebug(bool debug=true){fDebug=debug;};

    private:
	// the list of makers for all modules
	typedef std::map<std::string, maker> MakersList; 
	MakersList fModuleMakers;

	// the list of given options for all modules
	typedef std::map<std::string, modules::options*> OptionsList; 
	OptionsList fModuleOptions;

	// the list of expected arguments for all modules
	typedef std::vector<std::string> Arguments;
	typedef std::map<std::string, modules::factory::Arguments> ArgsList; 
	ArgsList fModuleArguments;

	// the most recent module to have been registered
	std::string fMostRecentRegister;

	// flag to control verbosity
	bool fDebug;
};

inline modules::factory* modules::factory::Instance(){
    static modules::factory* instance=NULL;
    if(!instance) {
	instance=new factory();
    }
    return instance;
}

inline void modules::factory::addArgument(const std::string& argument){
    addArgument(fMostRecentRegister,argument);
}

inline void modules::factory::addArgument(const std::string& module,const std::string& argument){
    fModuleArguments[module].push_back(argument);
}

inline std::string modules::factory::GetArgumentName(const std::string& module,const int& argument){
    try{
       return fModuleArguments[module].at(argument);
    }catch(...){
       char num[5];
       sprintf(num,"%d",argument);
       return std::string(num);
    }
}

#endif // MODULESFACTORY__HH_
