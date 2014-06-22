#include "IdSource.h"
#include <ostream>

ClassImp(IDs::source);

std::string IDs::source::str()const{
	return fChannel.str() + IDs::field_separator +fGenerator.str();
}

ostream& operator<< (ostream& os , IDs::source& id) {
  os<<id.str();
  return os;
}
