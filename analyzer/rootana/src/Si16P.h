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
  TH2 *fhPIDRight, *fhPIDLeft;

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
  const double fdTMuScPP;
  const double fdTPID;
  const double fdTScatter;
  const double fdTSiLow, fdTSiHigh;

  // Energy cuts
  const double fMuonEnergyCut; // ADC

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
  std::vector<double> CalculateTimes(const IDs::channel& chan, const std::vector<TPulseIsland*>& tpis);
  /// \brief
  /// Takes a vector of TPIs and calculates all of their energies.
  std::vector<double> CalculateEnergies(const IDs::channel& chan, const std::vector<TPulseIsland*>& tpis);
  /// \brief
  /// Remove muSc hits (muons, electrons, etc.) that are too close to one another in time.
  void RemoveMuScPileUp(std::vector<double>& times, std::vector<double>& energies);
  /// \brief
  /// Remove hits from muSc that are too low energy to be muons.
  void RemoveElectrons(std::vector<double>& times, std::vector<double>& energies);
  /// \brief
  /// Remove hits that likely scattered away form the target and into detector arms.
  ///
  /// \details
  /// If we see a hit quickly in the detector arms, the muon may have scattered.
  /// The negative here is that the detector now has a large acceptance for
  /// any capture products. Additionally, if the muon didn't scatter but this is
  /// actually a disappearance product, we still want to throw out the event
  /// because there's nothing to see later since the muon is already gone.
  void RemoveScatteredMuons(std::vector<double>& muTimes, std::vector<double>& muEnergies,
			    const std::vector<double>& siTimes);
  void RemoveScatteredMuons(std::vector<double>& muTimes, std::vector<double>& muEnergies,
			    const std::vector< std::vector<double> >& siTimes);

  /// \brief
  /// If there are multiple hists in the detector arms,
  /// we cannot really figure out which one to count, so let's
  /// ignore these.
  void RemoveMultipleDetectorHits(std::vector<double>& muTimes, std::vector<double>& muEnergies,
				  const std::vector<double>& siThickTimes);
  void RemoveMultipleDetectorHits(std::vector<double>& muTimes, std::vector<double>& muEnergies,
				  const std::vector< std::vector<double> >& siThinsTimes);

  /// \brief
  /// Find a matching hit in the thin detector for a hit in the thick
  /// and return the energy; return -1.0 if none found.
  ///
  /// \details
  /// Returns the first such matching hit in the corresponding thin detector.
  /// A natural prerequisite is that multiple hits have already been omitted.
  /// A matching hit is in siThickTime +/- fdTPID.
  double FindFirstMatchingThinHit(const double siThickTime,
				  const std::vector< std::vector<double> >& siThinTimes,
				  const std::vector< std::vector<double> >& siThinEnergies);

  static void ThrowIfInputsInsane(modules::options*);
};

#endif //SI16P_H_
