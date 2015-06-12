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
  if (bk[0] == 'D') { // New CAEN digitizer bank names
    if      (bk[1] == '4') return 14;
    else if (bk[1] == '5') return 12;
    else if (bk[1] == '7') return 14;
    else if (bk[1] == '8') return 12;
  } else if (bk[0] == 'C') { // Old CAEN digitizer bank names
    if      (bk[2] == 'U') return 14;
    else if (bk[2] == 'B') return 12;
  } else if (bk[0] == 'N') {
    if      (bk[1] != 'U') return 12;
  }
  return -1;
}  

#ifdef ROOT_VERSION
TSetupData* TSetupData::Instance() {
  return NULL;
}
#endif
