#ifndef MODULESREADER__HH_
#define MODULESREADER__HH_
#include <string>
#include <fstream>

#include "FillHistBase.h"
#include "ModulesOptions.h"
#include "ModulesManager.h"

namespace modules{
    class reader;
}

class modules::reader{
	typedef std::vector<std::stringstream> OptionsList;
	typedef std::map<std::string,OptionsList > SectionsList;
    public:
	reader(){};
	virtual ~reader(){};

    public:
	int ReadFile(const char* name);
	int OpenFile(const char* name, std::ifstream& infile);
	int FillSectionsList(std::ifstream& infile);
	int MakeModules(const SectionsList&);
    private:
	bool isComment( std::stringstream& line);
	std::string findModuleName( std::stringstream& line);
    private:

	SectionsList fAllOptions;
};
#endif // MODULESREADER__HH_
