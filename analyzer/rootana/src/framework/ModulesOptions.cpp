#include "ModulesOptions.h"
#include "ModulesParser.h"
#include "ModulesFactory.h"
#include <sstream>
#include <iostream>
#include <string.h>

#define PrintHelp std::cout<<__FILE__<<":"<<__LINE__<<": "
#define PrintValue(value) PrintHelp<<#value "= |"<<value<<"|"<<std::endl;

std::string modules::options::GetOption(const std::string& key)const{
    OptionsList_t::const_iterator it = fOptions.find(key);
    std::string value= it!=fOptions.end()? it->second : "";
    return value;
}
template <typename T>
T modules::options::GetOption(const std::string& key,const T& defVal)const{
    std::stringstream ss(GetOption(key));
    T val=defVal;
    ss>>val;
    return val;
}

int modules::options::GetInt(const std::string& name,int defVal)const{
    return GetOption<int>(name,defVal);
}

double modules::options::GetDouble(const std::string& name,double defVal)const{
    return GetOption<double>(name,defVal);
}

std::string modules::options::GetString(const std::string& name,const std::string& defVal)const{
	std::string ret_val=GetOption(name);
        modules::parser::TrimWhiteSpaceBeforeAfter(ret_val);
	if(ret_val=="") ret_val=defVal;
     return ret_val;
}

bool modules::options::GetBool(const std::string& name,bool defVal)const{
    if(!HasOption(name)) return defVal;
    std::string val=GetString(name);
    return  (val=="true")|| (val=="TRUE")|| (val=="YES")|| (val=="yes")|| (val=="on")|| (val=="ON")|| (val=="1");
}

int modules::options::GetVectorStringsByWhiteSpace(const std::string& name, std::vector<std::string>& vect)const{
    std::stringstream ss(GetOption(name));
    std::string val;
    int num_tokens=0;
    for(;ss>>val; num_tokens++) {
      vect.push_back(val);
    }
    return num_tokens;
}

int modules::options::GetVectorStringsByDelimiter(const std::string& name, std::vector<std::string>& vect, const char* delim)const{
    char line[2048];
    strcpy(line,GetOption(name).c_str());
    char* word = strtok(line,delim);
    int num_tokens=0;
    for(;word != NULL; num_tokens++) {
	    //std::cout<<"ModulesOptions:GetVectorStringsByDelimiter() "<<word<<std::endl;
        vect.push_back(word);
        word = strtok(NULL,delim);
    }
    return num_tokens;
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
    std::string name = modules::factory::Instance()->GetArgumentName(fModuleName,number);
    // Set the value of the corresponding option
    modules::options::SetOption(name,option);
}

bool modules::options::AppendToOption(const std::string& name, const std::string& option){
    // Get the option 
    OptionsList_t::iterator it= fOptions.find(name);

    // If no option exists return false
    if(it==fOptions.end()) return false;

    // Add the value to it
    it->second+=" : "+option;
    return true;
}

int modules::options::MakeIdNumber(){
	static int count=0;
	return count++;
}

std::string modules::options::StringDescription()const{
    const char* key_val_sep="=";
    const char* start_key_val="{";
    const char* stop_key_val="}";
    OptionsOrder_t::const_iterator it;
    std::string description;
    for(it=fOrder.begin();it!=fOrder.end();it++){
        description+=start_key_val;
        description+=(*it)->first;
        if((*it)->second!=""){
            description+=key_val_sep;
            description+=(*it)->second;
        }
        description+=stop_key_val;
    }
    return description;
}
