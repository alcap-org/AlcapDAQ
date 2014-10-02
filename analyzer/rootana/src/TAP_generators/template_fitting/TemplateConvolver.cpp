#include "TemplateConvolver.h"
#include "TTemplate.h"
#include "TDirectory.h"
#include "debug_tools.h"
#include <iostream>
#include <cmath>
#include "InterpolatePulse.h"
#include <TF1.h>
#include "EventNavigator.h"

using namespace Algorithm;

TemplateConvolver::TemplateConvolver(const IDs::channel ch, TTemplate* tpl, int peak_fit_samples, int left, int right):
     fChannel(ch), fTemplate(tpl),fQuadFit(peak_fit_samples), fPolarity(EventNavigator::GetSetupRecord().GetPolarity(ch)),
     fLeftSafety(left),fRightSafety(right), 
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

   return 0;
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

int TemplateConvolver::FindPeaks(bool expect_pile_up, const Algorithm::TpiMinusPedestal_iterator& waveform){
   if(expect_pile_up) return FindAllPeaks(fEnergySamples, fTimeSamples,&waveform);
   return FindBestPeak(fEnergySamples, fTimeSamples,&waveform);
}

int TemplateConvolver::FindAllPeaks(const SamplesVector& energy,
      const SamplesVector& time, const Algorithm::TpiMinusPedestal_iterator* pedestal){
   double last_sample=time.front();
   FoundPeaks tmp=FoundPeaks();
   for(SamplesVector::const_iterator i_sample=time.begin()+1; i_sample!=time.end(); ++i_sample){
// check if we've crossed zero gradient ( signs will be different => product is less than zero)
// and check we crossed in the right direction 
      if( (last_sample * *i_sample)<0 && (*i_sample - last_sample ) <0 ){
        int peak=i_sample - time.begin();
        if(pedestal) FitPeak(tmp, peak, energy, time,pedestal->GetPedestal());
        else FitPeak(tmp, peak, energy, time, 0);
        fPeaks.insert(tmp);
      }
      last_sample = *i_sample;
   }

   return fPeaks.size();
}

int TemplateConvolver::FindBestPeak(const SamplesVector& energy,
      const SamplesVector& time, const Algorithm::TpiMinusPedestal_iterator* pedestal){
   double last_sample=time.front();
   double best_amp=-1e9;
   FoundPeaks tmp=FoundPeaks();
   FoundPeaks best=FoundPeaks();
   int peak_count=0;
   for(SamplesVector::const_iterator i_sample=time.begin()+1; i_sample!=time.end(); ++i_sample){
      if( last_sample>0 && *i_sample<0){
        int peak=i_sample - time.begin();
        if(pedestal) FitPeak(tmp, peak, energy, time,pedestal->GetPedestal());
        else FitPeak(tmp, peak, energy, time, 0);
        if( (tmp.amplitude > best_amp) ){
            best=tmp;
	    best_amp=tmp.amplitude;
        }
        ++peak_count;
      }
      last_sample = *i_sample;
   }
   fPeaks.insert(best);
   return peak_count;
}


void TemplateConvolver::FitPeak(FoundPeaks& output, int index, const SamplesVector& energy, const SamplesVector& time, double pedestal){
   // about where do need to fit?
   int peak=(fQuadFit.GetSize()-1)/2;
   // if the index we've been asked to fit to is greater than the minimum needed samples then use that
   if( index > peak) peak=index-peak;
   // run the fit calculation
   fQuadFit.Fit(energy.begin() + peak, output.quad, output.linear, output.constant);

   output.time= index + output.linear/2/output.quad;
   output.pedestal=pedestal;
   output.amplitude=output.constant - output.linear*output.linear/4/output.quad;
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
   FindBestPeak(fTemplateACF, fTemplateACFDerivative, NULL);

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
