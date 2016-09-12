#ifndef TGeHit_h
#define TGeHit_h

#include <vector>
#include <string>

#include <TObject.h>
#include <TH1.h>

#include "TPulseIsland.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief
/// A uniform format for data from many different detectors.
/// \ingroup alcapana
/// \ingroup rootana
///
/// \details
/// The %TGeHit (TPI) class to hold information relevant to a HPGeHit, combining slow and fast and the electron scintillator. The seed is a hit in the Energy signal
////////////////////////////////////////////////////////////////////////////////

class TGeHit : public TObject {

 public:
  /// This defaultconstructor I believe exists so ROOT can load these from file and is not used explcitly.
  TGeHit();
  TGeHit(int ch);
  ~TGeHit();
  
  /// The constructor we should use.
  //TGeHit();

  //Get Functions*********************
  int GetChannel() {return channel;}
  
  //PulseBasedMethods
  float GetEPulseHeight() { return ePulse->GetPulseHeight(); }
  float GetEPulseIntegral() { return ePulse->GetPulseIntegral(); }
  float GetTPulseHeight() { return tPulse->GetPulseHeight(); } 
  float GetEFixedPedestal() { return ePulse->GetPedestal(); }  
  float GetTFixedPedestal() { return tPulse->GetPedestal(); }
  
  float GetEPulseAmp() { return GetEPulseAmpFixedPed(); }
  float GetTPulseAmp() { return GetEPulseAmpFixedPed(); }
  
  float GetEPulseAmpBlockPed() ;
  float GetEPulseAmpFixedPed();
  
  float GetTPulseAmpBlockPed();
  float GetTPulseAmpFixedPed();
  
  double GetEnergy(double a, double b,bool pedestal_subtract=false);// should become energy calibrated pulse height
  float GetBlockEPedestal() { return blockEPedestal; }
  float GetBlockTPedestal() { return blockTPedestal; }
  
  TPulseIsland* GetEPulse(){ return ePulse; }
  TPulseIsland* GetTPulse(){ return tPulse; }
  
  double GetTime() { return fTime + tPulse->GetClockTickInNs()*tPulse->GetTimeStamp(); }
  
  bool FoundTPulse() { return foundTPulse; }
  bool MatchEVsT() { return matchEVsT; }
  bool SecondPulse() { return secondPulse; }
  bool Shape(){ return shape; }
  bool GetElectronCoincidence() { return electroncoincidence; } 
  bool GetOppositeElectronCoincidence() { return opelectroncoincidence; } 
  
  bool GoodHit();
  
  
  //Set Functions************************
  void SetChannel(int ch) { channel = ch; }
  bool SetEPulse(TPulseIsland* pulse); //returns true if E pulse if above threshold
  void SetTPulse(TPulseIsland* pulse) { tPulse = pulse; }
  void SetEPedestal(float value) { blockEPedestal = value; }
  void SetTPedestal(float value) { blockTPedestal = value; }
  void SetTime(double value) { fTime = value;}
  void SetElectronCoincidence(bool value) { electroncoincidence=value; }
  void SetOppositeElectronCoincidence(bool value) { opelectroncoincidence=value;}
  
  //Pulse analysis ****
  double SearchTPulses();
  int PulseShapeAnalysis();
  bool InEVsTBand(float eAmp, float tAmp);
  bool InAmpVsIntBand(float amp, float in);
  
  struct TimeSortHits {
    bool operator()(TGeHit h1, TGeHit h2) {
      double t1 = h1.GetTime();
      double t2 = h2.GetTime();
      return t1 < t2;
    }
  };


 private:
 
  TPulseIsland* ePulse; //TPulseIsland contains the whole island
  TPulseIsland* tPulse;
  int channel;
  
  float blockEPedestal;
  float blockTPedestal;
  
  double fTime; //time of the pulse within the island in ns
  
  //Hit quality bits
  bool foundTPulse;
  bool matchEVsT;
  bool secondPulse;
  bool shape;
  bool electroncoincidence;
  bool opelectroncoincidence;
  
  //Pulse shape parameters (yes, would be better to store it in TSetupData
  //for E vs T
  float aLow1;
  float aHigh1;
  float bLow1;
  float bHigh1;
  
  float aLow2;
  float aHigh2;
  float bLow2;
  float bHigh2;
  
  //intergal versus 
  float cLow1;
  float cHigh1;
  float dLow1;
  float dHigh1;
  
  float cLow2;
  float cHigh2;
  float dLow2;
  float dHigh2;
  
  float softT1Threshold;
  float softT2Threshold;
  
  int cutLow; //parameters to cut out the original pulse and look for a second on the Island.
  int cutHigh;
  //float energy; // in ADC channels
  //double slowTime; // in ns
  //double fastTime; // in ns
  
  int pulseLength;
  
  private:
  /// Copying is made explicitly private since we do not need it yet.
  //TGeHit(const TGeHit& src);
  /// Assignment is made explicitly private since we do not need it yet.
  //TGeHit operator=(const TGeHit& rhs); 
  
  void Reset();


  ClassDef(TGeHit, 3);
};

#endif

/** @}*/
