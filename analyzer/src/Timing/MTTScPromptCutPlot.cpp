////////////////////////////////////////////////////////////////////////////////
/// \defgroup MTTScPromptCutPlot
///
/// \brief
/// Prompt timing cuts for TTSc
/// @{
////////////////////////////////////////////////////////////////////////////////

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <cmath>
#include <string>
#include <map>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include "TH1D.h"

//JG: added alcap includes
/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"


/*-- Module declaration --------------------------------------------*/
static INT MTTScPromptCutPlot(EVENT_HEADER*, void*);
static TH1* histogram_init(const char[]);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

static const int MAXNCRATE = 9;
static const int MAXNCHAN  = 8;
static const std::string filtered_bank_prefix("TScPromptCut_");
static TH1* vvhTTScPromptCutPlots[MAXNCRATE][MAXNCHAN] = {{}};


ANA_MODULE MTTScPromptCutPlot_module =
{
  "MTTScPromptCutPlot",    /* module name           */
  "John R Quirk",          /* author                */
  MTTScPromptCutPlot,      /* event routine         */
  NULL,                    /* BOR routine           */
  NULL,                    /* EOR routine           */
  NULL, /* init routine          */
  NULL,                    /* exit routine          */
  NULL,                    /* parameter structure   */
  0,                       /* structure size        */
  NULL,                    /* initial parameters    */
};

/*-- module event routine -----------------------------------------*/
INT MTTScPromptCutPlot(EVENT_HEADER *pheader, void *pevent) {

  const std::map< std::string, std::vector<TPulseIsland*> >& pulses_map =
    gData->fPulseIslandToChannelMap;

  for (int icrate = 0; icrate < MAXNCRATE; ++icrate) {
    for (int ichan = 0; ichan < MAXNCHAN; ++ichan) {
      char bank[8], filtered_bank[64];
      sprintf(bank, "D%d%02d", icrate, ichan);
      sprintf(filtered_bank, "%s%s", filtered_bank_prefix.c_str(), bank);
      if (pulses_map.count(filtered_bank)) {
        if (!vvhTTScPromptCutPlots[icrate][ichan])
          vvhTTScPromptCutPlots[icrate][ichan] = histogram_init(bank);
        TH1* hist = vvhTTScPromptCutPlots[icrate][ichan];
        const std::vector<TPulseIsland*>& pulses = pulses_map.at(filtered_bank);
        for (int ipulse = 0; ipulse < pulses.size(); ++ipulse)
          hist->Fill(pulses[ipulse]->GetPulseHeight());
      }
    }
  }

  return SUCCESS;
}

TH1* histogram_init(const char bank[]) {
  char name[64], title[64];
  sprintf(name, "hTTScPromptCutPlots_%s", bank);
  sprintf(title, "Pulse heights with prompt cut %s",
          gSetup->GetDetectorName(bank).c_str());
  int nbins = std::pow(2., gSetup->GetNBits(bank));
  return new TH1D(name, title, nbins, 0., nbins);
}

/// @}
