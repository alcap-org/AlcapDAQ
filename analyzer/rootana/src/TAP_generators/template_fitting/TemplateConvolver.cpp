#include "TemplateConvolver.h"
#include "TTemplate.h"
#include "debug_tools.h"
#include <iostream>
#include <cmath>

using namespace Algorithm;

TemplateConvolver::TemplateConvolver(const IDs::channel ch, TTemplate* tpl, double peak_cut):
     fChannel(ch), fTemplate(tpl),fLeftSafety(10),fRightSafety(30),fFoundPeakCut(peak_cut){
   fTemplateLength=fTemplate->GetHisto()->GetNbinsX() - fLeftSafety - fRightSafety;
   if(fTemplateLength <0) return;

   TH1_wrapper hist(fTemplate->GetHisto());
   fEnergyConvolve=new Convolver<TH1_c_iterator>(hist.begin(fLeftSafety), hist.end(fRightSafety));
   
   const int num_weights=2;
   const int weights[num_weights]={-1,1};
   fTimeConvolve=new Convolver<const int*>(weights,weights+num_weights);
}

TemplateConvolver::~TemplateConvolver(){
  delete fTimeConvolve;
  delete fEnergyConvolve;
}

int TemplateConvolver::Convolve(const TPulseIsland* tpi){
   // initialize for convolution
   const std::vector<int>& samples=tpi->GetSamples();
   if(!ResetVectors(samples.size())){
     return -1;
   }

   // convole the waveform with the template
   /*SamplesVector::iterator last=*/(*fEnergyConvolve)(samples.begin(),samples.end(),fEnergySamples.begin());

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
   fTimeSamples.resize(size -2 );
   return true;
}

int TemplateConvolver::FindPeaks(){
   FoundPeaks tmp;
   for(SamplesVector::const_iterator i_sample=fTimeSamples.begin(); i_sample!=fTimeSamples.end(); ++i_sample){
      if( fabs(*i_sample-fFoundPeakCut) <fFoundPeakCut){
        tmp.time=i_sample-fTimeSamples.begin();
        tmp.amplitude=fEnergySamples[tmp.time];
        tmp.second_diff=*i_sample;
        fPeaks.insert(tmp);
      }
   }
   return fPeaks.size();
}
