////////////////////////////////////////////////////////////////////////////////
/// \defgroup MDT5730Wavedump
/// \ingroup wavedump
/// \author Damien Alexander
///
/// \brief
/// Produces raw ascci txt file of adc values (newline delimited),
/// separation between events
/// @{
////////////////////////////////////////////////////////////////////////////////

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <map>
#include <utility>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <ios>

/* MIDAS includes */
#include "midas.h"

//JG: added alcap includes
/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"

/*-- Module declaration --------------------------------------------*/
static INT MDT5730Wavedump_init(void);
static INT MDT5730Wavedump_event(EVENT_HEADER*, void*);
static INT MDT5730Wavedump_eor(INT);

extern TGlobalData* gData;
extern TSetupData* gSetup;

//static std::vector<std::string> banks2dump;
static std::map<std::string, std::ofstream*> dumpfiles;

ANA_MODULE MDT5730Wavedump_module =
{
  "MDT5730Wavedump",             /* module name           */
  "Damien Alexander",            /* author                */
  MDT5730Wavedump_event,         /* event routine         */
  NULL,                          /* BOR routine           */
  MDT5730Wavedump_eor,           /* EOR routine           */
  MDT5730Wavedump_init,          /* init routine          */
  NULL,                          /* exit routine          */
  NULL,                          /* parameter structure   */
  0,                             /* structure size        */
  NULL,                          /* initial parameters    */
};

/*-- Wavedump_init -------------------------------------------------*/
INT MDT5730Wavedump_init(){
  std::map<std::string, std::string> bank2detmap = gSetup->fBankToDetectorMap;
  for(std::map<std::string, std::string>::iterator mIter = bank2detmap.begin();
      mIter != bank2detmap.end(); mIter++)  {

    std::string bankname = mIter->first;
    if(gSetup->IsWFD(bankname) && bankname[1] == '7'){
      //banks2dump.push_back(bankname);   //array of banks.  redundant with map
      std::string detname = gSetup->GetDetectorName(bankname);//get detname
      std::string outTitle = detname + ".dat";  //name of output file
      std::ofstream *outfile = new std::ofstream(outTitle.c_str(), std::ios::trunc);
      //outfile.open(outTitle.c_str(), std::ios::trunc);
      dumpfiles[bankname] = outfile;      // fill bank to output file map
    }
  }
  return SUCCESS;
}


/*-- Wavedump event cycle ------------------------------------------*/
INT MDT5730Wavedump_event(EVENT_HEADER *pheader, void *pevent) {

  //get pulse island map
  std::map<std::string, std::vector<TPulseIsland*> >& TPImap = gData->fPulseIslandToChannelMap;

  for(std::map<std::string, std::ofstream*>::iterator mIter=dumpfiles.begin(); mIter != dumpfiles.end(); mIter++){
  //iterate through wavedump files
    std::string bankname = mIter->first;
    std::ofstream *outfile = mIter->second;

    const std::vector<TPulseIsland*>& thePulses = TPImap.at(bankname);
    for(std::vector<TPulseIsland*>::const_iterator pIter = thePulses.begin(); pIter != thePulses.end(); pIter++) {
      //pull pulse island samples
      std::vector<int> theSamples = (*pIter)->GetSamples();
      //iterate through the samples
      for(std::vector<int>::iterator sIt = theSamples.begin(); sIt != theSamples.end(); sIt++) {
	//output sample << newline
	*outfile << (*sIt) << std::endl;

      }
      //optional blank line between pulses
      //outfile << std::endl;
    }
  }
  //close loops
  return SUCCESS;
}


INT MDT5730Wavedump_eor(int RunNumber) {
  for(std::map<std::string, std::ofstream*>::const_iterator mIter=dumpfiles.begin(); mIter != dumpfiles.end(); mIter++){
    std::ofstream *outfile = mIter->second;
    outfile->close();
  }
  return SUCCESS;
}
