#include "TSetupData.h"

ClassImp(TSetupData)

std::string TSetupData::GetBankName(std::string DetectorName){
  for(std::map<std::string, std::string>::iterator it = fBankToDetectorMap.begin();
      it != fBankToDetectorMap.end(); ++it){
    if(it->second == DetectorName) return it->first;
  }
  
  return std::string("");
}

void TSetupData::GetAllDetectors(std::map<std::string,std::string>& detectors)const{
   std::map<std::string, std::string>::const_iterator it;
   for (it=fBankToDetectorMap.begin();it!= fBankToDetectorMap.end();it++){
       if(it->second!="blank" ) detectors[it->second]=it->first;
   }
}

void TSetupData::GetAllDetectors(std::vector<std::string>& detectors)const{
   std::map<std::string, std::string>::const_iterator it;
   for (it=fBankToDetectorMap.begin();it!= fBankToDetectorMap.end();it++){
       if(it->second!="blank" ) detectors.push_back(it->second);
   }
}
