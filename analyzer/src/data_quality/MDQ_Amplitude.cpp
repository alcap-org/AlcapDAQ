////////////////////////////////////////////////////////////////////////////////
/// \defgroup MDQ_Amplitude
/// \ingroup lldq
/// \author Nam Tran
///
/// \brief
/// Histograms the TPI amplitudes for each detector.
/// Creates a number of amplitude plots, where the amplitude
/// is in units of ADC counts.
/// @{

/// \var DQ_Amplitude_histograms_map
/// \brief
/// Map of bank name to histogram of amplitudes for
/// the TPIs of a given MIDAS event.

/// \var DQ_Amplitude_histograms_normalised_map
/// \brief
/// Same as ::DQ_Amplitude_histograms_map but normalized
/// to number of muons according to TDC.

/// \var DQ_Amplitude_histograms_ped_sub_map
/// \brief
/// Same as ::DQ_Amplitude_histograms_map but the
/// amplitudes have the pedestal (according to ODB)
/// subtracted off.
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
INT  MDQ_Amplitude_init(void);
INT  MDQ_Amplitude(EVENT_HEADER*, void*);
INT  MDQ_Amplitude_eor(INT);

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

map <std::string, TH1F*> DQ_Amplitude_histograms_map;
map <std::string, TH1F*> DQ_Amplitude_histograms_normalised_map;
map <std::string, TH1F*> DQ_Amplitude_histograms_ped_sub_map;

extern TH1F* hDQ_TDCCheck_TTSc;

ANA_MODULE MDQ_Amplitude_module =
{
	"MDQ_Amplitude",                    /* module name           */
	"Nam Tran",              /* author                */
	MDQ_Amplitude,                      /* event routine         */
	NULL,                          /* BOR routine           */
	MDQ_Amplitude_eor,                          /* EOR routine           */
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
  if (!gDirectory->Cd("DataQuality_LowLevel")) {
    
    std::string dir_name("DataQuality_LowLevel/");
    gDirectory->mkdir(dir_name.c_str());
    gDirectory->Cd(dir_name.c_str());
  }

  // Create a histogram for each detector
  std::map<std::string, std::string> Bank2DetMap = gSetup->fBankToDetectorMap;

  typedef std::map<std::string, std::string>::iterator String2StringMapIter;

  for(String2StringMapIter mapIter = Bank2DetMap.begin(); 
      mapIter != Bank2DetMap.end(); mapIter++) { 

    std::string bankname = mapIter->first;
    std::string detname = gSetup->GetDetectorName(bankname);
    int n_bits = gSetup->GetNBits(bankname);
    int max_adc_value = std::pow(2, n_bits);

    // hDQ_Amplitude_[DetName]_[BankName]
    std::string histname = "hDQ_Amplitude_" + detname + "_" + bankname;
    std::string histtitle = "Amplitude of Pulses in " + detname;
    TH1F* hDQ_Histogram = new TH1F(histname.c_str(), histtitle.c_str(), 
				max_adc_value, 0, max_adc_value);
    hDQ_Histogram->GetXaxis()->SetTitle("Amplitude [adc]");
    hDQ_Histogram->GetYaxis()->SetTitle("Counts");
    DQ_Amplitude_histograms_map[bankname] = hDQ_Histogram;
    
    // The normalised histogram
    std::string normhistname = histname + "_normalised";
    std::string normhisttitle = histtitle + " (normalised)";
    TH1F* hDQ_Histogram_Normalised = new TH1F(normhistname.c_str(), normhisttitle.c_str(), max_adc_value,0,max_adc_value);
    hDQ_Histogram_Normalised->GetXaxis()->SetTitle("Amplitude [adc]");
    std::string yaxislabel = hDQ_Histogram->GetYaxis()->GetTitle();
    yaxislabel += " per TDC TSc Hit";
    hDQ_Histogram_Normalised->GetYaxis()->SetTitle(yaxislabel.c_str());
    DQ_Amplitude_histograms_normalised_map[bankname] = hDQ_Histogram_Normalised;
    
    // The pedestal subtracted histogram
    std::string pedsubhistname = histname + "_ped_sub";
    std::string pedsubhisttitle = histtitle + " (pedestal subtracted)";
    TH1F* hDQ_Histogram_PedSub = new TH1F(pedsubhistname.c_str(), pedsubhisttitle.c_str(), 2*max_adc_value,-max_adc_value,max_adc_value); // make sure we see any negative amplitudes
    hDQ_Histogram_PedSub->GetXaxis()->SetTitle("Amplitude [adc]");
    hDQ_Histogram_PedSub->GetYaxis()->SetTitle("Counts");
    DQ_Amplitude_histograms_ped_sub_map[bankname] = hDQ_Histogram_PedSub;
  }

  gDirectory->Cd("/MidasHists/");
  return SUCCESS;
}

/** This method does any last minute things to the histograms at the end of the run
 */
INT MDQ_Amplitude_eor(INT run_number) {
  
  // Some typedefs
  typedef map<string, vector<TPulseIsland*> > TStringPulseIslandMap;
  typedef pair<string, vector<TPulseIsland*> > TStringPulseIslandPair;
  typedef map<string, vector<TPulseIsland*> >::iterator map_iterator;
  
  // Fetch a reference to the gData structure that stores a map
  // of (bank_name, vector<TPulseIsland*>) pairs
  TStringPulseIslandMap& pulse_islands_map =
    gData->fPulseIslandToChannelMap;

  // Loop over the map and get each bankname, vector pair
  for (map_iterator mapIter = pulse_islands_map.begin(); mapIter != pulse_islands_map.end(); ++mapIter) {

    std::string bankname = mapIter->first;
    std::string detname = gSetup->GetDetectorName(bankname);
      
    // Make sure the histograms exist and then fill them
    if (DQ_Amplitude_histograms_normalised_map.find(bankname) != DQ_Amplitude_histograms_normalised_map.end()) {
      DQ_Amplitude_histograms_normalised_map[bankname]->Scale(1./hDQ_TDCCheck_TTSc->GetEntries());
    }
  }
  
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

	  // Get the histograms first
	  TH1F* hDQ_Amplitude = DQ_Amplitude_histograms_map[bankname];
	  //TH1F* hDQ_Amplitude_Norm = DQ_Amplitude_histograms_normalised_map[bankname];
	  TH1F* hDQ_Amplitude_PedSub = DQ_Amplitude_histograms_ped_sub_map[bankname];
	  // Loop over the TPulseIslands and plot the histogram
	  for (std::vector<TPulseIsland*>::iterator pulseIter = thePulses.begin();
				pulseIter != thePulses.end(); ++pulseIter) {
	    // Make sure the histograms exist and then fill them
			if (DQ_Amplitude_histograms_map.find(bankname) !=
					DQ_Amplitude_histograms_map.end()) 
			{ 
			  /*				bool underflow = false;
				const std::vector<int>& theSamples = (*pulseIter)->GetSamples();
				for (std::vector<int>::const_iterator sampleIter = theSamples.begin(); 
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
			  */
			  const std::vector<int>& theSamples = (*pulseIter)->GetSamples();
			  int peak_sample = (*pulseIter)->GetPeakSample();
			  hDQ_Amplitude->Fill(theSamples.at(peak_sample));
			  //hDQ_Amplitude_Norm->Fill(theSamples.at(peak_sample));
			  
			  int amplitude = (*pulseIter)->GetPulseHeight();
			  hDQ_Amplitude_PedSub->Fill(amplitude);
				
	    }
	  }
	}
	return SUCCESS;
}

/// @}
