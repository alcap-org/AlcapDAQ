////////////////////////////////////////////////////////////////////////////////
/// \defgroup MDQ_DigitizerOverflow
/// \ingroup lldq
/// \author Joe Grange
///
/// \brief
/// Plots information related to FADC sample overflow, as in
/// samples that went off scale.
/// 
/// \details
/// Creates ::hDQ_DigitizerOverflow_Fraction, ::hDQ_DigitizerOverflow_FractionByEvent,
/// ::hDQ_DigitizerOverflow_Total, ::hDQ_DigitizerOverFlow_TotalByEvent.
/// @{

/// \var hDQ_DigitizerOverflow_FractionByEvent
/// \brief
/// Plots the fraction of TPIs with any sample above the 
/// largest possible ADC value for each channel on an
/// event-by-event basis

/// \var hDQ_DigitizerOverflow_TotalByEvent;
/// \brief
/// Plots total number of TPulseIslands in each channel on
/// an event-by-event basis (for normalisation
/// hDQ_DigitizerOverflow_FractionByEvent)

/// \var hDQ_DigitizerOverflow_Fraction
/// \brief
/// Plots the fraction of TPIs with any sample above
/// the largest possible ADC value for each channel

/// \var hDQ_DigitizerOverflow_Total
/// \brief
/// Plots the total number of TPulseIslands in each channel
/// (for normalisation of hDQ_DigitizerOverflow_Fraction)
////////////////////////////////////////////////////////////////////////////////

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

static TH2* hDQ_DigitizerOverflow_FractionByEvent;
static TH2* hDQ_DigitizerOverflow_TotalByEvent;
static TH1* hDQ_DigitizerOverflow_Fraction;
static TH1* hDQ_DigitizerOverflow_Total;

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

  // Get run number
  char key_name[80];
  int size,run_number;
  sprintf(key_name, "Runinfo/Run number");
  size=sizeof(int);
  db_get_value(hDB,0,key_name, &run_number, &size, TID_INT,1);

  // Create the histograms
  hDQ_DigitizerOverflow_Fraction = new TH1D("hDQ_DigitizerOverflow_Fraction",Form("Fraction of TPIs with at least one overflow sample value, run %d",run_number),1,0,1);
  hDQ_DigitizerOverflow_Fraction->SetBit(TH1::kCanRebin);
  hDQ_DigitizerOverflow_Fraction->GetXaxis()->SetTitle("Bank (Detector)");
  hDQ_DigitizerOverflow_Fraction->GetYaxis()->SetTitle("Fraction of TPIs with overflow samples");

  hDQ_DigitizerOverflow_Total = new TH1D("hDQ_DigitizerOverflow_Total", "Total number of TPIs (for normalization)",1,0,1);
  hDQ_DigitizerOverflow_Total->SetBit(TH1::kCanRebin);
  hDQ_DigitizerOverflow_Total->GetXaxis()->SetTitle("Bank (Detector)");
  hDQ_DigitizerOverflow_Total->GetYaxis()->SetTitle("Number of TPIs with overflow samples");

  hDQ_DigitizerOverflow_FractionByEvent = new TH2D("hDQ_DigitizerOverflow_FractionByEvent", Form("Fraction of TPIs with at least one sample overflow value, run %d",run_number),1,0,1, 5000,0,5000);
  hDQ_DigitizerOverflow_FractionByEvent->SetBit(TH1::kCanRebin);
  hDQ_DigitizerOverflow_FractionByEvent->GetXaxis()->SetTitle("Bank (Detector)");
  hDQ_DigitizerOverflow_FractionByEvent->GetYaxis()->SetTitle("MIDAS event number");

  hDQ_DigitizerOverflow_TotalByEvent = new TH2I("hDQ_DigitizerOverflow_TotalByEvent", "Total number of TPIs (for normalization)",1,0,1, 5000,0,5000);
  hDQ_DigitizerOverflow_TotalByEvent->SetBit(TH1::kCanRebin);
  hDQ_DigitizerOverflow_TotalByEvent->GetXaxis()->SetTitle("Bank (Detector)");
  hDQ_DigitizerOverflow_TotalByEvent->GetYaxis()->SetTitle("MIDAS event number");

  // We want the number of x-bins to be the same in all the histograms so that we
  // can use the TrendPlot macro (which doesn't check that bin labels are the same)
  std::map<std::string, std::string> bank_to_detector_map = gSetup->fBankToDetectorMap;
  for(std::map<std::string, std::string>::iterator mapIter = bank_to_detector_map.begin(); 
      mapIter != bank_to_detector_map.end(); mapIter++) { 

    std::string bankname = mapIter->first;
    std::string detname = gSetup->GetDetectorName(bankname);
    std::string binname = bankname + "(" + detname + ")";

    if(!TSetupData::IsWFD(bankname)) continue;

    hDQ_DigitizerOverflow_Fraction->Fill(binname.c_str(), 0);
    hDQ_DigitizerOverflow_Total->Fill(binname.c_str(), 0);
    hDQ_DigitizerOverflow_FractionByEvent->Fill(binname.c_str(), 0.0, 0.0);
    hDQ_DigitizerOverflow_TotalByEvent->Fill(binname.c_str(), 0.0, 0.0);
  }

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

	  if(!TSetupData::IsWFD(bankname)) continue;

	  int n_bits = gSetup->GetNBits(bankname);
	  int trigger_polarity = gSetup->GetTriggerPolarity(bankname);
	  //overflow = max bin for pos trigger polarity, zero otherwise
	  overflow_value = (std::pow(2, n_bits) - 1);

	  // Loop over the TPulseIslands and plot the histogram
	  for (std::vector<TPulseIsland*>::iterator pulseIter = thePulses.begin(); pulseIter != thePulses.end(); ++pulseIter) {

	    pulse_overflow = 0;

	    //loop over samples, check for overflow value
	    const std::vector<int>& theSamples = (*pulseIter)->GetSamples();
	    for (std::vector<int>::const_iterator sampleIter = theSamples.begin(); sampleIter != theSamples.end(); ++sampleIter){
	      int sample_number = sampleIter - theSamples.begin();
	      int sample_value = *sampleIter;

	      if (sample_value >= overflow_value && trigger_polarity == 1){
		//printf(" sample overflow! sample #%d value %d midas event #%d bankname %s detname %s\n",sample_number,sample_value,midas_event_number,bankname.c_str(),detname.c_str());
		pulse_overflow = 1;
		break;
	      }
	      else if(sample_value <= 0 && trigger_polarity == -1){
		pulse_overflow = 1;
		break;
	      }
                
	      //this should be done at the pulse level, not the sample level
	      /*
	      std::string binname = bankname + "(" + detname + ")";
              hDQ_DigitizerOverflow_Total->Fill(binname.c_str(),1);
              hDQ_DigitizerOverflow_TotalByEvent->Fill(binname.c_str(),midas_event_number,1);
              if ((pulse_overflow)){
                hDQ_DigitizerOverflow_Fraction->Fill(binname.c_str(),1);
                hDQ_DigitizerOverflow_FractionByEvent->Fill(binname.c_str(),midas_event_number,1);
                //printf(" total pulses %d integral %f\n",total_pulses,hDQ_ADCSampleOverflow->Integral());
		
              }
	      */
	    }
	    std::string binname = bankname + "(" + detname + ")";
	    hDQ_DigitizerOverflow_Total->Fill(binname.c_str(),1);
	    hDQ_DigitizerOverflow_TotalByEvent->Fill(binname.c_str(),midas_event_number,1);
	    if ((pulse_overflow)){
	      hDQ_DigitizerOverflow_Fraction->Fill(binname.c_str(),1);
	      hDQ_DigitizerOverflow_FractionByEvent->Fill(binname.c_str(),midas_event_number,1);
	   
	    }
	  }
	}

	return SUCCESS;
}

/** This method does any last minute things to the histograms at the end of the run
 */
INT MDQ_DigitizerOverflow_eor(INT run_number) {

//Total hist has more bins than overflow hist (includes banks with no overflow samples)
int nBanksTotal = hDQ_DigitizerOverflow_Total->GetNbinsX();
int nBanksOverflow = hDQ_DigitizerOverflow_Fraction->GetNbinsX();
int nMidasEvents = hDQ_DigitizerOverflow_FractionByEvent->GetNbinsY();
int nOverflow = 0;
int nTotal = 0;

//look for matching bank names, scale
for (int i=1; i<=nBanksTotal; i++){
  //printf(" bin i: %d total label i %s\n",i,hDQ_ADCSampleTotal->GetXaxis()->GetBinLabel(i));
  for (int j=1; j<=nBanksOverflow; j++){
    //printf(" bin j %d overflow label %s\n",j,hDQ_DigitizerOverflow_Fraction->GetXaxis()->GetBinLabel(j));
    if (strcmp(hDQ_DigitizerOverflow_Total->GetXaxis()->GetBinLabel(i),hDQ_DigitizerOverflow_Fraction->GetXaxis()->GetBinLabel(j))==0){
    	nOverflow = 0;
        nOverflow = hDQ_DigitizerOverflow_Fraction->GetBinContent(j); 
        nTotal = 0;
        nTotal = hDQ_DigitizerOverflow_Total->GetBinContent(i);

    	//printf("match! total label %s overflow label %s\n",hDQ_DigitizerOverflow_Total->GetXaxis()->GetBinLabel(i),hDQ_DigitizerOverflow_Fraction->GetXaxis()->GetBinLabel(j));
        if (nTotal>0) hDQ_DigitizerOverflow_Fraction->SetBinContent(j,(double)nOverflow/nTotal);
        
        for (int k=1; k<nMidasEvents; k++){
          nOverflow = 0;
          nOverflow = hDQ_DigitizerOverflow_FractionByEvent->GetBinContent(j,k);
          nTotal = 0;
          nTotal = hDQ_DigitizerOverflow_TotalByEvent->GetBinContent(i,k);//i,k to match banks
          if (nTotal>0 && nOverflow>0){
            hDQ_DigitizerOverflow_FractionByEvent->SetBinContent(j,k,(double)nOverflow/nTotal);
            //printf(" bankname %s midas event %d nOverflow is %d nTotal is %d ratio is %f\n",hDQ_DigitizerOverflow_Total->GetXaxis()->GetBinLabel(i),k,nOverflow,nTotal,(float)nOverflow/nTotal);
          }  
      }
    }
  }
}

return SUCCESS;

}

/// @}
