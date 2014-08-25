#ifndef CALCCOARSETIMEOFFSET_H_
#define CALCCOARSETIMEOFFSET_H_

#include "BaseModule.h"
#include "IdSource.h"
class TH1F;
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
class CalcCoarseTimeOffset : public BaseModule {
  typedef std::vector<IDs::source> SourceVector;

 public:
  /// \brief
  /// Constructor description. If necessary, add a details tag like above.
  ///
  /// \param[in] opts Describe the options this module takes.
  CalcCoarseTimeOffset(modules::options* opts);
  /// \brief
  /// Is anything done in the destructor?
  ~CalcCoarseTimeOffset();

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

  void CreateHistogramsIfDontExist(const std::string& source, const std::string& generator);

  /// \brief
  /// Don't forget to forget descirptions for each field...
  ///
  /// \details
  /// ...and don't hesitate to include details.
  std::string fDetNameA, fDetNameB;

  std::map<std::string, TH1F*> oned_plots;
  SourceVector fDetASources, fDetBSources;
};

#endif //CALCCOARSETIMEOFFSET_H_
