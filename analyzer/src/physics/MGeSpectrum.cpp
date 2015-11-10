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

static TH1D* hGeSpectrum_HiGain;
static TH1D* hGeSpectrum_LoGain;
static double adc_slope_calib_higain;
static double adc_offset_calib_higain;
static std::string ge_bankname_higain;
static double adc_slope_calib_logain;
static double adc_offset_calib_logain;
static std::string ge_bankname_logain;

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

  std::string detname = "GeHiGain";
  ge_bankname_higain = gSetup->GetBankName(detname);
  adc_slope_calib_higain = gSetup->GetADCSlopeCalib(ge_bankname_higain);
  adc_offset_calib_higain = gSetup->GetADCOffsetCalib(ge_bankname_higain);

  detname = "GeLoGain";
  ge_bankname_logain = gSetup->GetBankName(detname);
  adc_slope_calib_logain = gSetup->GetADCSlopeCalib(ge_bankname_logain);
  adc_offset_calib_logain = gSetup->GetADCOffsetCalib(ge_bankname_logain);

  // While looping through the arms create the dE/dx plots
  double min_energy = 0;
  double max_energy = 5000;
  double energy_width = 1;
  int n_energy_bins = (max_energy - min_energy) / energy_width;

  std::string histname = "hGeSpectrum_HiGain";
  std::string histtitle = "Germanium Spectrum (High Gain)";
  hGeSpectrum_HiGain = new TH1D(histname.c_str(), histtitle.c_str(), n_energy_bins,min_energy,max_energy);
  hGeSpectrum_HiGain->GetYaxis()->SetTitle("Counts");
  hGeSpectrum_HiGain->GetXaxis()->SetTitle("Energy [keV]");

  histname = "hGeSpectrum_LoGain";
  histtitle = "Germanium Spectrum (Low Gain)";
  hGeSpectrum_LoGain = new TH1D(histname.c_str(), histtitle.c_str(), n_energy_bins,min_energy,max_energy);
  hGeSpectrum_LoGain->GetYaxis()->SetTitle("Counts");
  hGeSpectrum_LoGain->GetXaxis()->SetTitle("Energy [keV]");

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
	std::vector<TPulseIsland*>& ge_pulses_higain = gData->fPulseIslandToChannelMap[ge_bankname_higain];
	for (std::vector<TPulseIsland*>::const_iterator i_pulse = ge_pulses_higain.begin(); i_pulse != ge_pulses_higain.end(); ++i_pulse) {
	  double pulse_energy = (adc_slope_calib_higain * (*i_pulse)->GetPulseHeight()) + adc_offset_calib_higain; 
	  //	  double pulse_energy = (*i_pulse)->GetPulseHeight();
	  hGeSpectrum_HiGain->Fill(pulse_energy);
	}

	std::vector<TPulseIsland*>& ge_pulses_logain = gData->fPulseIslandToChannelMap[ge_bankname_logain];
	for (std::vector<TPulseIsland*>::const_iterator i_pulse = ge_pulses_logain.begin(); i_pulse != ge_pulses_logain.end(); ++i_pulse) {
	  double pulse_energy = (adc_slope_calib_logain * (*i_pulse)->GetPulseHeight()) + adc_offset_calib_logain; 
	  //	  double pulse_energy = (*i_pulse)->GetPulseHeight();
	  hGeSpectrum_LoGain->Fill(pulse_energy);
	}

	return SUCCESS;
}
