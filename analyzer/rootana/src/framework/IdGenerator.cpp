#include "IdGenerator.h"
#include <ostream>


ClassImp(IDs::generator);

std::string IDs::generator::str()const{
  if( fConfig==kAnyConfig) return fType;
  return fType+"::"+fConfig;
}

ostream& operator<< (ostream& os , IDs::generator& id) {
  os<<id.str();
  return os;
}
