#ifndef FASTTEMPLATEFITAPGENERATOR_H__
#define FASTTEMPLATEFITAPGENERATOR_H__

#include "TSetupData.h"
#include "TVAnalysedPulseGenerator.h"
#include "definitions.h"
#include "TemplateArchive.h"
#include "TemplateFastFitter.h"
#include "TTemplate.h"
#include "TTemplateFitAnalysedPulse.h"
#include "TAPAlgorithms.h"

class FastTemplateFitAPGenerator:public TVAnalysedPulseGenerator {

 public:
    FastTemplateFitAPGenerator(TAPGeneratorOptions* opts);
    virtual ~FastTemplateFitAPGenerator();

 public:
    virtual int ProcessPulses( const PulseIslandList&,AnalysedPulseList&);

    // This function should return true if this generator could break up a TPI
    // into more than one TAP
    virtual bool MayDivideTPIs(){return true;};
   

    static const char* TapType() {
        return TTemplateFitAnalysedPulse::Class()->GetName();
    }

  private:
    bool PassesIntegralRatio(const TPulseIsland* pulse,double& integral, double& ratio)const;
    bool RefitWithTwo( TH1D* tpi, TTemplateFitAnalysedPulse*& tap_one, TTemplateFitAnalysedPulse*& tap_two)const;
    void InitializeSecondPulse( TH1D* tpi, const TTemplateFitAnalysedPulse* tap_one, int& second_time)const;

  private:
    static TemplateArchive* fTemplateArchive;
    bool fAttemptRefit;
    double fIntegralMax, fIntegralMin;
    double fIntegralRatioMax, fIntegralRatioMin;
    double fTemplateAmp, fTemplatePed, fTemplateTime;
    double fInitPedestal;
    double fChi2MinToRefit;
    TTemplate* fTemplate;
    TemplateFastFitter* fFitter;
    Algorithm::IntegralRatio* fIntegralRatio;
    Algorithm::MaxBinAmplitude fMaxBin;

};

#endif //FASTTEMPLATEFITAPGENERATOR_H__
