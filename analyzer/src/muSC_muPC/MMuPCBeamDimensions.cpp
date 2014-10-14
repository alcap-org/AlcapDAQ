////////////////////////////////////////////////////////////////////////////////
/// \defgroup MMuPCBeamDimensions
/// \author Andrew Edmonds
///
/// \brief
/// Plots the dimensions of the beam from the muPC
///
/// \details
/// More details soon
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
INT  MMuPCBeamDimensions_init(void);
INT  MMuPCBeamDimensions(EVENT_HEADER*, void*);
INT  MMuPCBeamDimensions_eor(INT);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

TH1F* hDQ_TDCCheck_muSc;
TH1F* hDQ_TDCCheck_muScA;
TH1F* hDQ_TDCCheck_muPC;
TH1F* hDQ_TDCCheck_Unknown;

TH1F* hDQ_TDCCheck_muSc_time;
TH1F* hDQ_TDCCheck_muSc_rate;
TH1F* hDQ_TDCCheck_TDiff;


ANA_MODULE MMuPCBeamDimensions_module =
{
	"MMuPCBeamDimensions",                    /* module name           */
	"Andrew Edmonds",              /* author                */
	MMuPCBeamDimensions,                      /* event routine         */
	NULL,                          /* BOR routine           */
	MMuPCBeamDimensions_eor,                          /* EOR routine           */
	MMuPCBeamDimensions_init,                 /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};

/** This method initializes histograms.
*/
INT MMuPCBeamDimensions_init()
{
    // See if the DataQuality_LowLevel/ directory already exists
  if (!gDirectory->Cd("muPC")) {
    
    std::string dir_name("muPC/");
    gDirectory->mkdir(dir_name.c_str());
    gDirectory->Cd(dir_name.c_str());
  }

  // Create some histograms
  hDQ_TDCCheck_muPC = new TH1F("hDQ_TDCCheck_muPC", "Number of Hits in TDC (muPC)", 3,0,3);
  hDQ_TDCCheck_muPC->GetXaxis()->SetTitle("muPC");
  hDQ_TDCCheck_muPC->GetYaxis()->SetTitle("Number of Hits");

  gDirectory->Cd("/MidasHists/");
  return SUCCESS;
}

/** This method does any last minute things to the histograms at the end of the run
 */
INT MMuPCBeamDimensions_eor(INT run_number) {

  // Get the run duration to scale the histogram
  HNDLE hDB, hKey;
  char keyName[200];

  if(cm_get_experiment_database(&hDB, NULL) != CM_SUCCESS){
    printf("Warning: Could not connect to ODB database!\n");
    return false;
  }

  return SUCCESS;
}

/** This method fills the histograms
 */
INT MMuPCBeamDimensions(EVENT_HEADER *pheader, void *pevent)
{
	// Get the event number
	int midas_event_number = pheader->serial_number;

	// Get a pointer to the hit data in the TDC
	// NB copied and modified from MMuSCAnalysisC.cpp
	channel_hit *hit_bank;
	int hit_bank_size = bk_locate(pevent, "HITS", (DWORD *) &hit_bank);
	hit_bank_size = hit_bank_size * sizeof(DWORD) / sizeof(channel_hit);

	// At the moment just loop through the hits and print the information
	// Parameter values have been obtained from MMuSCAnalysisMQL.cpp and MMuPC1AnalysisMQL.cpp
	for (int i = 0; i < hit_bank_size; ++i) {
	  if (hit_bank[i].parameter >= 4001 && hit_bank[i].parameter <= 4074)
	    //	    printf("muPC hit! Hit #%d: time = %f, parameter = %d\n", i, hit_bank[i].time, hit_bank[i].parameter);
	    hDQ_TDCCheck_muPC->Fill(1);
	}

	return SUCCESS;
}

/// @}
