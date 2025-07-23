// Author: Cole Kampa
// Date: 08/20/24
// Using this file to store first pass of time offsets (ns) for various aspects of neutron detectors, vetos, gamma detectors. This has already included the SyncPulseOffset to match the TDC and ADC.
// Note: it isn't clear if this will be the best way to handle this, but will allow getting the first pass of the analysis.
// For now, I am using the mean value of a basic peak fit for 3 Al runs (6892, 7311, 7370). The fit uses a Gaussian + linear function. Peaks are roughly isolated from TCorr histograms, grabbing the prompt gamma peak.
// I have noticed variations in some of the offset values between targets. This should be investigated later.

#include <string>
#include <map>

// Typical ordering: NdetD, NdetU, NdetDV, NdetUV, TVSc (veto not always present)
// FIXME! Need to add Ge detector and veto, and TSc (pulse matching)
namespace AlCap {
  // Timing/MTDCWFDTCorr.cpp
  // dt = detTDC - detADC
  // NdetD, NdetU, GeCHEH
  //const double detTDC_min_detADC[3] = {14.00, 14.02, -4540.0}; // pretrigger missing in map -- random init
  const double detTDC_min_detADC[3] = {14.00, 14.02, -821.0};
  // Timing/MTScTCorrWFD.cpp
  // dt = detADC - TScADC
  // NdetD, NdetU
  const double detADC_min_TScADC[2] = {69.20, 48.44};
  // NdetD, NdetU, GeCHEH (COULD NOT FIND)
  //const double detADC_min_TScADC[3] = {69.20, 48.44, };
  // Timing/MTTScTCorrWFD.cpp
  // dt = detADC - TScTDC
  // NdetD, NdetU, GeCHEH
  //const double detADC_min_TScTDC[3] = {42.37, 20.78, 4705.0}; // pretrigger missing in map -- random init
  const double detADC_min_TScTDC[3] = {42.37, 20.78, 975.0};
  // Timing/MTTScTCorrTDC.cpp
  // dt = detTDC - TScTDC
  // NdetD, NdetU, NdetDV, NdetUV, TVSc, GeCHT, GeV
  //const double detTDC_min_TScTDC[5] = {57.93, 39.57, 2.65, 7.56, 9.05};
  const double detTDC_min_TScTDC[7] = {57.93, 39.57, 2.65, 7.56, 9.05, 120.0, -7.25};
  // PRETRIGGER (for CF)
  //const int pretrigger = 40; // seems like it should be 60 from DAQ settings, but plotte pulses seem to cross threshold at tick 40
  // assume fixed pretriggers for the entire run (could extract from ODB but would require processing dev)
  // using values from run06892.odb (Al golden run)
  std::map<std::string, double> fDetectorToPretriggerMap = {
    /* DT5730  -- all of these have a fast rise time so applying CF does not make sense.*/
    // {"TSc", 5.0},
    // {"NdetU", 17.0},
    // {"NdetD", 17.0},
    // {"GeCHT", 20.0},
    /* V1724 */
    {"GeCHEH", 60.0},
    {"GeCHEL", 60.0},
  };
}
