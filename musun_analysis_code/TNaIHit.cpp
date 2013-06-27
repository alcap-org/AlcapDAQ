#include "TNaIHit.h"
#include "TOctalFADCIsland.h"
#include "Parameters.h"

extern OCTALFADC_PARAM octalfadc_parameters;

ClassImp(TNaIHit)

TNaIHit::TNaIHit() :  fTimingTime(0.), fEnergyTime(0.), fTimingAmp(0.), fEnergyAmp(0.), fTimePulse(NULL), fEnergyPulse(NULL), nAverage(1), fCount(-1), fPileUp(false) 
{}

TNaIHit::TNaIHit(const TOctalFADCIsland& timePulse, const TOctalFADCIsland& energyPulse, int count, int nAv)
{
  fCount=count;
  nAverage = nAv;
  fPileUp = false;
  fNoisy = false;
  SetFromTimingPulse(timePulse);
  SetFromEnergyPulse(energyPulse);
}

TNaIHit::TNaIHit(const TOctalFADCIsland& timePulse, const TOctalFADCIsland& energyPulse, int count)
{
  fCount=count;
  nAverage = 1;
  fPileUp = false;
  fNoisy = false;
  SetFromTimingPulse(timePulse);
  SetFromEnergyPulse(energyPulse);
  SetDetector(1+(int)(timePulse.GetFADCChannel()/2));
}

/** Sets the time and energy parameters of TNaIHit for the timing data.
  * Uses the CF time of the pulse for the time and the maximum value
  * as the amplitude.
 */
void TNaIHit::SetFromTimingPulse(const TOctalFADCIsland& timePulse)
{
  double timingClockPeriod = octalfadc_parameters.kClockPeriod[timePulse.GetFADCChannel()];
  double cftime = (timePulse.GetCFBlockTime()) * timingClockPeriod;

  SetTimingTime( cftime );
  if(nAverage> 1) { SetTimingAmp( timePulse.GetAverageMax(nAverage)); } else {SetTimingAmp( timePulse.GetMax());}
  // SetTimingAmp( timePulse.GetMax());

  SetTimingIsland( &timePulse );
  if(timePulse.GetPulseQuality()==3){SetNoisy(true);}
}
 
/** Sets the time and energy parameters of TNaIHit for the energy data.
  * Uses the maximum bin of the pulse for the time and the maximum value
  * as the amplitude.
 */
void TNaIHit::SetFromEnergyPulse(const TOctalFADCIsland& energyPulse)
{
  double energyClockPeriod = octalfadc_parameters.kClockPeriod[energyPulse.GetFADCChannel()];
  double maxbintime = (energyPulse.GetMaxBinBlockTime()) * energyClockPeriod;
 
  SetEnergyTime( maxbintime );
  if(nAverage > 1) { SetEnergyAmp( energyPulse.GetAverageMax(nAverage)); } else {SetEnergyAmp( energyPulse.GetMax());}
  //SetEnergyAmp( energyPulse.GetMax());
  SetEnergyIsland( &energyPulse );
}
 
