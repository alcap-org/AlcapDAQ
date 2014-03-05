#include "ModulesOptions.h"
#include <sstream>

std::string modules::options::GetOption(const std::string& name)const{
    OptionsList_t::const_iterator it = fOptions.find(name);
    std::string options= it!=fOptions.end()? it->second : "";
    return options;
}

template <typename T>
T modules::options::GetOption(const std::string& name)const{
    std::stringstream ss(GetOption(name));
    T val=T();
    ss>>val;
    return val;
}

int modules::options::GetInt(const std::string& name)const{
    return GetOption<int>(name);
}

double modules::options::GetDouble(const std::string& name)const{
    return GetOption<double>(name);
}

std::string modules::options::GetString(const std::string& name)const{
    return GetOption(name);
}

bool modules::options::GetBool(const std::string& name)const{
    return GetOption<bool>(name);
}

std::vector<std::string> modules::options::GetVectorStrings(const std::string& name)const{
    std::stringstream ss(GetOption(name));
    std::vector<std::string> values;
    std::string val;
    while(ss>>val) values.push_back(val);
    return values;

}
