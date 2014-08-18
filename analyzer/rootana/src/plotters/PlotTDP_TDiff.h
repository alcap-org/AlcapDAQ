#ifndef PLOTTDP_TDIFF_H_
#define PLOTTDP_TDIFF_H_

#include "BaseModule.h"
class TGlobalData;
class TSetupData;
namespace modules {class options;}

////////////////////////////////////////////////////////////////////////////////
/// \ingroup rootana_modules
/// \author Andrew Edmonds
///
/// \brief
/// This module plots the time difference between TDetectorPulses (TDPs) in 
/// two given detectors
///
/// \details
/// A longer, more descriptive block of text.
/// Specifics like members and methods will be described later.
/// You can add this to other groups instead of rootana_modules or in addition
/// to rootana_modules by adding more of the ingroup tags.
////////////////////////////////////////////////////////////////////////////////
class PlotTDP_TDiff : public BaseModule {

 public:
  /// \brief
  /// Constructor.
  ///
  /// \param[in] opts Describe the options this module takes.
  PlotTDP_TDiff(modules::options* opts);
  /// \brief
  /// Is anything done in the destructor?
  ~PlotTDP_TDiff();

 private:
  /// \brief
  /// In every entry the time difference between all the TDPs in one detector
  /// and all the TDPs in the other detector is calculated and plotted
  /// into a histogram
  /// 
  /// \return Non-zero to indicate a problem.
  virtual int ProcessEntry(TGlobalData *gData, const TSetupData *gSetup);
  /// \brief
  /// Before each run, the histograms are created.
  ///
  /// \return Non-zero to indicate a problem.
  virtual int BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup);
  /// \brief
  /// Nothing needs to be done after each run
  ///
  /// \return Non-zero to indicate a problem.
  virtual int AfterLastEntry(TGlobalData* gData,const TSetupData *setup);
};

#endif //PLOTTDP_TDIFF_H_
