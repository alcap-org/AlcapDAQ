#ifndef PLOTTAP_NMUONS_SI16_H_
#define PLOTTAP_NMUONS_SI16_H_

#include "BaseModule.h"
class TGlobalData;
class TSetupData;
namespace modules {class options;}

#include "TH2.h"

////////////////////////////////////////////////////////////////////////////////
/// \ingroup rootana_modules
/// \author Andrew Edmonds
///
/// \brief
/// Plots the number of muons in each Si16 strip
///
/// \details
/// Applies an ADC cut to select muons in the Si16 channels
////////////////////////////////////////////////////////////////////////////////
class PlotTAP_NMuons_Si16 : public BaseModule {

 public:
  /// \brief
  /// Constructor description. If necessary, add a details tag like above.
  ///
  /// \param[in] opts Describe the options this module takes.
  PlotTAP_NMuons_Si16(modules::options* opts);
  /// \brief
  /// Is anything done in the destructor?
  ~PlotTAP_NMuons_Si16();

 private:
  /// \brief
  /// What's calculated for every entry?
  /// Don't hesitate to repeat what was said in the class description.
  /// 
  /// \return Non-zero to indicate a problem.
  virtual int ProcessEntry(TGlobalData *gData, const TSetupData *gSetup);
  /// \brief
  /// What needes to be done before each run?
  /// Don't hesitate to repeat what was said in the class description.
  ///
  /// \return Non-zero to indicate a problem.
  virtual int BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup);
  /// \brief
  /// What needs to be done after each run?
  /// Don't hesitate to repeat what was said in the class description.
  ///
  /// \return Non-zero to indicate a problem.
  virtual int AfterLastEntry(TGlobalData* gData,const TSetupData *setup);

  // Don't forget to add descriptions for each field...
  /// \brief the low cut value for selecting muons in the Si
  ///
  /// \details used for doing something very intelligent and clever
  // ...and don't hesitate to include details.
  double fADCCutLow;
  double fADCCutHigh;

  /// \brief the histogram we will fill
  TH1F* fNMuonsPerStrip;
  TH2F* fSi16TAPAmplitudes;
  TH2F* fSi16TAPEnergies;
};

#endif //PLOTTAP_NMUONS_SI16_H_
