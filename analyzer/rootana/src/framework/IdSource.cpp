#include "IdSource.h"
#include <iostream>
#include "debug_tools.h"
#include "ModulesParser.h"
ClassImp(IDs::source);

std::string IDs::source::str(bool cpp_safe)const{
	std::string tmp= fChannel.str() + IDs::field_separator +fGenerator.str();
        if(cpp_safe) modules::parser::ToCppValid(tmp);
        return tmp;
}

ostream& operator<< (ostream& os ,const IDs::source& id) {
  os<<id.str();
  return os;
}

IDs::source& IDs::source::operator=(const std::string& rhs){
    // Find the first delimiter
    size_t delim=rhs.find(IDs::field_separator.c_str());
    if(delim==std::string::npos){
        std::cout<<"Warning: Strange looking string given to IDs::source: "<<rhs<<std::endl;
        Channel()=rhs;
    } else{
        Channel()=rhs.substr(0,delim);
        Generator()=rhs.substr(delim+1);
    }
    return *this;
}

void IDs::source::Debug()const{
    Channel().Debug();
    Generator().Debug();
}
