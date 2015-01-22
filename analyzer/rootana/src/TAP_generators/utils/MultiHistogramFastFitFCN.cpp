#include "MultiHistogramFastFitFCN.h"

#include <cmath>
#include <iostream>
#include <cassert>

using std::cout;
using std::endl;

MultiHistogramFastFitFCN::MultiHistogramFastFitFCN(double refine_factor):
  fRefineFactor(refine_factor),fTemplates(2){
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

  switch(fNTemplatesToFit){
    case 1:
       fChi2= FitOne(par.at(0));
    break;
    case 2:
       fChi2= FitTwo(par.at(0),par.at(1));
    break;
    default: fChi2= -1;
  }
  return fChi2;
}

double MultiHistogramFastFitFCN::FitOne(double time_offset)const{
  // Find the sums over the pulse being fit to
  const int N = fTemplateHist->GetNbinsX();
  double Y=0, Y_cross=0, Y_sq=0;
  for(int i=0; i<N;i+=fRefineFactor){
     int bin=i*1./fRefineFactor+time_offset;
     double y=fPulseHist->GetBinContent(bin);
     Y +=y;
     Y_sq +=y*y;
     Y_cross += y*fTemplateHist->GetBinContent(i);
  }

  // Calculate the amplitude scale factor
  double a = fInvertedSums_one.el11 * Y_cross - fInvertedSums_one.el12 * Y;
  a /= fInvertedSums_one.determinant;
  fTemplates.at(0).fAmplitudeScale=a;

  // Calculate the pedestal
  fPedestal = fInvertedSums_one.el22 * Y - fInvertedSums_one.el12 * Y_cross;
  fPedestal /= fInvertedSums_one.determinant;

  // Calculate the chi square
  double chi_2 = a*a*sum_sq_tpl[0] + N*fPedestal*fPedestal
               + Y_sq - 2*a*Y_cross - 2*fPedestal*Y + 2*a*fPedestal*sum_tpl[0];
  return chi_2;
}

double MultiHistogramFastFitFCN::FitTwo(double time_offset, double time_offset2)const{
 return 1.;
}

//{
//  double tpl_height,tpl_error;
//  for (int i = bounds[0]+(fRefineFactor/2.0); i <= bounds[1]-(fRefineFactor/2.0); i += fRefineFactor) { 
//    // calculate the chi^2 based on the centre of the 5 bins to avoid getting
//    // abonus from mathcing all 5.  We shift and scale the template so that it
//    // matches the pulse.  This is because, when we have a normalised template,
//    // we will get the actual amplitude, pedestal and time from the fit and not
//    // just offsets
//    tpl_height=0;
//    tpl_error=0;
//    
//    for(TemplateList::const_iterator i_tpl=fTemplates.begin(); 
//         i_tpl!=fTemplates.end(); ++i_tpl){
//       tpl_height+=i_tpl->GetHeight(i,fTemplateHist);
//       tpl_error+=i_tpl->fAmplitudeScale*i_tpl->GetError2(i,fTemplateHist);
//    }
//
//    double delta = fPulseHist->GetBinContent(i) - tpl_height;
//    chi2 += delta*delta / tpl_error;
//  }
//
//  return chi2;
//}

void MultiHistogramFastFitFCN::Initialise(){
  // k is the separation of the two templates
  // i is the index of the sample in the first template being looked at
  
  const int N  = fTemplateHist->GetNbinsX();
 
  // Make sure we can store all the values
  sum_tpl.resize(N/fRefineFactor);
  sum_sq_tpl.resize(N/fRefineFactor);
  sum_cross_tpl.resize(N/fRefineFactor);
  fInvertedSums_two.resize(N/fRefineFactor);
 
  // Make sure the last sum is 0
  sum_tpl[N] = sum_sq_tpl[N] = sum_cross_tpl[N] =0;
 
  double T_i, T_i_minus_k;
 
  // Step over k in reverse
  for( int k=N-1; k>=0; k-=fRefineFactor){
     T_i=fTemplateHist->GetBinContent(N-k+1); // +1 since bin 0 is the underflow bin in a TH1
     sum_tpl[k]=sum_tpl[k+1] + T_i;
     sum_sq_tpl[k]=sum_sq_tpl[k+1] + T_i*T_i;
     // Now calculate the cross terms for this separation
     // Since i < 0, T_i is 0 we can start at i=k
     for( int i=k; i<N; i+=fRefineFactor){
        T_i=fTemplateHist->GetBinContent(i+1);           // +1 since bin 0 is the underflow bin in a TH1
        T_i_minus_k=fTemplateHist->GetBinContent(i-k+1); // +1 since bin 0 is the underflow bin in a TH1
        sum_cross_tpl[k]+=T_i * T_i_minus_k;
     }
     
     // Now calculate the cross terms for this separation
     // Now compute the components of the inverted matrix of the sums for two templates
     fInvertedSums_two[k].el11 = N*sum_sq_tpl[k] - sum_tpl[k]*sum_tpl[k];
     fInvertedSums_two[k].el12 = sum_tpl[k]*sum_tpl[k] - N*sum_cross_tpl[k];
     fInvertedSums_two[k].el13 = sum_cross_tpl[k] * sum_tpl[k] - sum_tpl[0] * sum_sq_tpl[k];
     fInvertedSums_two[k].el22 = N*sum_sq_tpl[0] - sum_tpl[0]*sum_tpl[0];
     fInvertedSums_two[k].el23 = sum_cross_tpl[k] * sum_tpl[0] - sum_sq_tpl[0]*sum_tpl[k];
     fInvertedSums_two[k].el33 = sum_sq_tpl[0] * sum_sq_tpl[k] - sum_cross_tpl[k]*sum_cross_tpl[k];
     fInvertedSums_two[k].determinant = N*(sum_sq_tpl[0]*sum_sq_tpl[k]         + sum_cross_tpl[k]*sum_cross_tpl[k]  )
                                        - (sum_sq_tpl[0]*sum_tpl[k]*sum_tpl[k] + sum_sq_tpl[k]*sum_tpl[0]*sum_tpl[0]);

  }
  // Now compute the components of the inverted matrix of the sums for a single template
  fInvertedSums_one.el11 = N;
  fInvertedSums_one.el12 = sum_tpl[0];
  fInvertedSums_one.el22 = sum_sq_tpl[0];
  fInvertedSums_one.determinant = N*sum_sq_tpl[0] + sum_tpl[0]*sum_tpl[0];
 
  // Debugging
  for( int i=0; i<N; ++i){
    if(i%100==0) cout<<"k, sum [k], sum_squares[k], sum_cross[k]"<<endl;
     cout<<i<<", "<<sum_tpl[i]<<", "<<sum_sq_tpl[i]<<", "<<sum_cross_tpl[i]<<endl;
  }
 
}

void MultiHistogramFastFitFCN::SetFitNTemplates(int n_tpls){
  fNTemplatesToFit=n_tpls;
  if(fNTemplatesToFit > 2) {
    cout<<"ERROR: MultiHistogramFastFitFCN::FitNTemplates: Attempting to fit more than 2 templates ("<< n_tpls<<", in fact) which is not implemented!"<<endl;
  }
}
