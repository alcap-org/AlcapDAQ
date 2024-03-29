#include "TOctalFADCBankReader.h"
#include "midas.h"
#include <stdio.h>
#include <algorithm>


using std::string;
using std::vector;

TOctalFADCBankReader::TOctalFADCBankReader(string bankname)
  : fBankName(bankname)
{
}

/** Reads the specified MIDAS bank, interpretting the raw data format into
* a vector<TOctalFADCIsland*>. Steps to use:
*
* \code
* TOctalFADCBankReader reader("BANK"); // BANK is the MIDAS bank name
* reader.ProcessEvent(pheader, pevent);
* vector<TOctalFADCIsland*> islands = reader.GetIslandVectorCopy();
* // Do stuff with islands
* \endcode
*
* Automatically calls ClearEvent() at the beginning of ProcessEvent().
*/
void TOctalFADCBankReader::ProcessEvent(EVENT_HEADER* pheader, void *pevent)
{
  ClearEvent();

  unsigned char* raw; // Points at the raw data, one byte at a time.
  int bankSize = bk_locate(pevent,fBankName.c_str(),&raw);
  int nSamples = bankSize / kDataBlockSize;

  vector<int> islandSamples;
  islandSamples.reserve(kSamplesArrayInitialSize);

  bool allowLowSampls = 1;
  int islandTimestamp = 0; // time of first 4-sample block in stitched island
  int lastTimestamp = -1; // To see the end of the last island.
  for(int i=0; i < nSamples; i++){
    // data format:
    //
    // bits
    // 79 FADC 0 / 1
    // 78-52 timestamp
    // 51-48 overflow
    // 47-36 sampleB0
    // 35-24 sampleA0
    // 23-12 sampleB1
    // 11-0 sampleA1
    //
    // Island samples in the order A1 B1 A0 B0

    int timestamp = ((raw[i*10+0] & 0x7f) << 20) |
                     (raw[i*10+1] << 12) |
                     (raw[i*10+2] << 4) |
                     (raw[i*10+3] >> 4);
    bool overflowB0 = ((raw[i*10+3] & 0x08) != 0);
    bool overflowA0 = ((raw[i*10+3] & 0x04) != 0);
    bool overflowB1 = ((raw[i*10+3] & 0x02) != 0);
    bool overflowA1 = ((raw[i*10+3] & 0x01) != 0);
    int sampleB0 = (overflowB0 << 12) |
                   (raw[i*10+4] << 4) |
                   (raw[i*10+5] >> 4);
    int sampleA0 = (overflowA0 << 12) |
                   ((raw[i*10+5] & 0xf) << 8) |
                   (raw[i*10+6]);
    int sampleB1 = (overflowB1 << 12) |
                   (raw[i*10+7] << 4) |
                   (raw[i*10+8] >> 4);
    int sampleA1 = (overflowA1 << 12) |
                   ((raw[i*10+8] & 0xf) << 8) |
                   (raw[i*10+9]);

    if(timestamp != lastTimestamp + 1) {
      if(islandSamples.size() > 4) {
          // This is a new island, so put the old
          // island on fData and start again.
          // Remember, time stamps are actually 4 samples long.
          fData.push_back(
            new TOctalFADCIsland(islandTimestamp*4,islandSamples)
          );
          islandSamples.clear();
      }
      else if (islandSamples.size() <= 4) {
	islandSamples.clear();
      }
      // Ignore islands with 4 or less samples.
      islandTimestamp = timestamp;
    }

    islandSamples.push_back(sampleA1);
    islandSamples.push_back(sampleB1);
    islandSamples.push_back(sampleA0);
    islandSamples.push_back(sampleB0);

    lastTimestamp = timestamp;

    //last pulse in the block!
    if ((i==nSamples-1) && (islandSamples.size() > 4)){
      fData.push_back(
        new TOctalFADCIsland(islandTimestamp*4,islandSamples)
      );
    }

#if 0
for(int k=0; k<10; k++){
printf("%x ",raw[i*10 + k]);
}
printf("\n");
printf("fadc=%x t=%2x %2x %2x %2x %2x\n", i, timestamp,
sampleA1, sampleB1, sampleA0,sampleB0);
#endif

  }

  if(fData.size()>1)
  {
    //Time sort islands
    std::sort(fData.begin(), fData.end(),TOctalFADCIsland::TimeSortFADCIslands());
    StitchIslands();
  } // Some Pulses are split over two separate islands. Put them together again
 // printf(".............. FADC islands sorted .............\n");
}

void TOctalFADCBankReader::StitchIslands()
{

  vector<TOctalFADCIsland*> fData_temp;
  //fData_temp.clear();
  
  TOctalFADCIsland* last = fData.at(0);
  //if(fData.size()>1){printf("Size first Island: %d\n",fData.at(1).GetNSamples());}
  fData_temp.push_back(last);
  int counter_temp = 0; int counter = 0;
  
  for(unsigned int i = 1; i<fData.size(); i++)
  {
    if(fData.at(i)->GetTime() - (last->GetTime() + last->GetNSamples()) >2 )
    {
      last = fData.at(i);
      fData_temp.push_back(last);
      counter_temp++;
    }
    else
    {
      vector<int>& samples = fData_temp.at(counter_temp)->GetSampleVector();

      for(int j=0;j<fData.at(i)->GetNSamples();j++)
      {
        samples.push_back(fData.at(i)->GetSampleVector().at(j));
      }
      counter++;
      last = fData_temp.at(counter_temp);
    }
  }

  //printf("Size original: %d, size stitched: %d, counter %d\n",fData.size(),fData_temp.size(),counter);

  fData.clear();
  fData = fData_temp;
  fData_temp.clear();

  //printf("Size new: %d, size temp: %d, counter %d , counter_temp %d\n",fData.size(),fData_temp.size(),counter,counter_temp);

}

void TOctalFADCBankReader::ClearEvent()
{
  for(int i=0; i<fData.size(); i++) {
    if(fData[i]) { delete fData[i]; fData[i] = NULL; }
  }
  fData.clear();
}
