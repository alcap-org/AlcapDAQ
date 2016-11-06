/********************************************************************\

Name:         MTDCV1290_tsync
Created by:   Volodya Tishchenko

Contents:     recalculate times relative to sync. pulse

\********************************************************************/

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
#include <TGraph.h>

/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"
#include "TPulseIsland.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

/*-- Module declaration --------------------------------------------*/
static INT module_init(void);
static INT module_bor(INT);
static INT module_event(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

ANA_MODULE MTDCV1290_tsync_module =
{
	"MTDCV1290_tsync",             /* module name           */
	"Volodya Tishchenko",          /* author                */
	module_event,                  /* event routine         */
	module_bor,                    /* BOR routine           */
	NULL,                          /* EOR routine           */
	module_init,                   /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};

/** This method initializes histograms.
*/
INT module_init() 
{
  // The following histograms are created for each channel:
  // hPulseHeights: ADC value (x-axis) vs number of pulses (y-axis)
  // hPulseTimes: time stamp (x-axis) vs number of pulses (y-axis)
  // hPulseRawCount: number of pulses (y-axis) - channels on x-axis?
  // hPulseShapes: sample number (x-axis) vs ADC value (y-axis) vs pulse (z-axis)
  
  return SUCCESS;
}


// Resets the histograms at the beginning of each run
// so that the online display updates
INT module_bor(INT run_number) 
{  
  return SUCCESS;
}

/** This method processes one MIDAS block, producing a vector
 * of TOctalFADCIsland objects from the raw Octal FADC data.
 */
INT module_event(EVENT_HEADER *pheader, void *pevent) {

  //printf("MTDCV1290_analysis_raw(): event %06i ==================================== \n",pheader->serial_number);

  std::map< std::string, std::vector<int64_t> >& tdc_map = gData->fTDCHitsToChannelMap;

  const std::string tsync_channel_name = string("T401");

  typedef std::map< std::string, std::vector<int64_t> >::iterator map_iterator;
  for (map_iterator theMapIter = tdc_map.begin(); theMapIter != tdc_map.end(); theMapIter++) 
    {
      std::string bankname = theMapIter->first;

      //std::string detname = gSetup->GetDetectorName(bankname);
      std::vector<int64_t> theHits = theMapIter->second;
      
      //std::cout << "TDC bank [" << bankname << "] : " << theHits.size() << " events " << std::endl;
      //if ( bankname == "T401" )
      //std::cout << "TDC bank [" << bankname << "] : " << theHits.size() << " events " << std::endl;

      if ( gr_nhits_map[bankname] )
	{
	  Int_t np = gr_nhits_map[bankname]->GetN();
	  gr_nhits_map[bankname]->SetPoint(np, pheader->serial_number, theHits.size());
	}
      if ( h1_time_map[bankname] )
	{
	  for (unsigned int i=0; i<theHits.size(); i++ )
	    {
	      h1_time_map[bankname]->Fill( theHits[i] );
	    }
	}


      if ( h1_autocorr_map[bankname] ) 
	{    
	  for (unsigned int i=0; i<theHits.size(); i++ )
	    {
	      int64_t t_i = theHits[i];
	      for ( unsigned int j=i+1; j<theHits.size(); j++)
		{
		  int64_t t_j = theHits[j];
		  h1_autocorr_map[bankname]->Fill(t_j-t_i); 
		  break;
		}
	    }

	}

      // Loop over the TPulseIslands and plot the histogram
      //for (std::vector<TPulseIsland*>::iterator pulseIter = thePulses.begin();
      //   pulseIter != thePulses.end(); pulseIter++) {
      //}
    }

  
  

  return SUCCESS;
}
