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
	typedef std::map<std::string,std::string> OptionsList_t;
	typedef std::vector<OptionsList_t::iterator> OptionsOrder_t;

    public:
	options(const std::string& name):fModuleName(name){
		fIdNumber=MakeIdNumber();
	};
	virtual ~options(){};

	void SetOption(const std::string& name, const std::string& option);
	bool AppendToOption(const std::string& name, const std::string& option);
	void AddArgument(const int& number, const std::string& option);
    public:
	int GetInt(const std::string&,int defVal=-1)const;
	double GetDouble(const std::string&,double defVal=1.)const;
	std::string GetString(const std::string&,const std::string& defVal="")const;
	bool GetBool(const std::string&,bool defVal=false)const;

	int GetVectorStringsByWhiteSpace(const std::string&, std::vector<std::string>& vect)const;
	int GetVectorStringsByDelimiter(const std::string&, std::vector<std::string>& vect,const char* delim=":")const;
	int GetVectorStrings(const std::string& name, std::vector<std::string>& vect)const {
             return GetVectorStringsByWhiteSpace(name,vect);
	};

	bool HasOption(const std::string&)const;
	bool GetNumOptions()const{return fOptions.size();};
	std::string GetOption(const std::string&)const;
	//OptionsList_t::iterator begin(){return fOptions.begin();};
	OptionsList_t::const_iterator begin()const{return fOptions.begin();};
	//OptionsList_t::iterator end(){return fOptions.end();};
	OptionsList_t::const_iterator end()const{return fOptions.end();};

	void DumpOptions(const std::string& prefix="     ")const;
    private:
	template <typename T>
	    T GetOption(const std::string&,const T& defVal)const;

	static int MakeIdNumber();
    private:
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
