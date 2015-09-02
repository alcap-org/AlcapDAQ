////////////////////////////////////////////////////////////////////////////////
/// \defgroup MTDCWFDMatchEff
/// \author John R Quirk
///
/// \brief
/// Timing correlation matching efficiency.
/// @{
////////////////////////////////////////////////////////////////////////////////

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <cstring>
#include <string>
#include <map>
#include <algorithm>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include "TH1D.h"
#include "TDirectory.h"

/* AlCap includes */
#include "AlCap.h"
#include "TGlobalData.h"
#include "TSetupData.h"


/*-- Module declaration --------------------------------------------*/
static INT MTDCWFDMatchEff_init(void);
static INT MTDCWFDMatchEff(EVENT_HEADER*, void*);

static std::vector<double> calc_times(const std::vector<TPulseIsland*>&, const double, const double);
static std::vector<double> calc_times(const std::vector<int64_t>&, const double, const double);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

using namespace AlCap;
namespace {
  const double TIME_LOW = -300., TIME_HIGH = 300; //ns
  TH1* vvhTDCWFDMatchEff[NCRATE][MAXNCHANWFD][2];
  std::string BANKS[NCRATE][MAXNCHANWFD][2];
}

ANA_MODULE MTDCWFDMatchEff_module =
{
  "MTDCWFDMatchEff",    /* module name           */
  "John R Quirk",       /* author                */
  MTDCWFDMatchEff,      /* event routine         */
  NULL,                 /* BOR routine           */
  NULL,                 /* EOR routine           */
  MTDCWFDMatchEff_init, /* init routine          */
  NULL,                 /* exit routine          */
  NULL,                 /* parameter structure   */
  0,                    /* structure size        */
  NULL,                 /* initial parameters    */
};

/*--module init routine --------------------------------------------*/
INT MTDCWFDMatchEff_init() {
  TDirectory* cwd = gDirectory;
  gDirectory->mkdir("TDCWFDMatchEff/")->cd();

  for (int icrate = 0; icrate < NCRATE; ++icrate) {
    for (int ich = 0; ich < NCHANWFD[icrate]; ++ich) {
      char tdc_bank[5], wfd_bank[5];
      sprintf(wfd_bank, "D%d%02d", icrate, ich);
      const std::string det = gSetup->GetDetectorName(wfd_bank);
      if (det.substr(0, 2) == "Ge")
	sprintf(tdc_bank, "%s", gSetup->GetBankName("TGeCHT").c_str());
      else
	sprintf(tdc_bank, "%s", gSetup->GetBankName("T"+det).c_str());

      if (!strlen(tdc_bank)) {
        printf("INFO: MTDCWFDTCorr: No corresponding TDC for bank %s\n",
               wfd_bank);
	BANKS[icrate][ich][0] = BANKS[icrate][ich][1] = "";
        continue;
      }

      BANKS[icrate][ich][0] = wfd_bank;
      BANKS[icrate][ich][1] = tdc_bank;
      printf("TDC bank %s, WFD bank %s, Detector %s\n",
	     tdc_bank, wfd_bank, det.c_str());
      char title[64], fmt[64];
      sprintf(fmt, "h%%sMatchEf_%s", det.c_str());
      sprintf(title, fmt, "WFD");
      vvhTDCWFDMatchEff[icrate][ich][0] = new TH1I(title, title, 10, 0., 10.);
      vvhTDCWFDMatchEff[icrate][ich][0]->GetXaxis()->SetTitle("Matches");
      sprintf(title, fmt, "TDC");
      vvhTDCWFDMatchEff[icrate][ich][1] = new TH1I(title, title, 10, 0., 10.);
      vvhTDCWFDMatchEff[icrate][ich][1]->GetXaxis()->SetTitle("Matches");
    }
  }

  cwd->cd();
  return SUCCESS;
}


/*-- module event routine -----------------------------------------*/
INT MTDCWFDMatchEff(EVENT_HEADER *pheader, void *pevent) {
  const std::map< std::string, std::vector<int64_t> >& tdc_map =
      gData->fTDCHitsToChannelMap;
  const std::map< std::string, std::vector<TPulseIsland*> >& wfd_map =
      gData->fPulseIslandToChannelMap;

  for (int icrate = 0; icrate < NCRATE; ++icrate) {
    if (gData->fTDCSynchronizationPulseIndex[icrate] == -1) continue;
    const double toff = gData->fTDCSynchronizationPulseOffset[icrate];

    for (int ich = 0; ich < NCHANWFD[icrate]; ++ich) {
      const std::string& wfd_bank = BANKS[icrate][ich][0];
      const std::string& tdc_bank = BANKS[icrate][ich][1];
      if (!vvhTDCWFDMatchEff[icrate][ich][0] ||
	  !tdc_map.count(tdc_bank) || !wfd_map.count(wfd_bank)) continue;

      const std::vector<double>& hits   = calc_times(tdc_map.at(tdc_bank), TICKTDC, 0.);
      const std::vector<double>& pulses = calc_times(wfd_map.at(wfd_bank), TICKWFD[icrate], toff);
      std::vector<double>::const_iterator h0 = hits.begin(), p0 = pulses.begin();
      std::vector<double>::const_iterator hf = hits.end(),   pf = pulses.end();
      for (std::vector<double>::const_iterator ih = h0, ip = p0;
	   ih < hf && ip < pf; ++ih, ++ip) {
	p0 = std::upper_bound(p0, pf, (*ih)+TIME_LOW);
	h0 = std::upper_bound(h0, hf, (*ip)+TIME_LOW);
	const int nh = std::upper_bound(p0, pf, (*ih)+TIME_HIGH) - p0;
	const int np = std::upper_bound(h0, hf, (*ip)+TIME_HIGH) - h0;
	vvhTDCWFDMatchEff[icrate][ich][0]->Fill(np);
	vvhTDCWFDMatchEff[icrate][ich][1]->Fill(nh);
      }
    }
  }
  return SUCCESS;
}

std::vector<double> calc_times(const std::vector<TPulseIsland*>& v, const double dt, const double toff) {
  std::vector<double> t(v.size());
  for (int i = 0; i < v.size(); ++i)
    t[i] = dt*v[i]->GetTimeStamp() + toff;
  return t;
}

std::vector<double> calc_times(const std::vector<int64_t>& v, const double dt, const double toff) {
  std::vector<double> t(v.size());
  for (int i = 0; i < v.size(); ++i)
    t[i] = dt*v[i] + toff;
  return t;
}

/// @}
