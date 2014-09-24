#include "TemplateConvolver.h"
#include "TTemplate.h"
#include "TDirectory.h"
#include "debug_tools.h"
#include <iostream>
#include <cmath>
#include "InterpolatePulse.h"
#include <TF1.h>

using namespace Algorithm;

namespace{
  typedef std::vector<int>::const_iterator IteratorType;
  struct ped_subt_it:public IteratorType{
     ped_subt_it(const IteratorType& it, double ped):IteratorType(it),fPedestal(ped){}
     ped_subt_it(const IteratorType& it):IteratorType(it),fPedestal(0){}
     double operator*()const{
       return IteratorType::operator*() - fPedestal;
     }
     double fPedestal;
  };
}

TemplateConvolver::TemplateConvolver(const IDs::channel ch, TTemplate* tpl, int peak_fit_samples):
     fChannel(ch), fTemplate(tpl),fQuadFit(peak_fit_samples),fLeftSafety(20),fRightSafety(60), 
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

int TemplateConvolver::Convolve(const TPulseIsland* tpi, double ped){
   // initialize for convolution
   const std::vector<int>& samples=tpi->GetSamples();
   if(!ResetVectors(samples.size())){
     return -1;
   }

   // convole the waveform with the template
   (*fEnergyConvolve)(ped_subt_it(samples.begin(),ped),ped_subt_it(samples.end(),ped),fEnergySamples.begin());

   // now run the timing filter (in the future this and the last step could be merged)
   (*fTimeConvolve)(fEnergySamples.begin(),fEnergySamples.end(),fTimeSamples.begin());

   // finaly get all peaks and return the total number found
   return FindPeaks(fEnergySamples, fTimeSamples);
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

int TemplateConvolver::FindPeaks(const SamplesVector& energy, const SamplesVector& time){
   double last_sample=time.front();
   for(SamplesVector::const_iterator i_sample=time.begin()+1; i_sample!=time.end(); ++i_sample){
      if( last_sample>0 && *i_sample<0){
        FitPeak(i_sample - time.begin(), energy, time);
      }
      last_sample = *i_sample;
   }

   return fPeaks.size();
}

void TemplateConvolver::FitPeak(int index, const SamplesVector& energy, const SamplesVector& time){
   FoundPeaks tmp=FoundPeaks();
   int peak=(fQuadFit.GetSize()-1)/2;
   if( peak <index) peak=index;
   fQuadFit.Fit(energy.begin() + peak, tmp.quad, tmp.linear, tmp.constant);

   tmp.time=tmp.linear/2/tmp.quad + index;
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
   (*fEnergyConvolve)(hist.begin(), hist.end(), fTemplateACF.begin());
   (*fTimeConvolve)(fTemplateACF.begin(), fTemplateACF.end(), fTemplateACFDerivative.begin());
   FindPeaks(fTemplateACF, fTemplateACFDerivative);

   // Prepare the quadratic coefficients of the template itself
   fTemplateQuad=fPeaks.begin()->quad;
   fTemplateLin=fPeaks.begin()->linear;
   fTemplateConst=fPeaks.begin()->constant;
   fTemplateScale=fTemplate->GetAmplitude();
   fTemplateScale/=fPeaks.begin()->constant - fPeaks.begin()->linear*fPeaks.begin()->linear/4/fPeaks.begin()->quad;

   // Convert to histograms
   TH1* tmp=functions::VectorToHist(fTemplateACF,"Template_ACF","Auto-correlation of template");
   TF1* fit=new TF1("Fit","[0]*(x-[3])**2+[1]*(x-[3])+[2]", 0 , tmp->GetNbinsX());
   fit->SetParameter(0,fTemplateQuad);
   fit->SetParameter(1,fTemplateLin);
   fit->SetParameter(2,fTemplateConst);
   fit->SetParameter(3,fPeaks.begin()->time - fTemplateTime );
   tmp->GetListOfFunctions()->Add(fit);
   tmp->Write();
   tmp=functions::VectorToHist(fTemplateACFDerivative,"Template_ACF_derivative","Derivative of auto-correlation of template");
   tmp->Write();

   fPeaks.clear();

}
