#include "ModulesOptions.h"
#include "ModulesManager.h"
#include <sstream>
#include <iostream>

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
    modules::options::AddOption(name,option);
}
