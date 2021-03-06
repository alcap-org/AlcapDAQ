////////////////////////////////////////////////////////////////////////////////
/// \defgroup MDQ_TDCCheck
/// \ingroup lldq
/// \author Andrew Edmonds
/// \editted by Damien Alexander
///
/// \brief
/// Plots information from the TDC, most importantly muSc, muScA, and muPC data.
///
/// \details
/// A number of TDC plots are created. There is a count of muSc, muScA, and muPC
/// hits. The muSc time is also plotted (this is in addition to the timing we
/// get from the digitizer the muSc is plugged into). A timing correlation
/// plot is made between the timing of the digitizer and the TDC. And finally
/// all unknown parameters from the TDC are counted up.
/// @{

/// \var hDQ_TDCCheck
/// \brief
/// Map of plots of Count of number of muScA hits in each
/// channel of TDC

/// \var hDQ_TDCCheck_Unknown
/// \brief
/// Count of unknown TDC parameters during a run.

/// \var hDQ_TDCCheck_TTSc_time
/// \brief
/// Time distribution of muon hits in TDC.

/// \var hDQ_TDCCheck_TTSc_rate
/// \brief
/// Rate of TSc hits in TDC averaged over the run.

/// \var hDQ_TDCCheck_TDiff
/// \brief
/// Timing correlation between TSc hits and other detector hits within TDC 

/// \var hDQ_TDCCheck_TOffset
/// \brief
/// Timing correlation between TSc hits in TDC and WFD
////////////////////////////////////////////////////////////////////////////////

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

/* MuCap includes */
#include "../muSC_muPC/common.h" // for channel_hit

/* ROOT includes */
#include <TH1.h>
#include <TDirectory.h>

/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"
#include "TPulseIsland.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

/*-- Module declaration --------------------------------------------*/
INT  MDQ_TDCCheck_init(void);
INT  MDQ_TDCCheck(EVENT_HEADER*, void*);
INT  MDQ_TDCCheck_eor(INT);
INT  MDQ_TDCCheck_bor(INT);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

static const int NCHANTDC = 16;
const double TIME_LOW = -10e3, TIME_HIGH = 10e3; //ns

static TH1* hDQ_TDCCheck[NCHANTDC];
static TH1* hDQ_TDCCheck_TCorr[NCHANTDC];

const int TDC_dataset = 2;  //R15a dataset

TH1F* hDQ_TDCCheck_nMuons;
TH1F* hDQ_TDCCheck_Muon_time;
TH1F* hDQ_TDCCheck_Muon_rate;
TH1F* hDQ_TDCCheck_TOffset;

TH1F* hDQ_TDCCheck_muScA;
TH1F* hDQ_TDCCheck_muPC;
TH1F* hDQ_TDCCheck_Unknown;

TH1F* hDQ_TDCCheck_TDiff;


ANA_MODULE MDQ_TDCCheck_module =
{
	"MDQ_TDCCheck",                /* module name           */
	"Damien Alexander",            /* author                */
	MDQ_TDCCheck,                  /* event routine         */
	MDQ_TDCCheck_bor,              /* BOR routine           */
	MDQ_TDCCheck_eor,              /* EOR routine           */
	MDQ_TDCCheck_init,             /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};


INT MDQ_TDCCheck_bor(int runnumber)
{
  /*
  if(runnumber < 3780){
    //R13
    TDC_dataset = 1;
  }
  else if(runnumber < 7425){
    //R15a
    TDC_dataset = 2;
  }
  else{
    //unknown set
    std::cout << "MDQ_TDCCheck error:  Run Number " << runnumber << " not associated with any known data set" << std::endl;
    return 1;
  }
  std::cout << "bor : TDC_dataset = " << TDC_dataset << std::endl;
  */
  return SUCCESS;
}


/** This method initializes histograms.
*/
INT MDQ_TDCCheck_init()
{
    // See if the DataQuality_LowLevel/ directory already exists
  if (!gDirectory->Cd("DataQuality_LowLevel")) {
    
    std::string dir_name("DataQuality_LowLevel/");
    gDirectory->mkdir(dir_name.c_str());
    gDirectory->Cd(dir_name.c_str());
  }
  // Create some histograms
  //-------------R15a histograms ---------//
  if(TDC_dataset == 2){
    
    hDQ_TDCCheck_TOffset = new TH1F("hDQ_TDCCheck_TOffset", "Time difference between Muon hit in TDC and WFD", 10000, TIME_LOW, TIME_HIGH);
    hDQ_TDCCheck_TOffset->GetXaxis()->SetTitle("Time Difference of TSc Hits");
    hDQ_TDCCheck_TOffset->GetYaxis()->SetTitle("Number of Hits");
    
    for(int ich = 0; ich<NCHANTDC; ++ich) {
      char bank[5], histname[64], histtitle[64];
      sprintf(bank, "T4%02d", ich);
      std::string detname = gSetup->GetDetectorName(bank);

      sprintf(histname, "hDQ_TDCCheck_%s_%s",detname.c_str(), bank);
      sprintf(histtitle, "Number of TDC hits in %s", detname.c_str());
      hDQ_TDCCheck[ich] = new TH1I(histname, histtitle, 3,0,3);
      hDQ_TDCCheck[ich]->GetXaxis()->SetTitle(detname.c_str());
      hDQ_TDCCheck[ich]->GetYaxis()->SetTitle("Number of Hits");

      sprintf(histname, "hDQ_TDCCheck_TCorr_%s_%s", detname.c_str(), bank);
      sprintf(histtitle, "TDC Timing Correlation, TSc and %s", detname.c_str());
      hDQ_TDCCheck_TCorr[ich] = new TH1F(histname, histtitle, 10000, TIME_LOW, TIME_HIGH);
      hDQ_TDCCheck_TCorr[ich]->GetXaxis()->SetTitle("Timing Difference(ns)");
    }

  }

  //-----------R13 Histograms ---------//
  if(TDC_dataset == 1){
    hDQ_TDCCheck_muScA = new TH1F("hDQ_TDCCheck_muScA", "Number of Hits in TDC (muScA)", 3,0,3);
    hDQ_TDCCheck_muScA->GetXaxis()->SetTitle("muScA");
    hDQ_TDCCheck_muScA->GetYaxis()->SetTitle("Number of Hits");

    hDQ_TDCCheck_muPC = new TH1F("hDQ_TDCCheck_muPC", "Number of Hits in TDC (muPC)", 3,0,3);
    hDQ_TDCCheck_muPC->GetXaxis()->SetTitle("muPC");
    hDQ_TDCCheck_muPC->GetYaxis()->SetTitle("Number of Hits");

    hDQ_TDCCheck_Unknown = new TH1F("hDQ_TDCCheck_Unknown", "Number of Hits in TDC (Unknown)", 7000,0,7000);
    hDQ_TDCCheck_Unknown->GetXaxis()->SetTitle("TDC Parameter");
    hDQ_TDCCheck_Unknown->GetYaxis()->SetTitle("Number of Hits");
  }

  //-------general histograms -------//
  hDQ_TDCCheck_nMuons = new TH1F("hDQ_TDCCheck_nMuons", "Number of Hits in Muon Channel", 3,0,3);
  hDQ_TDCCheck_nMuons->GetXaxis()->SetTitle("Muon Channel");
  hDQ_TDCCheck_nMuons->GetYaxis()->SetTitle("Number of Hits");

  hDQ_TDCCheck_Muon_time = new TH1F("hDQ_TDCCheck_Muon_time", "Time of TDC Hits for muon channel", 1200,0,120e6);
  hDQ_TDCCheck_Muon_time->GetXaxis()->SetTitle("Time of TSc Hit [ns]");
  hDQ_TDCCheck_Muon_time->GetYaxis()->SetTitle("Number of Hits");

  hDQ_TDCCheck_Muon_rate = new TH1F("hDQ_TDCCheck_Muon_rate", "Rate of TDC Hits in muon channel", 3,0,3);
  hDQ_TDCCheck_Muon_rate->GetXaxis()->SetTitle("Muon Channel");
  hDQ_TDCCheck_Muon_rate->GetYaxis()->SetTitle("Rate of Muon Hits [s^-1]");

  gDirectory->Cd("/MidasHists/");
  return SUCCESS;
}

/** This method does any last minute things to the histograms at the end of the run
 */
INT MDQ_TDCCheck_eor(INT run_number) {

  // Get the run duration to scale the histogram
  HNDLE hDB, hKey;
  char keyName[200];

  if(cm_get_experiment_database(&hDB, NULL) != CM_SUCCESS){
    printf("Warning: Could not connect to ODB database!\n");
    return false;
  }

  sprintf(keyName, "/Runinfo/Start time binary");
  if(db_find_key(hDB,0,keyName, &hKey) != SUCCESS){
    printf("Warning: Could not find key %s\n", keyName);
    return false;
  }
  KEY start_time_key;
  if(db_get_key(hDB, hKey, &start_time_key) != DB_SUCCESS){
    printf("Warning: Could not find key %s\n", keyName);
    return false;
  }
  DWORD StartTimes[start_time_key.num_values];
  int size = sizeof(StartTimes);
  if(db_get_value(hDB, 0, keyName, StartTimes, &size, TID_DWORD, 0) != DB_SUCCESS){
    printf("Warning: Could not retrieve values for key %s\n", keyName);
    return false;
  }

  sprintf(keyName, "/Runinfo/Stop time binary");
  if(db_find_key(hDB,0,keyName, &hKey) != SUCCESS){
    printf("Warning: Could not find key %s\n", keyName);
    return false;
  }
  KEY stop_time_key;
  if(db_get_key(hDB, hKey, &stop_time_key) != DB_SUCCESS){
    printf("Warning: Could not find key %s\n", keyName);
    return false;
  }
  DWORD StopTimes[stop_time_key.num_values];
  size = sizeof(StopTimes);
  if(db_get_value(hDB, 0, keyName, StopTimes, &size, TID_DWORD, 0) != DB_SUCCESS){
    printf("Warning: Could not retrieve values for key %s\n", keyName);
    return false;
  }

  int duration = StopTimes[0] - StartTimes[0]; // length of run in seconds (checked against run #2600)
  
  hDQ_TDCCheck_Muon_rate->Scale(1.0/duration);

  return SUCCESS;
}

/** This method fills the histograms
 */
INT MDQ_TDCCheck(EVENT_HEADER *pheader, void *pevent)
{
  // Get the event number
  int midas_event_number = pheader->serial_number;


  //------R15a TDC Analysis ------//
  if(TDC_dataset == 2){
    // Fetch a reference to the gData structure that stores a map
    // of (bank_name, vector<TPulseIsland*>) pairs
    const std::map<std::string, std::vector<TPulseIsland*> >& wfd_map =
      gData->fPulseIslandToChannelMap;

    const std::map<std::string, std::vector<int64_t> >& tdc_map =
      gData->fTDCHitsToChannelMap;

    std::string refbank = gSetup->GetBankName("TTSc");
    std::string pulsebank = gSetup->GetBankName("TSc");
    if(tdc_map.find(refbank) == tdc_map.end()){
      std::cout << "MDQ_TDCCheck Error : No TSc hits found in TDC" << std::endl;
      return 1;
    } 
    const std::vector<int64_t>& ref_hits = tdc_map.at(refbank);
    if(wfd_map.find(pulsebank) == wfd_map.end()){
      std::cout << "MDQ_TDCCheck Error : No TSc hits found in WFD" << std::endl;
      return 1;
    } 
    const std::vector<TPulseIsland*>& pulses = wfd_map.at(pulsebank);
    static const float clock_tick = 0.025; //ns conversion for TDC hits

    for(int ich = 0; ich < NCHANTDC; ++ich) {
      if(!hDQ_TDCCheck[ich]) continue;
      char tdc_bank[5];
      sprintf(tdc_bank, "T4%02d", ich);

      if(tdc_map.find(tdc_bank) == tdc_map.end()){
	std::cout << "MDQ_TDCCheck error:  bank " << tdc_bank << " not found" << std::endl;
	continue;
      } 
      const std::vector<int64_t>& hits = tdc_map.at(tdc_bank);
      hDQ_TDCCheck[ich]->Fill(1, hits.size());
    
      if(tdc_bank == refbank) {
	hDQ_TDCCheck_Muon_rate->Fill(1, hits.size());
	for(int i=0; i< hits.size(); i++){
	  hDQ_TDCCheck_Muon_time->Fill(hits[i]);
	  hDQ_TDCCheck_nMuons->Fill(1);
	}
      }
  
      for(int i = 0; i< hits.size(); i++) {
	for(int j = 0; j<ref_hits.size(); j++) {
	  const double dt = clock_tick * (hits[i] - ref_hits[j]);
	  if(dt < TIME_LOW)
	    break;
	  else if(dt < TIME_HIGH)
	    hDQ_TDCCheck_TCorr[ich]->Fill(dt);
	}
      }
    }
    
    for(int t = 0; t < ref_hits.size(); t++) {
      for(int p = 0; p < pulses.size(); p++) {
	const double dt = clock_tick * ref_hits[t] - pulses[p]->GetPulseTime();
	if(dt < TIME_LOW)
	  break;
	else if(dt < TIME_HIGH){
	  //std::cout << dt << std::endl;
	  hDQ_TDCCheck_TOffset->Fill(dt);
	}
      }
    }
    
  }

  
  //-----------R13 TDC Analysis ----------//
  if(TDC_dataset == 1){
    // Some typedefs
    typedef map<string, vector<TPulseIsland*> > TStringPulseIslandMap;
    typedef pair<string, vector<TPulseIsland*> > TStringPulseIslandPair;
    typedef map<string, vector<TPulseIsland*> >::iterator map_iterator;

    // Fetch a reference to the gData structure that stores a map
    // of (bank_name, vector<TPulseIsland*>) pairs
    TStringPulseIslandMap& pulse_islands_map =
      gData->fPulseIslandToChannelMap;

    // Get a pointer to the hit data in the TDC
    // NB copied and modified from MMuSCAnalysisC.cpp
    channel_hit *hit_bank;
    int hit_bank_size = bk_locate(pevent, "HITS", (DWORD *) &hit_bank);
    hit_bank_size = hit_bank_size * sizeof(DWORD) / sizeof(channel_hit);

    // At the moment just loop through the hits and print the information
    // Parameter values have been obtained from MMuSCAnalysisMQL.cpp and MMuPC1AnalysisMQL.cpp
    for (int i = 0; i < hit_bank_size; ++i) {
      if (hit_bank[i].parameter == 6011) {
	// printf("muSC hit! Hit #%d: time = %f, parameter = %d\n", i, hit_bank[i].time, hit_bank[i].parameter);
	hDQ_TDCCheck_nMuons->Fill(1);
	hDQ_TDCCheck_Muon_time->Fill(hit_bank[i].time);
	hDQ_TDCCheck_Muon_rate->Fill(1);

	// Plot the time difference between the time as given by the TDC and as given by the BU CAEN
	std::string detname = "muSc";
	std::string bankname = gSetup->GetBankName(detname);
	std::vector<TPulseIsland*> theMuScPulses = pulse_islands_map[bankname];

	// Loop over the TPulseIslands and plot the histogram
	for(std::vector<TPulseIsland*>::iterator pulseIter = theMuScPulses.begin(); pulseIter != theMuScPulses.end(); ++pulseIter) {

	  // Get the timestamp of the TPI in ns
	  int time_stamp = (*pulseIter)->GetTimeStamp();
	  double clock_tick_in_ns = (*pulseIter)->GetClockTickInNs();
	  double block_time = time_stamp * clock_tick_in_ns;

	  hDQ_TDCCheck_TDiff->Fill(block_time - hit_bank[i].time);
	}
      }

      else if (hit_bank[i].parameter == 6002)
	// printf("muSCA hit! Hit #%d: time = %f, parameter = %d\n", i, hit_bank[i].time, hit_bank[i].parameter);
	hDQ_TDCCheck_muScA->Fill(1);

      else if (hit_bank[i].parameter >= 4001 && hit_bank[i].parameter <= 4074)
	// printf("muPC hit! Hit #%d: time = %f, parameter = %d\n", i, hit_bank[i].time, hit_bank[i].parameter);
	hDQ_TDCCheck_muPC->Fill(1);
      else
	// printf("Unknown hit! Hit #%d: time = %f, parameter = %d\n", i, hit_bank[i].time, hit_bank[i].parameter);
	hDQ_TDCCheck_Unknown->Fill(hit_bank[i].parameter);
    }
  }
  

  return SUCCESS;
}

/// @}
