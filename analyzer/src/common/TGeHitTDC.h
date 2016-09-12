#ifndef TGeHitTDC_h
#define TGeHitTDC_h

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

class TGeHitTDC : public TObject {

 public:
  /// This defaultconstructor I believe exists so ROOT can load these from file and is not used explcitly.
  TGeHitTDC();
  TGeHitTDC(int ch);
  ~TGeHitTDC();
  
  /// The constructor we should use.
  //TGeHit();

  //Get Functions*********************
  int GetChannel() {return channel;}
  
  //PulseBasedMethods
  float GetEPulseHeight() { return ePulse->GetPulseHeight(); }
  float GetEPulseIntegral() { return ePulse->GetPulseIntegral(); }
  float GetEFixedPedestal() { return ePulse->GetPedestal(); }  
  
  float GetEPulseAmp() { return GetEPulseAmpFixedPed(); }
  float GetTPulseAmp() { return GetEPulseAmpFixedPed(); }
  
  float GetEPulseAmpBlockPed() ;
  float GetEPulseAmpFixedPed();
  
  double GetEnergy(double a, double b,bool noBLR = false, bool pedestal_subtract=false);// should become energy calibrated pulse height
  float GetBlockEPedestal() { return blockEPedestal; }
  float GetPedestalCorrection(){return pedestalCorrectionFromPreviousPulse ; }
  bool GetPostLightningFlag(){ return postLightning;}
  bool GetElectronCoincidence() { return electroncoincidence; } 
  bool GetOppositeElectronCoincidence() { return opelectroncoincidence; } 

  TPulseIsland* GetEPulse(){ return ePulse; }
  
  double GetTime() { return fTime; }
  
  bool SecondPulse() { return secondPulse; }
  bool Shape(){ return shape; }
  
  bool GoodHit();
  
  
  //Set Functions************************
  void SetChannel(int ch) { channel = ch; }
  bool SetEPulse(TPulseIsland* pulse); //returns true if E pulse if above threshold
  void SetEPedestal(float value) { blockEPedestal = value; }
  void SetTime(double value) { fTime = value;}
  void SetSecondPulse(bool value) {secondPulse = value; }
  
  void SetPedestalCorrection(float value){ pedestalCorrectionFromPreviousPulse = value;}
  void SetPostLightningFlag(bool value){ postLightning = value;}
  void SetElectronCoincidence(bool value) { electroncoincidence=value;}
  void SetOppositeElectronCoincidence(bool value) { opelectroncoincidence=value;}
  
  //Pulse analysis ****
  int PulseShapeAnalysis();
  bool InAmpVsIntBand(float amp, float in);
  
  struct TimeSortHits {
    bool operator()(TGeHitTDC h1, TGeHitTDC h2) {
      double t1 = h1.GetTime();
      double t2 = h2.GetTime();
      return t1 < t2;
    }
  };


 private:
 
  TPulseIsland* ePulse; //TPulseIsland contains the whole island
  int channel;
  
  float blockEPedestal;
  float pedestalCorrectionFromPreviousPulse;
  
  double fTime; //time of the pulse within the island in ns
  
  //Hit quality bits
  bool secondPulse;
  bool shape;
  bool postLightning;
  bool electroncoincidence;
  bool opelectroncoincidence;
  
  //Pulse shape parameters (yes, would be better to store it in TSetupData
  
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
  void Reset();


  ClassDef(TGeHitTDC, 2);
};

#endif

/** @}*/
