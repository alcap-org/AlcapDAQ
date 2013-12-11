#include "TSetupData.h"

ClassImp(TSetupData)

std::string TSetupData::GetBankName(std::string DetectorName){
  for(std::map<std::string, std::string>::iterator it = fBankToDetectorMap.begin();
      it != fBankToDetectorMap.end(); ++it){
    if(it->second == DetectorName) return it->first;
  }
  
  return std::string("");
}
