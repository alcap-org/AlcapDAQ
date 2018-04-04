#ifndef MultiHistogramFitFCN_h__
#define MultiHistogramFitFCN_h__

#include "Minuit2/FCNBase.h"
#include "TH1D.h"
#include "AlcapExcept.h"

#include "TSetupData.h"

#include "debug_tools.h"
#include <iostream>

#include <vector>

MAKE_EXCEPTION(MultiHistogramFitFCN, Base);
MAKE_EXCEPTION(SlimlyOverlappingTemplates,MultiHistogramFitFCN);

class MultiHistogramFitFCN : public ROOT::Minuit2::FCNBase {
 struct HistogramDetails_t {
   int fTimeOffset;
   double fAmplitudeScale, fAmplitudeScale2, fPedestal;
   double fAmplitudeError;
   const TH1D* fTemplateHist;
   int fTemplateHistNBinsX;
   double fTemplateContents[1000];
   double fTemplateErrors2[1000];
   int fNDoF;
   int fTriggerPolarity;
   inline double GetHeight(int template_bin)const;
   inline double GetError2(int template_bin)const;
 };
 // typedef std::vector<HistogramDetails_t> TemplateList;

 public:
  MultiHistogramFitFCN(double refine_factor=1);
  ~MultiHistogramFitFCN();

  void AddTemplate(const TH1D* hist, double time=0){
     HistogramDetails_t tmp;
     tmp.fTemplateHist=hist;
     tmp.fTimeOffset=time;
     tmp.fNDoF=0;

     int n_bins_for_template_pedestal = 5;
     double total = 0;
     for (int iBin = 1; iBin <= n_bins_for_template_pedestal; ++iBin) {
       total += hist->GetBinContent(iBin);
     }
     tmp.fPedestal = total / n_bins_for_template_pedestal;

     tmp.fTemplateHistNBinsX = tmp.fTemplateHist->GetNbinsX();
     for (int i_bin = 1; i_bin <= tmp.fTemplateHist->GetNbinsX(); ++i_bin) {
       tmp.fTemplateContents[i_bin] = tmp.fTemplateHist->GetBinContent(i_bin);
       tmp.fTemplateErrors2[i_bin] = tmp.fTemplateHist->GetBinError(i_bin)*tmp.fTemplateHist->GetBinError(i_bin);
     }
     
     fTemplates[fNUsedTemplates] = tmp;
     ++fNUsedTemplates;
  }
  void SetTemplateHist( int n , const TH1D* hist){
    fTemplates[n].fTemplateHist=hist;
  }
  void SetTimeOffset(int n, double offset){
    fTemplates[n].fTimeOffset=offset;
  }
  void SetTime(int n, double time){
    fTemplates[n].fTimeOffset=time - fTemplates[n].fTemplateHist->GetMaximumBin();
  }
  void SetAmplitude(int n, double amp){
    fTemplates[n].fAmplitudeScale=amp/fTemplates[n].fTemplateHist->GetMaximum();
    fTemplates[n].fAmplitudeScale2=fTemplates[n].fAmplitudeScale*fTemplates[n].fAmplitudeScale;
  }
  void SetInitialValues(int n, double time_offset, double amp){
    SetTime(n, time_offset);
    SetAmplitude(n, amp);
  }

  void SetPulseHist(const TH1D* pulse){
    fPulseHist=pulse;
    pulse_hist_n_bins_x = fPulseHist->GetNbinsX();
    for (int i_bin = 1; i_bin <= fPulseHist->GetNbinsX(); ++i_bin) {
      pulse_contents[i_bin] = fPulseHist->GetBinContent(i_bin);
      pulse_errors2[i_bin] = fPulseHist->GetBinError(i_bin)*fPulseHist->GetBinError(i_bin);;
    }
  }
  void SetRefineFactor(int refine_factor) {fRefineFactor = refine_factor;}
  
  double& GetPedestalOffset()const{return fPedestalOffset;}
  double& GetAmplitudeScaleFactor(int i)const{return fTemplates[i].fAmplitudeScale;}
  int& GetTimeOffset(int i)const{return fTemplates[i].fTimeOffset;}
  int GetNTemplates()const{return fNUsedTemplates;}

  /// @brief Used to calculate the chi-2
  /// @details
  /// The return value is the chi squared
  /// weighted by errors in fTemplateHist
  /// Parameters:
  /// 0. Pedestal 
  /// 1. Amplitude of first pulse
  /// 2. Amplitude of second pulse
  /// ....
  /// N Amplitude of Nth pulse
  double operator() (const std::vector<double>& par) const;

  /// Used for error... somehow?
  double Up() const {return 1.;}

  int GetNDoF() { return fNDoF; }

  double CalculateDelta(int i_global_bin) const;
    
 private:
  inline void UnpackParameters(const std::vector<double>& par)const;
  inline void GetHistogramBounds(int safety, int &low_edge, int& high_edge)const;

  /// record this for TemplateFitter to retrieve later (NB mutable so that it
  /// can be set in operator(), which is const)
  mutable int fNDoF;
  mutable double fChi2;
  mutable double fPedestalOffset;
  
  int fRefineFactor;

  static const int fNMaxTemplates = 5;
  int fNUsedTemplates;
  mutable HistogramDetails_t fTemplates[fNMaxTemplates];

  const TH1D* fPulseHist; // The histogram to fit
  double pulse_contents[1000]; // use arrays because they might be quicker
  double pulse_errors2[1000];
  int pulse_hist_n_bins_x;

  // setting things once here for optimisation
  mutable double tpl_height,tpl_error2;
  mutable double this_tpl_height,this_tpl_error2;
  mutable double pulse_sample_value, pulse_sample_error2, delta;
  mutable int i_pulse_bin, i_template_bin, i_global_bin, i_tpl;  
};

inline void MultiHistogramFitFCN::GetHistogramBounds(int safety, int &low_edge, int& high_edge)const{
  // We want to cover the full range of the pulse and the template
  // and will assume that we can extrapolate each with their pedestals
  
  // init to pulse histogram
  low_edge=safety;
  high_edge=fPulseHist->GetNbinsX()-safety;

  // for each template 
  int tpl_start, tpl_stop;
  for(int i_tpl=0; i_tpl < fNUsedTemplates; ++i_tpl){
    tpl_start=fTemplates[i_tpl].fTimeOffset + safety;
    tpl_stop=fTemplates[i_tpl].fTimeOffset+fTemplates[i_tpl].fTemplateHist->GetNbinsX() - safety;

    // find the maximum of all the starts of a template
    if(high_edge<tpl_stop) high_edge=tpl_stop;

    // find the minimum of all the ends of a template
    if(low_edge>tpl_start) low_edge=tpl_start;
  }
}

inline void MultiHistogramFitFCN::UnpackParameters(const std::vector<double>& par)const{
    if ( par.size() < fNUsedTemplates+1) return;
    const std::vector<double>::const_iterator begin=par.begin();
    std::vector<double>::const_iterator i_par=begin;
    fPedestalOffset=*i_par;
    for(++i_par ; i_par!=par.end(); ++i_par){
      int n= (i_par-begin-1) ;
      fTemplates[n].fAmplitudeScale=*i_par;
      //      std::cout << "AE4: " << *i_par << std::endl;
      fTemplates[n].fAmplitudeScale2=fTemplates[n].fAmplitudeScale*fTemplates[n].fAmplitudeScale;
    }
  }

#endif
