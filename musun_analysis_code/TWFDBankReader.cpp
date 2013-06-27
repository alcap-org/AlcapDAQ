#include "TWFDBankReader.h"
#include <stdlib.h>
using std::string;
#include <iostream>
using std::cerr; using std::cout; using std::endl;

extern TPC_PARAM tpc_parameters;

// There's no point in making an empty reader.
TWFDBankReader::TWFDBankReader()
{
  cerr << "TWFDBankReader: Error! Do not use default constructor." << endl;
  exit(1);
}

TWFDBankReader::TWFDBankReader(Int_t dbc, 
                               Int_t personality, 
                               string bankname)
  : fDBC(dbc), fPersonality(personality), fBankName(bankname),
    fBankPtr(NULL), fEventPtr(NULL), fSize(0), fNRequestedWords(0),
    fNReceivedWords(0), fStatus(0), fIsOK(true), fNBlocks(0)
{}

Int_t TWFDBankReader::GetDBC() { return fDBC; }
Int_t TWFDBankReader::GetPersonality() { return fPersonality; }
string TWFDBankReader::GetBankName() { return fBankName; }
Int_t TWFDBankReader::GetNIslands() { return fData.size(); }
Int_t TWFDBankReader::GetNBlocks() { return fNBlocks;}

/// Block length in 4-byte words is 2*DBC + 2 time words
inline Int_t TWFDBankReader::GetBlockLength_Words() { return 2*fDBC + 2; }

/// Number of ADC samples in the block, which is 8*DBC;
inline Int_t TWFDBankReader::GetNBlockSamples() { return 8*fDBC; }

Int_t TWFDBankReader::GetNRequestedWords() {return fNRequestedWords;}
Int_t TWFDBankReader::GetNReceivedWords() {return fNReceivedWords;}
Int_t TWFDBankReader::GetStatus() {return fStatus;}

/** Access the islands after they've been read. Returns a reference
  * like std::vector.
  */
TWFDRawIsland& TWFDBankReader::at(int index){ return fData.at(index); }

void TWFDBankReader::ProcessEvent(EVENT_HEADER* pheader, void *pevent)
{
  ClearEvent(); // reset everything to zero at the beginning of each event
  fEventPtr = pevent;

  if(fPersonality != 2) { //MuCap mode is 2
    cerr << "TWFDBankReader::ProcessEvent() Error! WFD personalities other"
         << " than 2 (MuCap mode) have not been implemented. Skipping bank:\n"
         << "\t" << fBankName
         << endl;
    fIsOK = false;
    return;
  }
  fSize = bk_locate(fEventPtr, fBankName.c_str(), &fBankPtr);
  if(fBankPtr == NULL) {
    cerr << "TWFDBankReader::ProcessEvent() Error! Bank not found:"
         << "\t" << fBankName
         << endl;
    fIsOK = false;
    return;
  }

  //Read status words
  if(fSize < 3) { // There are 3 status words
    cerr << "TWFDBankReader::ProcessEvent() Error? Bank size < 3 words"
         << "\tfor bank: " << fBankName
         << endl;
    fIsOK = false; return; 
  }
  fNRequestedWords = Int_t(fBankPtr[0]); // unsigned to signed cast.
  fNReceivedWords = Int_t(fBankPtr[1]);
  fStatus = Int_t(fBankPtr[2]);

  if( !CheckStatus() ) return; // CheckStatus returns false if bank is corrupt

  fNBlocks = (fSize-4) / GetBlockLength_Words();

  if( !ReadIslandsLoop() ) return; // ReadIslandsLoop returns false on failure

  //And we're done!
}

/** Performs some status checks on the bank. 
  * Returns false and flags fIsOK=false if bank is corrupt. 
  */
Bool_t TWFDBankReader::CheckStatus()
{
  if( fNRequestedWords == 0 ) { 
    cerr << "TWFDBankReader::ProcessEvent() Error? WFD Requested words = 0"
         << "\tfor bank: " << fBankName << "\n"
         << "\tSkipping bank."
         << endl;
    fIsOK = false; return false; 
  }
  if( fNRequestedWords >= 0x00100000 ) { 
    cerr << "TWFDBankReader::ProcessEvent() Error? WFD Requested words "
         << " > 0x00100000."
         << "\tfor bank: " << fBankName << "\n"
         << "\tSkipping bank."
         << endl;
    fIsOK = false; return false; 
  }
  if( fNRequestedWords != fNReceivedWords || fStatus!=0 ) {
    cerr << "TWFDBankReader::ProcessEvent() Error! WFD Requested words != "
         << "WFD received words for bank: " << fBankName << "\n"
         << "Requested: " << fNRequestedWords << " Received: " 
         << fNReceivedWords << " Status: " << fStatus << "\n"
         << "\tSkipping bank."
         << endl;
    fIsOK = false; return false;
  }
  // Check that the size of the bank is a multiple of the block size
  // (plus 4 status words).
  if( (fSize-4) % GetBlockLength_Words() !=0 ) { 
    cerr << "TWFDBankReader::ProcessEvent() Error! Bank size ("
         << fSize-4 << ") is not a multiple "
         << "of WFD block length (" << GetBlockLength_Words() << ") "
         << "\tfor bank: " << fBankName << "\n"
         << "\tSkipping bank."
         << endl;
    fIsOK=false; return false; 
  }
  return true;
}

/** Loops through the bank to accumulate WFD Blocks into fData.
  * Returns false on failure or corruption and flags fIsOK as false.
  */
Bool_t TWFDBankReader::ReadIslandsLoop()
{
  //pointer to beginning of wfd block data
  DWORD* pDataPtr = fBankPtr+3;
  int nBlocksRead = 0; 
  int blockoffset = 0; // offset to pDataPtr for the current block
  int blocktime = 0; // time of current block
  int conttime = -1; // GetNBlockSamples after the last island's time
  while( nBlocksRead<fNBlocks ){
    // offset for pDataPtr
    blockoffset = nBlocksRead*GetBlockLength_Words();
    // time is at the end of the adc samples
    blocktime = pDataPtr[blockoffset+GetBlockLength_Words()-1] & 0xEFFFFFFF;
    // conttime is the time of a potential continuation island
    conttime = fData.empty() ? -1 : fData.back().fTime+fData.back().fADC.size();
    // New island for first guy or if it's a new trigger
    if(nBlocksRead==0 || blocktime > conttime) { 
      fData.push_back( TWFDRawIsland() ); // make a new island
      fData.back().fTime = blocktime;     // set the new island's time
    } else if( blocktime < conttime ){ 
        cerr << "TWFDBankReader::ReadIslandsLoop() Error! Block has time: "
             << blocktime << " which is less than " << fData.back().fADC.size()
             << " clock ticks after the last block at " 
             << fData.back().fTime << "\n"
             << "Skipping Bank " << fBankName
             << endl;
        fIsOK=false; return false;
    } // Otherwise, it must be a continuation, so don't append to fData
    
    // At this point, the last island in fData is the one we should add too.
    std::vector<UChar_t>& rVec = fData.back().fADC; // local reference
    // Set a 1-byte pointer at the beginning of the new block's samples
    UChar_t* adc = (UChar_t*)&pDataPtr[blockoffset];
    for(int iadc=0; iadc<GetNBlockSamples(); iadc++) {
      /* WFD banks store samples non-sequentially. The samples within
       * each word are reversed:
       *       |DWORD  |DWORD  |DWORD
       * Bank: |3|2|1|0|7|6|5|4|11|10|...
       *
       * where |0| is the first sample, |1| is the second, etc.
       * the function (3-iadc%4) + 4*(iadc/4) reproduces this encoding.
       */ 
      rVec.push_back( adc[ (3-iadc%4) + 4*(iadc/4) ] );
      /* cout << "Set sample " << rVec.size() << " to "
           << "adc[" << (3-iadc%4) + 4*(iadc/4) << "] = " 
           << int(adc[ (3-iadc%4) + 4*(iadc/4) ])
           << endl; */ // In case you want to look at what's happening
    }

    // update loop variable
    nBlocksRead++;
  } // end while
}

/** Clears fData and resets things like fSize.
  * The vector is smart enough to not actually deallocate memory, so
  * no worries about efficiency
  */
void TWFDBankReader::ClearEvent()
{
  fData.clear();
  fSize = 0;
  fBankPtr = NULL;
  fEventPtr = NULL;
  fNRequestedWords = 0;
  fNReceivedWords = 0;
  fStatus = 0;
  fIsOK = true;
  fNBlocks = 0;
}
