#ifndef MODULESMANAGER__HH_
#define MODULESMANAGER__HH_
#include <map>
#include <string>

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

	// Get the instance of this class
	static manager* Instance();

    private:
	// the list of makers for all modules
	typedef std::map<std::string, maker> MakersList; 
	MakersList fModuleMakers;

	// the list of given options for all modules
	typedef std::map<std::string, modules::options*> OptionsList; 
	OptionsList fModuleOptions;
};

inline modules::manager* modules::manager::Instance(){
    static modules::manager* instance=NULL;
    if(!instance) {
	instance=new manager();
    }
    return instance;
}

#endif // MODULESMANAGER__HH_
