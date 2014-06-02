#include "IdGenerator.h"

ClassImp(IDs::generator);

const IDs::Type_t IDs::generator::fAnyType="*";
const IDs::Config_t IDs::generator::fAnyConfig="*";

std::string IDs::generator::str()const{
 return fType+"::"+fConfig;
}
