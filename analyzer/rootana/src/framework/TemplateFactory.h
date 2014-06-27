#ifndef TEMPLATEFACTORY_H_
#define TEMPLATEFACTORY_H_

#include <map>
#include <string>
#include <stdio.h>
#include <iostream>

#include "ModulesOptions.h"
#include "ModulesParser.h"

template <typename BaseModule, typename OptionsType>
class TemplateFactory{
    protected:
	/// protected constructor only callable by derived class 
	/// as this shoule be a singleton class
	TemplateFactory(){};
	virtual ~TemplateFactory(){
	    fModuleMakers.clear();
	    fModuleOptions.clear();
	}

    public:
	// typdef a function pointer for creating a module
	typedef BaseModule*(*maker)(OptionsType*);
	/// Add a maker function to the list of makers
	void registerModule(const std::string&, maker);
	/// Create a module with options
	virtual BaseModule* createModule(const std::string&, OptionsType*);
    /// Check if a module with this name has been registered
	bool canCreate(const std::string& name)const{return (bool) fModuleMakers.count(name);};
	/// Create a module and look up its options from the internal list
	BaseModule* createModule(const std::string& name);
	void addOptions(const std::string& name, OptionsType *opts);
	void addArguments(const std::string& all_args);
	void addArgument(const std::string& module,const std::string& argument);
	void addArgument(const std::string& argument);
	std::string GetArgumentName(const std::string& module,const int& argument);

	// Set the factory to output debugging commands
	void SetDebug(bool debug=true){fDebug=debug;};

  void PrintPossibleModules()const;

    private:
	// the list of makers for all modules
	typedef std::map<std::string, maker> MakersList; 
	MakersList fModuleMakers;

	// the list of given options for all modules
	typedef std::map<std::string, OptionsType*> OptionsList; 
	OptionsList fModuleOptions;

	// the list of expected arguments for all modules
	typedef std::vector<std::string> ArgumentsVector;
	typedef std::map<std::string, ArgumentsVector> ArgsList; 
	ArgsList fModuleArguments;

	// the most recent module to have been registered
	std::string fMostRecentRegister;

	// flag to control verbosity
	bool fDebug;
};

#include "TemplateFactory.tpl"


template <typename ConcreteClass,
	 typename BaseClass,
	 typename Options>
 BaseClass* RegistryProxyMaker(Options* opts){return new ConcreteClass(opts);};

template <typename ConcreteClass,
	 typename BaseClass,
	 typename Options,
	 typename Factory> 
class RegistryProxy{
  public: 
    RegistryProxy(const char* Name, const char* ArgumentNames) 
    { 
      Factory *f = Factory::Instance(); 
      f->registerModule( Name , &RegistryProxyMaker<ConcreteClass,BaseClass,Options>); 
      /* Add all the arguments to the manager */
      f->addArguments( ArgumentNames);
    }
};

#endif //TEMPLATEFACTORY_H_
