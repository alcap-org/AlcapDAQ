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
#include "TSimpleSiPulse.h"

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
extern map<string, vector<TSimpleSiPulse*> > theSimpleSiPulseMap;

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

	// Loop over the map and get each bankname, vector pair
	for (std::map<string, vector<TSimpleSiPulse*> >::iterator simpleSiMapIter = theSimpleSiPulseMap.begin();
			simpleSiMapIter != theSimpleSiPulseMap.end(); 
			simpleSiMapIter++) 
	{
		std::string bankname = simpleSiMapIter->first;
		std::vector<TSimpleSiPulse*> theSiPulses = simpleSiMapIter->second;
			
		// Loop over the TSimpleSiPulses and plot the histogram
		for (std::vector<TSimpleSiPulse*>::iterator siPulse = theSiPulses.begin(); siPulse != theSiPulses.end(); siPulse++) {

			double pulseheight;
			if ((*siPulse)->IsPositive())
			{
				pulseheight = (*siPulse)->GetMax() - (*siPulse)->GetPedestal();
			}
			else
			{
				TSimpleSiPulse *invertedPulse = (*siPulse)->Invert();
				pulseheight = invertedPulse->GetMax() - invertedPulse->GetPedestal();
			}

			if (strcmp(bankname.c_str(), "Nac0P") == 0)
				hSiL2Slow_Heights->Fill(pulseheight);
			else if (strcmp(bankname.c_str(), "Nbc0P") == 0)
				hSiR2Slow_Heights->Fill(pulseheight);

			else if (strcmp(bankname.c_str(), "Nec0P") == 0)
				hSiL2Fast_Heights->Fill(pulseheight);
			else if (strcmp(bankname.c_str(), "Nfc0P") == 0)
				hSiR2Fast_Heights->Fill(pulseheight);

			else if (strcmp(bankname.c_str(), "Nae0P") == 0)
				hSiL1_1Fast_Heights->Fill(pulseheight);
			else if (strcmp(bankname.c_str(), "Nbe0P") == 0)
				hSiL1_2Fast_Heights->Fill(pulseheight);
			else if (strcmp(bankname.c_str(), "Nce0P") == 0)
				hSiL1_3Fast_Heights->Fill(pulseheight);
			else if (strcmp(bankname.c_str(), "Nde0P") == 0)
				hSiL1_4Fast_Heights->Fill(pulseheight);
			else if (strcmp(bankname.c_str(), "Nee0P") == 0)
				hSiR1_1Fast_Heights->Fill(pulseheight);
			else if (strcmp(bankname.c_str(), "Nfe0P") == 0)
				hSiR1_2Fast_Heights->Fill(pulseheight);
			else if (strcmp(bankname.c_str(), "Nge0P") == 0)
				hSiR1_3Fast_Heights->Fill(pulseheight);
			else if (strcmp(bankname.c_str(), "Nhe0P") == 0)
				hSiR1_4Fast_Heights->Fill(pulseheight);

			else if (strcmp(bankname.c_str(), "Naf0P") == 0)
				hSiL1_1Slow_Heights->Fill(pulseheight);
			else if (strcmp(bankname.c_str(), "Nbf0P") == 0)
				hSiL1_2Slow_Heights->Fill(pulseheight);
			else if (strcmp(bankname.c_str(), "Ncf0P") == 0)
				hSiL1_3Slow_Heights->Fill(pulseheight);
			else if (strcmp(bankname.c_str(), "Ndf0P") == 0)
				hSiL1_4Slow_Heights->Fill(pulseheight);
			else if (strcmp(bankname.c_str(), "Nef0P") == 0)
				hSiR1_1Slow_Heights->Fill(pulseheight);
			else if (strcmp(bankname.c_str(), "Nff0P") == 0)
				hSiR1_2Slow_Heights->Fill(pulseheight);
			else if (strcmp(bankname.c_str(), "Ngf0P") == 0)
				hSiR1_3Slow_Heights->Fill(pulseheight);
			else if (strcmp(bankname.c_str(), "Nhf0P") == 0)
				hSiR1_4Slow_Heights->Fill(pulseheight);
		}
	}
	return SUCCESS;
}
