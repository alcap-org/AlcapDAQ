////////////////////////////////////////////////////////////////////////////////
/// \defgroup MTScTCorrWFD
///
/// \brief
/// Timing correlations from within WFD
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
static const int NCHANV1724  = 8;
static const int NCHANV1720 = 4;
static const int NCHANDT5730 = 8;
static const int NCHANDT5720 = 4;
const double TIME_LOW = -25e3, TIME_HIGH = 25e3;
static TH1* vhTScTCorrWFD[NCHANV1724+NCHANV1720+NCHANDT5730+NCHANDT5720];


ANA_MODULE MTScTCorrWFD_module =
{
  "MDT5730TScTCorrWFD",    /* module name           */
  "John R Quirk",          /* author                */
  MTScTCorrWFD,            /* event routine         */
  NULL,                    /* BOR routine           */
  NULL,                    /* EOR routine           */
  MTScTCorrWFD_init,       /* init routine          */
  NULL,                    /* exit routine          */
  NULL,                    /* parameter structure   */
  0,                       /* structure size        */
  NULL,                    /* initial parameters    */
};

/*--module init routine --------------------------------------------*/
INT MTScTCorrWFD_init() {
  int ihist = 0;
  for (int ich = 0; ich < NCHANV1724; ++ich, ++ihist) {
    char bank[5]; sprintf(bank, "D4%02d", ich);
    char hist[64]; sprintf(hist, "hTScTCorrWFD_%s", bank);
    vhTScTCorrWFD[ihist] = new TH1D(hist, hist, 100000, TIME_LOW, TIME_HIGH);
    vhTScTCorrWFD[ihist]->GetXaxis()->SetTitle("Timing Difference (ns)");
  }
  for (int ich = 0; ich < NCHANV1720; ++ich, ++ihist) {
    char bank[5]; sprintf(bank, "D8%02d", ich);
    char hist[64]; sprintf(hist, "hTScTCorrWFD_%s", bank);
    vhTScTCorrWFD[ihist] = new TH1D(hist, hist, 100000, TIME_LOW, TIME_HIGH);
    vhTScTCorrWFD[ihist]->GetXaxis()->SetTitle("Timing Difference (ns)");
  }
  for (int ich = 0; ich < NCHANDT5730; ++ich, ++ihist) {
    char bank[5]; sprintf(bank, "D7%02d", ich);
    char hist[64]; sprintf(hist, "hTScTCorrWFD_%s", bank);
    vhTScTCorrWFD[ihist] = new TH1D(hist, hist, 100000, TIME_LOW, TIME_HIGH);
    vhTScTCorrWFD[ihist]->GetXaxis()->SetTitle("Timing Difference (ns)");
  }
  for (int ich = 0; ich < NCHANDT5720; ++ich, ++ihist) {
    char bank[5]; sprintf(bank, "D7%02d", ich);
    char hist[64]; sprintf(hist, "hTScTCorrWFD_%s", bank);
    vhTScTCorrWFD[ihist] = new TH1D(hist, hist, 100000, TIME_LOW, TIME_HIGH);
    vhTScTCorrWFD[ihist]->GetXaxis()->SetTitle("Timing Difference (ns)");
  }
  return SUCCESS;
}


/*-- module event routine -----------------------------------------*/
INT MTScTCorrWFD(EVENT_HEADER *pheader, void *pevent) {
  const std::map< std::string, std::vector<TPulseIsland*> >& wfd_map = gData->fPulseIslandToChannelMap;
  const std::string ref_bank = gSetup->GetBankName("TSc");
  if (!wfd_map.count(ref_bank)) {
    printf("MPreTargTCorrWFD: No reference hits TSc!\n");
    return SUCCESS;
  }
  const std::vector<TPulseIsland*>& ref_pulses = wfd_map.at(ref_bank);

  int ihist = 0;
  for (int ich = 0; ich < NCHANV1724; ++ich, ++ihist) {
    char bank[5]; sprintf(bank, "D4%02d", ich);
    if (!wfd_map.count(bank)) continue;

    const std::vector<TPulseIsland*>& pulses = wfd_map.at(bank);
    for (int i = 0; i < pulses.size(); ++i) {
      for (int j = 0; j < ref_pulses.size(); ++j) {
        const int64_t dt = pulses[i]->GetPulseTime() - ref_pulses[j]->GetPulseTime();
        if (dt < TIME_LOW)
          break;
        else if (dt < TIME_HIGH)
          vhTScTCorrWFD[ihist]->Fill(dt);
      }
    }
  }
  for (int ich = 0; ich < NCHANV1720; ++ich, ++ihist) {
    char bank[5]; sprintf(bank, "D8%02d", ich);
    if (!wfd_map.count(bank)) continue;

    const std::vector<TPulseIsland*>& pulses = wfd_map.at(bank);
    for (int i = 0; i < pulses.size(); ++i) {
      for (int j = 0; j < ref_pulses.size(); ++j) {
        const int64_t dt = pulses[i]->GetPulseTime() - ref_pulses[j]->GetPulseTime();
        if (dt < TIME_LOW)
          break;
        else if (dt < TIME_HIGH)
          vhTScTCorrWFD[ihist]->Fill(dt);
      }
    }
  }
  for (int ich = 0; ich < NCHANDT5730; ++ich, ++ihist) {
    char bank[5]; sprintf(bank, "D7%02d", ich);
    if (!wfd_map.count(bank)) continue;

    const std::vector<TPulseIsland*>& pulses = wfd_map.at(bank);
    for (int i = 0; i < pulses.size(); ++i) {
      for (int j = 0; j < ref_pulses.size(); ++j) {
        const int64_t dt = pulses[i]->GetPulseTime() - ref_pulses[j]->GetPulseTime();
        if (dt < TIME_LOW)
          break;
        else if (dt < TIME_HIGH)
          vhTScTCorrWFD[ihist]->Fill(dt);
      }
    }
  }
  for (int ich = 0; ich < NCHANDT5720; ++ich, ++ihist) {
    char bank[5]; sprintf(bank, "D7%02d", ich);
    if (!wfd_map.count(bank)) continue;

    const std::vector<TPulseIsland*>& pulses = wfd_map.at(bank);
    for (int i = 0; i < pulses.size(); ++i) {
      for (int j = 0; j < ref_pulses.size(); ++j) {
        const int64_t dt = pulses[i]->GetPulseTime() - ref_pulses[j]->GetPulseTime();
        if (dt < TIME_LOW)
          break;
        else if (dt < TIME_HIGH)
          vhTScTCorrWFD[ihist]->Fill(dt);
      }
    }
  }
  return SUCCESS;
}

/// @}
