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
#include <TH2.h>
#include <TDirectory.h>

/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"
#include "TPulseIsland.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

#include <iostream>

/*-- Module declaration --------------------------------------------*/
INT  MMuPCBeamDimensions_init(void);
INT  MMuPCBeamDimensions(EVENT_HEADER*, void*);
INT  MMuPCBeamDimensions_eor(INT);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

TH2F* hmuPC_XYWires;
TH2F* hmuPC_XYWires_no_time_cut;
TH2F* hmuPC_XYWires_Random;
const double wire_spacing = 2; // 2mm
const double total_x_length = wire_spacing*(kMuPC1NumXWires - 1);
const double total_y_length = wire_spacing*(kMuPC1NumYWires - 1);


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
  hmuPC_XYWires = new TH2F("hmuPC_XYWires", "Plot of X-Y muPC Wire Hits", kMuPC1NumXWires,-total_x_length/2 - 0.5,total_x_length/2 + 0.5, kMuPC1NumYWires,-total_y_length/2 - 0.5, total_y_length/2 + 0.5);
  hmuPC_XYWires->GetXaxis()->SetTitle("X Position [mm]");
  hmuPC_XYWires->GetYaxis()->SetTitle("Y Position [mm]");

  hmuPC_XYWires_no_time_cut = new TH2F("hmuPC_XYWires_no_time_cut", "Plot of X-Y muPC Wire Hits (no time cut)", kMuPC1NumXWires,-total_x_length/2 - 0.5,total_x_length/2 + 0.5, kMuPC1NumYWires,-total_y_length/2 - 0.5, total_y_length/2 + 0.5);
  hmuPC_XYWires_no_time_cut->GetXaxis()->SetTitle("X Position [mm]");
  hmuPC_XYWires_no_time_cut->GetYaxis()->SetTitle("Y Position [mm]");

  /*
  hmuPC_XYWires_Random = new TH2F("hmuPC_XYWires_Random", "Plot of X-Y muPC Wire Hits drawn randomly from the other histogram", kMuPC1NumXWires*10,-total_x_length/2 - 0.5,total_x_length/2 + 0.5, kMuPC1NumYWires*10,-total_y_length/2 - 0.5, total_y_length/2 + 0.5);
  hmuPC_XYWires_Random->GetXaxis()->SetTitle("X Position [mm]");
  hmuPC_XYWires_Random->GetYaxis()->SetTitle("Y Position [mm]");
  */
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

  /*  int n_random_draws = 1000000;
  double x, y;
  for (int i_draw = 0; i_draw < n_random_draws; ++i_draw) {
    hmuPC_XYWires->GetRandom2(x, y);
    hmuPC_XYWires_Random->Fill(x, y);
  }
  */
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

	channel_hit* prev_x_wire = NULL;
	channel_hit* prev_y_wire = NULL;
	channel_hit* prev_muSc_hit = NULL;

	// Parameter values have been obtained from MMuSCAnalysisMQL.cpp and MMuPC1AnalysisMQL.cpp
	for (int i = 0; i < hit_bank_size; ++i) {
	  int parameter = hit_bank[i].parameter;

	  if (parameter >= 4001 && parameter <= (4000 + kMuPC1NumXWires)) {
	    prev_x_wire = &hit_bank[i];
	  }
	  if (parameter >= 4051 && parameter <= (4050 + kMuPC1NumYWires)) {
	    prev_y_wire = &hit_bank[i];
	  }
	  if (parameter == 6011) {
	    prev_muSc_hit = &hit_bank[i];
	  }
	  
	  if (prev_x_wire && prev_y_wire && prev_muSc_hit) {
	    double x_wire_time = prev_x_wire->time;
	    double y_wire_time = prev_y_wire->time;
	    double muSc_time = prev_muSc_hit->time;

	    double tdiff = std::fabs(x_wire_time - y_wire_time);

	    int x_wire = (prev_x_wire->parameter - 4000);
	    int y_wire = (prev_y_wire->parameter - 4050);
	      
	    double x_pos = (total_x_length/2) - wire_spacing*(kMuPC1NumXWires - x_wire);
	    double y_pos = (total_y_length/2) - wire_spacing*(kMuPC1NumYWires - y_wire);
	    hmuPC_XYWires_no_time_cut->Fill(x_pos, y_pos);

	    if (tdiff <= 95) {
	      
	      // Want a coincidence with the muSc
	      double muSc_tdiff = std::fabs(x_wire_time - muSc_time);
	      if (muSc_tdiff <= 160) {
		hmuPC_XYWires->Fill(x_pos, y_pos);
	      }
	      prev_x_wire = NULL; prev_y_wire = NULL; prev_muSc_hit = NULL;
	    }
	    else {
	      prev_x_wire = NULL; prev_y_wire = NULL; prev_muSc_hit = NULL;
	    }
	  }
	}

	return SUCCESS;
}

/// @}
