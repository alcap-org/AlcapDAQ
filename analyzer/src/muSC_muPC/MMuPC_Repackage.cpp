////////////////////////////////////////////////////////////////////////////////
/// \author Joe Grange
///
/// \brief
/// repacks muPC data output from MMuPC1AnalysisMQL.MQL
/// @{

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

/* ROOT includes */
#include <TH1.h>
#include <TH2.h>
#include <TDirectory.h>

/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"
#include "../common/TMuPCCluster.h"

using std::vector;

/*-- Module declaration --------------------------------------------*/
INT  MMuPC_Repackage_init(void);
INT  MMuPC_Repackage(EVENT_HEADER*, void*);
INT  MMuPC_Repackage_eor(INT);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

#include "TDirectory.h"
#include "TFile.h"
#include "TApplication.h"
#include "TROOT.h"
extern TDirectory * gManaHistsDir;
extern TFile * gManaOutputFile;
extern TApplication * manaApp;
extern TROOT * gROOT;

ANA_MODULE MMuPC_Repackage_module =
{
	"MMuPC_Repackage",                    /* module name           */
	"Joe Grange",              /* author                */
	MMuPC_Repackage,                      /* event routine         */
	NULL,                          /* BOR routine           */
	MMuPC_Repackage_eor,                          /* EOR routine           */
	MMuPC_Repackage_init,                 /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};

/** This method initializes histograms.
*/
INT MMuPC_Repackage_init()
{

  //printf("MMuPC_Repackage inside init! \n");

  return SUCCESS;
}

/** This method does any last minute things to the histograms at the end of the run
 */
INT MMuPC_Repackage_eor(INT run_number) {


  return SUCCESS;
}

/** This method fills the histograms
 */
INT MMuPC_Repackage(EVENT_HEADER *pheader, void *pevent)
{

  // Get the event number
  int midas_event_number = pheader->serial_number;

  // Get the MPC1 bank and get a pointer to the raw data
  double* raw; // points at the raw data

  int bankSize = bk_locate(pevent,"MPC1",&raw);

  bankSize = bankSize*sizeof(DWORD)/sizeof(double);

  typedef vector<TMuPCCluster*> myVectorCluster;

  myVectorCluster& cluster_vector =
    gData->fMuPCClusterVector;

  double timestamp = 0.;
  double wire_number_x = 0.;
  double wire_number_y = 0.;

  //std::vector<TMuPCCluster*> mupc_clusters;

  vector<TMuPCCluster*> mupc_clusters;

  for (int i=0; i<bankSize; i+=3){

    wire_number_x = 0.;
    wire_number_x = (double)*(raw+i);

    wire_number_y = 0.;
    wire_number_y = (double)*(raw+i+1);

    timestamp = 0.;
    timestamp = (double)*(raw+i+2);

    cluster_vector.push_back(new TMuPCCluster(timestamp,wire_number_x,wire_number_y));

  }

  return SUCCESS;

}

/// @}
