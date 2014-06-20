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
  /// Constructor description. If necessary, add a details tag like above.
  ///
  /// \param[in] opts Describe the options this module takes.
  PlotRMSNoise(modules::options* opts);
  /// \brief
  /// Is anything done in the destructor?
  ~PlotRMSNoise();

 private:
  /// \brief
  /// What's calculated for every entry?
  /// Don't hesitate to repeat what was said in the class description.
  /// 
  /// \param[in] gData See BaseModule::ProcessEntry
  /// \param[in] gSetup See BaseModule::ProcessEntry
  /// \return Non-zero to indicate a problem.
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
  /// \brief
  /// What needes to be done before each run?
  /// Don't hesitate to repeat what was said in the class description.
  ///
  /// \param[in] gData See BaseModule::BeforeFirstEntry
  /// \param[in] setup See BaseModule::BeforeFirstEntry
  /// \return Non-zero to indicate a problem.
  virtual int BeforeFirstEntry(TGlobalData* gData, TSetupData *setup);
  /// \brief
  /// What needs to be done after each run?
  /// Don't hesitate to repeat what was said in the class description.
  ///
  /// \param[in] gData See BaseModule::AfterLastEntry
  /// \param[in] setup See BaseModule::AfterLastEntry
  /// \return Non-zero to indicate a problem.
  virtual int AfterLastEntry(TGlobalData* gData, TSetupData *setup);

  /// \brief
  /// The map that we store the RMS noise histograms for each channel in
  std::map<std::string, TH1D*> fRMSNoiseHistograms;
};

#endif //PLOTRMSNOISE_H_
