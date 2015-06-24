////////////////////////////////////////////////////////////////////////////////
/// \defgroup MTTScTCorrWFD
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
#include <algorithm>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include "TH1D.h"
#include "TH2D.h"

//JG: added alcap includes
/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"


/*-- Module declaration --------------------------------------------*/
static INT MTTScTCorrWFD_init(void);
static INT MTTScTCorrWFD(EVENT_HEADER*, void*);
std::vector<int> calculate_tpi_times(const std::vector<TPulseIsland*>&);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

/// \brief
/// List of BU CAEN bank names for the event loop.
static const int NCRATE = 9;
static const int NCHAN[NCRATE] = {0, 0, 0,
                                  0, 8, 4,
                                  0, 8, 8};
static const double WFD_TICK[NCRATE] = {0., 0., 0.,
                                        0., 1e3/16.1290, 4,
                                        0., 2, 4};
//const double TIME_LOW = -20e3, TIME_HIGH = 20e3;
const double TIME_DIFFERENCE_MAX = 10e6; //ns
//static std::vector< std::vector<TH1*> > vvhTTScTCorrWFD(NCRATE);
static std::vector< std::vector<TH2*> > vvhTTScTCorrWFD(NCRATE);


ANA_MODULE MTTScTCorrWFD_module =
{
  "MTTScTCorrWFD",    /* module name           */
  "John R Quirk",     /* author                */
  MTTScTCorrWFD,      /* event routine         */
  NULL,               /* BOR routine           */
  NULL,               /* EOR routine           */
  MTTScTCorrWFD_init, /* init routine          */
  NULL,               /* exit routine          */
  NULL,               /* parameter structure   */
  0,                  /* structure size        */
  NULL,               /* initial parameters    */
};

/*--module init routine --------------------------------------------*/
INT MTTScTCorrWFD_init() {
  for (int icrate = 0; icrate < NCRATE; ++icrate) {
    for (int ichan = 0; ichan < NCHAN[icrate]; ++ichan) {
      char bank[8], name[64], title[128];
      sprintf(bank, "D%d%02d", icrate, ichan);
      sprintf(name, "hTTScTCorrWFD_%s", bank);
      sprintf(title,
              "TTSc timing correlation %s;Time TTSc (ns);Time Difference (ns)",
              gSetup->GetDetectorName(bank).c_str());
      //TH1* hist = new TH1D(name, title, 10000, TIME_LOW, TIME_HIGH);
      TH2* hist = new TH2D(name, title,
                           1000, 0., 100e6,  // 0-100e6 ns (100 ms)
                           20000, 0., 100e6);
                           // -TIME_DIFFERENCE_MAX,
                           // TIME_DIFFERENCE_MAX); // 0-100e6 ns (100 ms)
      vvhTTScTCorrWFD[icrate].push_back(hist);
    }
  }
  return SUCCESS;
}


/*-- module event routine -----------------------------------------*/
INT MTTScTCorrWFD(EVENT_HEADER *pheader, void *pevent) {
  const std::map< std::string, std::vector<TPulseIsland*> >& pulses_map =
    gData->fPulseIslandToChannelMap;
  const std::map< std::string, std::vector<int64_t> >& hits_map =
    gData->fTDCHitsToChannelMap;

  const std::string mu_bank = gSetup->GetBankName("TTSc");
  if (!hits_map.count(mu_bank)) {
    printf("MTTScTCorrWFD: No reference hits TTSc!\n");
    return SUCCESS;
  }
  std::vector<int64_t> mu_hits = hits_map.at(mu_bank);
  std::sort(mu_hits.begin(), mu_hits.end());

  for (int icrate = 0; icrate < NCRATE; ++icrate) {
    // char sync_det[16]; sprintf("SyncCrate%d", icrate);
    // const double t0_crate = WFD_TICK[icrate] * pulses_map.at(gSetup->GetBankName(sync_det))[0]->GetTimeStamp();
    for (int ichan = 0; ichan < NCHAN[icrate]; ++ichan) {
      char bank[8]; sprintf(bank, "D%d%02d", icrate, ichan);
      if (!pulses_map.count(bank)) continue;

      std::vector<int> pulses = calculate_tpi_times(pulses_map.at(bank));
      std::sort(pulses.begin(), pulses.end());
      const double wfd_tick = gSetup->GetClockTick(bank);
      //const double wfd_tick = WFD_TICK[icrate];
      printf("WFD tick %s: %g\n", bank, wfd_tick);
      for (int ipulse = 0; ipulse < pulses.size(); ++ipulse) {
        //if (pulses[ipulse]->GetPulseHeight() < 600) continue;
        const double tpulse = wfd_tick * pulses[ipulse];
        for (int imu = 0; imu < mu_hits.size(); ++imu) {
          static const double tdc_tick = 0.025; // ns
          const double tmu = tdc_tick * mu_hits[imu];
          // const double dt = tpulse - tmu;
          // if (dt < -TIME_DIFFERENCE_MAX)
            // continue;
          // else if (dt < TIME_DIFFERENCE_MAX)
            vvhTTScTCorrWFD[icrate][ichan]->Fill(tpulse, tmu);
          // else
            // break;
        //const double dt = tpulse - tdc_tick * mu_hits[imu];
        //if (dt < TIME_LOW)
        //  break;
        //else if (dt < TIME_HIGH)
        //  vvhTTScTCorrWFD[icrate][ichan]->Fill(dt);
        }
      }
    }
  }

  return SUCCESS;
}

std::vector<int> calculate_tpi_times(const std::vector<TPulseIsland*>& tpis) {
  std::vector<int> ts;
  ts.reserve(tpis.size());
  for (int i = 0; i < tpis.size(); ++i)
    ts.push_back(tpis[i]->GetTimeStamp());
  return ts;
}

/// @}
