/********************************************************************\

Name:         MdEdxPlot
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

typedef std::vector<std::vector<TPulseIsland*>::iterator> vecOfIterToPulses;

/*-- Module declaration --------------------------------------------*/
INT  MdEdxPlot_init(void);
INT  MdEdxPlot(EVENT_HEADER*, void*);
std::map<double, double> MakePulseTimeHeightMap(std::vector<TPulseIsland*> thePulses);
std::map<double, double> MakePulseHeightSums(std::vector<std::map<double, double> > timeHeightMapsVector, double time_difference);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

static TH2D* hdEdx_left;
static TH2D* hdEdx_right;

std::vector<std::string> left_thin_banknames;
std::vector<std::string> left_thick_banknames;
std::vector<std::string> right_thin_banknames;
std::vector<std::string> right_thick_banknames;

ANA_MODULE MdEdxPlot_module =
{
	"MdEdxPlot",                    /* module name           */
	"Andrew Edmonds",              /* author                */
	MdEdxPlot,                      /* event routine         */
	NULL,                          /* BOR routine           */
	NULL,                          /* EOR routine           */
	MdEdxPlot_init,                 /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};

/** This method initializes histograms.
*/
INT MdEdxPlot_init()
{
  // The dE/dx histogram is created for the left and right arms of the detector:
  // energy in Si1 (x-axis) vs total energy in Si1 + Si2 (y-axis)

  std::map<std::string, std::string> bank_to_detector_map = gSetup->fBankToDetectorMap;
  for(std::map<std::string, std::string>::iterator mapIter = bank_to_detector_map.begin(); 
      mapIter != bank_to_detector_map.end(); mapIter++) { 

    std::string bankname = mapIter->first;
    std::string detname = gSetup->GetDetectorName(bankname);

    if (detname.substr(0,4) == "SiL1" && detname.substr(7,4) == "slow")
      left_thin_banknames.push_back(bankname);
    else if (detname.substr(0,4) == "SiL2" && detname.substr(5,4) == "slow")
      left_thick_banknames.push_back(bankname);
    else if (detname.substr(0,4) == "SiR1" && detname.substr(7,4) == "slow")
      right_thin_banknames.push_back(bankname);
    else if (detname.substr(0,4) == "SiR2" && detname.substr(5,4) == "slow")
      right_thick_banknames.push_back(bankname);
  }

  int max_adc_value = 4096; // probably want to check which channel we're in before setting this
  max_adc_value = 7500;
  // hdEdx
  std::string histname = "hdEdx_left";
  std::string histtitle = "dE/dx plot for the left arm";
  hdEdx_left = new TH2D(histname.c_str(), histtitle.c_str(), max_adc_value,0,max_adc_value, max_adc_value,0,max_adc_value);
  hdEdx_left->GetXaxis()->SetTitle("Pulse Height in Si1");
  hdEdx_left->GetYaxis()->SetTitle("Total Pulse Height in Si1 and Si2");

  histname = "hdEdx_right";
  histtitle = "dE/dx plot for the right arm";
  hdEdx_right = new TH2D(histname.c_str(), histtitle.c_str(), max_adc_value,0,max_adc_value, max_adc_value,0,max_adc_value);
  hdEdx_right->GetXaxis()->SetTitle("Pulse Height in Si1");
  hdEdx_right->GetYaxis()->SetTitle("Total Pulse Height in Si1 and Si2");

  return SUCCESS;
}

/** This method processes one MIDAS block, producing a vector
 * of TOctalFADCIsland objects from the raw Octal FADC data.
 */
INT MdEdxPlot(EVENT_HEADER *pheader, void *pevent)
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
	std::vector<std::map<double, double> > thinSiLeft_TimeHeightMaps;
	for (std::vector<std::string>::iterator bankNameIter = left_thin_banknames.begin();
	     bankNameIter != left_thin_banknames.end(); bankNameIter++) {
	  
	  std::vector<TPulseIsland*> temp_vector = gData->fPulseIslandToChannelMap[*bankNameIter];
	  //skip bank if it doesen't exist
	  if (temp_vector.size() == 0) continue;
	  
	  thinSiLeft_TimeHeightMaps.push_back(MakePulseTimeHeightMap(temp_vector));
	}
	std::map<double, double> thinLeftSum;
	if (thinSiLeft_TimeHeightMaps.size() != 0)
	  thinLeftSum = MakePulseHeightSums(thinSiLeft_TimeHeightMaps, 1000);

	std::vector<std::map<double, double> > totalLeft_TimeHeightMaps;
	for (std::vector<std::string>::iterator leftThickIter = left_thick_banknames.begin();
	     leftThickIter != left_thick_banknames.end(); leftThickIter++) {

	  std::vector<TPulseIsland*> temp_vector = gData->fPulseIslandToChannelMap[*leftThickIter];
	  //skip bank if it doesen't exist
	  if (temp_vector.size() == 0) continue;
	  
	  totalLeft_TimeHeightMaps.push_back(MakePulseTimeHeightMap(temp_vector));
	}
	if (thinLeftSum.size() != 0)
	  totalLeft_TimeHeightMaps.push_back(thinLeftSum);

	std::map<double, double> totalLeftSum;
	if (totalLeft_TimeHeightMaps.size() != 0)
	  totalLeftSum = MakePulseHeightSums(totalLeft_TimeHeightMaps, 1000);

	for (std::map<double, double>::iterator thinHit = thinLeftSum.begin(), totalHit = totalLeftSum.begin(); 
	     thinHit != thinLeftSum.end() || totalHit != totalLeftSum.end(); thinHit++, totalHit++) {

	  hdEdx_left->Fill((*thinHit).second, (*totalHit).second);
	}	
	//
	////////////////////////////


	////////////////////////////
	// SiR
	// Just loop over the banks we know are connected to these detectors
	std::vector<std::map<double, double> > thinSiRight_TimeHeightMaps;
	for (std::vector<std::string>::iterator bankNameIter = left_thin_banknames.begin();
	     bankNameIter != left_thin_banknames.end(); bankNameIter++) {
	  
	  std::vector<TPulseIsland*> temp_vector = gData->fPulseIslandToChannelMap[*bankNameIter];
	  //skip bank if it doesen't exist
	  if (temp_vector.size() == 0) continue;
	  
	  thinSiRight_TimeHeightMaps.push_back(MakePulseTimeHeightMap(temp_vector));
	}
	std::map<double, double> thinRightSum;
	if (thinSiRight_TimeHeightMaps.size() != 0)
	  thinRightSum = MakePulseHeightSums(thinSiRight_TimeHeightMaps, 1000);

	std::vector<std::map<double, double> > totalRight_TimeHeightMaps;
	for (std::vector<std::string>::iterator leftThickIter = left_thick_banknames.begin();
	     leftThickIter != left_thick_banknames.end(); leftThickIter++) {

	  std::vector<TPulseIsland*> temp_vector = gData->fPulseIslandToChannelMap[*leftThickIter];
	  //skip bank if it doesen't exist
	  if (temp_vector.size() == 0) continue;
	  
	  totalRight_TimeHeightMaps.push_back(MakePulseTimeHeightMap(temp_vector));
	}
	if (thinRightSum.size() != 0)
	  totalRight_TimeHeightMaps.push_back(thinRightSum);

	std::map<double, double> totalRightSum;
	if (totalRight_TimeHeightMaps.size() != 0)
	  totalRightSum = MakePulseHeightSums(totalRight_TimeHeightMaps, 1000);

	for (std::map<double, double>::iterator thinHit = thinRightSum.begin(), totalHit = totalRightSum.begin(); 
	     thinHit != thinRightSum.end() || totalHit != totalRightSum.end(); thinHit++, totalHit++) {

	  hdEdx_left->Fill((*thinHit).second, (*totalHit).second);
	}	

	return SUCCESS;
}

// MakePulseTimeHeightMap()
// -- Takes a vector of TPulseIsland* and creates a map of pulse time and pulse height
// -- This is used to add pulses for the thin Si (and also for any detectors?)
std::map<double, double> MakePulseTimeHeightMap(std::vector<TPulseIsland*> thePulses) {

  std::map<double, double> output;

  for (std::vector<TPulseIsland*>::iterator pulseIter = thePulses.begin(); 
       pulseIter != thePulses.end(); pulseIter++) {

    output[(*pulseIter)->GetPulseTime()] = (*pulseIter)->GetPulseHeight();
  }

  return output;  
}

// MakePulseHeightSums()
// -- Creates a map of (time, total height) from the a vector of (time, height) maps
// -- This function also checks that the pulses are within a certain time difference of each other
std::map<double, double> MakePulseHeightSums(std::vector<std::map<double, double> > timeHeightMapsVector, double time_difference) {

  // Create the iterators
  std::vector<std::map<double, double>::iterator > pulseIters;
  std::vector<std::map<double, double>::iterator > backwardIters;

  for (int b = 0; b < timeHeightMapsVector.size(); b++) {
    pulseIters.push_back((timeHeightMapsVector.at(b)).begin()); 
    backwardIters.push_back((timeHeightMapsVector.at(b)).end());
  }

  // Prepare the output map:
  std::map<double, double> output;

  while ( pulseIters.size() > 0 ) {

    //Iterators/pointers into pulseIters and backwardIters must not be held past
    //one loop cycle as they may become invalidated.

    //Get the time of the next (unused) TPulseIslands (one from each bank)
    //work out which is the earliest
    double min_time = 99999999999.;  //something huge.
    for (int b = 0; b < pulseIters.size(); ++b){
      double time = (pulseIters.at(b))->first;
      min_time = std::min(min_time , time);
      //      printf("Bank #%d: time = %f ms\n", b, time * 1e-6);
      //Or use GetTimeStamp()?
    }
    //    printf("Min time = %f ms\n", min_time*1e-6);

    //Sum all the pulseHeights at a given time 
    double pulseHeight = 0;
    double pulseTime = 0;
    int nSummed = 0;
    for (int b = 0; b < pulseIters.size(); ++b){
      std::pair<double, double> timeHeightPair = *(pulseIters.at(b));
      //Need to define a comparison for CONSISTENT_TIME
      if ( std::abs(timeHeightPair.first - min_time) < time_difference ) {
	pulseHeight += timeHeightPair.second;
	pulseTime += timeHeightPair.first;
	++nSummed;
	//and increment the iterator because we used the island
	++(pulseIters.at(b));
	//	printf("Bank #%d: height = %f, time = %f ms\n", b, timeHeightPair.second, timeHeightPair.first * 1e-6);
      }
    } // for (int b)

    //append to output
    //    printf("Summed time = %f ms, Summed height = %f\n", (pulseTime/nSummed)*1e-6, pulseHeight);
    output[pulseTime/nSummed] = pulseHeight;


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
