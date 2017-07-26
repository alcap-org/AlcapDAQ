#ifndef TMETree_H_
#define TMETree_H_

#include "BaseModule.h"
class TGlobalData;
class TSetupData;
namespace modules {class options;}
namespace IDs {class channel;}
class TH1F;
class TH2F;
class TCanvas;
class TApplication;
#include "TTree.h"
#include "SimplePulse.h"

////////////////////////////////////////////////////////////////////////////////
/// \ingroup rootana_modules
/// \ingroup rootana_plotters
/// \author Andrew Edmonds
///
/// \brief
/// Testing the TMEs for R15b, this will plot the central muon energy
///
/// \details
/// A longer, more descriptive block of text.
/// Specifics like members and methods will be described later.
/// You can add this to other groups instead of rootana_modules or in addition
/// to rootana_modules by adding more of the ingroup tags.
////////////////////////////////////////////////////////////////////////////////
class TMETree : public BaseModule {

    public:
        /// \brief
        /// Constructor description. If necessary, add a details tag like above.
        ///
        /// \param[in] opts Describe the options this module takes.
        TMETree(modules::options* opts);
        /// \brief
        /// Is anything done in the destructor?
        ~TMETree();

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

        typedef std::vector<IDs::channel> DetectorList;

        DetectorList fAllDets;

	TTree* fTMETree;
	int fRunId;
	int fBlockId;
	int fTMEId;

	double fTMEWindowWidth;
	std::string fCentralMuonChannel;
	int fCentralMuonTPIID;
	double fCentralMuonEnergy;
	double fCentralMuonTime;

	double fTimeToPrevTME;
	double fTimeToNextTME;
	bool fAnyDoubleCountedPulses;

	std::map<std::string, int> fNPulses;
	std::map<std::string, std::vector<SimplePulse> > fChannels;

};

#endif //CHECKTMES_H_
