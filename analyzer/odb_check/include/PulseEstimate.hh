/***********************************
Class: PulseEstimate
Author: John R. Quirk

This class takes data quality histograms from an
alcapana production and predicts what certain
correct ODB values are.
***********************************/

#ifndef PULSEESTIMATE_H__
#define PULSEESTIMATE_H__

class TH1;

////////////////////////////////////////////////////////////////////
/// \brief Estimates corrections for certian ODB values.
///
/// %PulseEstimate takes data quality histograms from an alcapana
/// production and predicts what certain ODB values are.
/// These are the pedestal, polarity, and timing offset.
////////////////////////////////////////////////////////////////////
class PulseEstimate {

private:
  /// Estimateed pedestal from the Y projection of the shapes (persistance) histogram.
  /// The bin containing the peak is considered the pedestal.
  int fPedestal;
  /// Estimated polarity from fPedestal and the Y projection of the
  /// shapes histogram. The non-zero bin that is farthest from the peak
  /// determines the polarity. Whichever is farther indicates the
  /// polarity of the pulse. The first and last 5 bins are ignored
  /// so that problems with certain detectors (premplifier reset
  /// in the germanium) don't interfere.
  int fPolarity;
  /// The estimated timing offset from the muSc time difference histograms.
  /// The bin containing the peak is used.
  int fOffset; 

public:
  /// Default constructor
  PulseEstimate();

  /// Estimate ODB values based on data quality histograms.
  ///
  /// @param[in] pulses The Y projection of the shapes histograms.
  /// @param[in] timing The muSc timeing difference histogram.
  void Estimate(TH1* pulses, TH1* timing);
  int GetPedestal() const; ///< @return Estimated pedestal
  int GetPolarity() const; ///< @return Estimated polarity
  int GetOffset() const; ///< @return Estimated timing offset (TimeShift)
};

#endif
