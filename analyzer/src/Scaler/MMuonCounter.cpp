////////////////////////////////////////////////////////////////////////////////
/// \defgroup MMuonCounter
/// \ingroup lldq
/// \author John Quirk
///
////////////////////////////////////////////////////////////////////////////////

/* Standard includes */
#include <cstdio>
#include <string>
#include <map>

/* MIDAS includes */
#include "midas.h"

/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

/*-- Module declaration --------------------------------------------*/
static INT MMuonCounter_init(void);
static INT MMuonCounter_eor(INT);
static INT MMuonCounter(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

namespace {
  std::string MUONBANK;
  int NBLOCKS = 0;
}

ANA_MODULE MMuonCounter_module =
{
  "MMuonCounter",    /* module name           */
  "John R Quirk",    /* author                */
  MMuonCounter,      /* event routine         */
  NULL,              /* BOR routine           */
  MMuonCounter_eor,  /* EOR routine           */
  MMuonCounter_init, /* init routine          */
  NULL,              /* exit routine          */
  NULL,              /* parameter structure   */
  0,                 /* structure size        */
  NULL,              /* initial parameters    */
};

INT MMuonCounter_init() {
  MUONBANK = gSetup->GetBankName("TTSc");
  return SUCCESS;
}

INT MMuonCounter_eor(INT run_number) {
  gData->SetNBlocks(NBLOCKS);
  return SUCCESS;
}
INT MMuonCounter(EVENT_HEADER *pheader, void *pevent) {
  const map< string, vector<int64_t> >& tdc_map = gData->fTDCHitsToChannelMap;
  ++NBLOCKS;
  if (tdc_map.count(MUONBANK))
    gData->SetNMuBlock(tdc_map.at(MUONBANK).size());
  else
    printf("Cannot find muon bank %s!\n", MUONBANK.c_str());
  return SUCCESS;
}

/// @}
