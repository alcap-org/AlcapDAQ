#include "MultiHistogramFastFitFCN.h"

#include <cmath>
#include <iostream>
#include <cassert>

MultiHistogramFastFitFCN::MultiHistogramFastFitFCN(double refine_factor):
  fRefineFactor(refine_factor){
}

MultiHistogramFastFitFCN::~MultiHistogramFastFitFCN() {
}

double MultiHistogramFastFitFCN::HistogramDetails_t::GetHeight(double t, const TH1* template_hist)const{
    int bin=t - fTimeOffset;
    double remainder=t - fTimeOffset - bin;
    assert( remainder<1 );
    double y0 = template_hist->GetBinContent(bin);
    double y1 = template_hist->GetBinContent(bin+1);
    return (y0 + remainder*((y1-y0)))*fAmplitudeScale;
}

double MultiHistogramFastFitFCN::HistogramDetails_t::GetError2(double t, const TH1* template_hist)const{
    int bin= t - fTimeOffset;
    double error= template_hist->GetBinError(bin);
    return error*error;
}

double MultiHistogramFastFitFCN::operator() (const std::vector<double>& par) const {
  // Chi2 fit with pedestal P, amplitude A, and timing T
  // Warning: The time is truncated to an int, so if there's
  // so if the step size in MINUIT is smalled than 1,
  // Any value of T will probably be seen as minimized, which it
  // almost certainly will not be.
  UnpackParameters(par);
  double chi2 = 0.;

  int safety = 6*fRefineFactor; // remove a few bins from the fit
  int bounds[2];
  GetHistogramBounds(safety,bounds[0], bounds[1]);
  
  if( (bounds[1]-bounds[0]) < 40*fRefineFactor ) throw Except::SlimlyOverlappingTemplates();

  // Calculate the degrees of freedom ( #data - #fit_params)
  // +1 because we include both ends of the bounds when we loop through
  fNDoF = ((bounds[1] - bounds[0] + 1)/fRefineFactor) - par.size(); 

  double tpl_height,tpl_error;
  for (int i = bounds[0]+(fRefineFactor/2.0); i <= bounds[1]-(fRefineFactor/2.0); i += fRefineFactor) { 
    // calculate the chi^2 based on the centre of the 5 bins to avoid getting
    // abonus from mathcing all 5.  We shift and scale the template so that it
    // matches the pulse.  This is because, when we have a normalised template,
    // we will get the actual amplitude, pedestal and time from the fit and not
    // just offsets
    tpl_height=0;
    tpl_error=0;
    
    for(TemplateList::const_iterator i_tpl=fTemplates.begin(); 
         i_tpl!=fTemplates.end(); ++i_tpl){
       tpl_height+=i_tpl->GetHeight(i,fTemplateHist);
       tpl_error+=i_tpl->fAmplitudeScale*i_tpl->GetError2(i,fTemplateHist);
    }

    double delta = fPulseHist->GetBinContent(i) - tpl_height;
    chi2 += delta*delta / tpl_error;
  }

  return chi2;
}

void MultiHistogramFastFitFCN::Initialise(){
  // k is the separation of the two templates
  // i is the index of the sample in the first template being looked at
  
  int N  = fTemplateHist->GetNbinsX();
 
  // Make sure we can store all the values
  sum_tpl.resize(N);
  sum_sq_tpl.resize(N);
  sum_cross_tpl.resize(N);
 
  // Make sure the last sum is 0
  sum_tpl[N] = sum_sq_tpl[N] = sum_cross_tpl[N] =0;
 
  double T_i, T_i_minus_k;
 
  // Step over k in reverse
  for( int k=N-1; k>=0; --k){
     T_i=fTemplateHist->GetBinContent(N-k+1); // +1 since bin 0 is the underflow bin in a TH1
     sum_tpl[k]=sum_tpl[k+1] + T_i;
     sum_sq_tpl[k]=sum_sq_tpl[k+1] + T_i*T_i;

     // Now calculate the cross terms for this separation
     // Since i < 0, T_i is 0 we can start at i=k
     for( int i=k; i<N; ++i){
        T_i=fTemplateHist->GetBinContent(i+1);           // +1 since bin 0 is the underflow bin in a TH1
        T_i_minus_k=fTemplateHist->GetBinContent(i-k+1); // +1 since bin 0 is the underflow bin in a TH1
        sum_cross_tpl[k]+=T_i * T_i_minus_k;
     }
  }
 
  // Debugging
  for( int i=0; i<N; ++i){
     DEBUG_VALUE(sum_tpl[i],sum_sq_tpl[i],sum_cross_tpl[i]);
  }
 
}
