////////////////////////////////////////////////////////////////////////////////
/// \defgroup MDQ_WFD_TCorr
/// \ingroup lldq
/// \author Damien Alexander
///
/// \brief
/// Plots time difference between TSc and channels in WFDs
///
/// \details
/// 
/// @{

/// \var hDQ_WFD_TCorr
/// \brief
/// Map of plots of Count of number of muScA hits in each
/// channel of TDC
////////////////////////////////////////////////////////////////////////////////

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>
#include <sstream>
#include <cmath>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include <TH1.h>
#include <TDirectory.h>

/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"
#include "TPulseIsland.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

/*-- Module declaration --------------------------------------------*/
INT  MDQ_WFD_TCorr_init(void);
INT  MDQ_WFD_TCorr(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

static const int NCRATE = 9;
static const int NCHAN[NCRATE] = {0, 0, 0, 0, 8, 4, 0, 8, 8};
const double TIME_LOW = -10e3, TIME_HIGH = 10e3; //ns

static std::vector< std::vector<TH1*> > vvhDQ_WFD_TCorr(NCRATE);


ANA_MODULE MDQ_WFD_TCorr_module =
{
	"MDQ_WFD_TCorr",               /* module name           */
	"Damien Alexander",            /* author                */
	MDQ_WFD_TCorr,                 /* event routine         */
	NULL,                          /* BOR routine           */
        NULL,                          /* EOR routine           */
	MDQ_WFD_TCorr_init,            /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};


/** This method initializes histograms. */
INT MDQ_WFD_TCorr_init()
{
    // See if the DataQuality_LowLevel/ directory already exists
  if (!gDirectory->Cd("DataQuality_LowLevel")) {
    
    std::string dir_name("DataQuality_LowLevel/");
    gDirectory->mkdir(dir_name.c_str());
    gDirectory->Cd(dir_name.c_str());
  }

  for (int icrate = 0; icrate < NCRATE; ++icrate) {
    for (int ichan = 0; ichan < NCHAN[icrate]; ++ichan) {
      char bank[8], name[64], title[128];
      sprintf(bank, "D%d%02d", icrate, ichan);
      std::string detname = gSetup->GetDetectorName(bank);
      sprintf(name, "hDQ_WFD_TCorr_%s_%s", detname.c_str(), bank);
      sprintf(title, "TSc timing correlation %s;Timing Difference (ns)",
              gSetup->GetDetectorName(bank).c_str());
      TH1* hist = new TH1D(name, title, 10000, TIME_LOW, TIME_HIGH);
      vvhDQ_WFD_TCorr[icrate].push_back(hist);
    }
  }


  gDirectory->Cd("/MidasHists/");
  return SUCCESS;
}

INT MDQ_WFD_TCorr(EVENT_HEADER *pheader, void *pevent)
{
  //Get the event number
  int midas_event_number = pheader->serial_number;

  const std::map< std::string, std::vector<TPulseIsland*> >& pulses_map =
    gData->fPulseIslandToChannelMap;

  const std::string mu_bank = gSetup->GetBankName("TSc");
  if (!pulses_map.count(mu_bank)) {
    printf("MTScTCorrWFD: No reference pulses TSc!\n");
    return SUCCESS;
  }
  const std::vector<TPulseIsland*>& mu_pulses = pulses_map.at(mu_bank);
  const double mu_tick = gSetup->GetClockTick(mu_bank);

  for (int icrate = 0; icrate < NCRATE; ++icrate) {
    for (int ichan = 0; ichan < NCHAN[icrate]; ++ichan) {
      char bank[8]; sprintf(bank, "D%d%02d", icrate, ichan);
      if (!pulses_map.count(bank)) continue;

      const std::vector<TPulseIsland*>& pulses = pulses_map.at(bank);
      const double pulse_tick = gSetup->GetClockTick(bank);

      for (int ipulse = 0; ipulse < pulses.size(); ++ipulse) {
        if (pulses[ipulse]->GetPulseHeight() < 600) continue;
        const double tpulse = pulse_tick * pulses[ipulse]->GetTimeStamp();
        for (int imu = 0; imu < mu_pulses.size(); ++imu) {
          const double dt = tpulse - mu_tick * mu_pulses[imu]->GetTimeStamp();
          if (dt < TIME_LOW)
            break;
          else if (dt < TIME_HIGH)
            vvhDQ_WFD_TCorr[icrate][ichan]->Fill(dt);
        }
      }
    }
  }


  return SUCCESS;
}
