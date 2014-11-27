#ifndef PLOTTME_EVDE_H_
#define PLOTTME_EVDE_H_

#include "IdSource.h"
#include "BaseModule.h"
#include <string>

class TH2F;
class TH1F;
class TGlobalData;
class TSetupData;
namespace modules {class options;}

namespace{
namespace LR{
    enum Type{kLeft=0, kRight, kNum};
    const char* str(Type,bool big=false);
    const char* str(int e,bool big=false){return str((Type)e, big);}
}
namespace Ch{
    enum Type{k1_1=0, k1_2, k1_3, k1_4, k2, kNum};
    const char* str(Type);
    const char* str(int e){return str((Type)e);}
}
}

////////////////////////////////////////////////////////////////////////////////
/// \ingroup rootana_modules
/// \author Ben Krikler
///
/// \brief
/// Make the E vs dE plot using TMEs
///
///////////////////////////////////////////////////////////////////////////////1
class PlotTME_EvdE : public BaseModule {

    struct SourceSet{
        TH2F *E_vs_dE_no_pileUp, *E_vs_dE_with_pileUp;
        IDs::source sources[Ch::kNum];
    };

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

        /// @brief Get the source for a given channel in the list of TDP
        const IDs::source& GetTDPSource(const std::string& ch );

    private:
        SourceSet fSourceList[LR::kNum];
        double fSiWindow;
};

#endif //PLOTTME_EVDE_H_
