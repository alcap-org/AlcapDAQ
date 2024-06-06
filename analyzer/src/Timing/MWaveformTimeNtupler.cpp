////////////////////////////////////////////////////////////////////////////////
/// \defgroup MWaveformTimeNtupler
/// \ingroup timing
/// \author Cole Kampa
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
#include <algorithm>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include <TH1.h>
#include <TH2.h>
#include <TDirectory.h>
#include "TF1.h"
#include "TFile.h"
#include "TApplication.h"
#include "TROOT.h"
//#include "TMath.h"
//#include "Math/Interpolator.h"
#include "TNtuple.h"

/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"
#include "TPulseIsland.h"

using namespace std;

/*-- Module declaration --------------------------------------------*/
INT  MWaveformTimeNtupler_init(void);
INT  MWaveformTimeNtupler(EVENT_HEADER*, void*);
INT  MWaveformTimeNtupler_eor(INT);
INT  MWaveformTimeNtupler_BookHistograms();
// flip polarity
//std::vector<int> MEnergyPlotsRedux_FlipSamplePolarity(const std::vector<int>&, const int, int);
// define pedestal parameters
//void MEnergyPlotsRedux_SetPedestalParams(const std::vector<int>&, int, int, std::string, float&, float&, float&, float&, float&);
// integration (analytic)
//float MEnergyPlotsRedux_GetPedestalIntegral(int, int, float&, float&, float&);
// general integration (Simpson's)
//float MEnergyPlotsRedux_GetIntegral(const std::vector<int>&, int, int);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

std::map<std::string, TNtuple*> Ntuple_map_TimeStamp;
//std::map<std::string, TH1F*> IntegralEnergy_map, AmplitudeEnergy_map, FitEnergy_map, Integral_map, Amplitude_map, Fit_map;
//std::map<std::string, TH2F*> IntegralVAmplitude_map;

bool WaveformTimeNtupler_firstEvent = true;
//int block = 0;

ANA_MODULE MWaveformTimeNtupler_module =
{
	"MWaveformTimeNtupler",                  /* module name           */
	"Cole Kampa",            /* author                */
	MWaveformTimeNtupler,                    /* event routine         */
	NULL,                          /* BOR routine           */
	MWaveformTimeNtupler_eor,                /* EOR routine           */
	MWaveformTimeNtupler_init,               /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};

/////////////////////////////////////////////////////////////////

INT MWaveformTimeNtupler_init(){
  return SUCCESS;
}

INT MWaveformTimeNtupler_BookHistograms() 
{
  TDirectory* cwd = gDirectory;
  gDirectory->mkdir("WaveformTimeNtupler")->cd();

  const std::map<std::string, std::vector<TPulseIsland*> >& tpi_map = gData->fPulseIslandToChannelMap;

  for(std::map<std::string, std::vector<TPulseIsland*> >::const_iterator mIter = tpi_map.begin(); mIter != tpi_map.end(); mIter++){
    std::string bankname = mIter->first;
    std::string detname = gSetup->GetDetectorName(bankname);
    // ntuple
    std::string ntupname = "ntup_" + detname + "_" + bankname;
    std::string ntuptitle = "ntuple for " + detname;
    TNtuple* ntuple = new TNtuple(ntupname.c_str(), ntuptitle.c_str(), "Event:iPulse:Timestamp:PulseHeight");
    Ntuple_map_TimeStamp[bankname] = ntuple;
  }

  cwd->cd();
  return SUCCESS;
}

INT MWaveformTimeNtupler_eor(INT run_number)
{
  return SUCCESS;  // currently nothing to do here
}

INT MWaveformTimeNtupler(EVENT_HEADER *pheader, void *pevent)
{
  // Get the event number
  int midas_event_number = pheader->serial_number;
  //block++;
  if(WaveformTimeNtupler_firstEvent == true){
    MWaveformTimeNtupler_BookHistograms();
    WaveformTimeNtupler_firstEvent = false;
  }

  const std::map<std::string, std::vector<TPulseIsland*> >& tpi_map = gData->fPulseIslandToChannelMap;

  for(std::map<std::string, std::vector<TPulseIsland*> >::const_iterator mIter = tpi_map.begin(); mIter != tpi_map.end(); mIter++){
    const std::string bankname = mIter->first;
    const std::string detname = gSetup->GetDetectorName(bankname);
    const std::vector<TPulseIsland*>& pulses = mIter->second;

    int iPulse=0;
    int fTimestamp;
    double PulseHeight;

    for(std::vector<TPulseIsland*>::const_iterator pIter = pulses.begin(); pIter != pulses.end(); pIter++){
      iPulse++;
      fTimestamp = (*pIter)->GetTimeStamp();
      // pedestal subtracted pulse height (pedestal defined as mean of first N=10 samples)
      // accounts for polarity --> should always be > 0.
      PulseHeight = (*pIter)->GetPulseHeight();
      // fill the ntuple
      //const float vals [3] = {block, iPulse, fTimestamp};
      const float vals [4] = {midas_event_number, iPulse, fTimestamp, PulseHeight};
      Ntuple_map_TimeStamp[bankname]->Fill(vals);
    }
  }

  return SUCCESS;
}

