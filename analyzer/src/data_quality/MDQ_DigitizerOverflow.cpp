/********************************************************************\

Name:         MDQ_DigitizerOverflow
Created by:   Joe Grange

Contents:     hDQ_DigitizerOverflowFraction
              - Fraction of TPIs with overflow samples,
              only for fadc at the moment.  Todo: implement caen.

              hDQ_DigitizerOverflowFractionByEvent
              - Fraction of TPIs with overflow samples by event

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
INT  MDQ_DigitizerOverflow_init(void);
INT  MDQ_DigitizerOverflow(EVENT_HEADER*, void*);
INT  MDQ_DigitizerOverflow_eor(INT);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

map <std::string, TH1F*> DQ_DigitizerOverflow_histograms_map;
static TH2* hDQ_DigitizerOverflowFractionByEvent;
static TH2* hDQ_DigitizerTotalByEvent;
static TH1* hDQ_DigitizerOverflowFraction;
static TH1* hDQ_DigitizerTotal;

ANA_MODULE MDQ_DigitizerOverflow_module =
{
	"MDQ_DigitizerOverflow",                    /* module name           */
	"Joe Grange",              /* author                */
	MDQ_DigitizerOverflow,                      /* event routine         */
	NULL,                          /* BOR routine           */
	MDQ_DigitizerOverflow_eor,                          /* EOR routine           */
	MDQ_DigitizerOverflow_init,                 /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};

/** This method initializes histograms.
*/
INT MDQ_DigitizerOverflow_init()
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

    // hDQ_DigitizerOverflowFraction_[DetName]
    std::string histname = "hDQ_DigitizerOverflow_" + detname;
    std::string histtitle = "Number of overflow samples in " + detname;
    TH1F* hDQ_Histogram = new TH1F(histname.c_str(), histtitle.c_str(),100,0,100);
    hDQ_Histogram->GetXaxis()->SetTitle("MIDAS event number");
    hDQ_Histogram->GetYaxis()->SetTitle("Number of overflow samples");
    hDQ_Histogram->SetBit(TH1::kCanRebin);
    DQ_DigitizerOverflow_histograms_map[bankname] = hDQ_Histogram;
  }

   hDQ_DigitizerOverflowFraction = new TH1D("hDQ_DigitizerOverflowFraction", "Fraction of TPIs with at least one overflow sample value",1,0,1);
   hDQ_DigitizerOverflowFraction->SetBit(TH1::kCanRebin);
   hDQ_DigitizerOverflowFraction->GetXaxis()->SetTitle("bank name");
   hDQ_DigitizerOverflowFraction->GetYaxis()->SetTitle("Fraction of TPIs with overflow samples");

   hDQ_DigitizerTotal = new TH1D("hDQ_DigitizerTotal", "Total number of TPIs (for normalization)",1,0,1);
   hDQ_DigitizerTotal->SetBit(TH1::kCanRebin);
   hDQ_DigitizerTotal->GetXaxis()->SetTitle("bank name");
   hDQ_DigitizerTotal->GetYaxis()->SetTitle("Number of TPIs with overflow samples");

   hDQ_DigitizerOverflowFractionByEvent = new TH2D("hDQ_DigitizerOverflowFractionByEvent", "Fraction of TPIs with at least one sample overflow value",1,0,1, 1000,0,1000);
   hDQ_DigitizerOverflowFractionByEvent->SetBit(TH1::kCanRebin);
   hDQ_DigitizerOverflowFractionByEvent->GetXaxis()->SetTitle("bank name");
   hDQ_DigitizerOverflowFractionByEvent->GetYaxis()->SetTitle("MIDAS event number");

   hDQ_DigitizerTotalByEvent = new TH2I("hDQ_DigitizerTotalByEvent", "Total number of TPIs (for normalization)",1,0,1, 1000,0,1000);
   hDQ_DigitizerTotalByEvent->SetBit(TH1::kCanRebin);
   hDQ_DigitizerTotalByEvent->GetXaxis()->SetTitle("bank name");
   hDQ_DigitizerTotalByEvent->GetYaxis()->SetTitle("MIDAS event number");

  gDirectory->Cd("/MidasHists/");
  return SUCCESS;
}

/** This method fills the histograms
 */
INT MDQ_DigitizerOverflow(EVENT_HEADER *pheader, void *pevent)
{
	// Get the event number
	int midas_event_number = pheader->serial_number;

        bool pulse_overflow = 0;
        int overflow_value = 0;

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

          if (TSetupData::IsFADC(bankname))        overflow_value = 4095;
          if (TSetupData::IsHoustonCAEN(bankname)) overflow_value = 16383;
          if (TSetupData::IsBostonCAEN(bankname))  overflow_value = 4095;

	  // Loop over the TPulseIslands and plot the histogram
	  for (std::vector<TPulseIsland*>::iterator pulseIter = thePulses.begin(); pulseIter != thePulses.end(); ++pulseIter) {

	    // Make sure the histograms exist and then fill them
	    if (DQ_DigitizerOverflow_histograms_map.find(bankname) != DQ_DigitizerOverflow_histograms_map.end()) {

              pulse_overflow = 0;

              //loop over samples, check for overflow value
              std::vector<int> theSamples = (*pulseIter)->GetSamples();
              for (std::vector<int>::iterator sampleIter = theSamples.begin(); sampleIter != theSamples.end(); ++sampleIter){
                int sample_number = sampleIter - theSamples.begin();
                int sample_value = *sampleIter;
                //if (sample_value == 8191){
                if (sample_value >= overflow_value){
                //if ((sample_value > 4095)&&(TSetupData::IsFADC(bankname))){
                //if ((sample_value == 8191)&&(TSetupData::IsFADC(bankname))){
                  //printf(" sample overflow! sample #%d value %d midas event #%d bankname %s detname %s\n",sample_number,sample_value,midas_event_number,bankname.c_str(),detname.c_str());
                  DQ_DigitizerOverflow_histograms_map[bankname]->Fill(midas_event_number,1);
                  pulse_overflow = 1;

                }
                
              }

            //if (TSetupData::IsFADC(bankname)){
              hDQ_DigitizerTotal->Fill(bankname.c_str(),1);
              hDQ_DigitizerTotalByEvent->Fill(bankname.c_str(),midas_event_number,1);
              if ((pulse_overflow)){
                hDQ_DigitizerOverflowFraction->Fill(bankname.c_str(),1);
                hDQ_DigitizerOverflowFractionByEvent->Fill(bankname.c_str(),midas_event_number,1);
                //printf(" total pulses %d integral %f\n",total_pulses,hDQ_ADCSampleOverflow->Integral());
              }
            //}

	    }
	  }
	}

	return SUCCESS;
}

/** This method does any last minute things to the histograms at the end of the run
 */
INT MDQ_DigitizerOverflow_eor(INT run_number) {

//Total hist has more bins than overflow hist (includes banks with no overflow samples)
int nBanksTotal = hDQ_DigitizerTotal->GetNbinsX();
int nBanksOverflow = hDQ_DigitizerOverflowFraction->GetNbinsX();
int nMidasEvents = hDQ_DigitizerOverflowFractionByEvent->GetNbinsY();
int nOverflow = 0;
int nTotal = 0;

//look for matching bank names, scale
for (int i=1; i<=nBanksTotal; i++){
  //printf(" bin i: %d total label i %s\n",i,hDQ_ADCSampleTotal->GetXaxis()->GetBinLabel(i));
  for (int j=1; j<=nBanksOverflow; j++){
    //printf(" bin j %d overflow label %s\n",j,hDQ_DigitizerOverflowFraction->GetXaxis()->GetBinLabel(j));
    if (strcmp(hDQ_DigitizerTotal->GetXaxis()->GetBinLabel(i),hDQ_DigitizerOverflowFraction->GetXaxis()->GetBinLabel(j))==0){
    	nOverflow = 0;
        nOverflow = hDQ_DigitizerOverflowFraction->GetBinContent(j); 
        nTotal = 0;
        nTotal = hDQ_DigitizerTotal->GetBinContent(i);

    	printf("match! total label %s overflow label %s\n",hDQ_DigitizerTotal->GetXaxis()->GetBinLabel(i),hDQ_DigitizerOverflowFraction->GetXaxis()->GetBinLabel(j));
        if (nTotal>0) hDQ_DigitizerOverflowFraction->SetBinContent(j,(double)nOverflow/nTotal);
        
        for (int k=1; k<nMidasEvents; k++){
          nOverflow = 0;
          nOverflow = hDQ_DigitizerOverflowFractionByEvent->GetBinContent(j,k);
          nTotal = 0;
          nTotal = hDQ_DigitizerTotalByEvent->GetBinContent(i,k);//i,k to match banks
          if (nTotal>0 && nOverflow>0){
            hDQ_DigitizerOverflowFractionByEvent->SetBinContent(j,k,(double)nOverflow/nTotal);
            printf(" bankname %s midas event %d nOverflow is %d nTotal is %d ratio is %f\n",hDQ_DigitizerTotal->GetXaxis()->GetBinLabel(i),k,nOverflow,nTotal,(float)nOverflow/nTotal);
          }  
      }
    }
  }
}

}

