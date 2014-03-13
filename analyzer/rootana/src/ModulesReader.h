#ifndef MODULESREADER__HH_
#define MODULESREADER__HH_
#include <string>
#include <fstream>
#include <vector>

//#include "FillHistBase.h"
#include "ModulesOptions.h"
#include "ModulesManager.h"

namespace modules{
    class reader;
}

class modules::reader{
	typedef std::vector<std::string> OptionsList;
	typedef std::map<std::string,modules::options* > SectionsList;
	typedef std::vector<std::pair<std::string, modules::options*> > ModuleList;
	struct Option_t { std::string key; std::string value;};

    public:
	reader():fShouldPrint(false){};
	virtual ~reader(){};

    public:
	int ReadFile(const char* name);
	int OpenFile(const char* name, std::ifstream& infile);
	int FillSectionsList(std::ifstream& infile);
	void PrintAllOptions()const;

	size_t GetNumModules()const{return fModules.size();};
	std::string GetModule(unsigned int i)const{return fModules[i].first;};
	modules::options* GetOptions(unsigned int i)const{return fModules[i].second;};
	void SetDebug();

    private:
	bool AddSection(const std::string& name,const std::string& type="");
	int AddModule(std::string line);
	void ProcessGlobalOption(Option_t opt);
	void AddOption(const std::string& module, Option_t opt);
	Option_t SplitOption(const std::string& line);
	int MakeModules(const SectionsList&);
	bool isComment( std::stringstream& line);
	std::string findSectionName( std::stringstream& line);

    private:
	SectionsList fAllOptions;
	ModuleList fModules;
	static const char* fGlobalModule;
	int fLineNumber;
	bool fShouldPrint;
};

inline bool modules::reader::AddSection(const std::string& name,const std::string& type){
    if(!fAllOptions[name]){
	fAllOptions[name] =new modules::options(type);
	return true;
    }
    return false;
}

#endif // MODULESREADER__HH_
