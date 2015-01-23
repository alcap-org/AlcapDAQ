#include "MultiHistogramFastFitFCN.h"

#include <cmath>
#include <iostream>
#include <cassert>

using std::cout;
using std::endl;

MultiHistogramFastFitFCN::MultiHistogramFastFitFCN(double refine_factor):
  fRefineFactor(refine_factor),fTemplates(2),hSum(0),hSumSq(0),hSumCross(0){
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

	//DEBUG_VALUE(par.size(),par.front(),par.back());

  switch(fNTemplatesToFit){
    case 1:
       fChi2= FitOne(par.at(0));
    break;
    case 2:
	    DEBUG_PRINT("Fitting Two");
       fChi2= FitTwo(par.at(0),par.at(1));
    break;
    DEBUG_PRINT("Shouldn't be here!!");
    default: fChi2= -1;
  }

  return fChi2;
}

double MultiHistogramFastFitFCN::FitOne(double time_offset)const{
  // check for NaNs being fed in
  if(time_offset != time_offset) return DBL_MAX;
  // Check the bounds to use
  int N = fPulseHist->GetNbinsX() - time_offset;
  const int tpl_N=fTemplateHist->GetNbinsX();
  if( N > tpl_N ) N = tpl_N;
  const int k=tpl_N - N; // Get the value of k
  //DEBUG_VALUE(N,tpl_N,k);

  // Find the sums over the pulse being fit to
  double Y=0, Y_cross=0, Y_sq=0;
  for(int i=0; i<N;i+=fRefineFactor){
     int bin=i*1./fRefineFactor+time_offset;
     double y=fPulseHist->GetBinContent(bin);
     Y +=y;
     Y_sq +=y*y;
     Y_cross += y*fTemplateHist->GetBinContent(i);
  }

  // Calculate the amplitude scale factor
  double a = fInvertedSums[k].one_11 * Y_cross - fInvertedSums[k].one_12 * Y;
  a /= fInvertedSums[k].one_determinant;
  fTemplates.at(0).fAmplitudeScale=a;

  // Calculate the pedestal
  fPedestal = fInvertedSums[k].one_22 * Y - fInvertedSums[k].one_12 * Y_cross;
  fPedestal /= fInvertedSums[k].one_determinant;

  // Calculate the chi square
  double chi_2 = a*a*sum_sq_tpl[k] + (N-k) *fPedestal*fPedestal
               + Y_sq - 2*a*Y_cross - 2*fPedestal*Y + 2*a*fPedestal*sum_tpl[k];

  // copy over fit parameters
  fFitParameters.determinant=fInvertedSums[k].one_determinant;
  fFitParameters.n=N;
  fFitParameters.sum_y=Y;
  fFitParameters.sum_y_T=Y_cross;
  fFitParameters.sum_sq_T=sum_sq_tpl[k];
  fFitParameters.sum_T=sum_tpl[k];
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
  fInvertedSums.resize(N/fRefineFactor);

  hTpl=new TH1F("hTpl","Input template",N,0,N-1);
  hTpl->SetDirectory(0);
  hSum=new TH1F("hSum","Sum over template",N,0,N-1);
  hSum->SetDirectory(0);
  hSumSq=new TH1F("hSumSq","Sum over squares of template",N,0,N-1);
  hSumSq->SetDirectory(0);
  hSumCross=new TH1F("hSumCross","Autocorrelation of template",N,0,N-1);
  hSumCross->SetDirectory(0);
 
  // Make sure the last sum is 0
  sum_tpl[N] = sum_sq_tpl[N] = sum_cross_tpl[N] =0;
 
  double T_i, T_i_minus_k;
 
  // Step over k in reverse
  for( int k=N-1; k>=0; k-=fRefineFactor){
     T_i=fTemplateHist->GetBinContent(N-k+1); // +1 since bin 0 is the underflow bin in a TH1
     hTpl->SetBinContent(N-k+1,T_i);
     sum_tpl[k]=sum_tpl[k+1] + T_i;
     sum_sq_tpl[k]=sum_sq_tpl[k+1] + T_i*T_i;
     // Now calculate the cross terms for this separation
     // Since i < 0, T_i is 0 we can start at i=k
     for( int i=k; i<N; i+=fRefineFactor){
        T_i=fTemplateHist->GetBinContent(i+1);           // +1 since bin 0 is the underflow bin in a TH1
        T_i_minus_k=fTemplateHist->GetBinContent(i-k+1); // +1 since bin 0 is the underflow bin in a TH1
        sum_cross_tpl[k]+=T_i * T_i_minus_k;
     }
     hSum->SetBinContent(k,sum_tpl[k]);
     hSumSq->SetBinContent(k,sum_sq_tpl[k]);
     hSumCross->SetBinContent(k,sum_cross_tpl[k]);
     
     // Now calculate the cross terms for this separation
     // Now compute the components of the inverted matrix of the sums for two templates
     fInvertedSums[k].two_11 = N*sum_sq_tpl[k] - sum_tpl[k]*sum_tpl[k];
     fInvertedSums[k].two_12 = sum_tpl[k]*sum_tpl[k] - N*sum_cross_tpl[k];
     fInvertedSums[k].two_13 = sum_cross_tpl[k] * sum_tpl[k] - sum_tpl[0] * sum_sq_tpl[k];
     fInvertedSums[k].two_22 = N*sum_sq_tpl[0] - sum_tpl[0]*sum_tpl[0];
     fInvertedSums[k].two_23 = sum_cross_tpl[k] * sum_tpl[0] - sum_sq_tpl[0]*sum_tpl[k];
     fInvertedSums[k].two_33 = sum_sq_tpl[0] * sum_sq_tpl[k] - sum_cross_tpl[k]*sum_cross_tpl[k];
     fInvertedSums[k].two_determinant = N*(sum_sq_tpl[0]*sum_sq_tpl[k]         + sum_cross_tpl[k]*sum_cross_tpl[k]  )
                                        - (sum_sq_tpl[0]*sum_tpl[k]*sum_tpl[k] + sum_sq_tpl[k]*sum_tpl[0]*sum_tpl[0]);

     // Now compute the components of the inverted matrix of the sums for a single template
     fInvertedSums[k].one_11 = N-k;
     fInvertedSums[k].one_12 = sum_tpl[k];
     fInvertedSums[k].one_22 = sum_sq_tpl[k];
     fInvertedSums[k].one_determinant = (N-k)*sum_sq_tpl[k] - sum_tpl[k]*sum_tpl[k];
  }
 
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
