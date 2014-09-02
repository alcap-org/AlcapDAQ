#ifndef PLOTTME_EVDE_H_
#define PLOTTME_EVDE_H_

#include "BaseModule.h"
class TGlobalData;
class TSetupData;
namespace modules {class options;}

////////////////////////////////////////////////////////////////////////////////
/// \ingroup rootana_modules
/// \author Ben Krikler
///
/// \brief
/// Make the E vs dE plot using TMEs
///
////////////////////////////////////////////////////////////////////////////////
class PlotTME_EvdE : public BaseModule {

    public:
        /// \brief
        /// Constructor 
        PlotTME_EvdE(modules::options* opts);
        ~PlotTME_EvdE();

    private:
        /// \brief
        /// Fills the EvdE histogram
        /// \return Non-zero to indicate a problem.
        virtual int ProcessEntry(TGlobalData *gData, const TSetupData *gSetup);
        /// \brief
        /// Set up the histograms and source IDs
        /// \return Non-zero to indicate a problem.
        virtual int BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup);
        /// \brief
        /// \return Non-zero to indicate a problem.
        virtual int AfterLastEntry(TGlobalData* gData,const TSetupData *setup);

    private:
};

#endif //PLOTTME_EVDE_H_
