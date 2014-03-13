#ifndef MODULESOPTIONS__HH_
#define MODULESOPTIONS__HH_
#include <map>
#include <vector>
#include <string>
#include <utility>

namespace modules{
    class options;
}

class modules::options{

    public:
	options(const std::string& name):fModuleName(name){};
	virtual ~options(){};

	void AddOption(const std::string& name, const std::string& option);
	void AddArgument(const int& number, const std::string& option);
    public:
	int GetInt(const std::string&)const;
	double GetDouble(const std::string&)const;
	std::string GetString(const std::string&)const;
	bool GetBool(const std::string&)const;
	//std::vector<std::string> GetVectorStrings(const std::string&)const;

	bool HasOption(const std::string&)const;
	std::string GetOption(const std::string&)const;

	void DumpOptions(const std::string& prefix="     ")const;
    private:
	template <typename T>
	    T GetOption(const std::string&)const;
    private:
	typedef std::map<std::string,std::string> OptionsList_t;
	typedef std::vector<OptionsList_t::iterator> OptionsOrder_t;
	OptionsList_t fOptions;
	OptionsOrder_t fOrder;

	std::string fModuleName;

};

inline void modules::options::AddOption(const std::string& name, const std::string& option){
    std::pair<OptionsList_t::iterator, bool> ret = fOptions.insert(make_pair(name,option));
    // if a new key was added, store an iterator in the order list
    if(ret.second) fOrder.push_back(ret.first);
}

inline bool modules::options::HasOption(const std::string& name)const{
    return fOptions.find(name)!=fOptions.end();
}

#endif  //MODULESOPTIONS__HH_
