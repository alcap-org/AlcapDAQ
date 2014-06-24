#ifndef PULSECANDIDATEFINDER_INVESTIGATEPARAMETERS_H_
#define PULSECANDIDATEFINDER_INVESTIGATEPARAMETERS_H_

#include "BaseModule.h"
class TGlobalData;
class TSetupData;
namespace modules {class options;}

#include "utils/PulseCandidateFinder.h"

////////////////////////////////////////////////////////////////////////////////
/// \ingroup rootana_modules
/// \author Andrew Edmonds
///
/// \brief
/// Produces plots either the sample heights or the sample differences for each TPulseIsland
///
/// \details
/// This module loops over TPulseIslands and sends them to PulseCandidateFinder::FillParameterHistogram()
/// to plot either the height of the sample above pedestals (if the TPulseIsland is on a slow channel)
/// or the difference between samples (if the TPulseIsland is on a fast channel).
/// 
/// The idea is that we can then look at these histograms to determine the
/// values of the parameters for the PulseCandidateFinder
////////////////////////////////////////////////////////////////////////////////
class PulseCandidateFinder_InvestigateParameters : public BaseModule{

 public:
  /// \brief
  /// The constructor.
  ///
  /// \param[in] opts Describe the options this module takes.
  PulseCandidateFinder_InvestigateParameters(modules::options* opts);

  /// \brief
  /// Empty destructor
  ~PulseCandidateFinder_InvestigateParameters();

 private:
  /// \brief
  /// In every event, we loop over all the detectors and create a ParameterHistogram,
  /// then we loop over the TPulseIslands and find the candidate pulses with PulseCandidateFinder::FindPulseCandidates().
  /// Finally, we fill the parameter histogram with either the height of all samples, or the differences between consecutive samples
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
  /// Nothing done at the end of each run
  ///
  /// \param[in] gData See BaseModule::AfterLastEntry
  /// \param[in] setup See BaseModule::AfterLastEntry
  /// \return Non-zero to indicate a problem.
  virtual int AfterLastEntry(TGlobalData* gData, TSetupData *setup);

 private:
  /// \brief
  /// The map that we store the parameter histograms for each channel
  std::map<std::string, TH1D*> fParameterHistograms;

  /// \brief
  /// Store the module options so that they can be passed to the PulseCandidateFinder
  modules::options* fOpts;
};

#endif //PULSECANDIDATEFINDER_INVESTIGATEPARAMETERS_H_
