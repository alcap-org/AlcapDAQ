#ifndef PLOTAMPVSMUSCTDIFF_H_
#define PLOTAMPVSMUSCTDIFF_H_

#include "BaseModule.h"
#include "IdSource.h"
class TGlobalData;
class TSetupData;
class TH2F;
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
class PlotAmpVsMuScTDiff : public BaseModule {
    typedef std::vector<IDs::source> SourceVector;

    public:
    /// \brief
    /// Constructor description. If necessary, add a details tag like above.
    ///
    /// \param[in] opts Describe the options this module takes.
    PlotAmpVsMuScTDiff(modules::options* opts);
    /// \brief
    /// Is anything done in the destructor?
    ~PlotAmpVsMuScTDiff();

    private:
    /// \brief
    /// What's calculated for every entry?
    /// Don't hesitate to repeat what was said in the class description.
    /// 
    /// \param[in] gData See BaseModule::ProcessEntry
    /// \param[in] gSetup See BaseModule::ProcessEntry
    /// \return Non-zero to indicate a problem.
    virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
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

    private:
    struct Plots{
        TH2F *coarse, *fine;
    };

    Plots GetHistogram(const IDs::source& detector, const IDs::source& muSc);


    std::map<std::string, Plots> fPlots;

    SourceVector fMuScSources;
    SourceVector fNotMuScSources;

};

#endif //PLOTAMPVSMUSCTDIFF_H_
