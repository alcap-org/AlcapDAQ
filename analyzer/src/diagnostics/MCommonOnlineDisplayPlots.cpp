/********************************************************************\

Name:         MCommonOnlineDisplayPlots
Created by:   Andrew Edmonds

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


/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include <TH1.h>
#include <TH2.h>

/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"
#include "TPulseIsland.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

/*-- Module declaration --------------------------------------------*/
INT MCommonOnlineDisplayPlots_init(void);
INT MCommonOnlineDisplayPlots_init_wfd(const std::string&, const std::string&);
INT MCommonOnlineDisplayPlots_init_tdc(const std::string&, const std::string&);
INT MCommonOnlineDisplayPlots_bor(INT);
INT MCommonOnlineDisplayPlots_bor_wfd(const std::string&, const std::string&);
INT MCommonOnlineDisplayPlots_bor_tdc(const std::string&, const std::string&);
INT MCommonOnlineDisplayPlots(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

map<std::string, TH1I*> height_histograms_map;
map<std::string, TH1I*> time_histograms_map;
map<std::string, TH2D*> shape_histograms_map;
map<std::string, TH1I*> latest_pulse_histograms_map;
map<std::string, TH1F*> tdc_rawtime_histograms_map;
map<std::string, TH1F*> tdc_rawtime_beginofblock_histograms_map;
map<std::string, TH1F*> tdc_rawtime_endofblock_histograms_map;
map<std::string, TH1I*> tdc_adjtime_histograms_map;
static TH1I* hPulseRawCount;


ANA_MODULE MCommonOnlineDisplayPlots_module =
{
	"MCommonOnlineDisplayPlots",    /* module name           */
	"Andrew Edmonds",               /* author                */
	MCommonOnlineDisplayPlots,      /* event routine         */
	MCommonOnlineDisplayPlots_bor,  /* BOR routine           */
	NULL,                           /* EOR routine           */
	MCommonOnlineDisplayPlots_init, /* init routine          */
	NULL,                           /* exit routine          */
	NULL,                           /* parameter structure   */
	0,                              /* structure size        */
	NULL,                           /* initial parameters    */
};

/** This method initializes histograms.
*/
INT MCommonOnlineDisplayPlots_init() {
  // The following histograms are created for each channel:
  // hPulseHeights: ADC value (x-axis) vs number of pulses (y-axis)
  // hPulseTimes: time stamp (x-axis) vs number of pulses (y-axis)
  // hPulseRawCount: number of pulses (y-axis) - channels on x-axis?
  // hPulseShapes: sample number (x-axis) vs ADC value (y-axis) vs pulse (z-axis)

  std::map<std::string, std::string> bank_to_detector_map =
      gSetup->fBankToDetectorMap;
  for(std::map<std::string, std::string>::iterator mapIter = bank_to_detector_map.begin();
      mapIter != bank_to_detector_map.end(); mapIter++) {
    std::string bankname = mapIter->first;
    std::string detname = mapIter->second;
    INT ret = SUCCESS;
    if (TSetupData::IsWFD(bankname))
     ret = MCommonOnlineDisplayPlots_init_wfd(bankname, detname);
    else if (TSetupData::IsTDC(bankname))
      ret = MCommonOnlineDisplayPlots_init_tdc(bankname, detname);
    else
      printf("Online Plots Error: Unknown bank %s\n", bankname.c_str());
  }

  // hPulseRawCount
  std::string histname = "hHitRawCount";
  std::string histtitle = "Plot of the raw counts in each channels";
  hPulseRawCount = new TH1I(histname.c_str(), histtitle.c_str(), 1,0,1);
  hPulseRawCount->GetXaxis()->SetTitle("Channel");
  hPulseRawCount->GetYaxis()->SetTitle("Number of Hits");
  hPulseRawCount->SetBit(TH1::kCanRebin);

  return SUCCESS;
}

INT MCommonOnlineDisplayPlots_init_wfd(const std::string& bank,
                                       const std::string& det) {
  const int n_digitizer_bits = gSetup->GetNBits(bank);
  const int64_t max_adc_value = std::pow(2, n_digitizer_bits);
  const double clock_tick = gSetup->GetClockTick(bank);

  // hPulseHeights
  std::string histname = "h" + bank + "_Heights";
  std::string histtitle = "Plot of the pulse heights in the " + det + " channels";
  TH1I* hPulseHeights = new TH1I(histname.c_str(), histtitle.c_str(),
                                 max_adc_value, 0, max_adc_value);
  hPulseHeights->GetXaxis()->SetTitle("Pulse Height [ADC value]");
  hPulseHeights->GetYaxis()->SetTitle("Number of Pulses");
  height_histograms_map[bank] = hPulseHeights;
  // hPulseTimes
  histname = "h" + bank + "_Times";
  histtitle = "Plot of the pulse times in the " + det + " channels";
  TH1I* hPulseTimes = new TH1I(histname.c_str(), histtitle.c_str(),
                               2000, 0, 2e8);
  hPulseTimes->GetXaxis()->SetTitle("Time");
  hPulseTimes->GetYaxis()->SetTitle("Number of Pulses");
  time_histograms_map[bank] = hPulseTimes;
  // hPulseShapes
  double min_time = 0;
  double max_time = 10000;
  double n_bins = (max_time - min_time) / clock_tick;
  histname = "h" + bank + "_Shapes";
  histtitle = "Plot of the pulse shapes in the " + det + " channels";
  TH2D* hPulseShapes = new TH2D(histname.c_str(), histtitle.c_str(),
                                n_bins,min_time,max_time,
                                (max_adc_value+1)/10, 0, max_adc_value+1);
  hPulseShapes->GetXaxis()->SetTitle("Time [ns]");
  hPulseShapes->GetYaxis()->SetTitle("ADC Value");
  shape_histograms_map[bank] = hPulseShapes;
  //hLatestPulse
  histname = "h" + bank + "_LatestPulse";
  histtitle = "Plot of the latest pulse in the " + det + " channels";
  TH1I* hLatestPulse = new TH1I(histname.c_str(), histtitle.c_str(), n_bins,min_time,max_time);
  hLatestPulse->GetXaxis()->SetTitle("Time [ns]");
  hLatestPulse->GetYaxis()->SetTitle("ADC Value");
  latest_pulse_histograms_map[bank] = hLatestPulse;

  return SUCCESS;
}

INT MCommonOnlineDisplayPlots_init_tdc(const std::string& bank,
                                       const std::string& det) {
  std::string histname = "h" + bank + "_RawTime";
  std::string histtitle = "TDC Raw Times in " + det;
  TH1F* hTDCRawTime = new TH1F(histname.c_str(), histtitle.c_str(),
                               1000, 0, 110.e6);
  hTDCRawTime->SetXTitle("Time [ns]");
  tdc_rawtime_histograms_map[bank] = hTDCRawTime;

  histname = "h" + bank + "_RawTime_BeginOfBlock";
  histtitle = "TDC Raw Times in " + det + "(begin of block)";
  TH1F* hTDCRawTime_BeginOfBlock = new TH1F(histname.c_str(), histtitle.c_str(),
                               1000, 0, 5000);
  hTDCRawTime_BeginOfBlock->SetXTitle("Time [ns]");
  tdc_rawtime_beginofblock_histograms_map[bank] = hTDCRawTime_BeginOfBlock;

  histname = "h" + bank + "_RawTime_EndOfBlock";
  histtitle = "TDC Raw Times in " + det + "(end of block)";
  TH1F* hTDCRawTime_EndOfBlock = new TH1F(histname.c_str(), histtitle.c_str(),
                               1000, 99e6, 102e6);
  hTDCRawTime_EndOfBlock->SetXTitle("Time [ns]");
  tdc_rawtime_endofblock_histograms_map[bank] = hTDCRawTime_EndOfBlock;

  histname = "h" + bank + "_AdjTime";
  histtitle = "TDC Adjusted Times in " + det;
  TH1I* hTDCAdjTime = new TH1I(histname.c_str(), histtitle.c_str(),
                               1000, 0, 110.e6);
  hTDCAdjTime->SetXTitle("Time [ns]");
  tdc_adjtime_histograms_map[bank] = hTDCAdjTime;

  return SUCCESS;
}

// Resets the histograms at the beginning of each run
// so that the online display updates
INT MCommonOnlineDisplayPlots_bor(INT run_number) {

  std::map<std::string, std::string> bank_to_detector_map = gSetup->fBankToDetectorMap;
  for(std::map<std::string, std::string>::iterator mapIter = bank_to_detector_map.begin();
      mapIter != bank_to_detector_map.end(); mapIter++) {

    std::string bankname = mapIter->first;
    if (TSetupData::IsWFD(bankname)) {
      height_histograms_map[bankname]->Reset();
      time_histograms_map[bankname]->Reset();
      shape_histograms_map[bankname]->Reset();
      latest_pulse_histograms_map[bankname]->Reset();
    } else if (TSetupData::IsTDC(bankname)) {
      tdc_rawtime_histograms_map[bankname]->Reset();
      tdc_rawtime_beginofblock_histograms_map[bankname]->Reset();
      tdc_rawtime_endofblock_histograms_map[bankname]->Reset();
      tdc_adjtime_histograms_map[bankname]->Reset();
    }
  }
  hPulseRawCount->Reset();
  return SUCCESS;
}

/** This method processes one MIDAS block, producing a vector
 * of TOctalFADCIsland objects from the raw Octal FADC data.
 */
INT MCommonOnlineDisplayPlots(EVENT_HEADER *pheader, void *pevent) {
  // Some typedefs
  typedef std::map<string, vector<TPulseIsland*> >::iterator map_iterator;

  // Fetch a reference to the gData structure that stores a map
  // of (bank_name, vector<TPulseIsland*>) pairs
  std::map<std::string, std::vector<TPulseIsland*> >& pulses_map =
    gData->fPulseIslandToChannelMap;

  // Loop over the map and get each bankname, vector pair
  for (map_iterator theMapIter = pulses_map.begin();
       theMapIter != pulses_map.end(); theMapIter++) {
    std::string bankname = theMapIter->first;
    std::vector<TPulseIsland*> thePulses = theMapIter->second;

    // Loop over the TPulseIslands and plot the histogram
    for (std::vector<TPulseIsland*>::iterator pulseIter = thePulses.begin();
         pulseIter != thePulses.end(); pulseIter++) {

      // Make sure the histograms exist and then fill them
      if (shape_histograms_map.find(bankname) != shape_histograms_map.end()) {
        TH2* shape_histogram = shape_histograms_map[bankname];
        TH1* latest_pulse_histogram = latest_pulse_histograms_map[bankname];
	double clock_tick = (*pulseIter)->GetClockTickInNs();
	latest_pulse_histogram->Reset();

	std::vector<int> theSamples = (*pulseIter)->GetSamples();
	for (std::vector<int>::iterator sampleIter = theSamples.begin(); sampleIter != theSamples.end(); sampleIter++) {
	  int sample_number = sampleIter - theSamples.begin();
	  int sample_value = *sampleIter;

	  shape_histogram->Fill(sample_number*clock_tick, sample_value);
	  latest_pulse_histogram->SetBinContent(sample_number, sample_value);
	}
      }
      if (height_histograms_map.find(bankname) != height_histograms_map.end()) {
	height_histograms_map[bankname]->Fill((*pulseIter)->GetPulseHeight());
      }
      if (time_histograms_map.find(bankname) != time_histograms_map.end()) {
	time_histograms_map[bankname]->Fill((*pulseIter)->GetPulseTime());
      }
    }

    hPulseRawCount->Fill(bankname.c_str(), thePulses.size());
  }

  // TDC
  const std::map< std::string, std::vector<int64_t> >& tdcs_map =
    gData->fTDCHitsToChannelMap;
  std::map< std::string, std::vector<int64_t> >::const_iterator tdc;
  for (tdc = tdcs_map.begin(); tdc != tdcs_map.end(); ++tdc) {
    const std::vector<int64_t>& hits = tdc->second;
    if (!hits.empty()) {
      static const double clock_tick = 0.025; // ns
      TH1* hist = tdc_rawtime_histograms_map[tdc->first];
      TH1* hist_beginofblock = tdc_rawtime_beginofblock_histograms_map[tdc->first];
      TH1* hist_endofblock = tdc_rawtime_endofblock_histograms_map[tdc->first];
      //      hist->Fill(hits[0]);
      for (int i = 0; i < hits.size(); ++i) {
        hist->Fill(clock_tick*hits[i]);
        hist_beginofblock->Fill(clock_tick*hits[i]);
        hist_endofblock->Fill(clock_tick*hits[i]);
        //hTDC00_Diff->Fill(hits[i]-hits[i-1]);
      }
    }
  }
  return SUCCESS;
}
