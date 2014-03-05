#ifndef MODULESOPTIONS__HH_
#define MODULESOPTIONS__HH_
#include <map>
#include <vector>
#include <string>

namespace modules{
    class options;
}

class modules::options{

    options(){};
    virtual ~options(){};

    void SetOption(const std::string& name, const std::string& option);
    public:
	int GetInt(const std::string&)const;
	double GetDouble(const std::string&)const;
	std::string GetString(const std::string&)const;
	bool GetBool(const std::string&)const;
	std::vector<std::string> GetVectorStrings(const std::string&)const;

	bool HasOption(const std::string&)const;
	std::string GetOption(const std::string&)const;
    private:
	template <typename T>
	    T GetOption(const std::string&)const;
    private:
	typedef std::map<std::string,std::string> OptionsList_t;
	OptionsList_t fOptions;

};

inline void modules::options::SetOption(const std::string& name, const std::string& option){
    fOptions[name]=option;
}

inline bool modules::options::HasOption(const std::string& name)const{
    return fOptions.find(name)!=fOptions.end();
}

#endif  //MODULESOPTIONS__HH_
