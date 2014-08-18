#ifndef PLOTTDP_TDIFF_H_
#define PLOTTDP_TDIFF_H_

#include "BaseModule.h"
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
class PlotTDP_TDiff : public BaseModule {

 public:
  /// \brief
  /// Constructor description. If necessary, add a details tag like above.
  ///
  /// \param[in] opts Describe the options this module takes.
  PlotTDP_TDiff(modules::options* opts);
  /// \brief
  /// Is anything done in the destructor?
  ~PlotTDP_TDiff();

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
  /// \brief maximum x-value
  ///
  /// \details used for doing something very intelligent and clever
  // ...and don't hesitate to include details.
  int fXMax;
};

#endif //PLOTTDP_TDIFF_H_
