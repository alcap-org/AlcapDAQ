////////////////////////////////////////////////////////////////////////////////
/// \defgroup MTTScWFDTimeNtupler
/// \ingroup Timing
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
#include "AlCap.h"
#include "tOffsets.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "TPulseIsland.h"

using namespace AlCap;
//using namespace std;
namespace {
  std::map<std::string, TNtuple*> Ntuple_map;
  bool firstEvent = true;
}

/*-- Module declaration --------------------------------------------*/
INT  MTTScWFDTimeNtupler_init(void);
INT  MTTScWFDTimeNtupler(EVENT_HEADER*, void*);
INT  MTTScWFDTimeNtupler_eor(INT);
INT  MTTScWFDTimeNtupler_BookHistograms();

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

ANA_MODULE MTTScWFDTimeNtupler_module =
{
	"MTTScWFDTimeNtupler",                  /* module name           */
	"Cole Kampa",            /* author                */
	MTTScWFDTimeNtupler,                    /* event routine         */
	NULL,                          /* BOR routine           */
	MTTScWFDTimeNtupler_eor,                /* EOR routine           */
	MTTScWFDTimeNtupler_init,               /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};

/////////////////////////////////////////////////////////////////

INT MTTScWFDTimeNtupler_init(){
  return SUCCESS;
}

INT MTTScWFDTimeNtupler_BookHistograms()
{
  TDirectory* cwd = gDirectory;
  gDirectory->mkdir("TTScWFDTimeNtupler")->cd();

  const std::map<std::string, std::vector<TPulseIsland*> >& tpi_map = gData->fPulseIslandToChannelMap;

  // loop through ADC pulses
  for(std::map<std::string, std::vector<TPulseIsland*> >::const_iterator mIter = tpi_map.begin(); mIter != tpi_map.end(); mIter++){
    std::string bankname = mIter->first;
    std::string detname = gSetup->GetDetectorName(bankname);
    //at this point I'd like the length of TPulseIslands to set my maximum
    //const std::vector<TPulseIsland*>& pulses = mIter->second;
    //std::vector<TPulseIsland*>::const_iterator firstPulse = pulses.begin();
    //if(pulses.size() <= 1) continue;
    //firstPulse++; // just in case
    //std::vector<int> samples = (*firstPulse)->GetSamples();
    //int n_samples = samples.size();
    //const int max_adc = std::pow(2, gSetup->GetNBits(bankname));

    //double max_bin_PS = max_adc * n_samples;
    //if(detname == "NdetD" || detname == "NdetU") max_bin_PS /= 20;
    //else if(detname == "LaBr3") max_bin_PS /= 6;
    //else if(detname == "GeCHEH" || detname == "GeCHEL") max_bin_PS /= 4;
    //else continue;
    //double max_amp_e = (*firstPulse)->GetEnergyAmp(max_adc);
    //double max_int_e = (*firstPulse)->GetEnergyInt(max_bin_PS);

    //double max_fit = 7.5;
    //if(detname == "GeCHEH") max_fit = 2.5;
    //if(detname == "GeCHEL") max_fit = 6.0;
    //if(detname == "TSc") max_fit = 15000.0;
    //if(detname == "LaBr3") max_fit = 15.0;

    if (detname != "NdetD" && detname != "NdetU") continue;

    // ntuple
    // stores: Integral, Amplitude
    std::string ntupname = "ntup_" + detname + "_" + bankname;
    std::string ntuptitle = "ntuple for " + detname;
    // define ntuple
    // dev notes: Amplitude not strictly necessary, but wanting to check when comparing to EnergyPlots ntuple.
    TNtuple* ntuple = new TNtuple(ntupname.c_str(), ntuptitle.c_str(), "Event:iPulse:Amplitude:TSWFDtick:TSWFD_raw:iSync:SyncOff:TSWFD");
    //TNtuple* ntuple = new TNtuple(ntupname.c_str(), ntuptitle.c_str(), "Event:iPulse:Integral:Amplitude:iMax:nSamp:SIntegral:LIntegral:SIntegral_pedsub:LIntegral_pedsub:IntegralRatio:status_IntegralRatio:Polarity:pedBegin:pedEnd:pedX0:pedSlope:pedInt:pedIntS:pedIntL");
    // turn off autosave
    ntuple->SetAutoSave(0);
    Ntuple_map[bankname] = ntuple;
    
  }

  cwd->cd();
  return SUCCESS;
}

INT MTTScWFDTimeNtupler_eor(INT run_number)
{
  return SUCCESS;  // currently nothing to do here
}

INT MTTScWFDTimeNtupler(EVENT_HEADER *pheader, void *pevent)
{
  // Get the event number
  int midas_event_number = pheader->serial_number;
  if(firstEvent == true){
    MTTScWFDTimeNtupler_BookHistograms();
    firstEvent = false;
  }

  const std::map<std::string, std::vector<TPulseIsland*> >& tpi_map = gData->fPulseIslandToChannelMap;

  for(std::map<std::string, std::vector<TPulseIsland*> >::const_iterator mIter = tpi_map.begin(); mIter != tpi_map.end(); mIter++){
    const std::string bankname = mIter->first;
    const int icrate = std::stoi(bankname.substr(1, 1));
    const std::string detname = gSetup->GetDetectorName(bankname);
    const std::vector<TPulseIsland*>& pulses = mIter->second;
    // SyncPulse
    // toff = synctdc - syncwfd, so wfd+toff puts time on equal footing to TDC.
    // note: if index=-1, toff ~ 0 (closest numerical float value to zero)
    const double toff = gData->fTDCSynchronizationPulseOffset[icrate];
    // index = -1 is the flag for failed sync
    const int iSync = gData->fTDCSynchronizationPulseIndex[icrate];
    // int polarity = gSetup->GetTriggerPolarity(bankname);
    // int nbits = gSetup->GetNBits(bankname);
    // const int max_adc = std::pow(2, gSetup->GetNBits(bankname));

    if (detname != "NdetD" && detname != "NdetU") continue;
    /*
    if(detname != "TSc"
       && detname != "GeCHEH"
       && detname != "GeCHEL"
       && detname != "NdetD"
       && detname != "NdetU"
       && detname != "LaBr3")
      continue;
    */

    // const std::string bankname = mIter->first;
    // const std::string detname = gSetup->GetDetectorName(bankname);
    // const std::vector<TPulseIsland*>& pulses = mIter->second;

    int iPulse=0;
    int Timestamp_tick;
    double fTimestamp_raw, fTimestamp;
    double Amplitude;

    // int iPulse = 0;

    for(std::vector<TPulseIsland*>::const_iterator pIter = pulses.begin(); pIter != pulses.end(); pIter++){
      iPulse++;
      // pulse time
      Timestamp_tick = (*pIter)->GetTimeStamp();
      // convert to ns
      fTimestamp_raw = TICKWFD[icrate]*Timestamp_tick;
      // add SyncPulseOffset (all times are w.r.t. TDC)
      fTimestamp = fTimestamp_raw + toff;
      Amplitude = (*pIter)->GetPulseHeight();

      // fill the ntuple
      //TNtuple* ntuple = new TNtuple(ntupname.c_str(), ntuptitle.c_str(), "Event:iPulse:Amplitude:TSWFDtick:TSWFD_raw:iSync:SyncOff:TSWFD");
      const float vals [8] = {midas_event_number, iPulse, Amplitude, Timestamp_tick, fTimestamp_raw, iSync, toff, fTimestamp};
      Ntuple_map[bankname]->Fill(vals);
    }
  }

  return SUCCESS;
}


