#ifndef TEMPLATEFACTORY_H_
#define TEMPLATEFACTORY_H_

#include <map>
#include <string>
#include <stdio.h>
#include <iostream>
#include <stdexcept>

#include "ModulesOptions.h"
#include "ModulesParser.h"

template <typename BaseModule, typename OptionsType>
class TemplateFactory{
    public:
	// typdef a function pointer for creating a module
	typedef BaseModule*(*ModuleMaker)(OptionsType*);
	typedef std::vector<std::string> ArgumentsVector;

    private:
    struct PerModule{
        OptionsType* opts;
        ModuleMaker maker;
        ArgumentsVector arguments;
        std::string product;
        PerModule():opts(NULL),maker(NULL){}
    };
    typedef std::map<std::string, PerModule> ModuleList;

    protected:
	/// protected constructor only callable by derived class 
	/// as this shoule be a singleton class
	TemplateFactory(const std::string& name):fName(name){};
	virtual ~TemplateFactory(){
	    fModules.clear();
	}

    public:
	/// Add a ModuleMaker function to the list of makers
	void registerModule(const std::string&, ModuleMaker,const std::string& out="");
	/// Create a module with options
	virtual BaseModule* createModule(const std::string&, OptionsType* opts=NULL);
    /// Check if a module with this name has been registered
	bool canCreate(const std::string& name)const{return (bool) fModules.count(name);};
	void addOptions(const std::string& name, OptionsType *opts);
	void addArguments(const std::string& all_args);
	void addArgument(const std::string& module,const std::string& argument);
	void addArgument(const std::string& argument);
	std::string GetArgumentName(const std::string& module,const int& argument);
	std::string GetProduct(const std::string& module);

	// Set the factory to output debugging commands
	void SetDebug(bool debug=true){fDebug=debug;};

    void PrintPossibleModules()const;

    private:
    const PerModule& GetModuleDetails(const std::string&)const;

    // The list of all properties for each module
    ModuleList fModules;

	// the most recent module to have been registered
	std::string fMostRecentRegister;

	// flag to control verbosity
	bool fDebug;
    std::string fName;
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
    RegistryProxy(const char* Name, const char* ArgumentNames, const char* Product="") 
    { 
      Factory *f = Factory::Instance(); 
      f->registerModule( Name , &RegistryProxyMaker<ConcreteClass,BaseClass,Options>, Product); 
      // Add all the arguments to the manager 
      f->addArguments( ArgumentNames);
    }
};

#endif //TEMPLATEFACTORY_H_
