#include "ModulesOptions.h"
#include "ModulesParser.h"
#include "ModulesFactory.h"
#include <sstream>
#include <iostream>
#include <string.h>
#include <TFormula.h>
#include "debug_tools.h"

modules::bad_value::bad_value(const char* module, const char* name, const char* range, double val):
    std::out_of_range(Form("For '%s', option '%s' has value %f,"
                " which fails '%s'", module,name,val,range)){
    }

std::string modules::options::GetOption(const std::string& key, bool complain)const{
    OptionsList_t::const_iterator it = fOptions.find(key);
    if(it==fOptions.end()){
        if(complain){
            throw missing_option(fModuleName,key);
        }
        return "";
    }
    return it->second;
}

template <typename T>
T modules::options::GetOption(const std::string& key,const T& defVal,bool complain, const std::string& range)const{
    std::stringstream ss(GetOption(key,complain));
    T val=defVal;
    ss>>val;
    if(!range.empty()) CheckValid(key,range.c_str(),val);
    return val;
}

void modules::options::CheckValid(const std::string& name,
        const char* expression, double value)const{
    std::string expr(expression);
    modules::parser::ReplaceWords(expr,name,"x");
    TFormula formula((name+"_validity").c_str(), expr.c_str());
    formula.Compile();
    double eval=formula.Eval(value);
    if(eval!=1){
        throw bad_value(fModuleName.c_str(),name.c_str(),expression,value);
    }
}

int modules::options::GetInt(const std::string& name, const std::string& range)const{
    return GetOption<int>(name,0,true,range);
}
int modules::options::GetInt(const std::string& name, int defVal, const std::string& range)const{
    return GetOption<int>(name,defVal,false,range);
}

double modules::options::GetDouble(const std::string& name, const std::string& range)const{
    return GetOption<double>(name,0,true,range);
}
double modules::options::GetDouble(const std::string& name, double defVal, const std::string& range)const{
    return GetOption<double>(name,defVal,false,range);
}

std::string modules::options::GetString(const std::string& name)const{
    std::string ret_val=GetOption(name,true);
    modules::parser::TrimWhiteSpaceBeforeAfter(ret_val);
    return ret_val;
}

std::string modules::options::GetString(const std::string& name,const std::string& defVal)const{
    std::string ret_val=GetOption(name,false);
    modules::parser::TrimWhiteSpaceBeforeAfter(ret_val);
    if(ret_val=="") ret_val=defVal;
    return ret_val;
}

bool modules::options::GetBool(const std::string& name)const{
    return modules::parser::IsTrue(GetString(name));
}

bool modules::options::GetBool(const std::string& name,bool defVal)const{
    return modules::parser::IsTrue(GetString(name,Form("%d",defVal)));
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
