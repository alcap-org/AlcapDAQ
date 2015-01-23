#ifndef TemplateFastFitter_h_
#define TemplateFastFitter_h_

#include "TemplateFastFitParameters.h"
#include "MultiHistogramFastFitFCN.h"

#include "TFitterMinuit.h"
#include "TPulseIsland.h"
#include "TTemplate.h"

#include "definitions.h"
#include "AlcapExcept.h"

MAKE_EXCEPTION(TemplateFastFitter, Base);
MAKE_EXCEPTION(MismatchedTemplateRefineFactors, TemplateFastFitter);

class TDirectory;

class TemplateFastFitter {
  void operator=(const TemplateFastFitter&);

  struct TemplateDetails_t{
    double fAmplitudeScaleFactor;
    double fTimeOffset;
    TemplateDetails_t():
	    fAmplitudeScaleFactor(definitions::DefaultValue),
	    fTimeOffset(definitions::DefaultValue) {}
  };
  typedef std::vector<TemplateDetails_t> TemplateList;

 public:
  TemplateFastFitter(const IDs::channel& ch);
  ~TemplateFastFitter();
  void Init();

 public:
  enum FitStatus{
    kSuccess=0 ,
    kPedestalOutOfBounds= -100,
    kAmplitudeOutOfBounds= -101,
    kTimeOutOfBounds= -102,
    kFitNotConverged= 1,
    kInitialised=-200
  };
  void SetTemplate(TTemplate* tpl);

  /// @name getters
  /// @{
  double GetPedestal()const { return fPedestal; }
  double GetAmplitude(int i)const { return fTemplate->GetAmplitude() * fTemplates.at(i).fAmplitudeScaleFactor;}
  double GetAmplitudeScaleFactor(int i)const { return fTemplates.at(i).fAmplitudeScaleFactor;}
  double GetTime(int i)const { return fTemplate->GetTime() + fTemplates.at(i).fTimeOffset; }
  double GetTimeOffset(int i)const { return fTemplates.at(i).fTimeOffset; }
  double GetChi2()const { return fChi2; }
  double GetNDoF()const { return fNDoF; }
  TemplateFastFitParameters GetFitParameters()const { return fFitFCN->GetFitParameters(); }
  /// @}

  /// @name setters:
  /// @{
  void SetAmplitudeSF(int i, double amp_sf) {  fTemplates.at(i).fAmplitudeScaleFactor = amp_sf;}
  void SetTimeOffset(int i, double time_off) {  fTemplates.at(i).fTimeOffset = time_off; }
  /// @}

  /// @name fitting
  /// @{
  int FitWithOne(const TH1D* hPulse);
  int FitWithTwo(const TH1D* hPulse);
  /// @}

  void SaveSumHistograms(TDirectory* dir);

 private:
  int Fit(const TH1* hPulse);
  
 private:
  TFitterMinuit* fMinuitFitter;
  MultiHistogramFastFitFCN* fFitFCN;
  IDs::channel fChannel;
  double fMaxADC,fMinADC;

  double fPedestal;
  int fRefineFactor;
  TemplateList fTemplates;
  TTemplate* fTemplate;
  int fFreeTemplate;

  double fChi2;
  int fNDoF; // number of degress of freedom in fit
};

#endif // TemplateFastFitter_h_
