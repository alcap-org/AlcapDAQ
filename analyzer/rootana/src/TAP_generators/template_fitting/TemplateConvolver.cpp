#include "TemplateConvolver.h"
#include "TTemplate.h"
#include "debug_tools.h"
#include <iostream>
#include <cmath>
#include "InterpolatePulse.h"

using namespace Algorithm;

typedef std::vector<int>::const_iterator IteratorType;
namespace{
  struct ped_subt_it:public IteratorType{
     ped_subt_it(const IteratorType& it, double ped):IteratorType(it),fPedestal(ped){}
     ped_subt_it(const IteratorType& it):IteratorType(it),fPedestal(0){}
     double operator*()const{
       return IteratorType::operator*() - fPedestal;
     }
     double fPedestal;
  };
}

TemplateConvolver::TemplateConvolver(const IDs::channel ch, TTemplate* tpl, double peak_cut):
     fChannel(ch), fTemplate(tpl),fLeftSafety(20),fRightSafety(60),fFoundPeakCut(peak_cut){
   fTemplateLength=fTemplate->GetHisto()->GetNbinsX() - fLeftSafety - fRightSafety;
   if(fTemplateLength <0) return;

   TH1_wrapper hist(fTemplate->GetHisto());
   fEnergyConvolve=new Convolver<TH1_c_iterator>(hist.begin(fLeftSafety), hist.end(fRightSafety));
   
   const int num_weights=2;
   const int weights[num_weights]={-1,1};
   fTimeWeights.assign(weights, weights+num_weights);
   fTimeConvolve=new Convolver<std::vector<int>::iterator>(fTimeWeights.begin(),fTimeWeights.end());
   AutoCorrelateTemplate(fTemplate->GetHisto());
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
   return FindPeaks();
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

int TemplateConvolver::FindPeaks(){
   FoundPeaks tmp;
   bool last_was_positive=fTimeSamples.front() > 0;
   for(SamplesVector::const_iterator i_sample=fTimeSamples.begin()+1; i_sample!=fTimeSamples.end(); ++i_sample){
      if( last_was_positive && *i_sample<0){
        tmp.time=i_sample-fTimeSamples.begin();
        tmp.amplitude=fEnergySamples[tmp.time];
        tmp.second_diff=*i_sample;
        fPeaks.insert(tmp);
      }
      last_was_positive = *i_sample >0;
   }
   return fPeaks.size();
}

void TemplateConvolver::AutoCorrelateTemplate(const TH1* histogram){
   TH1_wrapper hist(histogram);
   SamplesVector acf_out(histogram->GetNbinsX()- fTemplateLength);
   SamplesVector time_out(acf_out.size()- fTimeWeights.size());
  (*fEnergyConvolve)(hist.begin(), hist.end(), acf_out.begin());
  (*fTimeConvolve)(acf_out.begin(), acf_out.end(), time_out.begin());
  functions::VectorToHist(acf_out,"Template_ACF","Auto-correlation of template");
  functions::VectorToHist(time_out,"Template_ACF_derivative","Derivative of auto-correlation of template");
}
