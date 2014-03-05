#include "ModulesManager.h"

modules::ModuleBase* modules::manager::createModule(const std::string& name, modules::options* opts){
    // get the maker for the requested module
    modules::ModuleBase* module=NULL;
    MakersList::iterator it  = fModuleMakers.find(name);
    if(it != fModuleMakers.end() ){
	// make the module
	maker make= it->second;
	module=make(opts);
    }
    return module;
}

modules::ModuleBase* modules::manager::createModule(const std::string& name){
    // get the options for this module
    OptionsList::iterator it_opt=fModuleOptions.find(name);
    modules::options* opts = NULL;
    if(it_opt != fModuleOptions.end()) opts=it_opt->second;

    // make the module
    return createModule(name,opts);
}

void modules::manager::registerModule(const std::string& name, modules::manager::maker make){
    fModuleMakers[name]=make;
}

void modules::manager::addOptions(const std::string& name, modules::options *opts){
    fModuleOptions[name]=opts;
}
