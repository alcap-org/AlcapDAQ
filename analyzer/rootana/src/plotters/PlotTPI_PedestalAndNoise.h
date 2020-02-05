#ifndef PLOTRMSNOISE_H_
#define PLOTRMSNOISE_H_

#include "BaseModule.h"
class TGlobalData;
class TSetupData;
namespace modules {class options;}

#include <TH2.h>

////////////////////////////////////////////////////////////////////////////////
/// \ingroup rootana_modules
/// \author Andrew Edmonds
///
/// \brief
/// Plots the pedestal and noise of each TPulseIsland and writes these to an SQLite database
///
/// \details
/// This module loops over TPulseIsland and plots the mean and RMS of the first n_samples into a
/// 2D histogram.
/// At the end of the run, it writes the mean values of each axis to an SQLite database along
/// with the channel and bank names. This SQLite database can then be used by PulseCandidateFinder to
/// set parameter values based on a certain number of sigma.
////////////////////////////////////////////////////////////////////////////////
class PlotTPI_PedestalAndNoise : public BaseModule {

 public:
  /// \brief
  /// The constructor.
  ///
  /// \detailed
  /// The constructor looks in the opts for "n_samples" and sets the fNSamples variable (default = 5).
  ///
  /// \param[in] opts Describe the options this module takes.
  PlotTPI_PedestalAndNoise(modules::options* opts);
  /// \brief
  /// Empty destructor
  ~PlotTPI_PedestalAndNoise();

 private:
  /// \brief
  /// In every event, we loop over all the detectors and all the TPulseIslands
  /// and calculate the RMS and mean of the first fNSamples samples in the
  /// digitised pulse (i.e. the noise and the pedestal respectively)
  ///
  /// \param[in] gData See BaseModule::ProcessEntry
  /// \param[in] gSetup See BaseModule::ProcessEntry
  /// \return Non-zero to indicate a problem.
  virtual int ProcessEntry(TGlobalData *gData,const TSetupData *gSetup);

  /// \brief
  /// Nothing is done before each run
  ///
  /// \param[in] gData See BaseModule::BeforeFirstEntry
  /// \param[in] setup See BaseModule::BeforeFirstEntry
  /// \return Non-zero to indicate a problem.
  virtual int BeforeFirstEntry(TGlobalData* gData,const  TSetupData *setup);

  /// \brief
  /// After each run, we write the mean and RMS of the first fNSamples to
  /// an SQLite database via SetupNavigator
  ///
  /// \param[in] gData See BaseModule::AfterLastEntry
  /// \param[in] setup See BaseModule::AfterLastEntry
  /// \return Non-zero to indicate a problem.
  virtual int AfterLastEntry(TGlobalData* gData,const  TSetupData *setup);

  void BookHistogram(const std::string& bank, const std::string& det);

  /// \brief
  /// The map that we store the pedestal vs. noise histograms for each channel
  std::map<std::string, TH2D*> fPedestalVsNoiseHistograms;

  /// \brief
  /// The number of samples we will look at in the TPulseIsland
  /// to calculate the mean and RMS (opt = "n_samples")
  const int fNSamples;

  /// \brief
  /// A bool to decide whether we export the pedestal and noise to the SQLite database
  /// (opt = "export_sql", default = false)
  const bool fExportSQL;
};

#endif //PLOTRMSNOISE_H_
