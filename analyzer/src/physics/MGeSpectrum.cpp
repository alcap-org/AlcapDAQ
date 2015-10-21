/********************************************************************\

Name:         MGeSpectrum
Created by:   Andrew Edmonds

Contents:     A module that gives the Ge spectrum

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
INT  MGeSpectrum_init(void);
INT  MGeSpectrum(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

static TH1D* hGeSpectrum;
static double adc_slope_calib;
static double adc_offset_calib;
static std::string ge_bankname;

ANA_MODULE MGeSpectrum_module =
{
	"MGeSpectrum",                    /* module name           */
	"Andrew Edmonds",              /* author                */
	MGeSpectrum,                      /* event routine         */
	NULL,                          /* BOR routine           */
	NULL,                          /* EOR routine           */
	MGeSpectrum_init,                 /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};

/** This method initializes histograms.
*/
INT MGeSpectrum_init()
{
  // The dE/dx histogram is created for the left and right arms of the detector:
  // energy in Si1 (x-axis) vs total energy in Si1 + Si2 (y-axis)

  std::string detname = "Ge-S";
  ge_bankname = gSetup->GetBankName(detname);
  adc_slope_calib = gSetup->GetADCSlopeCalib(ge_bankname);
  adc_offset_calib = gSetup->GetADCOffsetCalib(ge_bankname);

  // While looping through the arms create the dE/dx plots
  double min_energy = 0;
  double max_energy = 5000;
  double energy_width = 100;
  int n_energy_bins = (max_energy - min_energy) / energy_width;

  std::string histname = "hGeSpectrum";
  std::string histtitle = "Germanium Spectrum";
  hGeSpectrum = new TH1D(histname.c_str(), histtitle.c_str(), n_energy_bins,min_energy,max_energy);
  hGeSpectrum->GetYaxis()->SetTitle("Counts");
  hGeSpectrum->GetXaxis()->SetTitle("Energy [keV]");

  return SUCCESS;
}

/** This method processes one MIDAS block
 */
INT MGeSpectrum(EVENT_HEADER *pheader, void *pevent)
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

	// Get the pulses for the germanium
	std::vector<TPulseIsland*> ge_pulses = gData->fPulseIslandToChannelMap[bankname];
	for (std::vector<TPulseIsland*>::const_iterator i_pulse = ge_pulses.begin(); i_pulse != ge_pulses.end(); ++i_pulse) {
	  double pulse_energy = adc_slope_calib * (*i_pulse)->GetPulseHeight() + adc_offset_calib; 
	  hGeSpectrum->Fill(pulse_energy);
	}

	return SUCCESS;
}
