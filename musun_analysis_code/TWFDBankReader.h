#ifndef TWFDBankReader_h
#define TWFDBankReader_h

#include <string>
#include <vector>
#include "Rtypes.h"
#include "midas.h"
#include "Parameters.h"
#include "TWFDRawIsland.h"

/** This class is meant to wrap all dealings with MIDAS Banks for WFDs.
  * No class that needs WFD islands will need to include midas.h
  * This class should not be specific to the TPC or Gondola WFDs.
  * Reusability is the goal. It would be good if this class could
  * avoid accessing the ODB as well.
  */
class TWFDBankReader {
  private:
  Int_t fDBC;
  Int_t fPersonality;
  std::string fBankName; // Must use string::c_str for MIDAS

  DWORD* fBankPtr;
  void* fEventPtr;

  Int_t fSize;
  Int_t fNRequestedWords;
  Int_t fNReceivedWords;
  Int_t fStatus;
  Bool_t fIsOK;

  std::vector<TWFDRawIsland> fData;
  
  Int_t fNBlocks;

  Bool_t CheckStatus();
  Bool_t ReadIslandsLoop();

  protected:
  // There's no point in making an empty reader.
  TWFDBankReader();

  public:
  TWFDBankReader(Int_t dbc, Int_t personality, std::string bankname);
  
  Int_t GetDBC();
  Int_t GetPersonality();
  std::string GetBankName();
  Int_t GetNIslands();
  Int_t GetNBlocks();
  inline Int_t GetBlockLength_Words();
  inline Int_t GetNBlockSamples();

  Int_t GetNRequestedWords();
  Int_t GetNReceivedWords();
  Int_t GetStatus();
  
  TWFDRawIsland& at(int index); // returns reference like std::vector

  void ProcessEvent(EVENT_HEADER* pheader, void *pevent);
  void ClearEvent();
};
#endif
