#include "TGeHitTDC.h"

#include <cmath>
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <numeric>

#include "TF1.h"

using std::vector;
using std::string;


ClassImp(TGeHitTDC)

TGeHitTDC::TGeHitTDC() 
{
}


TGeHitTDC::TGeHitTDC(int ch)
{
   Reset();
   channel = ch;

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
   pedestalCorrectionFromPreviousPulse = 0.;
   postLightning = false;
 
}

TGeHitTDC::~TGeHitTDC()
{
  //Reset();
}

void TGeHitTDC::Reset() {
  channel = 0;
  ePulse = NULL;

  secondPulse = false;
  shape = false;
  
  blockEPedestal=0.;
  
  fTime=0.;
}

bool TGeHitTDC::SetEPulse(TPulseIsland* pulse)
{ 
  ePulse = pulse;
  float ph = ePulse->GetPulseHeight();
  if (ph > GetEFixedPedestal() + 20) return true;
  else return false; 
}

float TGeHitTDC::GetEPulseAmpBlockPed()
{
  float ph = ePulse->GetPulseHeight();
  return ph - blockEPedestal;
}

float TGeHitTDC::GetEPulseAmpFixedPed()
{
  float ph = ePulse->GetPulseHeight();
  float ped = ePulse->GetPedestal();//returns the fixed pedestal in the odb
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

int TGeHitTDC::PulseShapeAnalysis()
{
  float ph = GetEPulseHeight();
  float in = GetEPulseIntegral();
  bool good = InAmpVsIntBand( ph , in );
  shape = good;
  return 1;
} 


bool TGeHitTDC::InAmpVsIntBand(float amp, float in)
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

bool TGeHitTDC::GoodHit()
{
  if( !SecondPulse() &&  Shape() && ePulse->GetPulseLength() == pulseLength && !GetPostLightningFlag())  return true;
  else return false;
}

double TGeHitTDC::GetEnergy(double a, double b, bool pedestal_subtract)
{
  double ped = GetEFixedPedestal() + pedestalCorrectionFromPreviousPulse; //correction set by the history of the detector, i.e. previous hit(s)
  double amp = GetEPulseHeight();
  double block_ped = GetBlockEPedestal();
  
  double out = 0.;
  if(pedestal_subtract == false) { out = a*amp+b; }
  else { out = (amp-block_ped)*a; }
  
  return out;  
}

