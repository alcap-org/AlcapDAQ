/********************************************************************\

Name:         MCombinePulses
Created by:   Andrew Edmonds

Contents:     One module that fills out histograms for the pulse heights, pulse shapes and the raw counts for all digitizer channels. These are all in one module to be more efficient in terms of minimising the number of times we loop through the channels.

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

/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"
#include "TPulseIsland.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

/*-- Module declaration --------------------------------------------*/
INT  MCombinePulses_init(void);
INT  MCombinePulses_bor(INT);
INT  MCombinePulses(EVENT_HEADER*, void*);

std::vector<std::vector<int> > SumPulses(std::vector<std::vector<TPulseIsland*> > timeHeightMapsVector, double time_difference);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

static std::vector<std::string> left_thin_banknames;
static std::vector<std::string> right_thin_banknames;
static TH2D* hSiL1PulseShapes;
static TH2D* hSiR1PulseShapes;
static TH1I* hSiL1PulseHeights;
static TH1I* hSiR1PulseHeights;

ANA_MODULE MCombinePulses_module =
{
	"MCombinePulses",                    /* module name           */
	"Andrew Edmonds",              /* author                */
	MCombinePulses,                      /* event routine         */
	MCombinePulses_bor,                          /* BOR routine           */
	NULL,                          /* EOR routine           */
	MCombinePulses_init,                 /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};

/** This method initializes histograms.
*/
INT MCombinePulses_init()
{
  // The following histograms are created for each channel:
  // hPulseHeights: ADC value (x-axis) vs number of pulses (y-axis)
  // hPulseTimes: time stamp (x-axis) vs number of pulses (y-axis)
  // hPulseRawCount: number of pulses (y-axis) - channels on x-axis?
  // hPulseShapes: sample number (x-axis) vs ADC value (y-axis) vs pulse (z-axis)

  std::map<std::string, std::string> bank_to_detector_map = gSetup->fBankToDetectorMap;
  for(std::map<std::string, std::string>::iterator mapIter = bank_to_detector_map.begin(); 
      mapIter != bank_to_detector_map.end(); mapIter++) { 

    std::string bankname = mapIter->first;
    std::string detname = gSetup->GetDetectorName(bankname);

    if (detname.substr(0,4) == "SiL1" && detname.substr(7,4) == "fast")
      left_thin_banknames.push_back(bankname);
    else if (detname.substr(0,4) == "SiR1" && detname.substr(7,4) == "fast")
      right_thin_banknames.push_back(bankname);
  }

  long max_adc_value = 13000;

  // hSiL1PulseShapes
  std::string histname = "hSiL1PulseShapes";
  std::string histtitle = "Plot of the combined pulse shapes in SiL1";
  hSiL1PulseShapes = new TH2D(histname.c_str(), histtitle.c_str(), 64,-0.5,63.5,max_adc_value+1,0,max_adc_value+1);      
  hSiL1PulseShapes->GetXaxis()->SetTitle("Time Stamp");
  hSiL1PulseShapes->GetYaxis()->SetTitle("ADC Value");

  // hSiR1PulseShapes
  histname = "hSiR1PulseShapes";
  histtitle = "Plot of the combined pulse shapes in SiR1";
  hSiR1PulseShapes = new TH2D(histname.c_str(), histtitle.c_str(), 64,-0.5,63.5,max_adc_value+1,0,max_adc_value+1);      
  hSiR1PulseShapes->GetXaxis()->SetTitle("Time Stamp");
  hSiR1PulseShapes->GetYaxis()->SetTitle("ADC Value");

  // hSiL1PulseHeights
  histname = "hSiL1PulseHeights";
  histtitle = "Plot of the combined pulse heights in SiL1";
  hSiL1PulseHeights = new TH1I(histname.c_str(), histtitle.c_str(), max_adc_value+1,0,max_adc_value+1);      
  hSiL1PulseHeights->GetXaxis()->SetTitle("ADC Value");
  hSiL1PulseHeights->GetYaxis()->SetTitle("Number of Pulses");

  // hSiR1PulseHeights
  histname = "hSiR1PulseHeights";
  histtitle = "Plot of the combined pulse heights in SiR1";
  hSiR1PulseHeights = new TH1I(histname.c_str(), histtitle.c_str(), max_adc_value+1,0,max_adc_value+1);      
  hSiR1PulseHeights->GetXaxis()->SetTitle("ADC Value");
  hSiR1PulseHeights->GetYaxis()->SetTitle("Number of Pulses");

  return SUCCESS;
}

// Resets the histograms at the beginning of each run so that the online display updates
INT MCombinePulses_bor(INT run_number) {

  hSiL1PulseShapes->Reset();  
  hSiR1PulseShapes->Reset();
  hSiL1PulseHeights->Reset();  
  hSiR1PulseHeights->Reset();
}

/** This method processes one MIDAS block, producing a vector
 * of TOctalFADCIsland objects from the raw Octal FADC data.
 */
INT MCombinePulses(EVENT_HEADER *pheader, void *pevent)
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

	////////////////////////////
	// SiL
	// Just loop over the banks we know are connected to these detectors
	std::vector<std::vector<TPulseIsland*> > thinSiLeftPulses;
	for (std::vector<std::string>::iterator bankNameIter = left_thin_banknames.begin();
	     bankNameIter != left_thin_banknames.end(); bankNameIter++) {

	  std::vector<TPulseIsland*> temp_vector = gData->fPulseIslandToChannelMap[*bankNameIter];
	  //skip bank if it doesen't exist
	  if (temp_vector.size() == 0) continue;
	    
	  thinSiLeftPulses.push_back(temp_vector);
	}
	std::vector<std::vector<int> > allPulses = SumPulses(thinSiLeftPulses, 1000);

	for (std::vector<std::vector<int> >::iterator allPulseIter = allPulses.begin(); allPulseIter != allPulses.end(); ++allPulseIter) {
	  
	  // While filling the persistent scope histogram, look for the minimum sample value for the pulse height
	  int min_sample_value = 99999;
	  for (std::vector<int>::iterator sampleIter = (*allPulseIter).begin(); sampleIter != (*allPulseIter).end(); ++sampleIter) {

	    //	    printf("Filling %d at position %d\n", *sampleIter, sampleIter - (*allPulseIter).begin());
	    if (*sampleIter < min_sample_value)
	      min_sample_value = *sampleIter;

	    hSiL1PulseShapes->Fill(sampleIter - (*allPulseIter).begin(), *sampleIter);
	  }

	  // Take into account the number of channels seen otherwise we get a 4 peak plot
	  if (min_sample_value < 6000)
	    min_sample_value /= 2;
	  else if (min_sample_value < 9000)
	    min_sample_value /= 3;
	  else if (min_sample_value < 12000)
	    min_sample_value /= 4;

	  hSiL1PulseHeights->Fill( -1*(min_sample_value - ((thinSiLeftPulses[0])[0])->GetPedestal(10)) );
	}
	///////////////////////////////////////



	////////////////////////////
	// SiR
	// Just loop over the banks we know are connected to these detectors
	std::vector<std::vector<TPulseIsland*> > thinSiRightPulses;
	for (std::vector<std::string>::iterator bankNameIter = right_thin_banknames.begin();
	     bankNameIter != right_thin_banknames.end(); bankNameIter++) {

	  std::vector<TPulseIsland*> temp_vector = gData->fPulseIslandToChannelMap[*bankNameIter];
	  //skip bank if it doesen't exist
	  if (temp_vector.size() == 0) continue;
	    
	  thinSiRightPulses.push_back(temp_vector);
	}
	allPulses = SumPulses(thinSiRightPulses, 1000);

	for (std::vector<std::vector<int> >::iterator allPulseIter = allPulses.begin(); allPulseIter != allPulses.end(); ++allPulseIter) {
	  
	  // While filling the persistent scope histogram, look for the minimum sample value for the pulse height
	  int min_sample_value = 99999;
	  for (std::vector<int>::iterator sampleIter = (*allPulseIter).begin(); sampleIter != (*allPulseIter).end(); ++sampleIter) {

	    //	    printf("Filling %d at position %d\n", *sampleIter, sampleIter - (*allPulseIter).begin());
	    if (*sampleIter < min_sample_value)
	      min_sample_value = *sampleIter;

	    hSiR1PulseShapes->Fill(sampleIter - (*allPulseIter).begin(), *sampleIter);
	  }

	  // Take into account the number of channels seen otherwise we get a 4 peak plot
	  if (min_sample_value < 6000)
	    min_sample_value /= 2;
	  else if (min_sample_value < 9000)
	    min_sample_value /= 3;
	  else if (min_sample_value < 12000)
	    min_sample_value /= 4;

	  hSiR1PulseHeights->Fill( -1*(min_sample_value - ((thinSiRightPulses[0])[0])->GetPedestal(10)) );
	}
	///////////////////////////////////////
	return SUCCESS;
}

// SumPulses()
// -- Creates a vector of sample vectors from a vector of TPulseIsland vectors
// -- This function also checks that the pulses are within a certain time difference of each other
std::vector<std::vector<int> > SumPulses(std::vector<std::vector<TPulseIsland*> > pulses, double time_difference) {

  // Create the iterators
  std::vector<std::vector<TPulseIsland*>::iterator > pulseIters;
  std::vector<std::vector<TPulseIsland*>::iterator > backwardIters;

  for (int b = 0; b < pulses.size(); b++) {
    pulseIters.push_back((pulses.at(b)).begin()); 
    backwardIters.push_back((pulses.at(b)).end());
  }

  // Prepare the output map:
  std::vector<std::vector<int> > output;

  while ( pulseIters.size() > 0 ) {

    //Iterators/pointers into pulseIters and backwardIters must not be held past
    //one loop cycle as they may become invalidated.

    //Get the time of the next (unused) TPulseIslands (one from each bank)
    //work out which is the earliest
    double min_time = 99999999999.;  //something huge.
    for (int b = 0; b < pulseIters.size(); ++b){
      double time = (*(pulseIters.at(b)))->GetPulseTime();
      min_time = std::min(min_time , time);
      //      printf("Bank #%d: time = %f ms\n", b, time * 1e-6);
      //Or use GetTimeStamp()?
    }
    //    printf("Min time = %f ms\n", min_time*1e-6);

    //Sum all the pulseHeights at a given time 
    std::vector<int> temp_output;
    for (int b = 0; b < pulseIters.size(); ++b){
      TPulseIsland* pulse = *(pulseIters.at(b));
      //Need to define a comparison for CONSISTENT_TIME
      if ( std::abs(pulse->GetPulseTime() - min_time) < time_difference ) {
	//	printf("New Pulse\n");
	// Now loop through the samples
	std::vector<int> theSamples = pulse->GetSamples();

	// if we haven't set the size of temp_output yet, then reserve the number of elements we need
	if (temp_output.size() == 0) {
	  for (int i = 0; i < theSamples.size(); ++i) {
	    temp_output.push_back(0); 
	  }
	}
	// if temp_output is larger than the given samples vector, reduce the size
	else if (temp_output.size() >= theSamples.size())
	  temp_output.erase(temp_output.end() - (temp_output.size() - theSamples.size()), temp_output.end());

	for (std::vector<int>::iterator sampleIter = theSamples.begin(); sampleIter != theSamples.end(); ++sampleIter) {
	  //printf("Value in pulse: %d\n", *sampleIter);
	  //	  printf("Value in temp_output: %d\n", temp_output.at(sampleIter - theSamples.begin()));
	  if ( (sampleIter - theSamples.begin()) >= temp_output.size() )
	    break;

	  temp_output.at(sampleIter - theSamples.begin()) += *sampleIter;
	}

	//and increment the iterator because we used the island
	++(pulseIters.at(b));
      }
    } // for (int b)
    output.push_back(temp_output);

    //Delete the iterators to finished banks. Go through in reverse to
    //avoid invalidation problems
    for (int b = pulseIters.size()-1; b >= 0 ; --b){
      if (pulseIters.at(b) == backwardIters.at(b)){
	pulseIters.erase(pulseIters.begin() + b);
	backwardIters.erase(backwardIters.begin() + b);
      }  
    } // for (int b -reversed)
  } // while (size > 0)

  return output;
}
