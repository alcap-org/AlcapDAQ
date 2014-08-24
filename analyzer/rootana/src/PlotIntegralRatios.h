#ifndef PLOTINTEGRALRATIOS_H_
#define PLOTINTEGRALRATIOS_H_

#include "BaseModule.h"
class TGlobalData;
class TSetupData;
class TH1F;
class TH2F;
namespace modules {class options;}
#include "IdSource.h"

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
class PlotIntegralRatios : public BaseModule {
    struct SourcePlots_t{
        IDs::source src;
        TH1F* ratio;
        TH2F* full_v_tail;
    };
  typedef std::vector<SourcePlots_t> SourceList_t;

 public:
  /// \brief
  /// Constructor description. If necessary, add a details tag like above.
  ///
  /// \param[in] opts Describe the options this module takes.
  PlotIntegralRatios(modules::options* opts);
  /// \brief
  /// Is anything done in the destructor?
  ~PlotIntegralRatios();

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

  SourceList_t fSourcesToPlot;
  IDs::source fSource;
};

#endif //PLOTINTEGRALRATIOS_H_
