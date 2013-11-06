/********************************************************************\

  Name:         MSiPulseHeight
  Created by:   Andrew Edmonds

  Contents:     A module to plot the pulse heights of the silicon detector

\********************************************************************/

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include <TH1.h>
#include <TH2.h>

/* AlCap includes */
#include "TOctalFADCIsland.h"
#include "TOctalFADCBankReader.h"
#include "TGlobalData.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

/*-- Module declaration --------------------------------------------*/
INT  MSiPulseHeight_init(void);
INT  MSiPulseHeight(EVENT_HEADER*, void*);
vector<string> GetAllFADCBankNames();
double GetClockTickForChannel(string bank_name);

extern HNDLE hDB;
extern TGlobalData* gData;

static TH1* hSiL2Slow_Heights; // Bank: Nac0
static TH1* hSiR2Slow_Heights; // Bank: Nbc0

static TH1* hSiL2Fast_Heights; // Bank: Nec0
static TH1* hSiR2Fast_Heights; // Bank: Nfc0

static TH1* hSiL1_1Fast_Heights; // Bank: Nae0
static TH1* hSiL1_2Fast_Heights; // Bank: Nbe0
static TH1* hSiL1_3Fast_Heights; // Bank: Nce0
static TH1* hSiL1_4Fast_Heights; // Bank: Nde0
static TH1* hSiR1_1Fast_Heights; // Bank: Nee0
static TH1* hSiR1_2Fast_Heights; // Bank: Nfe0
static TH1* hSiR1_3Fast_Heights; // Bank: Nge0
static TH1* hSiR1_4Fast_Heights; // Bank: Nhe0

static TH1* hSiL1_1Slow_Heights; // Bank: Naf0
static TH1* hSiL1_2Slow_Heights; // Bank: Nbf0
static TH1* hSiL1_3Slow_Heights; // Bank: Ncf0
static TH1* hSiL1_4Slow_Heights; // Bank: Ndf0
static TH1* hSiR1_1Slow_Heights; // Bank: Nef0
static TH1* hSiR1_2Slow_Heights; // Bank: Nff0
static TH1* hSiR1_3Slow_Heights; // Bank: Ngf0
static TH1* hSiR1_4Slow_Heights; // Bank: Nhf0

static vector<TOctalFADCBankReader*> fadc_bank_readers;

ANA_MODULE MSiPulseHeight_module =
{
  "MSiPulseHeight",                    /* module name           */
  "Andrew Edmonds",              /* author                */
  MSiPulseHeight,                      /* event routine         */
  NULL,                          /* BOR routine           */
  NULL,                          /* EOR routine           */
  MSiPulseHeight_init,                 /* init routine          */
  NULL,                          /* exit routine          */
  NULL,                          /* parameter structure   */
  0,                             /* structure size        */
  NULL,                          /* initial parameters    */
};

/** This method initializes histograms.
  */
INT MSiPulseHeight_init()
{
  // This histogram has the bank names labeled on the X-axis, and the midas
  // block number on the Y-axis.
  // This uses the TH1::kCanRebin mechanism to expand automatically to the
  // number of FADC banks.
  int n_bins = 100;
  int x_min = 0;
  int x_max = 100;
  
  hSiL2Slow_Heights = new TH1I("hSiL2Slow_Heights", "Plot of the pulse heights from the SiL2 slow detector", n_bins, x_min, x_max);
  hSiL2Slow_Heights->SetBit(TH1::kCanRebin);
  hSiR2Slow_Heights = new TH1I("hSiR2Slow_Heights", "Plot of the pulse heights from the SiR2 slow detector", n_bins, x_min, x_max);
  hSiR2Slow_Heights->SetBit(TH1::kCanRebin);
  
  
  hSiL2Fast_Heights = new TH1I("hSiL2Fast_Heights", "Plot of the pulse heights from the SiL2 fast detector", n_bins, x_min, x_max);
  hSiL2Fast_Heights->SetBit(TH1::kCanRebin);
  hSiR2Fast_Heights = new TH1I("hSiR2Fast_Heights", "Plot of the pulse heights from the SiR2 fast detector", n_bins, x_min, x_max);
  hSiR2Fast_Heights->SetBit(TH1::kCanRebin);
  
  
  hSiL1_1Fast_Heights = new TH1I("hSiL1_1Fast_Heights", "Plot of the pulse heights from the SiL1_1 fast detector", n_bins, x_min, x_max);
  hSiL1_1Fast_Heights->SetBit(TH1::kCanRebin);
  hSiL1_2Fast_Heights = new TH1I("hSiL1_2Fast_Heights", "Plot of the pulse heights from the SiL1_2 fast detector", n_bins, x_min, x_max);
  hSiL1_2Fast_Heights->SetBit(TH1::kCanRebin);
  hSiL1_3Fast_Heights = new TH1I("hSiL1_3Fast_Heights", "Plot of the pulse heights from the SiL1_3 fast detector", n_bins, x_min, x_max);
  hSiL1_3Fast_Heights->SetBit(TH1::kCanRebin);
  hSiL1_4Fast_Heights = new TH1I("hSiL1_4Fast_Heights", "Plot of the pulse heights from the SiL1_4 fast detector", n_bins, x_min, x_max);
  hSiL1_4Fast_Heights->SetBit(TH1::kCanRebin);
  
  hSiR1_1Fast_Heights = new TH1I("hSiR1_1Fast_Heights", "Plot of the pulse heights from the SiR1_1 fast detector", n_bins, x_min, x_max);
  hSiR1_1Fast_Heights->SetBit(TH1::kCanRebin);
  hSiR1_2Fast_Heights = new TH1I("hSiR1_2Fast_Heights", "Plot of the pulse heights from the SiR1_2 fast detector", n_bins, x_min, x_max);
  hSiR1_2Fast_Heights->SetBit(TH1::kCanRebin);
  hSiR1_3Fast_Heights = new TH1I("hSiR1_3Fast_Heights", "Plot of the pulse heights from the SiR1_3 fast detector", n_bins, x_min, x_max);
  hSiR1_3Fast_Heights->SetBit(TH1::kCanRebin);
  hSiR1_4Fast_Heights = new TH1I("hSiR1_4Fast_Heights", "Plot of the pulse heights from the SiR1_4 fast detector", n_bins, x_min, x_max);
  hSiR1_4Fast_Heights->SetBit(TH1::kCanRebin);
  
  
  hSiL1_1Slow_Heights = new TH1I("hSiL1_1Slow_Heights", "Plot of the pulse heights from the SiL1_1 slow detector", n_bins, x_min, x_max);
  hSiL1_1Slow_Heights->SetBit(TH1::kCanRebin);
  hSiL1_2Slow_Heights = new TH1I("hSiL1_2Slow_Heights", "Plot of the pulse heights from the SiL1_2 slow detector", n_bins, x_min, x_max);
  hSiL1_2Slow_Heights->SetBit(TH1::kCanRebin);
  hSiL1_3Slow_Heights = new TH1I("hSiL1_3Slow_Heights", "Plot of the pulse heights from the SiL1_3 slow detector", n_bins, x_min, x_max);
  hSiL1_3Slow_Heights->SetBit(TH1::kCanRebin);
  hSiL1_4Slow_Heights = new TH1I("hSiL1_4Slow_Heights", "Plot of the pulse heights from the SiL1_4 slow detector", n_bins, x_min, x_max);
  hSiL1_4Slow_Heights->SetBit(TH1::kCanRebin);
  
  hSiR1_1Slow_Heights = new TH1I("hSiR1_1Slow_Heights", "Plot of the pulse heights from the SiR1_1 slow detector", n_bins, x_min, x_max);
  hSiR1_1Slow_Heights->SetBit(TH1::kCanRebin);
  hSiR1_2Slow_Heights = new TH1I("hSiR1_2Slow_Heights", "Plot of the pulse heights from the SiR1_2 slow detector", n_bins, x_min, x_max);
  hSiR1_2Slow_Heights->SetBit(TH1::kCanRebin);
  hSiR1_3Slow_Heights = new TH1I("hSiR1_3Slow_Heights", "Plot of the pulse heights from the SiR1_3 slow detector", n_bins, x_min, x_max);
  hSiR1_3Slow_Heights->SetBit(TH1::kCanRebin);
  hSiR1_4Slow_Heights = new TH1I("hSiR1_4Slow_Heights", "Plot of the pulse heights from the SiR1_4 slow detector", n_bins, x_min, x_max);
  hSiR1_4Slow_Heights->SetBit(TH1::kCanRebin);

  vector<string> bank_names = GetAllFADCBankNames();

  for(unsigned int i=0; i<bank_names.size(); i++) {
    fadc_bank_readers.push_back(new TOctalFADCBankReader(bank_names[i]));
  }

  return SUCCESS;
}

/** This method processes one MIDAS block, producing a vector
  * of TOctalFADCIsland objects from the raw Octal FADC data.
  */
INT MSiPulseHeight(EVENT_HEADER *pheader, void *pevent)
{
  // Get the event number
  int midas_event_number = pheader->serial_number;

  // Some typedefs
  typedef map<string, vector<TPulseIsland*> > TStringPulseIslandMap;
  typedef pair<string, vector<TPulseIsland*> > TStringPulseIslandPair;
  typedef map<string, vector<TPulseIsland*> >::iterator map_iterator;

  // Fetch a reference to the gData structure that stores a map
  // of (bank_name, vector<TPulseIsland*>) pairs
  TStringPulseIslandMap& pulse_islands_map =
    gData->fPulseIslandToChannelMap;

  // Iterate through the banks readers
  for (std::vector<TOctalFADCBankReader*>::iterator bankReaderIter = fadc_bank_readers.begin();
  	   bankReaderIter != fadc_bank_readers.end(); bankReaderIter++) {
  	   
  	   (*bankReaderIter)->ProcessEvent(pheader, pevent);
  	   std::vector<TOctalFADCIsland*> theOctalFADCIslands = (*bankReaderIter)->GetIslandVectorCopy();
  	   
  	   // Loop over the islands and fill the relevant histogram with the peak height
  	   for (std::vector<TOctalFADCIsland*>::iterator octalFADCIslandIter = theOctalFADCIslands.begin();
  	   		octalFADCIslandIter != theOctalFADCIslands.end(); octalFADCIslandIter++) {
  	   		
  	   		if (strcmp((*bankReaderIter)->GetBankName().c_str(), "Nac0") == 0)
    			hSiL2Slow_Heights->Fill((*octalFADCIslandIter)->GetMax()); // NB the pulses might be negative so this could be wrong!!!
			else if (strcmp((*bankReaderIter)->GetBankName().c_str(), "Nbc0") == 0)
    			hSiR2Slow_Heights->Fill((*octalFADCIslandIter)->GetMax());
    			
			else if (strcmp((*bankReaderIter)->GetBankName().c_str(), "Nec0") == 0)
    			hSiL2Fast_Heights->Fill((*octalFADCIslandIter)->GetMax());
			else if (strcmp((*bankReaderIter)->GetBankName().c_str(), "Nfc0") == 0)
				hSiR2Fast_Heights->Fill((*octalFADCIslandIter)->GetMax());

			else if (strcmp((*bankReaderIter)->GetBankName().c_str(), "Nae0") == 0)
				hSiL1_1Fast_Heights->Fill((*octalFADCIslandIter)->GetMax());
			else if (strcmp((*bankReaderIter)->GetBankName().c_str(), "Nbe0") == 0)
				hSiL1_2Fast_Heights->Fill((*octalFADCIslandIter)->GetMax());
			else if (strcmp((*bankReaderIter)->GetBankName().c_str(), "Nce0") == 0)
				hSiL1_3Fast_Heights->Fill((*octalFADCIslandIter)->GetMax());
			else if (strcmp((*bankReaderIter)->GetBankName().c_str(), "Nde0") == 0)
				hSiL1_4Fast_Heights->Fill((*octalFADCIslandIter)->GetMax());
			else if (strcmp((*bankReaderIter)->GetBankName().c_str(), "Nee0") == 0)
				hSiR1_1Fast_Heights->Fill((*octalFADCIslandIter)->GetMax());
			else if (strcmp((*bankReaderIter)->GetBankName().c_str(), "Nfe0") == 0)
				hSiR1_2Fast_Heights->Fill((*octalFADCIslandIter)->GetMax());
			else if (strcmp((*bankReaderIter)->GetBankName().c_str(), "Nge0") == 0)
				hSiR1_3Fast_Heights->Fill((*octalFADCIslandIter)->GetMax());
			else if (strcmp((*bankReaderIter)->GetBankName().c_str(), "Nhe0") == 0)
				hSiR1_4Fast_Heights->Fill((*octalFADCIslandIter)->GetMax());

			else if (strcmp((*bankReaderIter)->GetBankName().c_str(), "Naf0") == 0)
				hSiL1_1Slow_Heights->Fill((*octalFADCIslandIter)->GetMax());
			else if (strcmp((*bankReaderIter)->GetBankName().c_str(), "Nbf0") == 0)
				hSiL1_2Slow_Heights->Fill((*octalFADCIslandIter)->GetMax());
			else if (strcmp((*bankReaderIter)->GetBankName().c_str(), "Ncf0") == 0)
				hSiL1_3Slow_Heights->Fill((*octalFADCIslandIter)->GetMax());
			else if (strcmp((*bankReaderIter)->GetBankName().c_str(), "Ndf0") == 0)
				hSiL1_4Slow_Heights->Fill((*octalFADCIslandIter)->GetMax());
			else if (strcmp((*bankReaderIter)->GetBankName().c_str(), "Nef0") == 0)
				hSiR1_1Slow_Heights->Fill((*octalFADCIslandIter)->GetMax());
			else if (strcmp((*bankReaderIter)->GetBankName().c_str(), "Nff0") == 0)
				hSiR1_2Slow_Heights->Fill((*octalFADCIslandIter)->GetMax());
			else if (strcmp((*bankReaderIter)->GetBankName().c_str(), "Ngf0") == 0)
				hSiR1_3Slow_Heights->Fill((*octalFADCIslandIter)->GetMax());
			else if (strcmp((*bankReaderIter)->GetBankName().c_str(), "Nhf0") == 0)
				hSiR1_4Slow_Heights->Fill((*octalFADCIslandIter)->GetMax()); 
  	   }
  	   
  }
  return SUCCESS;
}
