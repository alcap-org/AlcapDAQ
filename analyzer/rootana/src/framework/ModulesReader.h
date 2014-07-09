#ifndef MODULESREADER__HH_
#define MODULESREADER__HH_
#include <string>
#include <fstream>
#include <vector>
#include <iostream>

//#include "BaseModule.h"
#include "ModulesOptions.h"

namespace modules{
    class reader;
}

class modules::reader{
	// useful typedefs and structs (but they're kept private to avoid
	// dependencies outisde of this class )
	typedef std::vector<std::string> OptionsList;
	typedef std::map<std::string,modules::options* > SectionsList;
	typedef std::vector<std::pair<std::string, modules::options*> > ModuleList;
	typedef std::map<std::string,int > ModuleCounts;
	enum OptionMode_t { kSet , kAppend };
	struct Option_t {
		OptionMode_t mode;
		std::string key;
		std::string value;
	};

    public:
	// default constructor
	reader():fShouldPrint(false),fDebugAll(false),fDumpContents(false){};
	// destructor.  Virtual in case someone ever decides to overload this
	// class
	virtual ~reader(){};

    public:
	int ReadFile(const char* name);
	void PrintAllOptions()const;
    int HowMany(const std::string& name)const;

	size_t GetNumModules()const{return fModules.size();};
	std::string GetModule(unsigned int i)const{return fModules[i].first;};
	modules::options* GetOptions(unsigned int i)const{return fModules[i].second;};
	void SetDebug();
	void SetDebugAll();

    private:
	int OpenFile(const char* name, std::ifstream& infile);
	bool AddSection(const std::string& name,const std::string& type="");
	int AddModule(std::string line);
	void ProcessGlobalOption(Option_t opt);
	void AddOption(const std::string& module, const Option_t& opt){ AddOption(fAllOptions[module],opt); }
	void AddOption(const std::string& module, const std::string& flag){ AddOption(fAllOptions[module],flag); }
	void AddOption(modules::options* module, const Option_t& opt);
	void AddOption(modules::options* module, const std::string& flag);
        void AddOptionAll(const std::string& key,const std::string& value="");
        void AddOptionAll(const Option_t& opt);

	Option_t SplitOption(const std::string& line);
	int MakeModules(const SectionsList&);
	bool isComment( std::stringstream& line);
	std::string findSectionName(const std::string& line);
	std::ostream& PrintProblem();

    private:
	SectionsList fAllOptions;
	ModuleList fModules;
	ModuleCounts fModulesCounts;
	static const char* fGlobalModule;
	int fLineNumber;
	bool fShouldPrint;
	bool fDebugAll;
	bool fDumpContents;
};

inline bool modules::reader::AddSection(const std::string& name,const std::string& type){
    if(fAllOptions.find(name)==fAllOptions.end()){
	fAllOptions[name] =new modules::options(type);
	return true;
    }
    return false;
}

inline void modules::reader::AddOptionAll(const Option_t& opt){
	AddOptionAll(opt.key,opt.value);
}

#endif // MODULESREADER__HH_
