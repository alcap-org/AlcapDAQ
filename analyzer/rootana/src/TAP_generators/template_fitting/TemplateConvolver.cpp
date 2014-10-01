#include "TemplateConvolver.h"
#include "TTemplate.h"
#include "TDirectory.h"
#include "debug_tools.h"
#include <iostream>
#include <cmath>
#include "InterpolatePulse.h"
#include <TF1.h>

using namespace Algorithm;

TemplateConvolver::TemplateConvolver(const IDs::channel ch, TTemplate* tpl, int peak_fit_samples):
     fChannel(ch), fTemplate(tpl),fQuadFit(peak_fit_samples),fLeftSafety(20),fRightSafety(120), 
     fTemplateLength(fTemplate->GetHisto()->GetNbinsX() - fLeftSafety - fRightSafety),
     fTemplateTime(fTemplate->GetTime()-fLeftSafety){
   if(fTemplateLength <0) return;

   TH1_wrapper hist(fTemplate->GetHisto());
   fEnergyConvolve=new Convolver<TH1_c_iterator>(hist.begin(fLeftSafety), hist.end(fRightSafety));
   
   const int num_weights=2;
   const int weights[num_weights]={-1,1};
   fTimeWeights.assign(weights, weights+num_weights);
   fTimeConvolve=new Convolver<std::vector<int>::iterator>(fTimeWeights.begin(),fTimeWeights.end());
}

TemplateConvolver::~TemplateConvolver(){
  delete fTimeConvolve;
  delete fEnergyConvolve;
}

int TemplateConvolver::Convolve(const Algorithm::TpiMinusPedestal_iterator& begin, const Algorithm::TpiMinusPedestal_iterator& end){
   // initialize for convolution
   if(!ResetVectors(end - begin)){
     return -1;
   }

   // convole the waveform with the template
   fEnergyConvolve->Process(begin,end, fEnergySamples.begin());

   // now run the timing filter (in the future this and the last step could be merged)
   fTimeConvolve->Process(fEnergySamples.begin(),fEnergySamples.end(),fTimeSamples.begin());

   // finaly get all peaks and return the total number found
   return FindPeaks(fEnergySamples, fTimeSamples,&begin);
}

bool TemplateConvolver::ResetVectors(int size){
   // make sure we're big enough for the number of samples we'll get
   size-=fTemplateLength;
   if( size < 1 ) return false;
   fEnergySamples.resize(size);
   fTimeSamples.resize(size - fTimeWeights.size() );
   fPeaks.clear();
   return true;
}

int TemplateConvolver::FindPeaks(const SamplesVector& energy,
      const SamplesVector& time, const Algorithm::TpiMinusPedestal_iterator* pedestal){
   double last_sample=time.front();
   for(SamplesVector::const_iterator i_sample=time.begin()+1; i_sample!=time.end(); ++i_sample){
      if( last_sample>0 && *i_sample<0){
        int peak=i_sample - time.begin();
        if(pedestal) FitPeak(peak, energy, time,0);
        else FitPeak(peak, energy, time, 0);
      }
      last_sample = *i_sample;
   }

   return fPeaks.size();
}

void TemplateConvolver::FitPeak(int index, const SamplesVector& energy, const SamplesVector& time, double pedestal){
   FoundPeaks tmp=FoundPeaks();
   int peak=(fQuadFit.GetSize()-1)/2;
   if( peak <index) peak=index;
   fQuadFit.Fit(energy.begin() + peak, tmp.quad, tmp.linear, tmp.constant);

   tmp.time=tmp.linear/2/tmp.quad + index;
   tmp.pedestal=pedestal;
   tmp.amplitude=tmp.constant - tmp.linear*tmp.linear/4/tmp.quad;
   fPeaks.insert(tmp);
}

void TemplateConvolver::CharacteriseTemplate(){
   fPeaks.clear();

   // prepare vectors / iterators
   TH1_wrapper hist(fTemplate->GetHisto());
   fTemplateACF.assign(fTemplate->GetHisto()->GetNbinsX()- fTemplateLength,0);
   fTemplateACFDerivative.assign(fTemplateACF.size()- fTimeWeights.size(),0);

   // Run convolution
   fEnergyConvolve->Process(hist.begin(), hist.end(), fTemplateACF.begin());
   fTimeConvolve->Process(fTemplateACF.begin(), fTemplateACF.end(), fTemplateACFDerivative.begin());
   FindPeaks(fTemplateACF, fTemplateACFDerivative, NULL);

   // Prepare the quadratic coefficients of the template itself
   fTemplateQuad=fPeaks.begin()->quad;
   fTemplateLin=fPeaks.begin()->linear;
   fTemplateConst=fPeaks.begin()->constant;
   fTemplateScale=fTemplate->GetAmplitude();
   fTemplateScale/=fPeaks.begin()->constant - fPeaks.begin()->linear*fPeaks.begin()->linear/4/fPeaks.begin()->quad;

   // Convert to histograms
   fTemplateACFHist=functions::VectorToHist(fTemplateACF,"Template_ACF","Auto-correlation of template");
   TF1* fit=new TF1("Fit","[0]*(x-[3])**2+[1]*(x-[3])+[2]", 0 , fTemplateACFHist->GetNbinsX());
   fit->SetParameter(0,fTemplateQuad);
   fit->SetParameter(1,fTemplateLin);
   fit->SetParameter(2,fTemplateConst);
   fit->SetParameter(3,fPeaks.begin()->time + fTemplateTime);
   fTemplateACFHist->GetListOfFunctions()->Add(fit);
   fTemplateACFHist->Clone()->Write();
   fTemplateACFDerivativeHist=functions::VectorToHist(fTemplateACFDerivative,"Template_ACF_derivative","Derivative of auto-correlation of template");
   fTemplateACFDerivativeHist->Clone()->Write();

   fPeaks.clear();

}
