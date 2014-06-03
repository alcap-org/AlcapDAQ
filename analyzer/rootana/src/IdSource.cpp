#include "IdSource.h"

ClassImp(IDs::source);

std::string IDs::source::str()const{
	return fChannel.str() + "::" +fGenerator.str();
}
