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

int TSetupData::GetNBits(const std::string& bk) const {
  if (bk[0] == 'D') {
    return GetNBits(bk[1] - '0');
  } else if (bk[0] == 'S') {
    if (bk[6] == '0') {
      return GetNBits(1);
    } else {
      return 14;
    }
  }
  return -1;
}

int TSetupData::GetNBits(const int crate) const {
  switch (crate) {
  case 1: return 12;
  case 4: return 14;
  case 5: return 12;
  case 7: return 14;
  case 8: return 12;
  default: return -1;
  }
}

int TSetupData::GetDownSampling(const char* bank, int run) {
  if (bank[0] == 'S' && bank[5] == '0') {
    switch (bank[9] - '0') {
      case 1: return 1<<2;
      case 2: return 1<<0;
      case 3: return 1<<(run < 8340 ? 0 : 2);
      case 4: return 1<<(run < 8540 ? 0 : 2);
      case 5: return 1<<(run < 8540 ? 0 : 2);
      case 6: return 1<<(run < 9800 ? 0 : 2);
    }
  }
  return 1;
}

#ifdef ROOT_VERSION
TSetupData* TSetupData::Instance() {
  return NULL;
}
#endif
