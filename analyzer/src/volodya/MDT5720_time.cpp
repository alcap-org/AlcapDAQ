/********************************************************************\

Name:         MDT5720_time
Created by:   Volodya Tishchenko

Contents:     calculates absolute time withing MIDAS block for DT5720

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
#include <TDirectory.h>
#include <TH1.h>
#include <TH2.h>

/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"
#include "TPulseIsland.h"

#include "TDefna.h"
#include "MDT5720_defna.h"
#include "MDT5720_time.h"

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
std::map< std::string, std::vector<double> > dt5720_time_map;

static map<std::string, TH1D*> h1_time_map;            // Time in MIDAS block in ns

ANA_MODULE MDT5720_time_module =
{
	"MDT5720_time",                /* module name           */
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
 

  // save current directory 
  TDirectory *dir0 = gDirectory;

  TDirectory *dir = dir0->mkdir("MDT5720_time");
  dir->cd();


  for (unsigned int i=0; i<NCHAN; i++)
    {
      std::string bankname( Form("D5%02d",i) );

      vector<double> v;
      dt5720_time_map[bankname] = v;
      
      TH1D *h1_time = new TH1D(Form("h1_time_%s",bankname.c_str()),Form("time in block, %s",bankname.c_str()),16384,0.0,12000.0);
      h1_time->SetXTitle("time (ms)");
      h1_time->SetYTitle("counts");
      h1_time_map[bankname] = h1_time;

    }

  // restore pointer of global directory
  dir0->cd();

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
  printf("MDT5720_analysis_raw(): event %06i ==================================== \n",pheader->serial_number);

  // clear old data
  for (unsigned int i=0; i<NCHAN; i++)
    {
      std::string bankname( Form("D5%02d",i) );
      dt5720_time_map[bankname].clear();
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
      
      if ( dt5720_defna_map.find(bankname) != dt5720_defna_map.end() && dt5720_time_map.find(bankname) !=  dt5720_time_map.end() )
	{
	  vector<TDefna> defna = dt5720_defna_map[ bankname ];	  
	  for (unsigned int i=0; i<thePulses.size(); i++)
	    {
	      double t = -1.0e32;
	      if ( defna[i].E >  0 )
		{
		  t = (defna[i].t0 + thePulses[i]->GetTimeStamp())*4.0; // time in ns
		  if (  h1_time_map[bankname] )
		    {
		      h1_time_map[bankname]->Fill( t*1.0e-6 );
		    }
		}
	      dt5720_time_map[bankname].push_back( t );
	      if ( i == 0 ) std::cout << "time of 1st pulse is " << thePulses[i]->GetTimeStamp() << std::endl;
	    } 
	}
    }
  

  return SUCCESS;
}
