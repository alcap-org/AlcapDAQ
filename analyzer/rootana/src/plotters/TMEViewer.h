#ifndef TMEViewer_H_
#define TMEViewer_H_

#include "BaseModule.h"
class TGlobalData;
class TSetupData;
namespace modules {class options;}
namespace IDs {class channel;}
class TH1F;
class TH2F;
class TCanvas;
class TApplication;
#include "TPad.h"

////////////////////////////////////////////////////////////////////////////////
/// \ingroup rootana_modules
/// \ingroup rootana_plotters
/// \author Andrew Edmonds
///
/// \brief
/// Testing the TMEs for my Al50 analysis
///
/// \details
/// A longer, more descriptive block of text.
/// Specifics like members and methods will be described later.
/// You can add this to other groups instead of rootana_modules or in addition
/// to rootana_modules by adding more of the ingroup tags.
////////////////////////////////////////////////////////////////////////////////
class TMEViewer : public BaseModule {

    public:
        /// \brief
        /// Constructor description. If necessary, add a details tag like above.
        ///
        /// \param[in] opts Describe the options this module takes.
        TMEViewer(modules::options* opts);
        /// \brief
        /// Is anything done in the destructor?
        ~TMEViewer();

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

        long int fNullCount, fTdpCount;

	bool fQuit;

        typedef std::vector<IDs::channel> DetectorList;
        DetectorList fDetectors;

	TCanvas* fCanvas;
	TPad* fPad_TimeWindow;
	TPad* fPad_TimeWindowZoomIn;
	TPad* fPad_Pulses;
	std::vector<TH1F*> fTPIHists;
	TApplication* fApp;
	TH2F* fTDiffPerDetector;
	TH2F* fTDiffPerDetector_ZoomIn;
};

#endif //CHECKTMES_H_
