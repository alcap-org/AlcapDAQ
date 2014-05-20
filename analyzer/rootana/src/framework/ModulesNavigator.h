#ifndef MODULESNAVIGATOR_H_
#define MODULESNAVIGATOR_H_

#include <map>
#include <string>
#include "ModulesFactory.h"

namespace modules{
	class navigator;
	// helper typedefs
	typedef std::map<std::string,modules::BaseModule*> list;
}

/// Singleton class tasked with:
///  - Storing all modules that have been created
///  - Providing random access to each module
///  - Providing an iterable list of modules for the main event loop
class modules::navigator{

      navigator():fModulesLoaded(false){};
      ~navigator(){};
      
  public:
      /// Static method to obtain the singleton instance of the navigator
      static inline navigator* Instance();
      
  public:
      /// read in a modules file, constructing each module and adding it to the list
      /// returns 0 on success and non-zero on failure
      int LoadConfigFile(const char* filename);
      
  public:
      // Get a named module.
      // Returns NULL if the module doesn't exist
      inline BaseModule* GetModule(const std::string& module)const;
      
      // Return an iterator to the first module in the list
      modules::list::iterator Begin(){return fModules.begin();};
      // Return a const iterator to the first module in the list
      modules::list::const_iterator Begin()const{return fModules.begin();};
      // Return an iterator to the last module in the list
      modules::list::iterator End(){return fModules.end();};
      // Return a const iterator to the last module in the list
      modules::list::const_iterator End()const{return fModules.end();};
      // Return the number of modules in the list
      unsigned int Size()const{return fModules.size();};
      
  private:
      bool fModulesLoaded;
      modules::list fModules;
};

inline modules::navigator* modules::navigator::Instance(){
    static modules::navigator* instance=new navigator();
    return instance;
}

inline modules::BaseModule* modules::navigator::GetModule(const std::string& module)const{
    modules::list::const_iterator mod=fModules.find(module.c_str());
    if(mod==fModules.end()) return NULL;
    return mod->second;
}

#endif //MODULESNAVIGATOR_H_
