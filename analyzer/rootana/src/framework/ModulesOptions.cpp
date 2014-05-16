#include "ModulesOptions.h"
#include "ModulesManager.h"
#include <sstream>
#include <iostream>

std::string modules::options::GetOption(const std::string& key)const{
    OptionsList_t::const_iterator it = fOptions.find(key);
    std::string value= it!=fOptions.end()? it->second : "";
    return value;
}

template <typename T>
T modules::options::GetOption(const std::string& key)const{
    std::stringstream ss(GetOption(key));
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

int modules::options::GetVectorStrings(const std::string& name, std::vector<std::string>& vect)const{
    std::stringstream ss(GetOption(name));
    std::string val;
    while(ss>>val) vect.push_back(val);
    return vect.size();
}

void modules::options::DumpOptions(const std::string& prefix)const{
    //std::cout<<"key  "<<" = "<<" value "<<std::endl;
    OptionsOrder_t::const_iterator it;
    for(it=fOrder.begin();it!=fOrder.end();it++){
        std::cout<<prefix<<(*it)->first;
	if((*it)->second!=""){
	   std::cout<<" = "<<(*it)->second;
	}
	std::cout<<std::endl;
    }
}

void modules::options::AddArgument(const int& number, const std::string& option){
    // Get the name of this argument
    std::string name = modules::manager::Instance()->GetArgumentName(fModuleName,number);
    // Set the value of the corresponding option
    modules::options::SetOption(name,option);
}

bool modules::options::AppendToOption(const std::string& name, const std::string& option){
    // Get the option 
    OptionsList_t::iterator it= fOptions.find(name);

    // If no option exists return false
    if(it==fOptions.end()) return false;

    // Add the value to it
    it->second+=" "+option;
    return true;
}
