/** \addtogroup odbcheck
 * @{
 */
#ifndef PULSEESTIMATE_H__
#define PULSEESTIMATE_H__

class TH1;
class TH2;

////////////////////////////////////////////////////////////////////
/// \brief Estimates corrections for certian ODB values.
///
/// %PulseEstimate takes data quality histograms from an alcapana
/// production and predicts what certain ODB values are.
/// These are the pedestal, polarity, and timing offset.
////////////////////////////////////////////////////////////////////
class PulseEstimate {

private:
  /// Estimateed pedestal from the Y projection of the first few (10) bins
  /// of the shapes (persistance) histogram.
  /// The bin containing the peak is considered the pedestal.
  int fPedestal;
  /// Estimated polarity from fPedestal and the Y projection of the
  /// entire shapes histogram. The non-zero bin that is farthest
  /// from the peak determines the polarity. Whichever is farther
  /// indicates the polarity of the pulse. The first and last 5 bins
  /// are ignored so that problems with certain detectors
  /// (premplifier reset in the germanium) don't interfere.
  int fPolarity;
  /// The estimated timing offset from the muSc time difference histograms.
  /// The bin containing the peak is used.
  int fOffset; 

public:
  /// Default constructor
  PulseEstimate();

  /// Estimate ODB values based on data quality histograms.
  ///
  /// @param[in] pulses The shapes (persistance) histogram.
  /// @param[in] timing The muSc timeing difference histogram.
  void Estimate(TH2* pulses, TH1* timing);

  /// \name Getters
  //@{
  /// @return Estimated pedestal
  int GetPedestal() const;
  /// @return Estimated polarity
  int GetPolarity() const;
  /// @return Estimated timing offset (TimeShift)
  int GetOffset() const;
  //@}
};

#endif
/** @}*/
