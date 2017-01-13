#ifndef PLOTCutOffPulses_H_
#define PLOTCutOffPulses_H_

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
/// Exports any pulses where the final sample is the largest
///
/// \details
////////////////////////////////////////////////////////////////////////////////
class PlotTPI_CutOffPulses : public BaseModule {

 public:
  /// \brief
  /// The constructor.
  ///
  /// \detailed
  /// The constructor looks in the opts for "n_samples" and sets the fNSamples variable (default = 5).
  ///
  /// \param[in] opts Describe the options this module takes.
  PlotTPI_CutOffPulses(modules::options* opts);
  /// \brief
  /// Empty destructor
  ~PlotTPI_CutOffPulses();

 private:
  /// \brief
  /// In every event, we loop over all the detectors and all the TPulseIslands
  /// and see if any TPIs final sample is the largest, if it is, we export it with ExportPulse
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
  /// Nothing is done after each run
  ///
  /// \param[in] gData See BaseModule::AfterLastEntry
  /// \param[in] setup See BaseModule::AfterLastEntry
  /// \return Non-zero to indicate a problem.
  virtual int AfterLastEntry(TGlobalData* gData,const  TSetupData *setup);

  int fNMaxExportPulses;
  int fNExportPulses;
  TH1F* fPulsesPlottedPerDetectorHist;
  TH1F* fPulsesPerDetectorHist;
  TH1F* fPulsesPlottedPerDetectorHist_Normalised;
};

#endif //PLOTCutOffPulses_H_
