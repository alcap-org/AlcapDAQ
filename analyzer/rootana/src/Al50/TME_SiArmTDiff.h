#ifndef TME_SiArmTDiff_H_
#define TME_SiArmTDiff_H_

#include "BaseModule.h"
class TGlobalData;
class TSetupData;
namespace modules {class options;}
namespace IDs {class channel;}
class TH1F;
class TH2F;
class TCanvas;
class TApplication;

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
class TME_SiArmTDiff : public BaseModule {

    public:
        /// \brief
        /// Constructor description. If necessary, add a details tag like above.
        ///
        /// \param[in] opts Describe the options this module takes.
        TME_SiArmTDiff(modules::options* opts);
        /// \brief
        /// Is anything done in the destructor?
        ~TME_SiArmTDiff();

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
        DetectorList fSiL1;
	IDs::channel* fSiL2;
        DetectorList fSiR1;
	IDs::channel* fSiR2;

	TH1F* fSiLTDiffs;
	TH1F* fSiRTDiffs;
};

#endif //CHECKTMES_H_
