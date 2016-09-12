#include "TGeHit.h"

#include <cmath>
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <numeric>

#include "TF1.h"

using std::vector;
using std::string;


ClassImp(TGeHit)

TGeHit::TGeHit() 
{
}


TGeHit::TGeHit(int ch)
{
   Reset();
   channel = ch;
   //E vs T shape parameters
   aLow1 = -0.583386745289; //cuts for E Vs T Ge1
   aHigh1 = -0.192671241745;
   bLow1 = 14773.1503653;
   bHigh1 = 14786.2161077;
  
   aLow2 = -0.622377210216;
   aHigh2 = -0.250913914826;
   bLow2 = 14566.5681729;
   bHigh2 = 14937.7028012;
   
   //Amp Vs Int Parameters
   cLow1 = 0.00872939169983;
   cHigh1 = 0.0130721393035;
   dLow1 = 0.0;
   dHigh1 = 1140.0;
  
   cLow2 = 0.00867415730337;
   cHigh2 = 0.0120453857791;
   dLow2 = 0.0;
   dHigh2 = 1554.0;
   
   
   softT1Threshold = 200.;
   softT2Threshold = 400.;
   
   cutLow = 10;
   cutHigh = 10;  
   
   pulseLength = 300; //number of samples
   
 
}

TGeHit::~TGeHit()
{
  //Reset();
}

void TGeHit::Reset() {
  channel = 0;
  ePulse = NULL;
  tPulse = NULL; 
  
  foundTPulse = false;
  matchEVsT = false;
  secondPulse = false;
  shape = false;
  
  blockEPedestal=0.;
  blockTPedestal=0.;
  
  electroncoincidence=false;
  opelectroncoincidence=false;
  
  fTime=0.;
}

bool TGeHit::SetEPulse(TPulseIsland* pulse)
{ 
  ePulse = pulse;
  float ph = ePulse->GetPulseHeight();
  if (ph > GetEFixedPedestal() + 20) return true;
  else return false; 
}

float TGeHit::GetEPulseAmpBlockPed()
{
  float ph = ePulse->GetPulseHeight();
  return ph - blockEPedestal;
}

float TGeHit::GetTPulseAmpBlockPed()
{
  float ph = tPulse->GetPulseHeight();
  return ph - blockTPedestal;
}

float TGeHit::GetEPulseAmpFixedPed()
{
  float ph = ePulse->GetPulseHeight();
  float ped = ePulse->GetPedestal();//returns the fixed pedestal in the odb
  return ph - ped;
}

float TGeHit::GetTPulseAmpFixedPed()
{
  float ph = tPulse->GetPulseHeight();
  float ped = tPulse->GetPedestal();//returns the fixed pedestal in the odb
  return ph - ped;
}

/** Pulse analysis

* Get max sample and max time. (if amp below threshold, se no T pulse flag, and stop)
* Check if T pulse amplitude agrees with E pulse 

yes: * set t and amp
     * look for second pulse
     
no: * look for second pulse

    no: go back to first pulse
        
    yes: check if T pulse amplitude agrees with E pulse
    
      yes: * set t and amp
           * set pile up flag
         
      no
**/
double TGeHit::SearchTPulses()
{
  float tAmp = GetTPulseHeight();
  float eAmp = GetEPulseHeight();
  int eTime = ePulse->GetPeakSample();
  int tTime = tPulse->GetPeakSample();
  double clockTick = tPulse->GetClockTickInNs();
  

  //check amplitude
  if(channel == 1)
  {
    if(GetTFixedPedestal()-softT1Threshold < tAmp)
    {
      foundTPulse=false;
      fTime = eTime*clockTick ;
      return fTime;
    }
  }
  if(channel == 2)
  {
    if(GetTFixedPedestal()-softT2Threshold < tAmp)
    {
      foundTPulse=false;
      fTime = eTime*clockTick ; //Use E pulse to set the time
      return fTime;
    }
  }
  foundTPulse = true; //one pulse found
  
  //check E Vs T Pulse Amplitude
  if(InEVsTBand(eAmp,tAmp))  {  matchEVsT = true;  }
  
  //time to look for a second pulse
  
  //cut out max pulse
  int timeStamp = tPulse->GetTimeStamp();
  int peakSample = tPulse->GetPeakSample();
  std::vector<int> samples = tPulse->GetSamples();
  std::string name = "dummy";  
   
  for(int i = 0; i < samples.size(); i++)
  {
    if( i > peakSample - cutLow && i < peakSample + cutLow)  samples.at(i) = GetTFixedPedestal();
  } 
  
  TPulseIsland pulseWithCut(timeStamp,samples,name);
    
  //search for second maximum
  int secondTime = pulseWithCut.GetPeakSample();
  float secondAmp = pulseWithCut.GetPulseHeight();
  
 
  if(channel == 1)
  {
    if(GetTFixedPedestal()-softT1Threshold < secondAmp) //watch out, this is for negative pulses
    {
      float t = tTime*clockTick ;
      fTime = t; //as there is no second pulse, set 'time' as the time of the first pulse
      return t;
    }
  }
  if(channel == 2)
  {
    if(GetTFixedPedestal() - softT2Threshold < secondAmp)
    {
      float t = tTime*clockTick ;
      fTime = t; //as there is no second pulse, set 'time' as the time of the first pulse
      return t;
    }
  }
  
  secondPulse = true; //at this point, two maxima above threshold are found
  
  if(InEVsTBand(eAmp,secondAmp))
  {
    //second pulse is found and it is a 'nice one. However, if the first one was also a 'nice one' take the time of the first one
    if( matchEVsT )
    {
      float t = tTime*clockTick ;
      fTime = t;
    }
    else
    {
      matchEVsT = true;
      float t = tPulse->GetClockTickInNs()*secondTime;
      fTime = t;
    }
  }
  else //second pulse is not a 'nice' one, largest pulse gives the time
  {
    float t = tTime*clockTick ;
    fTime = t;
  }
  
  return fTime;
 
  
}

int TGeHit::PulseShapeAnalysis()
{
  float ph = GetEPulseHeight();
  float in = GetEPulseIntegral();
  bool good = InAmpVsIntBand( ph , in );
  shape = good;
  return 1;
} 

bool TGeHit::InEVsTBand(float eAmp, float tAmp)
{
  float low,high;
  
  //std::cout << " channel " << channel << std::endl;
  
  if(channel == 1)
  {
    low = aLow1*eAmp + bLow1;
    high = aHigh1*eAmp + bHigh1;
      //std::cout << " tAmp: " << tAmp << " eAmp: " << eAmp << " high: " << high << " low: " << low << std::endl;
  }
  if(channel == 2)
  {
    low = aLow2*eAmp*1.0 + bLow2;
    high = aHigh2*eAmp*1.0 + bHigh2;
  }
 
  if(tAmp*1.0 < high && tAmp*1.0 > low) return true;
  else return false;  
}


bool TGeHit::InAmpVsIntBand(float amp, float in)
{
  float low,high;
  if(channel == 1)
  {
    low = cLow1*in + dLow1;
    high = cHigh1*in + dHigh1;
  }
  if(channel == 2)
  {
    low = cLow2*in + dLow2;
    high = cHigh2*in + dHigh2;
  }
  if(amp*1.0 < high && amp*1.0 > low) return true;
  else return false; 
}

bool TGeHit::GoodHit()
{
  if( FoundTPulse() && MatchEVsT() && !SecondPulse() &&  Shape() && ePulse->GetPulseLength() == pulseLength)  return true;
  else return false;
}

double TGeHit::GetEnergy(double a, double b, bool pedestal_subtract)
{
  double ped = GetEFixedPedestal();
  double amp = GetEPulseHeight();
  double block_ped = GetBlockEPedestal();
  
  double out = 0.;
  if(pedestal_subtract == false) { out = a*amp+b; }
  else { out = (amp-block_ped)*a; }
  
  return out;  
}

