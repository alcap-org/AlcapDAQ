/********************************************************************\

Name:         MTDCV1290_analysis_raw
Created by:   Volodya Tishchenko
Based on code of Andrew Edmonds

Contents:     basic analysis of raw TDC hits

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
INT MTDCV1290_analysis_raw_init(void);
INT MTDCV1290_analysis_raw_bor(INT);
INT MTDCV1290_analysis_raw(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;


static map<std::string, TGraph*> gr_nhits_map;    // number of hits vs midas segment
static map<std::string, TH1D*> h1_autocorr_map;   // autocorrelation 
static map<std::string, TH1D*> h1_time_map;       // time in midas segment


ANA_MODULE MTDCV1290_analysis_raw_module =
{
	"MTDCV1290_analysis_raw",      /* module name           */
	"Volodya Tishchenko",          /* author                */
	MTDCV1290_analysis_raw,        /* event routine         */
	MTDCV1290_analysis_raw_bor,    /* BOR routine           */
	NULL,                          /* EOR routine           */
	MTDCV1290_analysis_raw_init,   /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};

/** This method initializes histograms.
*/
INT MTDCV1290_analysis_raw_init() {
  // The following histograms are created for each channel:
  // hPulseHeights: ADC value (x-axis) vs number of pulses (y-axis)
  // hPulseTimes: time stamp (x-axis) vs number of pulses (y-axis)
  // hPulseRawCount: number of pulses (y-axis) - channels on x-axis?
  // hPulseShapes: sample number (x-axis) vs ADC value (y-axis) vs pulse (z-axis)
  
  return SUCCESS;
}


// Resets the histograms at the beginning of each run
// so that the online display updates
INT MTDCV1290_analysis_raw_bor(INT run_number) {
  
  // save current directory 
  TDirectory *dir0 = gDirectory;

  TDirectory *dir = dir0->mkdir("MTDCV1290_analysis_raw");
  dir->cd();

  for (unsigned int i=0; i<32; i++)
    {
      std::string bankname( Form("T4%02d",i) );

      TGraph *gr_nhits = new TGraph();
      gr_nhits->SetName( Form("gr_nhits_%s",bankname.c_str()) );
      dir->Append( gr_nhits );
      gr_nhits_map[bankname] = gr_nhits;

      //TH1D *h1_autocorr = new TH1D(Form("h1_autocorr_%s",bankname.c_str()),Form("Autocorrelation, %s",bankname.c_str()),16384,-2097152.0,2097152.0);      
      //TH1D *h1_autocorr = new TH1D(Form("h1_autocorr_%s",bankname.c_str()),Form("Autocorrelation, %s",bankname.c_str()),2097152,-4.1e12,4.1e12);      
      TH1D *h1_autocorr = new TH1D(Form("h1_autocorr_%s",bankname.c_str()),Form("Autocorrelation, %s",bankname.c_str()),2097152,-4.3e10,4.3e10);      
      h1_autocorr->SetXTitle("time (ct)");
      h1_autocorr_map[bankname] = h1_autocorr;

      TH1D *h1_time = new TH1D(Form("h1_time_%s",bankname.c_str()),Form("time in midas segment, %s",bankname.c_str()),2097152,0.0,190.3e10);      
      h1_time->SetXTitle("time (ct)");
      h1_time_map[bankname] = h1_time;

    }
  
  // restore pointer of global directory
  dir0->cd();


  return SUCCESS;
}

/** This method processes one MIDAS block, producing a vector
 * of TOctalFADCIsland objects from the raw Octal FADC data.
 */
INT MTDCV1290_analysis_raw(EVENT_HEADER *pheader, void *pevent) {


  std::map< std::string, std::vector<int64_t> >& tdc_map = gData->fTDCHitsToChannelMap;

  typedef std::map< std::string, std::vector<int64_t> >::iterator map_iterator;
  for (map_iterator theMapIter = tdc_map.begin(); theMapIter != tdc_map.end(); theMapIter++) 
    {
      std::string bankname = theMapIter->first;

      //std::string detname = gSetup->GetDetectorName(bankname);
      std::vector<int64_t> theHits = theMapIter->second;
      
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
