#ifndef PLOTRMSNOISE_H_
#define PLOTRMSNOISE_H_

#include "BaseModule.h"
class TGlobalData;
class TSetupData;
namespace modules {class options;}

////////////////////////////////////////////////////////////////////////////////
/// \ingroup rootana_modules
/// \author Andrew Edmonds
///
/// \brief
/// Plots the RMS noise of each TPulseIsland and writes the mean value to a text file
///
/// \details
/// This module loops over TPulseIsland and plots the RMS of the first n_samples into a
/// 1D histogram.
/// At the end of the run, it writes the mean value of this histogram to a text file along
/// with the channel and bank names. This text file can then be used by PulseCandidateFinder to 
/// set parameter values based on a certain number of sigma.
////////////////////////////////////////////////////////////////////////////////
class PlotRMSNoise : public BaseModule {

 public:
  /// \brief
  /// The constructor.
  ///
  /// \detailed
  /// The constructor looks in the opts for "n_samples" and sets the fNSamples variable (default = 5).
  ///
  /// \param[in] opts Describe the options this module takes.
  PlotRMSNoise(modules::options* opts);
  /// \brief
  /// Empty destructor
  ~PlotRMSNoise();

 private:
  /// \brief
  /// In every event, we loop over all the detectors and all the TPulseIslands
  /// and calculate the RMS and mean of the first fNSamples samples in the 
  /// digitised pulse (i.e. the noise and the pedestal respectively)
  /// 
  /// \param[in] gData See BaseModule::ProcessEntry
  /// \param[in] gSetup See BaseModule::ProcessEntry
  /// \return Non-zero to indicate a problem.
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);

  /// \brief
  /// Nothing is done before each run
  ///
  /// \param[in] gData See BaseModule::BeforeFirstEntry
  /// \param[in] setup See BaseModule::BeforeFirstEntry
  /// \return Non-zero to indicate a problem.
  virtual int BeforeFirstEntry(TGlobalData* gData, TSetupData *setup);

  /// \brief
  /// After each run, we write the mean and RMS of the first fNSamples to 
  /// a text file
  ///
  /// \param[in] gData See BaseModule::AfterLastEntry
  /// \param[in] setup See BaseModule::AfterLastEntry
  /// \return Non-zero to indicate a problem.
  virtual int AfterLastEntry(TGlobalData* gData, TSetupData *setup);

  /// \brief
  /// The map that we store the RMS noise histograms for each channel in
  std::map<std::string, TH1D*> fRMSNoiseHistograms;

  /// \brief
  /// The number of samples we will look at in the TPulseIsland
  /// to calculate the mean and RMS (opt = "n_samples", default = 5)
  int fNSamples;
};

#endif //PLOTRMSNOISE_H_
