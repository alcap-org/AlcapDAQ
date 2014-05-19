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
	options(const std::string& name):fModuleName(name){
		fIdNumber=MakeIdNumber();
	};
	virtual ~options(){};

	void SetOption(const std::string& name, const std::string& option);
	bool AppendToOption(const std::string& name, const std::string& option);
	void AddArgument(const int& number, const std::string& option);
    public:
	int GetInt(const std::string&)const;
	double GetDouble(const std::string&)const;
	std::string GetString(const std::string&)const;
	bool GetBool(const std::string&)const;
	int GetVectorStringsByWhiteSpace(const std::string&, std::vector<std::string>& vect)const;
	int GetVectorStringsByDelimiter(const std::string&, std::vector<std::string>& vect,const char* delim=":")const;
	int GetVectorStrings(const std::string& name, std::vector<std::string>& vect)const {
             return GetVectorStringsByWhiteSpace(name,vect);
	};

	bool HasOption(const std::string&)const;
	bool GetNumOptions()const{return fOptions.size();};
	std::string GetOption(const std::string&)const;

	void DumpOptions(const std::string& prefix="     ")const;
    private:
	template <typename T>
	    T GetOption(const std::string&)const;

	static int MakeIdNumber();
    private:
	typedef std::map<std::string,std::string> OptionsList_t;
	typedef std::vector<OptionsList_t::iterator> OptionsOrder_t;
	OptionsList_t fOptions;
	OptionsOrder_t fOrder;
	int fIdNumber;

	std::string fModuleName;

};

inline void modules::options::SetOption(const std::string& name, const std::string& option){
    std::pair<OptionsList_t::iterator, bool> ret = fOptions.insert(make_pair(name,option));
    // if a new key was added, store an iterator in the order list
    if(ret.second) fOrder.push_back(ret.first);
}

inline bool modules::options::HasOption(const std::string& name)const{
    return fOptions.find(name)!=fOptions.end();
}

#endif  //MODULESOPTIONS__HH_
