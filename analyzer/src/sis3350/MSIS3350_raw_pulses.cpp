/********************************************************************\

Name:         MSIS3350_raw_pulses
Created by:   Volodya Tishchenko

Contents:     raw pulse histograms for SIS3350 digitizer

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

static const int sis3350_n_channels = 4;
static const int sis3350_n_boards = 2;

static map<std::string, TH2D*> h2_pulses_map;       // ADC vs sample_nr histograms
static map<std::string, TH1D*> h1_time_map;         // time histograms


ANA_MODULE MSIS3350_raw_pulses_module =
{
	"MSIS3350_raw_pulses",         /* module name           */
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
INT module_init() {
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
  
  // save current directory 
  TDirectory *dir0 = gDirectory;

  TDirectory *dir = dir0->mkdir("MSIS3350_raw_pulses");
  dir->cd();

  for (unsigned int iboard=0; iboard<sis3350_n_boards; ++iboard)
    {

      for (unsigned int ich=0; ich<sis3350_n_channels; ich++)
	{
	  std::string bankname( Form("S5%d%d",iboard,ich) );
	  
	  /*
	    TGraph *gr_nhits = new TGraph();
	    gr_nhits->SetName( Form("gr_nhits_%s",bankname.c_str()) );
	    dir->Append( gr_nhits );
	    gr_nhits_map[bankname] = gr_nhits;
	  */
	  
	  /*
	    TH1D *h1_autocorr = new TH1D(Form("h1_autocorr_%s",bankname.c_str()),Form("Autocorrelation, %s",bankname.c_str()),2097152,-4.3e10,4.3e10);      
	    h1_autocorr->SetXTitle("time (ct)");
	    h1_autocorr_map[bankname] = h1_autocorr;
	  */
	  
	  /*
	    TH1D *h1_time = new TH1D(Form("h1_time_%s",bankname.c_str()),Form("time in midas segment, %s",bankname.c_str()),2097152,0.0,190.3e10);      
	    h1_time->SetXTitle("time (ct)");
	    h1_time_map[bankname] = h1_time;
	  */
	  
	  TH2D *h2_pulses = new TH2D(Form("h2_pulses_%s",bankname.c_str()),Form("ADC vs clock tick, %s",bankname.c_str()),1024,-0.5,1023.5,4096,-0.5,4095.5);      
	  h2_pulses->SetXTitle("time (ct)");
	  h2_pulses->SetYTitle("ADC");
	  h2_pulses_map[bankname] = h2_pulses;

	  TH1D *h1_time = new TH1D(Form("h1_time_%s",bankname.c_str()),Form("time, %s",bankname.c_str()),120000,-0.5,1.2e7);      
	  h1_time->SetXTitle("time (ct)");
	  h1_time->SetYTitle("counts");
	  h1_time_map[bankname] = h1_time;
	  
	}

    }

  // restore pointer of global directory
  dir0->cd();


  return SUCCESS;
}

INT module_event(EVENT_HEADER *pheader, void *pevent) 
{

  //printf("MSIS3350_raw_pulses(): event %06i ==================================== \n",pheader->serial_number);

  std::map< std::string, std::vector<TPulseIsland*> >& pulses_map = gData->fPulseIslandToChannelMap;

  typedef std::map< std::string, std::vector<TPulseIsland*> >::iterator map_iterator;
  for (map_iterator theMapIter = pulses_map.begin(); theMapIter != pulses_map.end(); theMapIter++) 
    {
      std::string bankname = theMapIter->first;


      //std::string detname = gSetup->GetDetectorName(bankname);
      //std::vector<int64_t> theHits = theMapIter->second;
      std::vector<TPulseIsland*> thePulses = theMapIter->second;

      /*
      std::cout << "bankname: " << bankname 
		<< " number of pulses: " << thePulses.size()
		<< std::endl;
      */

      TH2D *h2_pulses = h2_pulses_map[bankname];
      TH1D *h1_time   = h1_time_map  [bankname];

      for (unsigned int i=0; i<thePulses.size(); i++)
      //for (unsigned int i=1; i<thePulses.size(); i++)
	{
	  
	  //if ( h2_pulses_map[bankname] )
	  if ( h2_pulses )
	    {
	      std::vector<int> theSamples = thePulses[i]->GetSamples();
	      for (unsigned int j=0; j<theSamples.size(); j++)
		{
		  //h2_pulses_map[bankname]->Fill( j, theSamples[j]);
		  h2_pulses->Fill( j, theSamples[j]);
		}
	    }

	  if ( h1_time )
	    {
	      double time = thePulses[i]->GetTimeStamp();
	      h1_time->Fill( time );
	    }
	  
	}
      
      //std::cout << "TDC bank [" << bankname << "] : " << theHits.size() << " events " << std::endl;
      //if ( bankname == "T401" )
      //std::cout << "TDC bank [" << bankname << "] : " << theHits.size() << " events " << std::endl;

      /*
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
	  for (unsigned int i=0; i<theHits.size()-1; i++ )
	    {
	      int64_t t_i = theHits[i];
	      int64_t t_j = theHits[i+1];
	      h1_autocorr_map[bankname]->Fill(t_j-t_i); 
	      //Int_t np = gr_nhits_map[bankname]->GetN();
	      //gr_nhits_map[bankname]->SetPoint(np, np, t_j - t_i );
	    }

	}

      */

      // Loop over the TPulseIslands and plot the histogram
      //for (std::vector<TPulseIsland*>::iterator pulseIter = thePulses.begin();
      //   pulseIter != thePulses.end(); pulseIter++) {
      //}
    }

  
  

  return SUCCESS;
}
