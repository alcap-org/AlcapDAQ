#include "IdGenerator.h"
#include <iostream>

#include "debug_tools.h"

ClassImp(IDs::generator);

std::string IDs::generator::str()const{
  return fType+IDs::field_separator+fConfig;
}

ostream& operator<< (ostream& os ,const IDs::generator& id) {
  os<<id.str();
  return os;
}

IDs::generator& IDs::generator::operator=(const std::string& rhs){
    // Find the first delimiter
    size_t delim=rhs.find(IDs::field_separator.c_str());
    if(delim==std::string::npos){
        std::cout<<"Warning: Strange looking string given to IDs::source: "<<rhs<<std::endl;
        Type(rhs);
    } else{
        Type(rhs.substr(0,delim));
        Config(rhs.substr(delim+1));
    }
    return *this;
}

void IDs::generator::Debug()const{
    std::cout<<"Generator: "<<Type()<<std::endl;
    std::cout<<"Config: "<<Config()<<std::endl;
}
