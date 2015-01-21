#ifndef PLOTTME_EVDE_ACTIVESI_H_
#define PLOTTME_EVDE_ACTIVESI_H_

#include "IdSource.h"
#include "BaseModule.h"
#include <string>
#include "ActiveSiRMuStopAlgo.h"

#include <TF1.h>
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
class PlotTME_EvdE_ActiveSi : public BaseModule {

    public:
        /// \brief
        /// Constructor 
        PlotTME_EvdE_ActiveSi(modules::options* opts);
        ~PlotTME_EvdE_ActiveSi();

    private:
        virtual int ProcessEntry(TGlobalData *gData, const TSetupData *gSetup);
        virtual int BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup);
        virtual int AfterLastEntry(TGlobalData* gData,const TSetupData *setup);
        void FillSiR2Hits(const TMuonEvent* tme,int quad,double deltaE, double totalE, double time);

    private:
        enum Plots{ kNoCuts=0, kActiveStop,kProtonBand, kMuPP, kMuPP_muAmp, kMuPP_muAmp_T500, kNHists};
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
        Hists fHists[2][kNHists][5];
        double fMinTime, fMuScMax, fMuScMin;
        TMEAlgorithm::ActiveSiRStop fActiveStops;
        IDs::source fSiR2Source;

        double fEmissionTimeWidth, fEmissionTimeCentre;
        TF1 *fHighCut, *fLowCut;
        TH1 *fSiR2HitsThick[5];
        TH1 *fSiR2HitsThin[5];
        TH2 *fSiR2Hits_timeVsE;
        TH1 *fSiR2Hits_time, *fSiR2Hits_time_zoom;
};

#endif //PLOTTME_EVDE_ACTIVESI_H_
