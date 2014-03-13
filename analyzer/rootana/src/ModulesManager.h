#ifndef MODULESMANAGER__HH_
#define MODULESMANAGER__HH_
#include <map>
#include <string>
#include <stdio.h>

//#include "FillHistBase.h"
#include "ModulesOptions.h"
class FillHistBase;

namespace modules{
    // In the future I plan to rename the FillHistBase class, so this will help
   typedef FillHistBase ModuleBase;
    class manager;
}

class modules::manager{
    private:
	// Private constructor as this is a singleton class
	manager(){};
	~manager(){
	    fModuleMakers.clear();
	    fModuleOptions.clear();
	}

    public:
	// typdef a function pointer for creating a module
	typedef ModuleBase*(*maker)(options*);
	// Add a maker function to the list of makers
	void registerModule(const std::string&, maker);
	// get a pointer to the requested module
	ModuleBase* createModule(const std::string&, modules::options*);
	ModuleBase* createModule(const std::string& name);
	void addOptions(const std::string& name, modules::options *opts);
	void addArguments(const std::string& all_args);
	void addArgument(const std::string& module,const std::string& argument);
	void addArgument(const std::string& argument);
	std::string GetArgumentName(const std::string& module,const int& argument);

	// Get the instance of this class
	static manager* Instance();

    private:
	// the list of makers for all modules
	typedef std::map<std::string, maker> MakersList; 
	MakersList fModuleMakers;

	// the list of given options for all modules
	typedef std::map<std::string, modules::options*> OptionsList; 
	OptionsList fModuleOptions;

	// the list of expected arguments for all modules
	typedef std::vector<std::string> Arguments;
	typedef std::map<std::string, modules::manager::Arguments> ArgsList; 
	ArgsList fModuleArguments;

	// the most recent module to have been registered
	std::string fMostRecentRegister;
};

inline modules::manager* modules::manager::Instance(){
    static modules::manager* instance=NULL;
    if(!instance) {
	instance=new manager();
    }
    return instance;
}

inline void modules::manager::addArgument(const std::string& argument){
	addArgument(fMostRecentRegister,argument);
}

inline void modules::manager::addArgument(const std::string& module,const std::string& argument){
	fModuleArguments[module].push_back(argument);
}

inline std::string modules::manager::GetArgumentName(const std::string& module,const int& argument){
	try{
	return fModuleArguments[module].at(argument);
	}catch(...){
		char num[5];
		sprintf(num,"%d",argument);
		return std::string(num);
	}
}

#endif // MODULESMANAGER__HH_
