/********************************************************************\

Name:         MDQ_ADCSampleOverflow
Created by:   Joe Grange

Contents:     hDQ_ADCSampleOverflow
              - only for FADC at the moment.  Todo: implement caen.

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

/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"
#include "TPulseIsland.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

/*-- Module declaration --------------------------------------------*/
INT  MDQ_ADCSampleOverflow_init(void);
INT  MDQ_ADCSampleOverflow(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

map <std::string, TH1F*> DQ_ADCSampleOverflow_histograms_map;
static TH2* hDQ_ADCSampleOverflowByEvent;
static TH1* hDQ_ADCSampleOverflow;
static TH1* hDQ_ADCSampleTotal;

ANA_MODULE MDQ_ADCSampleOverflow_module =
{
	"MDQ_ADCSampleOverflow",                    /* module name           */
	"Joe Grange",              /* author                */
	MDQ_ADCSampleOverflow,                      /* event routine         */
	NULL,                          /* BOR routine           */
	NULL,                          /* EOR routine           */
	MDQ_ADCSampleOverflow_init,                 /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};

/** This method initializes histograms.
*/
INT MDQ_ADCSampleOverflow_init()
{
    // See if the DataQuality_LowLevel/ directory already exists
  if (!gDirectory->Cd("DataQuality_LowLevel")) {
    
    std::string dir_name("DataQuality_LowLevel/");
    gDirectory->mkdir(dir_name.c_str());
    gDirectory->Cd(dir_name.c_str());
  }

  // Create a histogram for each detector
  std::map<std::string, std::string> bank_to_detector_map = gSetup->fBankToDetectorMap;
  for(std::map<std::string, std::string>::iterator mapIter = bank_to_detector_map.begin(); 
      mapIter != bank_to_detector_map.end(); mapIter++) { 

    std::string bankname = mapIter->first;
    std::string detname = gSetup->GetDetectorName(bankname);

    // hDQ_ADCSampleOverflow_[DetName]
    std::string histname = "hDQ_ADCSampleOverflow_" + detname;
    std::string histtitle = "Number of samples with FADC overflow value 8191 in " + detname;
    TH1F* hDQ_Histogram = new TH1F(histname.c_str(), histtitle.c_str(),100,0,100);
    hDQ_Histogram->GetXaxis()->SetTitle("MIDAS event number");
    hDQ_Histogram->GetYaxis()->SetTitle("Number of overflow samples");
    hDQ_Histogram->SetBit(TH1::kCanRebin);
    DQ_ADCSampleOverflow_histograms_map[bankname] = hDQ_Histogram;
  }

   hDQ_ADCSampleOverflow = new TH1D("hDQ_FADCSampleOverflow", "Number of FADC TPIs with at least one overflow sample value 8191",1,0,1);
   hDQ_ADCSampleOverflow->SetBit(TH1::kCanRebin);
   hDQ_ADCSampleOverflow->GetXaxis()->SetTitle("FADC bank name");
   hDQ_ADCSampleOverflow->GetYaxis()->SetTitle("Number of TPIs with overflow samples");

   hDQ_ADCSampleTotal = new TH1D("hDQ_FADCSampleTotal", "Total number of FADC TPIs (for normalization)",1,0,1);
   hDQ_ADCSampleTotal->SetBit(TH1::kCanRebin);
   hDQ_ADCSampleTotal->GetXaxis()->SetTitle("FADC bank name");
   hDQ_ADCSampleTotal->GetYaxis()->SetTitle("Number of TPIs with overflow samples");

   hDQ_ADCSampleOverflowByEvent = new TH2I("hDQ_FADCSampleOverflowByEvent", "Number of samples with FADC overflow value 8191",1,0,1, 1000,0,1000);
   hDQ_ADCSampleOverflowByEvent->SetBit(TH1::kCanRebin);
   hDQ_ADCSampleOverflowByEvent->GetXaxis()->SetTitle("FADC bank name");
   hDQ_ADCSampleOverflowByEvent->GetYaxis()->SetTitle("MIDAS event number");

  gDirectory->Cd("/MidasHists/");
  return SUCCESS;
}

/** This method fills the histograms
 */
INT MDQ_ADCSampleOverflow(EVENT_HEADER *pheader, void *pevent)
{
	// Get the event number
	int midas_event_number = pheader->serial_number;

        bool pulse_overflow = 0;

	// Some typedefs
	typedef map<string, vector<TPulseIsland*> > TStringPulseIslandMap;
	typedef pair<string, vector<TPulseIsland*> > TStringPulseIslandPair;
	typedef map<string, vector<TPulseIsland*> >::iterator map_iterator;

	// Fetch a reference to the gData structure that stores a map
	// of (bank_name, vector<TPulseIsland*>) pairs
	TStringPulseIslandMap& pulse_islands_map =
		gData->fPulseIslandToChannelMap;

	// Loop over the map and get each bankname, vector pair
	for (map_iterator mapIter = pulse_islands_map.begin(); mapIter != pulse_islands_map.end(); ++mapIter) 
	{
	  std::string bankname = mapIter->first;
	  std::string detname = gSetup->GetDetectorName(bankname);
	  std::vector<TPulseIsland*> thePulses = mapIter->second;

	  // Loop over the TPulseIslands and plot the histogram
	  for (std::vector<TPulseIsland*>::iterator pulseIter = thePulses.begin(); pulseIter != thePulses.end(); ++pulseIter) {

	    // Make sure the histograms exist and then fill them
	    if (DQ_ADCSampleOverflow_histograms_map.find(bankname) != DQ_ADCSampleOverflow_histograms_map.end()) {

              pulse_overflow = 0;

              //loop over samples, check for overflow value
              std::vector<int> theSamples = (*pulseIter)->GetSamples();
              for (std::vector<int>::iterator sampleIter = theSamples.begin(); sampleIter != theSamples.end(); ++sampleIter){
                int sample_number = sampleIter - theSamples.begin();
                int sample_value = *sampleIter;
                //if (sample_value == 8191){
                if ((sample_value == 8191)&&(TSetupData::IsFADC(bankname))){
                  printf("fadc DQ: sample #%d value %d midas event #%d bankname %s detname %s\n",sample_number,sample_value,midas_event_number,bankname.c_str(),detname.c_str());
                  DQ_ADCSampleOverflow_histograms_map[bankname]->Fill(midas_event_number,1);
                  hDQ_ADCSampleOverflowByEvent->Fill(bankname.c_str(),midas_event_number,1);
                  pulse_overflow = 1;

                }
                
              }

            if (TSetupData::IsFADC(bankname)){
            hDQ_ADCSampleTotal->Fill(bankname.c_str(),1);
            if ((pulse_overflow)){
              hDQ_ADCSampleOverflow->Fill(bankname.c_str(),1);
              //printf(" total pulses %d integral %f\n",total_pulses,hDQ_ADCSampleOverflow->Integral());
              }
            }

	    }
	  }
	}

	return SUCCESS;
}
