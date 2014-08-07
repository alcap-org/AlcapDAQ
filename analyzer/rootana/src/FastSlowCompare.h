#ifndef FASTSLOWCOMPARE_H_
#define FASTSLOWCOMPARE_H_

#include "BaseModule.h"

#include <map>
#include <string>

class TGlobalData;
class TSetupData;
namespace modules {class options;}

class TH1I;
class TH2I;

#include <cmath>

////////////////////////////////////////////////////////////////////////////////
/// \ingroup rootana_modules
/// \author John R Quirk
///
/// \brief
/// A one line description of what your module does.
///
/// \details
/// A longer, more descriptive block of text.
/// Specifics like members and methods will be described later.
/// You can add this to other groups instead of rootana_modules or in addition
/// to rootana_modules by adding more of the ingroup tags.
////////////////////////////////////////////////////////////////////////////////
class FastSlowCompare : public BaseModule {

 private:
  double fTimeWindowWide;
  double fTimeWindowTight;

  std::map<std::string, std::string> fPairs;

  std::map<std::string, TH1I*> fHist_Time;
  std::map<std::string, TH1I*> fHist_MoreTime;
  std::map<std::string, TH1I*> fHist_NPerSlowWide;
  std::map<std::string, TH1I*> fHist_NPerSlowTight;

 public:
  /// \brief
  /// Constructor description. If necessary, add a details tag like above.
  ///
  /// \param[in] opts Describe the options this module takes.
  FastSlowCompare(modules::options* opts);
  /// \brief
  /// Is anything done in the destructor?
  ~FastSlowCompare();

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
  virtual int BeforeFirstEntry(TGlobalData* gData, TSetupData *setup);
  /// \brief
  /// What needs to be done after each run?
  /// Don't hesitate to repeat what was said in the class description.
  ///
  /// \param[in] gData See BaseModule::AfterLastEntry
  /// \param[in] setup See BaseModule::AfterLastEntry
  /// \return Non-zero to indicate a problem.
  virtual int AfterLastEntry(TGlobalData* gData, TSetupData *setup);

};

#endif //FASTSLOWCOMPARE_H_
