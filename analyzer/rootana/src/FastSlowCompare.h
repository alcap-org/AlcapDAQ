#ifndef FASTSLOWCOMPARE_H_
#define FASTSLOWCOMPARE_H_

#include "BaseModule.h"
#include "IdChannel.h"
#include "IdSource.h"

#include <map>
#include <string>

class TGlobalData;
class TSetupData;
namespace modules {class options;}

class TH2I;

#include <cmath>

////////////////////////////////////////////////////////////////////////////////
/// \ingroup rootana_modules
/// \author John R Quirk
///
/// \brief
/// Compares the timing of fast and slow pulses.
///
////////////////////////////////////////////////////////////////////////////////
class FastSlowCompare : public BaseModule {

 private:
  double fTimeWindowWide;
  double fTimeWindowTight;

  std::vector< std::pair<IDs::source, IDs::source> > fPairs;

  std::vector<TH2I*> fHist_Time;
  std::vector<TH2I*> fHist_MoreTime;
  std::vector<TH2I*> fHist_NPerSlowWide;
  std::vector<TH2I*> fHist_NPerSlowTight;

 public:
  /// \param[in] opts Describe the options this module takes.
  FastSlowCompare(modules::options* opts);

 private:
  /// \brief
  /// What's calculated for every entry?
  /// Don't hesitate to repeat what was said in the class description.
  /// 
  /// \param[in] gData See BaseModule::ProcessEntry
  /// \param[in] gSetup See BaseModule::ProcessEntry
  /// \return Non-zero to indicate a problem.
  virtual int ProcessEntry(TGlobalData *gData, const TSetupData *gSetup);
  /// \brief
  /// What needes to be done before each run?
  /// Don't hesitate to repeat what was said in the class description.
  ///
  /// \param[in] gData See BaseModule::BeforeFirstEntry
  /// \param[in] setup See BaseModule::BeforeFirstEntry
  /// \return Non-zero to indicate a problem.
  virtual int BeforeFirstEntry(TGlobalData* gData, const TSetupData *setup);
  /// \brief
  /// What needs to be done after each run?
  /// Don't hesitate to repeat what was said in the class description.
  ///
  /// \param[in] gData See BaseModule::AfterLastEntry
  /// \param[in] setup See BaseModule::AfterLastEntry
  /// \return Non-zero to indicate a problem.
  virtual int AfterLastEntry(TGlobalData* gData, const TSetupData *setup);
  void BookHistograms();

};

#endif //FASTSLOWCOMPARE_H_
