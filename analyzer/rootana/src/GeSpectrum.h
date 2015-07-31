#ifndef GESPECTRUM_H_
#define GESPECTRUM_H_

#include "BaseModule.h"
#include "TAPAlgorithms.h"
#include "IdChannel.h"

class TGlobalData;
class TSetupData;
namespace modules {class options;}

class TH1;
class TH2;
class TF1;

#include <cmath>

////////////////////////////////////////////////////////////////////////////////
/// \ingroup rootana_modules
/// \author John R Quirk and Andrew Edmonds
///
/// \brief
/// This produces a 2D histogram of the energy of the germanium pulse (y-axis) vs. the time differenve
/// between the muSc pulse and the germanium pulse (x-axis). 
///
/// \details
/// The produced histogram can then be analysed by separate scripts to do different things (e.g. apply
/// time cuts and fit to X-ray peaks to get the number of stopped muons or to look for longer lived states)
////////////////////////////////////////////////////////////////////////////////
class GeSpectrum : public BaseModule {

 private:
  // Histograms
  TH2* fHist_TimeEnergy;

  // Channels
  static const IDs::channel fGe;
  static const IDs::channel fMuSc;
  static const IDs::channel fGeEnergy; // specficially the slow pulse so that we can get the energy resolution

 public:
  /// \brief
  /// Constructor description. If necessary, add a details tag like above.
  ///
  /// \param[in] opts Describe the options this module takes.
  GeSpectrum(modules::options* opts);
  /// \brief
  /// Is anything done in the destructor?
  ~GeSpectrum();

 private:
  /// \brief
  /// Histograms the energy of the germanium pulse with the time difference 
  /// of between the germanium pulse and the muSc pulse
  /// 
  /// \param[in] gData See BaseModule::ProcessEntry
  /// \param[in] gSetup See BaseModule::ProcessEntry
  /// \return Non-zero to indicate a problem.
  virtual int ProcessEntry(TGlobalData *gData, const TSetupData *gSetup);
  /// \brief
  /// Does nothing.
  ///
  /// \param[in] gData See BaseModule::BeforeFirstEntry
  /// \param[in] setup See BaseModule::BeforeFirstEntry
  /// \return Non-zero to indicate a problem.
  virtual int BeforeFirstEntry(TGlobalData* gData, const TSetupData *setup);
  /// \brief
  /// Does nothing.
  ///
  /// \param[in] gData See BaseModule::AfterLastEntry
  /// \param[in] setup See BaseModule::AfterLastEntry
  /// \return Non-zero to indicate a problem.
  virtual int AfterLastEntry(TGlobalData* gData, const TSetupData *setup);

  // Histogram parameters
  double fMinTime;
  double fMaxTime;
  double fTimeBinWidth;
};

#endif //GESPECTRUM_H_
