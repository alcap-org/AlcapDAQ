#include "TSetupData.h"

ClassImp(TSetupData)

std::string TSetupData::GetBankName(std::string DetectorName) const{
   std::map<std::string, std::string>::const_iterator it;
   for(it = fBankToDetectorMap.begin(); it != fBankToDetectorMap.end(); ++it){
    if(it->second == DetectorName) return it->first;
  }
  
  return std::string("");
}

void TSetupData::GetAllDetectors(std::map<std::string,std::string>& detectors)const{
   std::map<std::string, std::string>::const_iterator it;
   for (it=fBankToDetectorMap.begin();it!= fBankToDetectorMap.end(); ++it){
       if(it->second!="blank" ) detectors[it->second]=it->first;
   }
}

void TSetupData::GetAllDetectors(std::vector<std::string>& detectors)const{
   std::map<std::string, std::string>::const_iterator it;
   for (it=fBankToDetectorMap.begin();it!= fBankToDetectorMap.end(); ++it){
       if(it->second!="blank" ) detectors.push_back(it->second);
   }
}

int TSetupData::GetNBits(const std::string& bk)const{
  if (bk[0] == 'D')
    return GetNBits(bk[1] - '0');
  if (bk[0] == 'S' && bk[1] == '5')
    return 12;
  return -1;
}

int TSetupData::GetNBits(const int crate) const {
  switch (crate) {
  case 4: return 14;
  case 5: return 12;
  case 7: return 14;
  case 8: return 12;
  default: return -1;
  }
}

#ifdef ROOT_VERSION
TSetupData* TSetupData::Instance() {
  return NULL;
}
#endif
