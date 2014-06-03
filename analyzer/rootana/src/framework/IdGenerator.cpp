#include "IdGenerator.h"

ClassImp(IDs::generator);

std::string IDs::generator::str()const{
  if(fConfig.empty()) return fType;
  return fType+"::"+fConfig;
}
