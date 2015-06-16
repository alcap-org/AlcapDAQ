////////////////////////////////////////////////////////////////////////////////
/// \defgroup MEVsTScTCorrWFD
///
/// \brief
/// Timing correlations from within WFD plotted against energy
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
#include "TH2D.h"

/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"


/*-- Module declaration --------------------------------------------*/
static INT MEVsTScTCorrWFD_init(void);
static INT MEVsTScTCorrWFD(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

const double TIME_LOW = -10e3, TIME_HIGH = 10e3;
static TH2D* vhEVsTScTCorrWFD[2];


ANA_MODULE MEVsTScTCorrWFD_module =
{
  "MEVsTScTCorrWFD",       /* module name           */
  "Damien Alexander",      /* author                */
  MEVsTScTCorrWFD,         /* event routine         */
  NULL,                    /* BOR routine           */
  NULL,                    /* EOR routine           */
  MEVsTScTCorrWFD_init,    /* init routine          */
  NULL,                    /* exit routine          */
  NULL,                    /* parameter structure   */
  0,                       /* structure size        */
  NULL,                    /* initial parameters    */
};

/*--module init routine --------------------------------------------*/
INT MEVsTScTCorrWFD_init() {
  const unsigned channels = 2;
  const std::string Ndet[channels] = {gSetup->GetBankName("NdetD"), gSetup->GetBankName("NdetU")};

  for(unsigned i = 0; i< channels; i++){
    std::string histname = "h" + Ndet[i] + "EVsTScTCorrWFD";
    std::string histtitle = "Energy vs time from TSc hit for " + Ndet[i];
    vhEVsTScTCorrWFD[i] = new TH2D(histname.c_str(), histtitle.c_str(), 5000, TIME_LOW, TIME_HIGH, 4096, 0, 6.5);
    vhEVsTScTCorrWFD[i]->GetXaxis()->SetTitle("Timing Difference (ns)");
    vhEVsTScTCorrWFD[i]->GetYaxis()->SetTitle("Energy (MeVee)");
  }
  return SUCCESS;
}


/*-- module event routine -----------------------------------------*/
INT MEVsTScTCorrWFD(EVENT_HEADER *pheader, void *pevent) {
  const std::map< std::string, std::vector<TPulseIsland*> >& wfd_map = gData->fPulseIslandToChannelMap;
  const std::string ref_bank = gSetup->GetBankName("TSc");
  if (!wfd_map.count(ref_bank)) {
    printf("MPreTargTCorrWFD: No reference hits TSc!\n");
    return SUCCESS;
  }

  const unsigned channels = 2;

  float calib_slope[channels] = {0.0003999, 0.0004015};
  float calib_inter[channels] = {0.008234, 0.009037};

  const std::vector<TPulseIsland*>& ref_pulses = wfd_map.at(ref_bank);
  const std::string Ndet[channels] = {gSetup->GetBankName("NdetD"), gSetup->GetBankName("NdetU")};

  for(unsigned k =0; k< channels; k++){
    const std::vector<TPulseIsland*>& pulses = wfd_map.at(Ndet[k]);
    for(unsigned i = 0; i < pulses.size(); i++){
      for(unsigned j = 0; j < ref_pulses.size(); j++){
	const int64_t dt = pulses[i]->GetPulseTime() - ref_pulses[j]->GetPulseTime();
	if(dt < TIME_LOW)
	  break;
	else if(dt < TIME_HIGH){
	  const int pulse_height = pulses[i]->GetPulseHeight(); 
	  float Energy = pulse_height*calib_slope[k] + calib_inter[k];
	  vhEVsTScTCorrWFD[k]->Fill(dt, Energy);
	}
      }//j loop
    }//i loop
  }//k loop
  return SUCCESS;
}


