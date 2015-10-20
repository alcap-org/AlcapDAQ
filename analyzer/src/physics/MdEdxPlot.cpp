/********************************************************************\

Name:         MdEdxPlot
Created by:   Andrew Edmonds

Contents:     A module that gives the dEdx plots for the left and right silicon detectors

Updated: 2015-10-19 for R2015b where we will have three silicon layers and will actually try to use this online...

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

const int n_arms = 2;
const int n_layers_per_arm = 3;
const int max_sectors_per_layer = 4; // for the first layer only!

static TH2D* hdEdx[n_arms];
static std::string arm_names[n_arms] = {"SiL", "SiR"};
static std::string si_bank_names[n_arms][n_layers_per_arm][max_sectors_per_layer];
static double adc_slope_calib[n_arms][n_layers_per_arm][max_sectors_per_layer];
static double adc_offset_calib[n_arms][n_layers_per_arm][max_sectors_per_layer];


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

  std::stringstream detname;
  for (int i_arm = 0; i_arm < n_arms; ++i_arm) {
    for (int i_layer = 0; i_layer < n_layers_per_arm; ++i_layer) {
      for (int i_quadrant = 0; i_quadrant < max_sectors_per_layer; ++i_quadrant) {
	detname << arm_names[i_arm] << i_layer+1;
	if (i_layer == 0) {
	  detname << "-" << i_quadrant+1;
	}
	detname << "-S";
	if (i_layer > 0 && i_quadrant > 0) {
	  detname.str(""); // don't want multiple copies of the thicker layers
	  detname << "N/A";
	}
	std::string bankname = gSetup->GetBankName(detname.str());
	si_bank_names[i_arm][i_layer][i_quadrant] = bankname;
	adc_slope_calib[i_arm][i_layer][i_quadrant] = gSetup->GetADCSlopeCalib(bankname);
	adc_offset_calib[i_arm][i_layer][i_quadrant] = gSetup->GetADCOffsetCalib(bankname);
	//	printf("AE: %s --> %s\n", detname.str().c_str(), (si_bank_names[i_arm][i_layer][i_quadrant]).c_str());
	detname.str("");
      }
    }

    // While looping through the arms create the dE/dx plots
    double min_edep = 0;
    double max_total_edep = 15000;
    double max_thin_edep = 5000;
    double edep_width = 100;
    int n_total_edep_bins = (max_total_edep - min_edep) / edep_width;
    int n_thin_edep_bins = (max_thin_edep - min_edep) / edep_width;

    //    int max_adc_value = 4096; // probably want to check which channel we're in before setting this
    //    max_adc_value = 7500;

    std::string histname = "hdEdx_" + arm_names[i_arm];
    std::string histtitle = "dE/dx plot for " + arm_names[i_arm];
    hdEdx[i_arm] = new TH2D(histname.c_str(), histtitle.c_str(), n_total_edep_bins,min_edep,max_total_edep, n_thin_edep_bins,min_edep,max_thin_edep);
    hdEdx[i_arm]->GetYaxis()->SetTitle("EDep in Si1");
    hdEdx[i_arm]->GetXaxis()->SetTitle("Total EDep in Si1, Si2 and Si3");
  }

  return SUCCESS;
}

/** This method processes one MIDAS block
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

	// Loop through the arms
	for (int i_arm = 0; i_arm < n_arms; ++i_arm) {
	  int pulse_counters[n_layers_per_arm][max_sectors_per_layer]; // to keep track of where we are in each vector
	  int vector_sizes[n_layers_per_arm][max_sectors_per_layer]; // record this for the future
	  
	  // Get the pulses for each detector (we'll have to go through each at the same time to get coincidences)
	  std::vector<TPulseIsland*> si_pulses[n_layers_per_arm][max_sectors_per_layer];
	  // Loop through the layers
	  for (int i_layer = 0; i_layer < n_layers_per_arm; ++i_layer) {
	    
	    // Loop through the quadrants
	    for (int i_quadrant = 0; i_quadrant < max_sectors_per_layer; ++i_quadrant) {
	      pulse_counters[i_layer][i_quadrant] = 0;

	      std::string bankname = si_bank_names[i_arm][i_layer][i_quadrant];
	      if (bankname == "") {
		vector_sizes[i_layer][i_quadrant] = 0;
		continue; // from looping through quadrants
	      }

	      // Get the TPulseIslands for this bank
	      si_pulses[i_layer][i_quadrant] = gData->fPulseIslandToChannelMap[bankname];
	      vector_sizes[i_layer][i_quadrant] = (si_pulses[i_layer][i_quadrant]).size();
	      //	      printf("AE: %s: %d pulses\n", bankname.c_str(), (int) (si_pulses[i_layer][i_quadrant]).size());
	      //skip bank if it doesen't exist
	      if (si_pulses[i_layer][i_quadrant].size() == 0) {
		continue;
	      }
	    }
	  }

	  // Now loop through each detector at the same time and start adding energy deposits and fill the histogram
	  bool done = false;
	  while (!done) {
	    double pulse_edeps[n_layers_per_arm];
	    double current_time = 0;

	    for (int i_layer = 0; i_layer < n_layers_per_arm; ++i_layer) {
	      pulse_edeps[i_layer] = 0; // start at 0
	      for (int i_quadrant = 0; i_quadrant < max_sectors_per_layer; ++i_quadrant) {
		int current_index = pulse_counters[i_layer][i_quadrant];
		//		printf("Layer #%d, Quadrant #%d, Index %d, size = %d\n", i_layer, i_quadrant, current_index, vector_sizes[i_layer][i_quadrant]);
		if (current_index >= vector_sizes[i_layer][i_quadrant]) {
		  continue;
		}

		// Want a time check
		if (current_time < 0.1) { // if we don't have a time yet
		  current_time = (si_pulses[i_layer][i_quadrant].at(current_index))->GetPulseTime();
		  pulse_edeps[i_layer] = 
		    (adc_slope_calib[i_arm][i_layer][i_quadrant]) * ((si_pulses[i_layer][i_quadrant].at(current_index))->GetPulseHeight())
		         + adc_offset_calib[i_arm][i_layer][i_quadrant]; 
		  //		  printf("Layer #%d, Q%d: time = %f, height = %f\n", i_layer, i_quadrant, current_time, pulse_edeps[i_layer]);
		  ++(pulse_counters[i_layer][i_quadrant]); // may miss things though...
		}
		else {
		  double time_difference = 10000; // 
		  double pulse_time = (si_pulses[i_layer][i_quadrant].at(current_index))->GetPulseTime();
		  //		  printf("Pulse time = %f\n", pulse_time);
		  if (std::fabs(current_time - pulse_time) < time_difference) {
		    pulse_edeps[i_layer] += 
		      (adc_slope_calib[i_arm][i_layer][i_quadrant]) * ((si_pulses[i_layer][i_quadrant].at(current_index))->GetPulseHeight())
		           + adc_offset_calib[i_arm][i_layer][i_quadrant]; 
		    //		    printf("Match! Layer #%d, Q%d: time = %f, total height = %f\n", i_layer, i_quadrant, pulse_time, pulse_edeps[i_layer]);
		    ++(pulse_counters[i_layer][i_quadrant]); // used this one
		  }
		}
	      }

	      // Now fill in the plots
	      double all_layer_pulse_edep = 0;
	      for (int i_layer = 0; i_layer < n_layers_per_arm; ++i_layer) {
		all_layer_pulse_edep += pulse_edeps[i_layer];
	      }
	      hdEdx[i_arm]->Fill(all_layer_pulse_edep, pulse_edeps[0]);
	    }
	    
	    // Check if any of the vectors are done
	    for (int i_layer = 0; i_layer < n_layers_per_arm; ++i_layer) {
	      for (int i_quadrant = 0; i_quadrant < max_sectors_per_layer; ++i_quadrant) {
		done = true;
		if (pulse_counters[i_layer][i_quadrant] < vector_sizes[i_layer][i_quadrant]) {
		  done = false;
		  break; // from quadrant loop
		}
	      }
	      if (done == false) { // if we already know we aren't finished
		break; // from the layer loop
	      }
	    }
	  }
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
