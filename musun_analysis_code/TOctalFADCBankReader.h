#ifndef TOctalFADCBankReader_h
#define TOctalFADCBankReader_h

#include <string>
#include <vector>
#include "TOctalFADCIsland.h"

#include "midas.h"

class TOctalFADCBankReader {
 public:
  explicit TOctalFADCBankReader(std::string bankname);
  
  std::string GetBankName() const { return fBankName; }

  int GetNIslands() const { return fData.size(); }
  TOctalFADCIsland& at(int index) { return fData.at(index); }
  std::vector<TOctalFADCIsland>& GetIslandVector() {return fData;}

  void ProcessEvent(EVENT_HEADER* pheader, void *pevent);
  void ClearEvent() { fData.clear(); }
  

  static const int kDataBlockSize = 10; // 10 bytes per block
  static const int kSamplesArrayInitialSize = 8192;

 protected:
  TOctalFADCBankReader() {} /// Don't use the unnamed constructor
  void StitchIslands();
  std::string fBankName;
  std::vector<TOctalFADCIsland> fData;

};

#endif
