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

#include <cmath>

////////////////////////////////////////////////////////////////////////////////
/// \ingroup rootana_modules
/// \author John R Quirk
///
/// \brief
/// This produces the height spectrum of the germanium based on timing cuts.
///
/// \details
/// We look near muon entrances for the xrays and several microseconds away for
/// longer lived nuclear decays. We also look at medium time distances for
/// captures. These refer to the "Energy", "EnergyFarOOT" and "EnergyOOT"
/// histograms respectively. Also plots time vs energy in tight window
/// in TimeEnergy histogram, and looks for timing correlations with muSc
/// in MeanTOffset within a MIDAS event (as each event might have a slightly
/// different offset due to clock reset).
////////////////////////////////////////////////////////////////////////////////
class GeSpectrum : public BaseModule {

 private:
  TH1* fHist_Energy;
  TH1* fHist_Time;
  TH1* fHist_MoreTime;
  TH1* fHist_EnergyOOT;
  TH1* fHist_EnergyFarOOT;
  TH2* fHist_TimeEnergy;
  TH1* fHist_MeanTOffset;

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
  /// Histograms the heights in the germanium with time cuts
  /// relative to muSc.
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

  /// \brief
  /// Takes a vector of TPIs and calculates all of their times.
  ///
  /// \details
  /// Since we'll be using the std::upper_bound/std::lower_bound methods,
  /// this makes it possible to use the times of the TPIs without
  /// writing some hacky compare function (that would use the TAPAlgorithms).
  std::vector<double> CalculateTimes(const IDs::channel& chan, const std::vector<TPulseIsland*>& tpis);
  /// \brief
  /// Takes a vector of TPIs and calculates all of their energies.
  std::vector<double> CalculateEnergies(const IDs::channel& chan, const std::vector<TPulseIsland*>& tpis);

  void ThrowIfInputsInsane(const modules::options*);
  void ThrowIfGeInsane(const std::vector<TPulseIsland*>& ge_fasts, const std::vector<TPulseIsland*>& ge_slows);


  // Algorithms
  const Algorithm::MaxBinAmplitude fMBAmpGe;
  const Algorithm::ConstantFractionTime fCFTimeGe, fCFTimeMuSc;

  // Channels
  static const IDs::channel fGeS;
  static const IDs::channel fGeF;
  static const IDs::channel fMuSc;
};

#endif //GESPECTRUM_H_
