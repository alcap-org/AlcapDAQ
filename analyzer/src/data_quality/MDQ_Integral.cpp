////////////////////////////////////////////////////////////////////////////////
/// \defgroup MDQ_Integral
/// \ingroup lldq
/// \author Damien Alexander
///
///////////////////////////////////////////////////////////////////////////////

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
#include <TH2.h>
#include <TDirectory.h>
#include "TFile.h"
#include "TApplication.h"
#include "TROOT.h"

/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"
#include "TPulseIsland.h"


/*-- Module declaration --------------------------------------------*/
INT  MDQ_Integral_init(void);
INT  MDQ_Integral(EVENT_HEADER*, void*);
INT  MDQ_Integral_eor(INT);
INT  BookHistograms();

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

std::map<std::string, TH1F*> DQ_Integral_histograms_map;
std::map<std::string, TH1F*> DQ_Integral_histograms_ped_sub_map;

bool DQ_Integral_firstEvent = true;

ANA_MODULE MDQ_Integral_module =
{
	"MDQ_Integral",                /* module name           */
	"Damien Alexander",            /* author                */
	MDQ_Integral,                  /* event routine         */
	NULL,                          /* BOR routine           */
	MDQ_Integral_eor,              /* EOR routine           */
	MDQ_Integral_init,             /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};

/////////////////////////////////////////////////////////////////


INT MDQ_Integral_init(){
  return SUCCESS;
}


INT BookHistograms() 
{
  TDirectory* cwd = gDirectory;
  gDirectory->mkdir("DQ_Integral")->cd();

  const std::map<std::string, std::vector<TPulseIsland*> >& tpi_map = gData->fPulseIslandToChannelMap;

  for(std::map<std::string, std::vector<TPulseIsland*> >::const_iterator mIter = tpi_map.begin(); mIter != tpi_map.end(); mIter++){
    std::string bankname = mIter->first;
    std::string detname = gSetup->GetDetectorName(bankname);
    //at this point I'd like the length of TPulseIslands to set my maximum
    const std::vector<TPulseIsland*>& pulses = mIter->second;
    std::vector<TPulseIsland*>::const_iterator firstPulse = pulses.begin();
    if(pulses.size() <= 1) continue;
    firstPulse++; // just in case
    std::vector<int> samples = (*firstPulse)->GetSamples();
    int n_samples = samples.size();
    const int max_adc = std::pow(2, gSetup->GetNBits(bankname));

    int max_bin_PS = max_adc * n_samples;
    if(detname != "GeCHT" && detname != "TSc") max_bin_PS = max_bin_PS/10;

    //hDQ_Integral_[detname]_[bankname]
    std::string histname = "hDQ_Integral_" + detname + "_" + bankname;
    std::string histtitle = "Integral of Pulses in " + detname;
    TH1F* hDQ_IntHist = new TH1F(histname.c_str(), histtitle.c_str(), max_adc, 0, max_adc * n_samples);
    hDQ_IntHist->GetXaxis()->SetTitle("Integral ");
    hDQ_IntHist->GetYaxis()->SetTitle("Counts");
    DQ_Integral_histograms_map[bankname] = hDQ_IntHist;

    //hDQ_Integral_[detname]_[bankname]_PedSub
    histname = "hDQ_Integral_" + detname + "_" + bankname + "_PedSub";
    histtitle = "Pedestal Subtracted Integral of Pulses in " + detname;
    TH1F* hDQ_IntHist_PS = new TH1F(histname.c_str(), histtitle.c_str(), max_adc, 0, max_bin_PS);
    hDQ_IntHist_PS->GetXaxis()->SetTitle("Integral (pedestal subtracted)");
    hDQ_IntHist_PS->GetYaxis()->SetTitle("Count");
    DQ_Integral_histograms_ped_sub_map[bankname] = hDQ_IntHist_PS;
  }

  cwd->cd();
  return SUCCESS;
}

INT MDQ_Integral_eor(INT run_number)
{
  return SUCCESS;  // currently nothing to do here
}

INT MDQ_Integral(EVENT_HEADER *pheader, void *pevent)
{
  if(DQ_Integral_firstEvent == true){
    BookHistograms();
    DQ_Integral_firstEvent = false;
  }

  const std::map<std::string, std::vector<TPulseIsland*> >& tpi_map = gData->fPulseIslandToChannelMap;

  for(std::map<std::string, std::vector<TPulseIsland*> >::const_iterator mIter = tpi_map.begin(); mIter != tpi_map.end(); mIter++){
    const std::string bankname = mIter->first;
    const std::vector<TPulseIsland*>& pulses = mIter->second;
    int polarity = gSetup->GetTriggerPolarity(bankname);

    for(std::vector<TPulseIsland*>::const_iterator pIter = pulses.begin(); pIter != pulses.end(); pIter++){
      if(DQ_Integral_histograms_map.find(bankname) == DQ_Integral_histograms_map.end()) continue;

      float integral = 0;
      float integral_ps = 0;
      float pedestal = (*pIter)->GetPedestal(8);

      const std::vector<int>& samples = (*pIter)->GetSamples();
      for(std::vector<int>::const_iterator sIter = samples.begin(); sIter != samples.end(); sIter++){
	integral += (*sIter);
	integral_ps += polarity * ((*sIter) - pedestal);
      }

      DQ_Integral_histograms_map[bankname]->Fill(integral);
      DQ_Integral_histograms_ped_sub_map[bankname]->Fill(integral_ps);
    }
  }

  return SUCCESS;
}
