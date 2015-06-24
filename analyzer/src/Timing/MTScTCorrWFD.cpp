////////////////////////////////////////////////////////////////////////////////
/// \defgroup MTScTCorrWFD
///
/// \brief
/// Timing correlations from within WFD to TDC target scintillator signal.
/// @{
////////////////////////////////////////////////////////////////////////////////

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
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
static INT MTScTCorrWFD_init(void);
static INT MTScTCorrWFD(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

/// \brief
/// List of BU CAEN bank names for the event loop.
static const int NCRATE = 9;
static const int NCHAN[NCRATE] = {0, 0, 0, 0, 8, 4, 0, 8, 8};
const double TIME_LOW = -20e3, TIME_HIGH = 20e3;
static std::vector< std::vector<TH1*> > vvhTScTCorrWFD(NCRATE);


ANA_MODULE MTScTCorrWFD_module =
{
  "MTScTCorrWFD",    /* module name           */
  "John R Quirk",     /* author                */
  MTScTCorrWFD,      /* event routine         */
  NULL,               /* BOR routine           */
  NULL,               /* EOR routine           */
  MTScTCorrWFD_init, /* init routine          */
  NULL,               /* exit routine          */
  NULL,               /* parameter structure   */
  0,                  /* structure size        */
  NULL,               /* initial parameters    */
};

/*--module init routine --------------------------------------------*/
INT MTScTCorrWFD_init() {
  for (int icrate = 0; icrate < NCRATE; ++icrate) {
    for (int ichan = 0; ichan < NCHAN[icrate]; ++ichan) {
      char bank[8], name[64], title[128];
      sprintf(bank, "D%d%02d", icrate, ichan);
      sprintf(name, "hTScTCorrWFD_%s", bank);
      sprintf(title, "TSc timing correlation %s;Timing Difference (ns)",
              gSetup->GetDetectorName(bank).c_str());
      TH1* hist = new TH1D(name, title, 10000, TIME_LOW, TIME_HIGH);
      vvhTScTCorrWFD[icrate].push_back(hist);
    }
  }
  return SUCCESS;
}


/*-- module event routine -----------------------------------------*/
INT MTScTCorrWFD(EVENT_HEADER *pheader, void *pevent) {
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
            vvhTScTCorrWFD[icrate][ichan]->Fill(dt);
        }
      }
    }
  }

  return SUCCESS;
}

/// @}
