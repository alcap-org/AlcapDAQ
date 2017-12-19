#ifndef TemplateMultiFitter_h_
#define TemplateMultiFitter_h_

#include "TFitterMinuit.h"
#include "TPulseIsland.h"
#include "TTemplate.h"
#include "MultiHistogramFitFCN.h"

#include "definitions.h"
#include "AlcapExcept.h"

MAKE_EXCEPTION(TemplateMultiFitter, Base);
MAKE_EXCEPTION(MismatchedTemplateRefineFactors, TemplateMultiFitter);

// User only ever deals with the absolute values of fits
// NOT the transformations values themselves
// For reference: we always fit the template to the pulse
// (for template creation we get the inverse transformations)

class TemplateMultiFitter {
  void operator=(const TemplateMultiFitter&);

  struct TemplateDetails_t{
    int fIndex;
    double fAmplitudeScaleFactor;
    double fTimeOffset;
    TTemplate fTemplate;
    // Absolute values for the pulse
    double GetTime()const{ return  fTemplate.GetTime()+fTimeOffset; }
    double GetAmplitude()const{ return  fTemplate.GetAmplitude()*fAmplitudeScaleFactor; }
    // Relative to the template
    double GetTimeOffset()const{ return  fTimeOffset; }
    double GetAmplitudeScaleFactor()const{ return  fAmplitudeScaleFactor; }
    void Set(double amp, double time){
      fAmplitudeScaleFactor=amp/fTemplate.GetAmplitude();
      fTimeOffset=time- fTemplate.GetTime();
    }
  };
  typedef std::vector<TemplateDetails_t> TemplateList;

  // A place to store the current best values of the fits
  struct FitValues_t{
    double time_offset;
    std::vector<double> parameters;
    double chi2;
    int ndof;
    int status;
  };

 public:
  TemplateMultiFitter(const IDs::channel& ch, int refine_factor = 0);
  ~TemplateMultiFitter();
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
  void AddTemplate(TTemplate* tpl);

  /// @name fitting
  /// @{
  /// \brief
  /// Sets the intial estimates for the template fitter
  void SetPedestalEstimate(double pedestal){fPedestal=pedestal;}
  void SetPulseEstimates( int index, double amp, double time){
      fTemplates.at(index).Set(amp,time);
  }
  int FitPulseToTemplate(int index,const TH1D* hPulse); // for template creation
  int FitWithOneTimeFree(int index,const TH1D* hPulse, double offset_range);
  int FitWithAllTimesFixed(const TH1D* hPulse);
  /// @}

  /// @name getters
  /// @{
  double GetPedestal()const { return fPedestal; }
  double GetAmplitude(int i)const { return fTemplates.at(i).GetAmplitude();}
  double GetTime(int i)const { return fTemplates.at(i).GetTime(); }
  double GetAmplitudeScaleFactor(int i)const { return fTemplates.at(i).GetAmplitudeScaleFactor();}
  double GetTimeOffset(int i)const { return fTemplates.at(i).GetTimeOffset(); }
  double GetChi2()const { return fChi2; }
  double GetNDoF()const { return fNDoF; }
  int GetNTemplates()const { return fTemplates.size(); }
  /// @}

 private:
  void PrepareForMinimizations(const TH1D* hPulse);
  void ResetFitValues();
  void TimeOffsetLoop(TemplateDetails_t& current_template, double offset_range);
  void FitThisTime(TemplateDetails_t& current_template, double time_offset);
  void StoreFinalBestValues(TemplateDetails_t& current_template);

 private:
  TFitterMinuit* fMinuitFitter;
  MultiHistogramFitFCN* fFitFCN;
  IDs::channel fChannel;
  double fMaxADC,fMinADC;

  double fPedestal;
  int fRefineFactor;
  TemplateList fTemplates;
  int fFreeTemplate;

  double fChi2;
  int fNDoF; // number of degress of freedom in fit

  FitValues_t fBestValues;
  FitValues_t fCurrentValues;
};

#endif
