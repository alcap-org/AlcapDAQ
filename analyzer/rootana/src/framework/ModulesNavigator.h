#ifndef MODULESNAVIGATOR_H_
#define MODULESNAVIGATOR_H_

#include <map>
#include <string>
#include <iostream>
#include <typeinfo>       // std::bad_cast
#include "ModulesFactory.h"
#include "ModulesReader.h"
class TFile;

namespace modules{
	class navigator;
	class reader;
	// helper typedefs
	typedef std::vector<std::pair<std::string,modules::BaseModule*> > ordered_list;
	typedef std::multimap<std::string,BaseModule*> list;
	typedef ordered_list::iterator iterator;
	typedef ordered_list::const_iterator const_iterator;
}

/// Singleton class tasked with:
///  - Storing all modules that have been created
///  - Providing random access to each module
///  - Providing an iterable list of modules for the main event loop
class modules::navigator{

      navigator();
      ~navigator(){};
      
  public:
      /// Static method to obtain the singleton instance of the navigator
      static inline navigator* Instance();
      
  public:
      /// read in a modules file, constructing each module and adding it to the list
      /// returns 0 on success and non-zero on failure
      int LoadConfigFile(const char* filename);

      /// Make all modules that were requested through the modules file
      /// Requires that LoadConfigFile has already been called successfully
      int MakeModules();
      
  public:
      /// Get a named module.
      // @return NULL if the module doesn't exist
      inline BaseModule* GetModule(const std::string& module)const;

      /// Get a module by its position in the modules list as specified by the
      /// MODULES file.
      /// @return NULL if the module doesn't exist
      inline BaseModule* GetModule(const int& i)const;

      // Get a named module and cast it to the correct type
      /// @return NULL if the module doesn't exist
      template <typename T>
      inline T* GetModule(const std::string& module)const;

      /// Get the number of instances that the named module has been requested
      int HowMany(const std::string& name)const;
      
      /// Return an iterator to the first module in the list
      modules::iterator Begin(){return fModules.begin();};
      /// Return a const iterator to the first module in the list
      modules::const_iterator Begin()const{return fModules.begin();};
      /// Return an iterator to the last module in the list
      modules::iterator End(){return fModules.end();};
      /// Return a const iterator to the last module in the list
      modules::const_iterator End()const{return fModules.end();};
      /// Return the number of modules in the list
      unsigned int Size()const{return fModules.size();};
      /// Return the number of modules in the list
      unsigned int GetNumModules()const{return fModules.size();};
      
      void SetDebug(bool d=true){fDebug=d;}
      bool Debug()const{return fDebug;}

      /// @brief Tell the navigator which output file to be used
      void SetOutFile(TFile* file){fOutFile=file;}

  private:
      void AddModule(const std::string&, BaseModule*);

  private:
      bool fModulesLoaded;
      bool fModulesMade;
      bool fDebug;
      modules::ordered_list fModules;
      modules::list fModulesSearch;
      modules::reader fModulesFile;
      TFile* fOutFile;
};

inline modules::navigator* modules::navigator::Instance(){
    static modules::navigator* instance=new navigator();
    return instance;
}

inline modules::BaseModule* modules::navigator::GetModule(const std::string& module)const{
    modules::list::const_iterator mod=fModulesSearch.find(module.c_str());
    if(mod==fModulesSearch.end()) return NULL;
    return mod->second;
}

template <typename T>
inline T* modules::navigator::GetModule(const std::string& module)const{
    BaseModule* base_mod=GetModule(module);
    if(!base_mod) return NULL;
    T* mod=NULL;
    try
    {
        mod=dynamic_cast<T*>(base_mod);
    }
    catch (std::bad_cast& bc)
    {
        std::cerr << "ModulesNavigator::GetModule() bad_cast caught: " << bc.what() << std::endl;
	      return NULL;
    }
    return mod;
}

inline modules::BaseModule* modules::navigator::GetModule(const int& i)const{
	return fModules[i].second;
}


#endif //MODULESNAVIGATOR_H_
