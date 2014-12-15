#ifndef PLOTTME_EVDE_H_
#define PLOTTME_EVDE_H_

#include "IdSource.h"
#include "BaseModule.h"
#include <string>

#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
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
///////////////////////////////////////////////////////////////////////////////1
class PlotTME_EvdE : public BaseModule {

    public:
        /// \brief
        /// Constructor 
        PlotTME_EvdE(modules::options* opts);
        ~PlotTME_EvdE();

    private:
        virtual int ProcessEntry(TGlobalData *gData, const TSetupData *gSetup);
        virtual int BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup);
        virtual int AfterLastEntry(TGlobalData* gData,const TSetupData *setup);

    private:
        enum Plots{ kNoCuts=0, kMuPP, kMuPP_muAmp, kMuPP_muAmp_T500, kNHists};
        struct Hists{
           TH2 *EvdE;
           TH1 *time;
           TH3 *EvdEvTime;
           void Fill(double totalE, double deltaE, double deltaT){
              EvdE->Fill(totalE,deltaE);
              time->Fill(deltaT);
              EvdEvTime->Fill(totalE,deltaE,deltaT);
           }
        };
        Hists fHists[2][kNHists][4];
        double fMinTime, fMuScMax, fMuScMin;
};

#endif //PLOTTME_EVDE_H_
