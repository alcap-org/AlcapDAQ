/********************************************************************\

Name:         MDT5730_defna_pulses
Created by:   Volodya Tishchenko

Contents:     defna pulse histograms for DT5730

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
#include "TDefna.h"
#include "MDT5730_defna.h"

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

static const int NCHAN = 8;

static map<std::string, TH2D*> h2_pulses_map;       // ADC vs time histograms
static map<std::string, TH1D*> h1_E_map;            // Energy vs time histograms
static map<std::string, TH1D*> h1_A_map;            // Amplitude vs time histograms


ANA_MODULE MDT5730_defna_pulses_module =
{
	"MDT5730_defna_pulses",        /* module name           */
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
INT module_bor(INT run_number) {
  
  // save current directory 
  TDirectory *dir0 = gDirectory;

  TDirectory *dir = dir0->mkdir("MDT5730_defna_pulses");
  dir->cd();

  for (unsigned int i=0; i<NCHAN; i++)
    {
      std::string bankname( Form("D7%02d",i) );


      TH2D *h2_pulses = new TH2D(Form("h2_pulses_%s",bankname.c_str()),Form("ADC vs clock tick, %s",bankname.c_str()),601,-300.5,300.5,16384,-0.5,16383.5);      
      h2_pulses->SetXTitle("time (ct)");
      h2_pulses->SetYTitle("ADC");
      h2_pulses_map[bankname] = h2_pulses;

      TH1D *h1_E = new TH1D(Form("h1_E_%s",bankname.c_str()),Form("Defna Energy, %s",bankname.c_str()),16384,-0.5,163830.5);
      h1_E->SetXTitle("Energy (ADC sum - pedestal)");
      h1_E->SetYTitle("counts");
      h1_E_map[bankname] = h1_E;

      TH1D *h1_A = new TH1D(Form("h1_A_%s",bankname.c_str()),Form("Defna Amplitude, %s",bankname.c_str()),16384,-0.5,16383.5);
      h1_A->SetXTitle("Amplitude (ADC max - pedestal)");
      h1_A->SetYTitle("counts");
      h1_A_map[bankname] = h1_A;

    }
  
  // restore pointer of global directory
  dir0->cd();


  return SUCCESS;
}

/** This method processes one MIDAS block, producing a vector
 * of TOctalFADCIsland objects from the raw Octal FADC data.
 */
INT module_event(EVENT_HEADER *pheader, void *pevent) 
{

  //printf("MDT5730_defna(): event %06i ==================================== \n",pheader->serial_number);

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

      if ( dt5730_defna_map.find(bankname) == dt5730_defna_map.end() ) continue;
      vector<TDefna> defna = dt5730_defna_map[bankname];

      TH2D *h2_pulses = h2_pulses_map[bankname];

      for (unsigned int i=0; i<thePulses.size(); i++)
	{
	  
	  Double_t t0 = defna[i].t0;
	  
	  //if ( h2_pulses_map[bankname] )
	  if ( h2_pulses && defna[i].A > 0 )
	    {
	      std::vector<int> theSamples = thePulses[i]->GetSamples();
	      //TH1D *h1 = new TH1D(Form("%s_event_%06i_pulse_%06i",bankname.c_str(),pheader->serial_number,i),"pulse",32,-0.5,31.5);
	      for (unsigned int j=0; j<theSamples.size(); j++)
		{
		  //h2_pulses_map[bankname]->Fill( j, theSamples[j]);
		  h2_pulses->Fill( j-t0, theSamples[j] );
		  //h2_pulses->Fill( j, theSamples[j] );
		  //h1->SetBinContent(j-t0+10, theSamples[j]);
		  //h1->SetBinContent(j, theSamples[j]);
		}
	      h1_A_map[bankname]->Fill( defna[i].A );
	      h1_E_map[bankname]->Fill( defna[i].E );
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
