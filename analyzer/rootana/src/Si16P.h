#ifndef SI16P_H_
#define SI16P_H_

#include "BaseModule.h"
#include "TAPAlgorithms.h"
#include "IdChannel.h"
#include "IdGenerator.h"

class TGlobalData;
class TSetupData;
namespace modules {class options;}

class TH1;
class TH2;
class TF1;

#include <cmath>

////////////////////////////////////////////////////////////////////////////////
/// \ingroup rootana_modules
/// \author John R Quirk
///
/// \brief
///
/// \details
////////////////////////////////////////////////////////////////////////////////
class Si16P : public BaseModule {

 private:
  // Histograms
  TH2 *fHist_PIDRight, *fHist_PIDLeft;

  // Algorithms
  const Algorithm::MaxBinAmplitude fMBAmp_MuSc;
  const Algorithm::MaxBinAmplitude fMBAmp_SiR11S, fMBAmp_SiR12S, fMBAmp_SiR13S, fMBAmp_SiR14S, fMBAmp_SiR2S;
  const Algorithm::MaxBinAmplitude fMBAmp_SiL11S, fMBAmp_SiL12S, fMBAmp_SiL13S, fMBAmp_SiL14S, fMBAmp_SiL2S;
  const Algorithm::ConstantFractionTime fCFTime_MuSc;
  const Algorithm::ConstantFractionTime fCFTime_SiR11S, fCFTime_SiR12S, fCFTime_SiR13S, fCFTime_SiR14S, fCFTime_SiR2S;
  const Algorithm::ConstantFractionTime fCFTime_SiL11S, fCFTime_SiL12S, fCFTime_SiL13S, fCFTime_SiL14S, fCFTime_SiL2S;
  TF1 *fADC2E_SiR11S, *fADC2E_SiR12S, *fADC2E_SiR13S, *fADC2E_SiR14S, *fADC2E_SiR2S;
  TF1 *fADC2E_SiL11S, *fADC2E_SiL12S, *fADC2E_SiL13S, *fADC2E_SiL14S, *fADC2E_SiL2S;

  // Time cuts (ns)
  const double fdTMuSc;
  const double fdTPID;
  const double fdTScatter;
  const double fdTDetectorPileup;
  const double fdTSiLow, fdTSiHigh;

  // Energy cuts
  const double fMuonCut; // ADC

  // Channels/Generator
  static const IDs::channel fMuSc;
  static const IDs::channel fSiR11S, fSiR12S, fSiR13S, fSiR14S, fSiR2S;
  static const IDs::channel fSiL11S, fSiL12S, fSiL13S, fSiL14S, fSiL2S;
  static const IDs::generator fGenerator;
  static const double fConstantFraction;

 public:
  /// \brief
  /// Constructor description. If necessary, add a details tag like above.
  ///
  /// \param[in] opts Describe the options this module takes.
  Si16P(modules::options* opts);
  /// \brief
  /// Is anything done in the destructor?
  ~Si16P();

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

  static void ThrowIfInputsInsane(modules::options*);
};

#endif //SI16P_H_
