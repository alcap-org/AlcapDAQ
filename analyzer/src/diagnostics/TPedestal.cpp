/********************************************************************\

Name:         TPedestal
Created by:   Mark Wong

Contents:     One module that fills out histograms for the pulse heights, pulse shapes and the raw counts for all digitizer channels. These are all in one module to be more efficient in terms of minimising the number of times we loop through the channels.

\********************************************************************/

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <map>
#include <utility>
#include <sstream>
#include <cmath>
#include <vector>
#include <algorithm>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TH1.h>
#include <TH2.h>
#include <TSpectrum.h>

/* AlCap includes */
#include "TGlobalData.h"
#include "TTrendTree.h"
#include "TSetupData.h"
#include "TPulseIsland.h"

#include "TDetector.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

/*-- Module declaration --------------------------------------------*/
INT TPedestal_init(void);
INT TPedestal_exit(void);
INT TPedestal_init_wfd(const std::string&, const std::string&);
INT TPedestal_bor(INT);
INT TPedestal_eor(INT);

Float_t * TPedestal_findPeaks(TH1I* height_histogram);

INT TPedestal(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;
extern TTrendTree* gTrendTree;

map<std::string, TH2D*> shape_histograms_map;
map<std::string, TH1I*> height_histograms_map;

map<std::string, TH2D*> pedestal_values_map;
map<std::string, TH1D*> pedestal_mean_map;
map<std::string, TH1D*> pedestal_stddev_map;
map<std::string, TH1D*> pedestal_rms_map;

map<std::string, vector<Float_t> > peaks_map;

ANA_MODULE TPedestal_module =
{
	"TPedestal",                 /* module name           */
	"Mark Wong",                    /* author                */
	TPedestal,                   /* event routine         */
	TPedestal_bor,               /* BOR routine           */
	TPedestal_eor,               /* EOR routine           */
	TPedestal_init,              /* init routine          */
	TPedestal_exit,                           /* exit routine          */
	NULL,                           /* parameter structure   */
	0,                              /* structure size        */
	NULL,                           /* initial parameters    */
};

/** This method initializes histograms.
*/
INT TPedestal_init() {
//  gROOT->ProcessLine("#include <vector>");

  std::map<std::string, std::string> bank_to_detector_map =
      gSetup->fBankToDetectorMap;
  for(std::map<std::string, std::string>::iterator mapIter = bank_to_detector_map.begin();
      mapIter != bank_to_detector_map.end(); mapIter++) {
    std::string bankname = mapIter->first;
    std::string detname = mapIter->second;
    INT ret = SUCCESS;
    if (TSetupData::IsWFD(bankname))
      ret = TPedestal_init_wfd(bankname, detname);
    else if (TSetupData::IsTDC(bankname))
      printf("Online Plots Error: Unknown bank %s\n", bankname.c_str());
  }
  return SUCCESS;
}

INT TPedestal_init_wfd(const std::string& bank,
                                       const std::string& det) {
  const int n_digitizer_bits = gSetup->GetNBits(bank);
  const int64_t max_adc_value = std::pow(2, n_digitizer_bits);
  const double clock_tick = gSetup->GetClockTick(bank);

  std::string histname = "";
  std::string histtitle = "";
  // hPulseHeights
  histname = "h" + bank + "_Heights";
  histtitle = "Plot of the pulse heights in the " + det + " channels";
  TH1I* hPulseHeights = new TH1I(histname.c_str(), histtitle.c_str(),
                                 max_adc_value, 0, max_adc_value);
  hPulseHeights->GetXaxis()->SetTitle("Pulse Height [ADC value]");
  hPulseHeights->GetYaxis()->SetTitle("Number of Pulses");
  height_histograms_map[bank] = hPulseHeights;
  // hPulseShapes
  double min_time = 0;
  double max_time = 10000;
  double n_bins = (max_time - min_time) / clock_tick;
  histname = "h" + bank + "_Shapes";
  histtitle = "Plot of the pulse shapes in the " + det + " channels";
  TH2D* hPulseShapes = new TH2D(histname.c_str(), histtitle.c_str(),
                                500,-0.5,599.5,
                                (max_adc_value+1)/10, 0, max_adc_value+1);
  hPulseShapes->GetXaxis()->SetTitle("Time [ct]");
  hPulseShapes->GetYaxis()->SetTitle("ADC Value");
  shape_histograms_map[bank] = hPulseShapes;
  //hPedestalValues
  histname = "h" + bank + "_values";
  histtitle = "Plot of pedestal values in the " + det + " channels";
  TH2D *hPedestalValues = new TH2D(histname.c_str(), histtitle.c_str(),
                                500, 0, 599.5,
                                (max_adc_value+1)/10, 0, max_adc_value+1);
  hPedestalValues->GetXaxis()->SetTitle("Time [ct]"); 
  hPedestalValues->GetYaxis()->SetTitle("ADC Value"); 
  hPedestalValues->GetXaxis()->SetRangeUser(100, 120);
  pedestal_values_map[bank] = hPedestalValues;
  //hPedestalMean
  histname = "h" + bank + "_Mean";
  histtitle = "Plot of pedestal mean in the " + det + " channels";
  TH1D *hPedestalMean = new TH1D(histname.c_str(), histtitle.c_str(),
                                500, 0, max_adc_value+1);
  hPedestalMean->GetXaxis()->SetTitle("ADC Value"); 
  pedestal_mean_map[bank] = hPedestalMean;
  //hPedestalStdDev
  histname = "h" + bank + "_StdDev";
  histtitle = "Plot of pedestal StdDev in the " + det + " channels";
  TH1D *hPedestalStdDev = new TH1D(histname.c_str(), histtitle.c_str(),
                                500, 0, max_adc_value+1);
  hPedestalStdDev->GetXaxis()->SetTitle("ADC Value"); 
  pedestal_stddev_map[bank] = hPedestalStdDev;
  //hPedestalRMS
  histname = "h" + bank + "_Rms";
  histtitle = "Plot of pedestal RMS in the " + det + " channels";
  TH1D *hPedestalRMS = new TH1D(histname.c_str(), histtitle.c_str(),
                                500, 0, max_adc_value+1);
  hPedestalRMS->GetXaxis()->SetTitle("ADC Value"); 
  pedestal_rms_map[bank] = hPedestalRMS;

  return SUCCESS;
}

// Resets the histograms at the beginning of each run
// so that the online display updates
INT TPedestal_bor(INT run_number) {
  gTrendTree->Init();
  gTrendTree->InitRunBranch<int>("run", 0);
  gTrendTree->FillRunTree("run", run_number);
  std::map<std::string, std::string> bank_to_detector_map = gSetup->fBankToDetectorMap;
  for(std::map<std::string, std::string>::iterator mapIter = bank_to_detector_map.begin();
      mapIter != bank_to_detector_map.end(); mapIter++) {
    std::string bankname = mapIter->first;
    if(TSetupData::IsWFD(bankname) ) {
      height_histograms_map[bankname]->Reset();
      shape_histograms_map[bankname]->Reset();
      pedestal_values_map[bankname]->Reset();
    }
  }
  return SUCCESS;
}
INT TPedestal_eor(INT run_number) {
  TDetector * detector = new TDetector("");
  gTrendTree->InitRunBranch<TDetector *>("detector", detector);
  std::vector<std::string> vName, vChannel;
  std::vector<Float_t> vMean, vStddev, vRms;
  std::string bankname = "";
  Float_t hMean = -1, hStddev = -1, hRms = -1;

  std::map<std::string, std::string> bank_to_detector_map = gSetup->fBankToDetectorMap;
  for(std::map<std::string, std::string>::iterator mapIter = bank_to_detector_map.begin();
      mapIter != bank_to_detector_map.end(); mapIter++) {

    bankname = mapIter->first;
//    if(bankname.find("D40") == std::string::npos) continue;
    hMean = pedestal_mean_map[bankname]->GetMean();
    hStddev = pedestal_stddev_map[bankname]->GetMean();
    hRms = pedestal_rms_map[bankname]->GetMean();
    if(bankname.find("_") != std::string::npos) {
      vName.push_back(bankname.substr(0, 7) );
      vChannel.push_back(bankname.substr(8) );
    } else {
      vName.push_back("");
      vChannel.push_back(bankname);
    }
    vMean.push_back(hMean);
    vStddev.push_back(hStddev);
    vRms.push_back(hRms);
  }
  detector->sName = vName;
  detector->sChannel = vChannel;
  detector->fMean = vMean;
  detector->fStddev = vStddev;
  detector->fRms = vRms;
  gTrendTree->FillRunTree("detector", detector);
  gTrendTree->EOR();
  return SUCCESS;
}

INT TPedestal_exit()
{
  return SUCCESS;
}

Float_t * TPedestal_findPeaks(TH1I* height_histogram)
{
  float sigma = 2;
  float threshold = 0.5;
  TSpectrum *spc = new TSpectrum(2); //2 peak candidates
  Int_t nfound = spc->Search(height_histogram, sigma, "", threshold);
  Float_t * xpeaks = spc->GetPositionX();
  std::sort(xpeaks, xpeaks + nfound);
  return xpeaks;
}
/** This method processes one MIDAS block, producing a vector
 * of TOctalFADCIsland objects from the raw Octal FADC data.
 */
INT TPedestal(EVENT_HEADER *pheader, void *pevent) {

  // Some typedefs
  typedef std::map<string, vector<TPulseIsland*> >::iterator map_iterator;

  // Fetch a reference to the gData structure that stores a map
  // of (bank_name, vector<TPulseIsland*>) pairs
  std::map<std::string, std::vector<TPulseIsland*> >& pulses_map = gData->fPulseIslandToChannelMap;
  Float_t * xpeaks;

  // Loop over the map and get each bankname, vector pair
  for (map_iterator theMapIter = pulses_map.begin();
       theMapIter != pulses_map.end(); theMapIter++) {
    std::string bankname = theMapIter->first;
    std::vector<TPulseIsland*> thePulses = theMapIter->second;
//    if(bankname.find("D40") == std::string::npos) continue;
    if(!TSetupData::IsWFD(bankname) ) continue;
    // Loop over the TPulseIslands and plot the histogram
    for (std::vector<TPulseIsland*>::iterator pulseIter = thePulses.begin();
         pulseIter != thePulses.end(); pulseIter++) {
      if (height_histograms_map.find(bankname) != height_histograms_map.end()) {
        height_histograms_map[bankname]->Fill((*pulseIter)->GetPulseHeight());
      }
    }
    xpeaks = TPedestal_findPeaks(height_histograms_map[bankname]);
  }

  for (map_iterator theMapIter = pulses_map.begin();
       theMapIter != pulses_map.end(); theMapIter++) {
    std::string bankname = theMapIter->first;
    std::vector<TPulseIsland*> thePulses = theMapIter->second;

//    if(bankname.find("D40") == std::string::npos) continue;
    // Loop over the TPulseIslands and plot the histogram
    for (std::vector<TPulseIsland*>::iterator pulseIter = thePulses.begin();
         pulseIter != thePulses.end(); pulseIter++) {
      if(TSetupData::IsWFD(bankname) )
      {
        if(sizeof(xpeaks)/sizeof(*xpeaks)>1 && xpeaks[1] > 2000 && (*pulseIter)->GetPulseHeight() > (xpeaks[1]-200) ) continue; //is the sync pulse height, remove sync pulse
      }
      // Make sure the histograms exist and then fill them
      TH2* shape_histogram = shape_histograms_map[bankname];
      TH2* pedestal_values_histogram = pedestal_values_map[bankname];
      TH1* pedestal_mean_histogram = pedestal_mean_map[bankname];
      TH1* pedestal_stddev_histogram = pedestal_stddev_map[bankname];
      TH1* pedestal_rms_histogram = pedestal_rms_map[bankname];
      pedestal_values_histogram->Reset();

      std::vector<int> theSamples = (*pulseIter)->GetSamples();
      for (std::vector<int>::iterator sampleIter = theSamples.begin(); sampleIter != theSamples.end(); sampleIter++) {
        int sample_number = sampleIter - theSamples.begin();
        int sample_value = *sampleIter;
        shape_histogram->Fill(sample_number, sample_value);      
        if(sample_number >= 0 && sample_number < 10) {
          pedestal_values_histogram->Fill(sample_number, sample_value);
        }
      }
      pedestal_mean_histogram->Fill(pedestal_values_histogram->GetMean(2) );
      pedestal_stddev_histogram->Fill(pedestal_values_histogram->GetStdDev(2) );
      double rootMeanSquare = std::sqrt ( pedestal_values_histogram->GetMean(2)*pedestal_values_histogram->GetMean(2) + pedestal_values_histogram->GetStdDev(2)*pedestal_values_histogram->GetStdDev(2) );
      pedestal_rms_histogram->Fill(rootMeanSquare ); 
    }
  }

  return SUCCESS;
}
