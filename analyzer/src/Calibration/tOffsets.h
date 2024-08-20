// Author: Cole Kampa
// Date: 08/20/24
// Using this file to store first pass of time offsets (ns) for various aspects of neutron detectors, vetos, gamma detectors. This has already included the SyncPulseOffset to match the TDC and ADC.
// Note: it isn't clear if this will be the best way to handle this, but will allow getting the first pass of the analysis.
// For now, I am using the mean value of a basic peak fit for 3 Al runs (6892, 7311, 7370). The fit uses a Gaussian + linear function. Peaks are roughly isolated from TCorr histograms, grabbing the prompt gamma peak.
// I have noticed variations in some of the offset values between targets. This should be investigated later.


// Typical ordering: NdetD, NdetU, NdetDV, NdetUV (veto not always present)
// FIXME! Need to add Ge detector and veto, TScV, and TSc
namespace AlCap {
  // Timing/MTDCWFDTCorr.cpp
  // dt = detTDC - detADC
  const double detTDC_min_detADC[2] = {14.00, 14.02};
  // Timing/MTScTCorrWFD.cpp
  // dt = detADC - TScADC
  const double detADC_min_TScADC[2] = {69.20, 48.44};
  // Timing/MTTScTCorrWFD.cpp
  // dt = detADC - TScTDC
  const double detADC_min_TScTDC[2] = {42.37, 20.78};
  // Timing/MTTScTCorrTDC.cpp
  // dt = detTDC - TScTDC
  const double detTDC_min_TScTDC[4] = {57.93, 39.57, 2.65, 7.56};
}
