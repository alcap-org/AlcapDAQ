#ifndef PLOTTDIFF_H_
#define PLOTTDIFF_H_

#include <iostream>
#include "TH2F.h"
#include "TH1F.h"
#include "BaseModule.h"
#include "IdSource.h"
class TGlobalData;
class TSetupData;
namespace modules {class options;}

////////////////////////////////////////////////////////////////////////////////
/// \ingroup rootana_modules
/// \author AuthorName
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
class PlotTAP_TDiff : public BaseModule {

 public:
  /// \brief
  /// Constructor description. If necessary, add a details tag like above.
  ///
  /// \param[in] opts Describe the options this module takes.
  PlotTAP_TDiff(modules::options* opts);
  /// \brief
  /// Is anything done in the destructor?
  ~PlotTAP_TDiff();

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
  /// \brief Prepare histograms.
  void BookHistograms(const TSetupData*);

  /// \brief
  /// Don't forget to forget descirptions for each field...
  ///
  /// \details
  /// ...and don't hesitate to include details.
  std::string fDetNameA, fDetNameB;
  double fTimeLow, fTimeHigh;
  bool fExportSQL;

  std::map<std::string, std::vector<TH2F*> > fHists;
  std::map<std::string, std::vector<TH1F*> > fProjs;

  std::vector<IDs::source> fDetASources, fDetBSources;
};

#endif //PLOTTDIFF_H_
