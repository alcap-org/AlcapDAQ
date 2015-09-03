////////////////////////////////////////////////////////////////////////////////
/// \defgroup MDQ_PulseShapes
/// \ingroup lldq
/// \author Nam Tran
///
/// \brief
/// Plots the persistance plots and Y projections of these, along
/// with pedestal and pedestal noise of each bank averaged over
/// the run.
///
/// \details
/// Creates a number of histograms.
/// @{

/// \var DQ_PulseShapes_histograms_map
/// \brief
/// Map of bank names to 2D histograms that represent the persitance
/// oscilliscope plots of TPIs.

/// \var DQ_PulseShapesProjectionY_histograms_map
/// \brief
/// Map of bank names to Y projections of full histograms
/// in ::DQ_PulseShapes_histograms_map.

/// \var hDQ_PulseShapes_Pedestals
/// \brief
/// Histogram with bank name on horizontal axis and and pedestal
/// on vertical. The pedestal is the bin with the peak
/// in ::DQ_PulseShapesProjectionY_histograms_map

/// \var hDQ_PulseShapes_Noises
/// \brief
/// Histogram with bank name on horizontal axis and noise on vertical.
/// The noise is twice the the first bin
/// in ::DQ_PulseShapesProjectionY_histograms_map at half the value
/// in ::hDQ_PulseShapes_Pedestals.
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
INT  MDQ_PulseShapes_init(void);
INT  MDQ_PulseShapes(EVENT_HEADER*, void*);
INT  MDQ_PulseShapes_eor(INT);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

map <std::string, TH2F*> DQ_PulseShapes_histograms_map;
map <std::string, TH1D*> DQ_PulseShapesProjectionY_histograms_map;
TH1F* hDQ_PulseShapes_Pedestals;
TH1F* hDQ_PulseShapes_Noises;

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
  std::string dir_name("DQ_PulseShapes/");
  if (!gDirectory->Cd(dir_name.c_str())) {
    gDirectory->mkdir(dir_name.c_str())->cd();
  }

  // Create a histogram for each detector
  std::map<std::string, std::string> Bank2DetMap = gSetup->fBankToDetectorMap;
  for(std::map<std::string, std::string>::iterator mapIter = Bank2DetMap.begin(); 
      mapIter != Bank2DetMap.end(); mapIter++) { 

    std::string bankname = mapIter->first;
    std::string detname = gSetup->GetDetectorName(bankname);

    if(!TSetupData::IsWFD(bankname)) continue;

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
INT MDQ_PulseShapes(EVENT_HEADER *pheader, void *pevent) {
  // Some typedefs
  typedef map<string, vector<TPulseIsland*> > TStringPulseIslandMap;
  typedef pair<string, vector<TPulseIsland*> > TStringPulseIslandPair;
  typedef map<string, vector<TPulseIsland*> >::const_iterator map_iterator;

  // Fetch a reference to the gData structure that stores a map
  // of (bank_name, vector<TPulseIsland*>) pairs
  const TStringPulseIslandMap& pulse_islands_map =
    gData->fPulseIslandToChannelMap;

  // Loop over the map and get each bankname, vector pair
  for (map_iterator mapIter = pulse_islands_map.begin(); 
       mapIter != pulse_islands_map.end(); ++mapIter) {
    const std::string& bankname = mapIter->first;
    const std::vector<TPulseIsland*>& thePulses = mapIter->second;
	  
    if (DQ_PulseShapes_histograms_map.find(bankname) ==
	DQ_PulseShapes_histograms_map.end()) continue;
    // Get the histograms before looping through the pulses
    TH2F* hDQ_PulseShapes = DQ_PulseShapes_histograms_map[bankname];

    // Loop over the TPulseIslands and plot the histogram
    for (std::vector<TPulseIsland*>::const_iterator pulseIter = thePulses.begin(),
	   endp = thePulses.end();
	 pulseIter != endp; ++pulseIter) {

      // Fill the Histograms
      const vector<int>& theSamples = (*pulseIter)->GetSamples();
      for (vector<int>::const_iterator sampleIter = theSamples.begin(),
	     ends = theSamples.end(); 
	   sampleIter != ends; ++sampleIter) {
	const int sample_number = sampleIter - theSamples.begin();
	hDQ_PulseShapes->Fill(sample_number,*sampleIter);
      }
    }
  }
  return SUCCESS;
}

INT MDQ_PulseShapes_eor(INT run_number) { // Make projection

  typedef std::map<std::string, std::string>::iterator String2StringMapIter;
  std::map<std::string, std::string> Bank2DetMap = gSetup->fBankToDetectorMap;

  for(String2StringMapIter mapIter = Bank2DetMap.begin(); 
      mapIter != Bank2DetMap.end(); mapIter++) { 
    std::string bankname = mapIter->first;
    std::string detname = gSetup->GetDetectorName(bankname);

    if (DQ_PulseShapes_histograms_map.find(bankname) !=
	DQ_PulseShapes_histograms_map.end()) {
      TH2F* hPulseShapes = DQ_PulseShapes_histograms_map[bankname];
      TH1D* hDQ_Histogram_projY = hPulseShapes->ProjectionY();

      DQ_PulseShapesProjectionY_histograms_map[bankname]->Add(hDQ_Histogram_projY, 1);
      hDQ_Histogram_projY->SetDirectory(0); // not save this in the output

      // Take pedestal and noise as mean and RMS of the projections
      // but first set the range so that we don't get the massive bins
      // at 0 or the max_adc_value
      hDQ_Histogram_projY->GetXaxis()->SetRange(2, hDQ_Histogram_projY->GetNbinsX()-1);
      int max_bin = hDQ_Histogram_projY->GetMaximumBin();

      double pedestal = hDQ_Histogram_projY->GetBinCenter(max_bin);
      double pedestal_value = hDQ_Histogram_projY->GetBinContent(max_bin);
      double noise = 0;
      for (int iBin = max_bin; iBin < hDQ_Histogram_projY->GetNbinsX(); ++iBin) {
	double value = hDQ_Histogram_projY->GetBinContent(iBin);
	if (value < 0.5*pedestal_value) { // rms as half max
	  noise = hDQ_Histogram_projY->GetBinCenter(iBin) - pedestal;
	  break;
	}
      }

      std::string binlabel = bankname + " (" + detname + ")";
      hDQ_PulseShapes_Pedestals->Fill(binlabel.c_str(), pedestal);
      hDQ_PulseShapes_Noises->Fill(binlabel.c_str(), 2*noise); 
      // at the moment, noise is just from the peak to the half-way
      // down on one side so multiply by 
    }
  }

  return SUCCESS;
}

/// @}
