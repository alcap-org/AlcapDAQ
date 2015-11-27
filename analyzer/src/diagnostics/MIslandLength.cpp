////////////////////////////////////////////////////////////////////////////////
/// \defgroup MIslandLength
/// \ingroup lldq
/// \author Nam Tran
///
/// \brief
/// Plots information related to length of TPIs in each bank
/// per MIDAS event.
///
/// \details
/// Creates several histograms.
/// @{

/// \var DQ_IslandLength_histograms_map
/// \brief
/// Map of bank name to histogram of TPI lengths per event.

/// \var DQ_IslandLength_histograms_normalised_map
/// \brief
/// Same as ::DQ_IslandLength_histograms_map but normalized
/// to number of muons according to TDC.
////////////////////////////////////////////////////////////////////////////////

/* Standard includes */
#include <cstdio>
#include <stdlib.h>
#include <string>
#include <map>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include <TH1.h>
#include <TDirectory.h>

/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"
#include "TPulseIsland.h"
#include "TDirectory.h"

using std::string;
using std::map;
using std::vector;

/*-- Module declaration --------------------------------------------*/
INT MIslandLength_init(void);
INT MIslandLength(EVENT_HEADER*, void*);
INT MIslandLength_eor(INT);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

namespace {
  TH1* hTPILength;
}

ANA_MODULE MIslandLength_module =
{
  "MIslandLength",    /* module name           */
  "Nam Tran",         /* author                */
  MIslandLength,      /* event routine         */
  NULL,               /* BOR routine           */
  NULL,               /* EOR routine           */
  MIslandLength_init, /* init routine          */
  NULL,               /* exit routine          */
  NULL,               /* parameter structure   */
  0,                  /* structure size        */
  NULL,               /* initial parameters    */
};

INT MIslandLength_init() {
  hTPILength = new TH1I("hTPILengthV1724", "Pulse Length in V1724;Samples", 1000, 0., 1000.);
  return SUCCESS;
}

INT MIslandLength(EVENT_HEADER *pheader, void *pevent) {
  const map< string, vector<TPulseIsland*> >& tpi_map =
    gData->fPulseIslandToChannelMap;
  const string bank("D400");

  if (!tpi_map.count(bank)) return SUCCESS;
  const vector<TPulseIsland*>& tpis = tpi_map.at(bank);
  for (int i = 0; i < tpis.size(); ++i)
    hTPILength->Fill(tpis[i]->GetPulseLength());
  return SUCCESS;
}

/// @}
