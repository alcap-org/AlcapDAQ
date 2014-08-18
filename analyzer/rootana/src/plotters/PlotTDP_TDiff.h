#ifndef PLOTTDP_TDIFF_H_
#define PLOTTDP_TDIFF_H_

#include "BaseModule.h"
class TGlobalData;
class TSetupData;
class TH1F;
class TH2F;
namespace modules {class options;}
namespace IDs {class source;}

////////////////////////////////////////////////////////////////////////////////
/// \ingroup rootana_plotters
/// \ingroup rootana_modules
/// \author Ben Krikler
///
/// \brief Profile TDetectorPulse s
///
/// \details
/// Plot the relative amplitudes, efficiencies and times of paired pulses
/// (TDetectorPulses, TDPs).
////////////////////////////////////////////////////////////////////////////////
class PlotTDP_TDiff : public BaseModule {
    public:     
        struct Detector_t {
            //amplitudes
            TH2F *amplitudes;
            TH1F *fast_only_amps, *slow_only_amps;
            TH1F *scale_factor;
            // efficiencies
            TH1F *fast_amps_slow_cut, *slow_amps_fast_cut;
            //times
            TH2F *time_diff_fast, *time_diff_slow;
            TH2F *times;

            int amp_status;
            double amp_inter, amp_grad;
        };
        typedef std::map<IDs::source,Detector_t> PlotsList_t;

    public:
        /// \brief Constructor 
        PlotTDP_TDiff(modules::options* opts);
        /// \brief Destructor
        ~PlotTDP_TDiff();

    private:
        /// \brief Fill all histograms
        virtual int ProcessEntry(TGlobalData *gData,const  TSetupData *gSetup);
        /// \brief Setup all histograms
        virtual int BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup);
        /// \brief Draw and fit all desired histograms
        virtual int AfterLastEntry(TGlobalData* gData,const TSetupData *setup);

    private:
        PlotsList_t fPlotsList;
        double fFastCut, fSlowCut;
        bool fCutHists;
};

#endif //PLOTTDPS_H_
