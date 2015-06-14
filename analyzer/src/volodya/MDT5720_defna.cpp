/********************************************************************\

Name:         MDT5720_defna
Created by:   Volodya Tishchenko

Contents:     defna pulse analysis for DT5720

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

/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"
#include "TPulseIsland.h"

#include "TDefna.h"
#include "MDT5720_defna.h"

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

static const int NCHAN = 4;

static std::map< std::string, TDefna_param > defna_param_map;
std::map< std::string, std::vector<TDefna> > dt5720_defna_map;

//static map<std::string, TH2D*> h2_pulses_map;       // ADC vs time histograms


ANA_MODULE MDT5720_defna_module =
{
	"MDT5720_defna",               /* module name           */
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
 
  for (unsigned int i=0; i<NCHAN; i++)
    {
      std::string bankname( Form("D5%02d",i) );

      TDefna_param defna_param;
      defna_param.polarity = -1;
      defna_param.nsamples_ped = 4;
      defna_param.threshold = 100;
      defna_param.presamples = 4;
      defna_param.postsamples = 4;

      defna_param_map[bankname] = defna_param;
  
      vector<TDefna> v_defna;

      dt5720_defna_map[bankname] = v_defna;
    }

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
INT module_event(EVENT_HEADER *pheader, void *pevent) 
{
  //printf("MTDCV1290_analysis_raw(): event %06i ==================================== \n",pheader->serial_number);

  // clear old data
  for (unsigned int i=0; i<NCHAN; i++)
    {
      std::string bankname( Form("D5%02d",i) );
      dt5720_defna_map[bankname].clear();
    }


  std::map< std::string, std::vector<TPulseIsland*> >& pulses_map = gData->fPulseIslandToChannelMap;

  typedef std::map< std::string, std::vector<TPulseIsland*> >::iterator map_iterator;
  for (map_iterator theMapIter = pulses_map.begin(); theMapIter != pulses_map.end(); theMapIter++) 
    {
      std::string bankname = theMapIter->first;

      std::vector<TPulseIsland*> thePulses = theMapIter->second;

      /*
      std::cout << "bankname: " << bankname 
		<< " number of pulses: " << thePulses.size()
		<< std::endl;
      */

      //TH2D *h2_pulses = h2_pulses_map[bankname];
      
      if ( defna_param_map.find(bankname) != defna_param_map.end() )
	{
	  TDefna_param defna_param = defna_param_map[bankname];
	  for (unsigned int i=0; i<thePulses.size(); i++)
	    {	      
	      TDefna defna;	  
	      defna_eval(thePulses[i], defna_param, defna);
	      dt5720_defna_map[bankname].push_back( defna );

	      //std::cout << defna.t0 << " " << defna.E << std::endl;

	    }
	  
	}
    }
  

  return SUCCESS;
}
