#ifndef STATS_H_
#define STATS_H_

#include "BaseModule.h"
#include "TAPAlgorithms.h"
#include "IdChannel.h"

class TGlobalData;
class TSetupData;
namespace modules {class options;}

class TH1;
class TNtupleD;

#include <cmath>

////////////////////////////////////////////////////////////////////////////////
/// \ingroup rootana_modules
/// \author John R Quirk
////////////////////////////////////////////////////////////////////////////////
class Stats : public BaseModule {

 private:
  // Histograms
  TNtupleD* ftRunStats; // TPP, muSc height cut
  TNtupleD* ftBlockStats; // LT, NMu, PPNMu

  const Algorithm::MaxBinAmplitude fMBAmpMuSc;
  const Algorithm::ConstantFractionTime fCFTimeMuSc;
  const double fTPP;
  const double fMuScHeightCut;

  // Channels
  static const IDs::channel fMuSc;

 public:
  /// \brief
  /// Constructor description. If necessary, add a details tag like above.
  ///
  /// \param[in] opts Describe the options this module takes.
  Stats(modules::options* opts);
  /// \brief
  /// Is anything done in the destructor?
  ~Stats();

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
  std::vector<double> CalculateTimes(const std::vector<TPulseIsland*>& tpis);
  /// \brief
  /// Takes a vector of TPIs and calculates all of their energies.
  std::vector<double> CalculateEnergies(const std::vector<TPulseIsland*>& tpis);

  void ThrowIfInputsInsane(const modules::options*);
  void RemovePileupMuScPulses(std::vector<double>& time, std::vector<double>& energy);
  void RemoveElectrons(std::vector<double>& time, std::vector<double>& energy);

};

#endif //LIVETIME_H_
