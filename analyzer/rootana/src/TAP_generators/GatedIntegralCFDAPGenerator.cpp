#include "TAPGeneratorFactory.h"
#include "GatedIntegralCFDAPGenerator.h"
#include "TPulseIsland.h"
#include "TAnalysedPulse.h"
#include "SetupNavigator.h"
#include "EventNavigator.h"
#include "ExportPulse.h"
#include "ModulesParser.h"
#include "debug_tools.h"

#include <iostream>
#include <cmath>
#include <sstream>
#include <algorithm>

GatedIntegralCFDAPGenerator::GatedIntegralCFDAPGenerator(TAPGeneratorOptions* opts):
  TVAnalysedPulseGenerator("GatedIntegralCFD",opts)
{
  fGateWidth = opts->GetInt("gate_width", 40);
  fConstantFraction = opts->GetDouble("constant_fraction", 0.3);
  fPedCalculation = opts->GetBool("ped_cal", true);
  if (fPedCalculation) {
    fNped = opts->GetInt("nped", 12);
  }
  if (opts->Debug()) {
    std::cout << "Settings for this GatedIntegralCFD: ";
    std::cout << "gate = " << fGateWidth;
    std::cout << ", CF = " << fConstantFraction;
    std::cout << ", recalculate ped = " << fPedCalculation;
    if (fPedCalculation) {
      std::cout << ", nped = " << fNped;
    }
    std::cout << std::endl;
  }
}

GatedIntegralCFDAPGenerator::~GatedIntegralCFDAPGenerator(){;}

int GatedIntegralCFDAPGenerator::ProcessPulses(
    const PulseIslandList& pulseList, AnalysedPulseList& analysedList)
{
  // Loop over all the TPIs given to us
  for (PulseIslandList::const_iterator i_tpi=pulseList.begin();
      i_tpi!=pulseList.end(); ++i_tpi){
    int tpi_ID = i_tpi - pulseList.begin();
    Analyse(tpi_ID, *i_tpi,analysedList);
  }
  return 0;
}

void GatedIntegralCFDAPGenerator::Analyse(int tpi_ID,
    const TPulseIsland* tpi, AnalysedPulseList& analysedList)
{
  if(tpi->GetPulseLength() < 14) return;
  if (fGateWidth >= tpi->GetPulseLength() || fGateWidth < 2) {
    return;
  }

  // Calculate pedestal pulse by pulse, if necessary
  if (fPedCalculation) {
    fPedestal = 0.;
    for (unsigned int isample = 0; isample < fNped; ++isample) {
      fPedestal += tpi->GetSamples().at(isample);
    }
    fPedestal /= fNped;
  }
  else
    fPedestal = SetupNavigator::Instance()->GetPedestal(GetChannel());

  // Prepare a ped subtracted island
  const std::vector<int>& orig_samples = tpi->GetSamples();
  std::vector<double> psub_pulse;
  int pol = tpi->GetTriggerPolarity();

  for (unsigned int isample = 0; isample < orig_samples.size(); ++isample) {
    psub_pulse.push_back((orig_samples.at(isample)-fPedestal)*pol);
  }
  // Gated integrator
  std::vector<double> integrated_pulse;
  double sum = 0.;
  for (int i = 0; i < fGateWidth; ++i) {
    sum += psub_pulse.at(i);
  }
  integrated_pulse.push_back(sum);

  for (unsigned int i = fGateWidth; i < psub_pulse.size(); ++i) {
    integrated_pulse.push_back(integrated_pulse.back() + 
        psub_pulse.at(i) - psub_pulse.at(i-fGateWidth));
  }

  double pulse_height = *(std::max_element(integrated_pulse.begin(),
      integrated_pulse.end()));
  double integral = pulse_height;

  // D-CFD
  std::vector<double>::const_iterator mdv_pos =
    std::max_element(psub_pulse.begin(), psub_pulse.end());
  double threshold = fConstantFraction * (*mdv_pos);
  double cftime = 0;

  for (unsigned int i = 0; i < mdv_pos - psub_pulse.begin(); ++i) {
    if (psub_pulse.at(i) < threshold && psub_pulse.at(i+1) > threshold) {
      cftime = i +
        (threshold - psub_pulse.at(i))/(psub_pulse.at(i+1) - psub_pulse.at(i));
      break;
    }
  }

  // Now that we've found the information we were looking for make a TAP to
  // hold it.  This method makes a TAP and sets the parent TPI info.  It needs
  // the index of the parent TPI in the container as an argument
  TAnalysedPulse* tap = MakeNewTAP(tpi_ID);
  tap->SetAmplitude(pulse_height);
  tap->SetTime(cftime);
  tap->SetIntegral(integral);

  // Finally add the new TAP to the output list
  analysedList.push_back(tap);
} 

void GatedIntegralCFDAPGenerator::DrawPulse(int original, int pulse_timestamp, int n_pulse_samples){
}

ALCAP_TAP_GENERATOR(GatedIntegralCFD, gate_width, constant_fraction, ped_cal, nped);
