#include "IdGenerator.h"
#include <ostream>


ClassImp(IDs::generator);

std::string IDs::generator::str()const{
  return fType+IDs::field_separator+fConfig;
}

ostream& operator<< (ostream& os ,const IDs::generator& id) {
  os<<id.str();
  return os;
}
