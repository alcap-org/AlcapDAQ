#include "IdGenerator.h"

ClassImp(IDs::generator);

std::string IDs::generator::str()const{
  if( fConfig==kAnyConfig) return fType;
  return fType+"::"+fConfig;
}
