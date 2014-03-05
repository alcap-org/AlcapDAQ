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

    public:
	reader(){};
	virtual ~reader(){};

    public:
	int ReadFile(const char* name);
	int OpenFile(const char* name, std::ifstream& infile);
	int FillSectionsList(std::ifstream& infile);
	void PrintAllOptions()const;


    private:
	bool AddSection(const std::string& name);
	int AddModule(std::string line);
	void AddOption(const std::string& module, const std::string& line);
	int MakeModules(const SectionsList&);
	bool isComment( std::stringstream& line);
	std::string findSectionName( std::stringstream& line);

    private:
	SectionsList fAllOptions;
	ModuleList fModules;
	static const char* fGlobalModule;
	int fLineNumber;
};

inline bool modules::reader::AddSection(const std::string& name){
    if(!fAllOptions[name]){
	fAllOptions[name] =new modules::options();
	return true;
    }
    return false;
}

#endif // MODULESREADER__HH_
