#ifndef PLOTTME_ACTIVESIRMUSTOPS_h
#define PLOTTME_ACTIVESIRMUSTOPS_h

#include "TDetectorPulse.h"
#include "BaseModule.h"
#include "definitions.h"
class TGlobalData;
class TSetupData;
namespace modules {class options;}
namespace IDs {class channel;}
class TH1F;
class TH2F;

////////////////////////////////////////////////////////////////////////////////
/// \ingroup rootana_modules
/// \ingroup rootana_plotters
/// \author Ben Krikler
///
/// \brief
/// Produce various plots that characterise the produced TMuonEvents
///
/// \details
/// A longer, more descriptive block of text.
/// Specifics like members and methods will be described later.
/// You can add this to other groups instead of rootana_modules or in addition
/// to rootana_modules by adding more of the ingroup tags.
////////////////////////////////////////////////////////////////////////////////
class PlotTME_activeSiRmuStops : public BaseModule {

    public:
        /// \brief
        /// Constructor description. If necessary, add a details tag like above.
        ///
        /// \param[in] opts Describe the options this module takes.
        PlotTME_activeSiRmuStops(modules::options* opts);
        /// \brief
        /// Is anything done in the destructor?
        ~PlotTME_activeSiRmuStops();

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

        void FillHistograms(const TMuonEvent* tme, const IDs::source& muSc_source, const IDs::source& siR2_source);

    private:
        long int fStoppedMus, fStoppedMus_PP;
        long int fNStopsThisEvent, fNStopsThisEvent_PP;
        long int fEventNo;

        IDs::channel fMuSc, fSiR2;
        TDetectorPulse::ParentChannel_t fChannel;

        double fMuScMax, fMuScMin, fSiR2Max, fSiR2Min;

        TH1F *fTDiff_PP, *fTDiff, *fTDiffMuons, *fTDiffMuons_PP, *fStopsPerEvent, *fStopsPerEvent_PP, *fStops, *fStops_PP;
        TH2F *fAmplitudes, *fAmplitudes_PP, *fTDiffVsAmpSiR2, *fTDiffVsAmpSiR2_PP, *fTDiffVsAmpSiR2_MuStop, *fTDiffVsAmpSiR2_MuStop_PP;
};

#endif //PLOTTME_ACTIVESIRMUSTOPS_h
