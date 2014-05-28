/********************************************************************\

Name:         MDQ_PulseShapes
Created by:   Nam Tran

Contents:     hDQ_PulseShapes_[DetName]_[BankName]
              - plots the persistency pulse plot

	      hDQ_PulseShapes_ProjectionY_[DetName]_[BankName]
              - plots the projection of the PusleShapes plot onto
                the y-axis

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
INT  MDQ_PulseShapes_init(void);
INT  MDQ_PulseShapes(EVENT_HEADER*, void*);
INT  MDQ_PulseShapes_eor(INT);

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

map <std::string, TH2F*> DQ_PulseShapes_histograms_map;
map <std::string, TH1D*> DQ_PulseShapesProjectionY_histograms_map;
TH1F* hDQ_PulseShapes_Pedestals;
TH1F* hDQ_PulseShapes_Noises;

static int GetLastPresampleBin(std::string bankname);

ANA_MODULE MDQ_PulseShapes_module =
{
	"MDQ_PulseShapes",                    /* module name           */
	"Nam Tran",              /* author                */
	MDQ_PulseShapes,                      /* event routine         */
	NULL,                          /* BOR routine           */
	MDQ_PulseShapes_eor,                          /* EOR routine           */
	MDQ_PulseShapes_init,                 /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};

/** This method initializes histograms.
*/
INT MDQ_PulseShapes_init()
{
  // See if the DataQuality_LowLevel/ directory already exists
  std::string dir_name("DataQuality_LowLevel/");
  if (!gDirectory->Cd(dir_name.c_str())) {
    gDirectory->mkdir(dir_name.c_str());
  }
  gDirectory->Cd(dir_name.c_str());

  // Create a histogram for each detector
  std::map<std::string, std::string> Bank2DetMap = gSetup->fBankToDetectorMap;
  for(std::map<std::string, std::string>::iterator mapIter = Bank2DetMap.begin(); 
      mapIter != Bank2DetMap.end(); mapIter++) { 

    std::string bankname = mapIter->first;
    std::string detname = gSetup->GetDetectorName(bankname);

    int n_bits = gSetup->GetNBits(bankname);
    int max_adc_value = std::pow(2, n_bits);

    // hDQ_PulseShapes_[DetName]_[BankName]
    std::string histname = "hDQ_PulseShapes_" + detname + "_" + bankname;
    std::string histtitle = "Pulse shape of " + detname;
    TH2F* hDQ_Histogram = new TH2F(histname.c_str(), histtitle.c_str(), 
				400, -0.5, 399.5,
				max_adc_value, 0, max_adc_value);
    hDQ_Histogram->GetXaxis()->SetTitle("Time Stamp [ns]");
    hDQ_Histogram->GetYaxis()->SetTitle("Pulse height [adc]");
    DQ_PulseShapes_histograms_map[bankname] = hDQ_Histogram;

    // hDQ_PulseShapesProjectionY_[DetName]_[BankName]
    histname = "hDQ_PulseShapes_ProjectionY_" + detname + "_" + bankname;
    histtitle = "Pulse shape Y-projection of " + detname;
    TH1D* hDQ_Histogram_TH1D = new TH1D(histname.c_str(), histtitle.c_str(), 
				max_adc_value, 0, max_adc_value);
    hDQ_Histogram_TH1D->GetXaxis()->SetTitle("ADC");
    hDQ_Histogram_TH1D->GetYaxis()->SetTitle("Arbitary unit");
    DQ_PulseShapesProjectionY_histograms_map[bankname] = hDQ_Histogram_TH1D;
  }

  // hDQ_PulseShapes_Pedestals
  std::string histname = "hDQ_PulseShapes_Pedestals";
  std::string histtitle = "Pedestals";
  hDQ_PulseShapes_Pedestals = new TH1F(histname.c_str(), histtitle.c_str(),1,0,1);
  hDQ_PulseShapes_Pedestals->GetXaxis()->SetTitle("Bank (Detector)");
  hDQ_PulseShapes_Pedestals->GetYaxis()->SetTitle("Pedestal [ADC]");
  hDQ_PulseShapes_Pedestals->SetBit(TH1::kCanRebin);

  // hDQ_PulseShapes_Noises
  histname = "hDQ_PulseShapes_Noises";
  histtitle = "Noises";
  hDQ_PulseShapes_Noises = new TH1F(histname.c_str(), histtitle.c_str(),1,0,1);
  hDQ_PulseShapes_Noises->GetXaxis()->SetTitle("Bank (Detector)");
  hDQ_PulseShapes_Noises->GetYaxis()->SetTitle("Noise (FWHM) [ADC]");
  hDQ_PulseShapes_Noises->SetBit(TH1::kCanRebin);

  // Back to root directory
  gDirectory->Cd("/MidasHists/");
  return SUCCESS;
}

/** This method fills the histograms
 */
INT MDQ_PulseShapes(EVENT_HEADER *pheader, void *pevent)
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
			if (DQ_PulseShapes_histograms_map.find(bankname) !=
					DQ_PulseShapes_histograms_map.end()) 
			{ 
				std::vector<int> theSamples = (*pulseIter)->GetSamples();
				for (std::vector<int>::iterator sampleIter = theSamples.begin(); 
						sampleIter != theSamples.end(); ++sampleIter)
				{
					int sample_number = sampleIter - theSamples.begin();
					//int sample_number = 0;
					int sample_value = *sampleIter;
					DQ_PulseShapes_histograms_map[bankname]->Fill(sample_number,sample_value);
				}
	    }
	  }
	}
	return SUCCESS;
}

INT MDQ_PulseShapes_eor(INT run_number) // Make projection
{
	typedef std::map<std::string, std::string>::iterator String2StringMapIter;
	std::map<std::string, std::string> Bank2DetMap = gSetup->fBankToDetectorMap;

	for(String2StringMapIter mapIter = Bank2DetMap.begin(); 
			mapIter != Bank2DetMap.end(); mapIter++) 
	{ 
		std::string bankname = mapIter->first;
		std::string detname = gSetup->GetDetectorName(bankname);

		if (DQ_PulseShapes_histograms_map.find(bankname) !=
				DQ_PulseShapes_histograms_map.end())
		{
       		        TH2F* hPulseShapes = DQ_PulseShapes_histograms_map[bankname];
			TH1D* hDQ_Histogram_projY = hPulseShapes->ProjectionY();

			DQ_PulseShapesProjectionY_histograms_map[bankname]->Add(
					hDQ_Histogram_projY, 1);
			hDQ_Histogram_projY->SetDirectory(0); // not save this in the output

			// Take pedestal and noise as mean and RMS of the projections
			// but first set the range so that we don't get the massive bins at 0 or the max_adc_value
			hDQ_Histogram_projY->GetXaxis()->SetRange(2, hDQ_Histogram_projY->GetNbinsX()-1);
			int max_bin = hDQ_Histogram_projY->GetMaximumBin();

			double pedestal = hDQ_Histogram_projY->GetBinCenter(max_bin);
			double pedestal_value = hDQ_Histogram_projY->GetBinContent(max_bin);
			double noise = 0;
			for (int iBin = max_bin; iBin < hDQ_Histogram_projY->GetNbinsX(); ++iBin) {
			  double value = hDQ_Histogram_projY->GetBinContent(iBin);
			  if (value < 0.5*pedestal_value) {
			    noise = hDQ_Histogram_projY->GetBinCenter(iBin) - pedestal;
			    break;
			  }
			}

			std::string binlabel = bankname + " (" + detname + ")";
			hDQ_PulseShapes_Pedestals->Fill(binlabel.c_str(), pedestal);
			hDQ_PulseShapes_Noises->Fill(binlabel.c_str(), 2*noise); // at the moment, noise is just from the peak to the half-way down on one side so multiply by 2
		}
	}

	return SUCCESS;
}
