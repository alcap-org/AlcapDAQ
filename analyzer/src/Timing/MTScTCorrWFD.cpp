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
#include "TDirectory.h"

//JG: added alcap includes
/* AlCap includes */
#include "AlCap.h"
#include "TGlobalData.h"
#include "TSetupData.h"


/*-- Module declaration --------------------------------------------*/
static INT MTScTCorrWFD_init(void);
static INT MTScTCorrWFD(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

using namespace AlCap;
namespace {
  const double TIME_LOW = -20e3, TIME_HIGH = 20e3;
  TH1* vvhTScTCorrWFD[NCRATE][MAXNCHANWFD];
  std::string WFDBANKS[NCRATE][MAXNCHANWFD];
}

ANA_MODULE MTScTCorrWFD_module =
{
  "MTScTCorrWFD",    /* module name           */
  "John R Quirk",    /* author                */
  MTScTCorrWFD,      /* event routine         */
  NULL,              /* BOR routine           */
  NULL,              /* EOR routine           */
  MTScTCorrWFD_init, /* init routine          */
  NULL,              /* exit routine          */
  NULL,              /* parameter structure   */
  0,                 /* structure size        */
  NULL,              /* initial parameters    */
};

/*--module init routine --------------------------------------------*/
INT MTScTCorrWFD_init() {
  TDirectory* cwd = gDirectory;
  gDirectory->mkdir("TScTCorrWFD")->cd();

  for (int icrate = 0; icrate < NCRATE; ++icrate) {
    for (int ich = 0; ich < NCHANWFD[icrate]; ++ich) {
      char bank[5]; sprintf(bank, "D%d%02d", icrate, ich);
      char histname[64]; sprintf(histname, "hTScTCorrWFD_%s", bank);
      char histtitle[64]; sprintf(histtitle, "TSc TCorr with %s", gSetup->GetDetectorName(bank).c_str());
      vvhTScTCorrWFD[icrate][ich] = new TH1D(histname, histtitle, 20000, TIME_LOW, TIME_HIGH);
      vvhTScTCorrWFD[icrate][ich]->GetXaxis()->SetTitle("Timing Difference (ns)");
      WFDBANKS[icrate][ich] = bank;
    }
  }
  cwd->cd();
  return SUCCESS;
}

/*-- module event routine -----------------------------------------*/
INT MTScTCorrWFD(EVENT_HEADER *pheader, void *pevent) {
  const std::map< std::string, std::vector<TPulseIsland*> >& wfd_map =
    gData->fPulseIslandToChannelMap;

  const static std::string ref_bank = gSetup->GetBankName("TSc");
  if (!wfd_map.count(ref_bank)) {
    printf("MTScTCorrWFD: No reference pulses TSc!\n");
    return SUCCESS;
  }
  const std::vector<TPulseIsland*>& ref_pulses = wfd_map.at(ref_bank);
  const static double tickref = TICKWFD[ref_bank[1]-'0'];

  for (int icrate = 0; icrate < NCRATE; ++icrate) {
    for (int ich = 0; ich < NCHANWFD[icrate]; ++ich) {
      if (!wfd_map.count(WFDBANKS[icrate][ich])) continue;

      const std::vector<TPulseIsland*>& pulses = wfd_map.at(WFDBANKS[icrate][ich]);

      for (int i = 0, j0 = 0; i < pulses.size(); ++i) {
        //if (pulses[ip]->GetPulseHeight() < 600) continue;
        const double t = TICKWFD[icrate] * pulses[i]->GetTimeStamp();
        for (int j = j0; j < ref_pulses.size(); ++j) {
          const double dt = t - tickref * ref_pulses[j]->GetTimeStamp();
          if (dt < TIME_LOW)
            break;
          else if (dt < TIME_HIGH)
            vvhTScTCorrWFD[icrate][ich]->Fill(dt);
	  else
	    ++j0;
        }
      }
    }
  }

  return SUCCESS;
}

/// @}
