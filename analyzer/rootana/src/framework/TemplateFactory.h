#ifndef TEMPLATEFACTORY_H_
#define TEMPLATEFACTORY_H_

#include <map>
#include <string>
#include <stdio.h>
#include <iostream>
#include <TDirectory.h>

#include "ModulesOptions.h"

template <typename BaseModule>
class TemplateFactory{
	typedef modules::options options;
    protected:
	/// protected constructor only callable by derived class 
	/./ as this shoule be a singleton class
	TemplateFactory(){};
	~TemplateFactory(){
	    fModuleMakers.clear();
	    fModuleOptions.clear();
	}

    public:
	/./ typdef a function pointer for creating a module
	typedef BaseModule*(*maker)(options*);
	/// Add a maker function to the list of makers
	void registerModule(const std::string&, maker);
	/// Create a module with options
	BaseModule* createModule(const std::string&, options*);
	/// Create a module and look up its options from the internal list
	BaseModule* createModule(const std::string& name);
	void addOptions(const std::string& name, options *opts);
	void addArguments(const std::string& all_args);
	void addArgument(const std::string& module,const std::string& argument);
	void addArgument(const std::string& argument);
	std::string GetArgumentName(const std::string& module,const int& argument);

	// Set the factory to output debugging commands
	void SetDebug(bool debug=true){fDebug=debug;};

    private:
	// the list of makers for all modules
	typedef std::map<std::string, maker> MakersList; 
	MakersList fModuleMakers;

	// the list of given options for all modules
	typedef std::map<std::string, options*> OptionsList; 
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

#endif //TEMPLATEFACTORY_H_
