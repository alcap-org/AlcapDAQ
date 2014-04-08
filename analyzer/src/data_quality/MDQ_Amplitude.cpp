/********************************************************************\

Name:         MDQ_Amplitude
Created by:   NT

Contents:     hDQ_Amplitude_[DetName] 
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
INT  MDQ_Amplitude_init(void);
INT  MDQ_Amplitude(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;


#include "TDirectory.h"
#include "TFile.h"
#include "TApplication.h"
#include "TROOT.h"
extern TDirectory * gManaHistsDir;
extern TFile * gManaOutputFile;
extern TApplication * manaApp;
extern TROOT * gROOT;

extern map <std::string, TH2F*> DQ_PulseShapesProjectionY_histograms_map;
map <std::string, TH1F*> DQ_Amplitude_histograms_map;

ANA_MODULE MDQ_Amplitude_module =
{
	"MDQ_Amplitude",                    /* module name           */
	"Nam Tran",              /* author                */
	MDQ_Amplitude,                      /* event routine         */
	NULL,                          /* BOR routine           */
	NULL,                          /* EOR routine           */
	MDQ_Amplitude_init,                 /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};

/** This method initializes histograms.
*/
INT MDQ_Amplitude_init()
{
	// See if the DataQuality_LowLevel/ directory already exists
	std::string dir_name("DataQuality_LowLevel/");
  if (!gDirectory->Cd(dir_name.c_str())) {
    gDirectory->mkdir(dir_name.c_str());
	}
	gDirectory->Cd(dir_name.c_str());

	dir_name = "Amplitude";
  if (!gDirectory->Cd(dir_name.c_str())) {
    gDirectory->mkdir(dir_name.c_str());
  }
	gDirectory->Cd(dir_name.c_str());

  // Create a histogram for each detector
  std::map<std::string, std::string> Bank2DetMap = gSetup->fBankToDetectorMap;

	typedef std::map<std::string, std::string>::iterator String2StringMapIter;

  for(String2StringMapIter mapIter = Bank2DetMap.begin(); 
      mapIter != Bank2DetMap.end(); mapIter++) { 

    std::string bankname = mapIter->first;
    std::string detname = gSetup->GetDetectorName(bankname);

    // hDQ_Amplitude_[DetName]
    std::string histname = "hDQ_Amplitude_" + detname;
    std::string histtitle = "Pulse shape of " + detname;
    TH1F* hDQ_Histogram = new TH1F(histname.c_str(), histtitle.c_str(), 
				4096, 0, 4095);
    hDQ_Histogram->GetXaxis()->SetTitle("Amplitude [adc]");
    hDQ_Histogram->GetYaxis()->SetTitle("Count");
    DQ_Amplitude_histograms_map[bankname] = hDQ_Histogram;
  }

  gDirectory->Cd("/MidasHists/");
  return SUCCESS;
}

/** This method fills the histograms
 */
INT MDQ_Amplitude(EVENT_HEADER *pheader, void *pevent)
{
	// Some typedefs
	typedef map<string, vector<TPulseIsland*> > TStringPulseIslandMap;
	typedef pair<string, vector<TPulseIsland*> > TStringPulseIslandPair;
	typedef map<string, vector<TPulseIsland*> >::iterator map_iterator;

	// Fetch a reference to the gData structure that stores a map
	// of (bank_name, vector<TPulseIsland*>) pairs
	TStringPulseIslandMap& pulse_islands_map = gData->fPulseIslandToChannelMap;

	// Loop over the map and get each bankname, vector pair
	for (map_iterator mapIter = pulse_islands_map.begin(); 
			mapIter != pulse_islands_map.end(); ++mapIter) 
	{
	  std::string bankname = mapIter->first;
	  std::string detname = gSetup->GetDetectorName(bankname);
	  std::vector<TPulseIsland*> thePulses = mapIter->second;
	  // Loop over the TPulseIslands and plot the histogram
		for (std::vector<TPulseIsland*>::iterator pulseIter = thePulses.begin();
				pulseIter != thePulses.end(); ++pulseIter) {
	    // Make sure the histograms exist and then fill them
			if (DQ_Amplitude_histograms_map.find(bankname) !=
					DQ_Amplitude_histograms_map.end()) 
			{ 
				bool underflow = false;
				std::vector<int> theSamples = (*pulseIter)->GetSamples();
				for (std::vector<int>::iterator sampleIter = theSamples.begin(); 
						sampleIter != theSamples.end(); ++sampleIter)
				{
					if (*sampleIter == 4096)
						underflow == true;
				}
				if (!underflow)
				{
					int amplitude = (*pulseIter)->GetPulseHeight();
					DQ_Amplitude_histograms_map[bankname]->Fill(amplitude);
				}
	    }
	  }
	}
	return SUCCESS;
}
