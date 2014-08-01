#ifndef PLOTTDPS_H_
#define PLOTTDPS_H_

#include "BaseModule.h"
class TGlobalData;
class TSetupData;
class TH1F;
class TH2F;
namespace modules {class options;}
namespace IDs {class source;}

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
class PlotTDPs : public BaseModule {
    public:     
        struct Detector_t {
            TH2F* amplitudes;
            TH1F* fast_only_amps, *slow_only_amps;
            TH1F* fast_amps_slow_cut, *slow_amps_fast_cut;
            TH1F* time_diff;
        };
        typedef std::map<IDs::source,Detector_t> PlotsList_t;

    public:
        /// \brief
        /// Constructor description. If necessary, add a details tag like above.
        ///
        /// \param[in] opts Describe the options this module takes.
        PlotTDPs(modules::options* opts);
        /// \brief
        /// Is anything done in the destructor?
        ~PlotTDPs();

    private:
        /// \brief
        /// What's calculated for every entry?
        /// Don't hesitate to repeat what was said in the class description.
        /// 
        /// \param[in] gData See BaseModule::ProcessEntry
        /// \param[in] gSetup See BaseModule::ProcessEntry
        /// \return Non-zero to indicate a problem.
        virtual int ProcessEntry(TGlobalData *gData,const  TSetupData *gSetup);
        /// \brief
        /// What needes to be done before each run?
        /// Don't hesitate to repeat what was said in the class description.
        ///
        /// \param[in] gData See BaseModule::BeforeFirstEntry
        /// \param[in] setup See BaseModule::BeforeFirstEntry
        /// \return Non-zero to indicate a problem.
        virtual int BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup);
        /// \brief
        /// What needs to be done after each run?
        /// Don't hesitate to repeat what was said in the class description.
        ///
        /// \param[in] gData See BaseModule::AfterLastEntry
        /// \param[in] setup See BaseModule::AfterLastEntry
        /// \return Non-zero to indicate a problem.
        virtual int AfterLastEntry(TGlobalData* gData,const TSetupData *setup);

    private:
        PlotsList_t fPlotsList;
        double fFastCut, fSlowCut;
};

#endif //PLOTTDPS_H_
