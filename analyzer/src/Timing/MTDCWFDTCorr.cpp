////////////////////////////////////////////////////////////////////////////////
/// \defgroup MTDCWFDTcorr
/// \author John R Quirk
///
/// \brief
/// Timing correlations from within TDC
/// @{
////////////////////////////////////////////////////////////////////////////////

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <cstring>
#include <string>
#include <map>
#include <cmath>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include "TH1D.h"
#include "TH2D.h"
#include "TDirectory.h"

/* AlCap includes */
#include "AlCap.h"
#include "TGlobalData.h"
#include "TSetupData.h"


/*-- Module declaration --------------------------------------------*/
extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

using namespace AlCap;
using std::string;
using std::vector;
using std::map;

static INT MTDCWFDTCorr_init(void);
static INT MTDCWFDTCorr_eor(INT);
static INT MTDCWFDTCorr(EVENT_HEADER*, void*);

namespace {
  TDirectory * DIR;
  const double TIME_LOW = -1e5, TIME_HIGH = 5e5; //ns
  TH2* vvhTDCWFDTCorrT[NCRATE][MAXNCHANWFD];
  TH2* vvhTDCWFDTCorrE[NCRATE][MAXNCHANWFD];
  TH2* vvhTDCWFDTCorrT_Norm[NCRATE][MAXNCHANWFD];
  TH2* vvhTDCWFDTCorrE_Norm[NCRATE][MAXNCHANWFD];
  string WFD_TDC_BANK[NCRATE][MAXNCHANWFD];
  string WFD_BANK_NAME[NCRATE][MAXNCHANWFD];
}

ANA_MODULE MTDCWFDTCorr_module =
{
  "MTDCWFDTCorr",    /* module name           */
  "John R Quirk",    /* author                */
  MTDCWFDTCorr,      /* event routine         */
  NULL,              /* BOR routine           */
  MTDCWFDTCorr_eor,  /* EOR routine           */
  MTDCWFDTCorr_init, /* init routine          */
  NULL,              /* exit routine          */
  NULL,              /* parameter structure   */
  0,                 /* structure size        */
  NULL,              /* initial parameters    */
};

/*--module init routine --------------------------------------------*/
INT MTDCWFDTCorr_init() {
  TDirectory* cwd = gDirectory;
  DIR = gDirectory->mkdir("TDCWFDTCorr/");
  DIR->cd();

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
        printf("INFO: MTDCWFDTCorr: No corresponding TDC for bank %s (%s)\n",
               wfd_bank, det.c_str());
	WFD_TDC_BANK[icrate][ich] = WFD_BANK_NAME[icrate][ich] = "";
        continue;
      }

      const double emax = std::pow(2., gSetup->GetNBits(wfd_bank));
      WFD_BANK_NAME[icrate][ich] = wfd_bank;
      WFD_TDC_BANK[icrate][ich]  = tdc_bank;
      printf("TDC bank %s, WFD bank %s, Detector %s\n",
	     tdc_bank, wfd_bank, det.c_str());
      char hist[128];
      sprintf(hist, "hTDCWFDTCorr_%s_T", det.c_str());
      vvhTDCWFDTCorrT[icrate][ich] = new TH2D(hist, hist, 2000, TIME_LOW, TIME_HIGH, 200, 0., 100.e6);
      vvhTDCWFDTCorrT[icrate][ich]->Sumw2();
      vvhTDCWFDTCorrT[icrate][ich]->SetTitle("Timing Correlation");
      vvhTDCWFDTCorrT[icrate][ich]->GetXaxis()->SetTitle("Timing Difference TDC-WFD (ns)");
      vvhTDCWFDTCorrT[icrate][ich]->GetYaxis()->SetTitle("TDC Block Time (ns)");

      sprintf(hist, "hTDCWFDTCorrE_%s", det.c_str());
      vvhTDCWFDTCorrE[icrate][ich] = new TH2D(hist, hist, 2000, TIME_LOW, TIME_HIGH, emax/8, 0., emax);
      vvhTDCWFDTCorrE[icrate][ich]->GetXaxis()->SetTitle("Timing Difference (ns)");

      vvhTDCWFDTCorrE[icrate][ich]->GetYaxis()->SetTitle("Energy (ADC)");
      sprintf(hist, "hTDCWFDTCorr_%s_Norm_T", det.c_str());
      vvhTDCWFDTCorrT_Norm[icrate][ich] = new TH2D(hist, hist, 2000, TIME_LOW, TIME_HIGH, 200, 0., 100.e6);
      vvhTDCWFDTCorrT_Norm[icrate][ich]->Sumw2();
      vvhTDCWFDTCorrT_Norm[icrate][ich]->SetTitle("Timing Correlation (Normalized)");
      vvhTDCWFDTCorrT_Norm[icrate][ich]->GetXaxis()->SetTitle("Timing Difference TDC-WFD (ns)");
      vvhTDCWFDTCorrT_Norm[icrate][ich]->GetYaxis()->SetTitle("TDC Block Time (ns)");
      sprintf(hist, "hTDCWFDTCorr_%s_Norm_E", det.c_str());
      vvhTDCWFDTCorrE_Norm[icrate][ich] = new TH2D(hist, hist, 2000, TIME_LOW, TIME_HIGH, emax/16, 0., emax);
      vvhTDCWFDTCorrE_Norm[icrate][ich]->Sumw2();
      vvhTDCWFDTCorrE_Norm[icrate][ich]->SetTitle("Timing Correlation (Normalized)");
      vvhTDCWFDTCorrE_Norm[icrate][ich]->GetXaxis()->SetTitle("Timing Difference TDC-WFD (ns)");
      vvhTDCWFDTCorrE_Norm[icrate][ich]->GetYaxis()->SetTitle("Energy (ADC)");
      
    }
  }

  cwd->cd();
  return SUCCESS;
}

INT MTDCWFDTCorr_eor(INT run_number) {
  TDirectory* cwd = gDirectory;
  DIR->cd();
  for (int icrate = 0; icrate < NCRATE; ++icrate)
    for (int ich = 0; ich < NCHANWFD[icrate]; ++ich)
      if (vvhTDCWFDTCorrT_Norm[icrate][ich]) {
	vvhTDCWFDTCorrT_Norm[icrate][ich]->Scale(1./gData->NBlocks());
	vvhTDCWFDTCorrE_Norm[icrate][ich]->Scale(1./gData->NBlocks());
	string name(vvhTDCWFDTCorrT[icrate][ich]->GetName());
	name.erase(name.size()-2);
	vvhTDCWFDTCorrT[icrate][ich]->ProjectionX(name.c_str());
	name = vvhTDCWFDTCorrT_Norm[icrate][ich]->GetName();
	name.erase(name.size()-2);
	vvhTDCWFDTCorrT_Norm[icrate][ich]->ProjectionX(name.c_str());
      }
  cwd->cd();
}

/*-- module event routine -----------------------------------------*/
INT MTDCWFDTCorr(EVENT_HEADER *pheader, void *pevent) {
  const map< string, vector<int64_t> >& tdc_map =
      gData->fTDCHitsToChannelMap;
  const map< string, vector<TPulseIsland*> >& wfd_map =
      gData->fPulseIslandToChannelMap;
 
  for (int icrate = 0; icrate < NCRATE; ++icrate) {
    if (gData->fTDCSynchronizationPulseIndex[icrate] == -1) continue;
    const double toff = gData->fTDCSynchronizationPulseOffset[icrate];

    for (int ich = 0; ich < NCHANWFD[icrate]; ++ich) {
      const string& tdc_bank = WFD_TDC_BANK[icrate][ich];
      const string& wfd_bank = WFD_BANK_NAME[icrate][ich];
      if (tdc_bank.empty() || wfd_bank.empty()) continue;
      if (!tdc_map.count(WFD_TDC_BANK[icrate][ich]) ||
	  !wfd_map.count(WFD_BANK_NAME[icrate][ich])) continue;

      const vector<int64_t>&       times  = tdc_map.at(tdc_bank);
      const vector<TPulseIsland*>& pulses = wfd_map.at(wfd_bank);
      for (int t = 0, p0 = 0; t < times.size(); ++t) {
	const double norm = 2./(times.size()+pulses.size());
        for (int p = p0; p < pulses.size(); ++p) {
	  double time_wfd = 0;
	  const vector<int>& samples = pulses[p]->GetSamples();


          const double dt = TICKTDC*times[t] -
                            TICKWFD[icrate]*pulses[p]->GetTimeStamp() -
	                    toff;
          if (dt < TIME_LOW) {
            break;
          } else if (dt < TIME_HIGH) {
            vvhTDCWFDTCorrT[icrate][ich]->Fill(dt, TICKTDC*times[t]);
            vvhTDCWFDTCorrE[icrate][ich]->Fill(dt, pulses[p]->GetPulseHeight());
            vvhTDCWFDTCorrT_Norm[icrate][ich]->Fill(dt, TICKTDC*times[t], norm);
            vvhTDCWFDTCorrE_Norm[icrate][ich]->Fill(dt, pulses[p]->GetPulseHeight(), norm);
	  } else {
	    ++p0;
	  }
        }
      }
    }
  }

  return SUCCESS;
}

/// @}
