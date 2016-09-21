#ifndef TMuonHit_h
#define TMuonHit_h

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
/// The %TMuonHit (TPI) class to hold information relevant to a MuSC(A)Hit, combining muSCLo, Hi and A. The seed is a muSCLo signal
////////////////////////////////////////////////////////////////////////////////

class TMuonHit : public TObject {

 public:
  
  TMuonHit();
  TMuonHit(int value);
  ~TMuonHit();
  
  //Get Functions*********************
  bool LowNotHi() { return lowNotHi; }
  bool MuSCHiPileUp() { return muSCHiPileUp; } 
  bool MuSCLoPileUp() { return muSCLoPileUp; }
  bool MuSCAPileUp() { return muSCAPileUp; }
  bool MuSCHiPileUpWide() { return muSCHiPileUpWide; } 
  bool MuSCLoPileUpWide() { return muSCLoPileUpWide; }
  bool MuSCAPileUpWide() { return muSCAPileUpWide; }
  bool BookEnd() { return bookEnd; }
  bool HasElectron() { return electron; }
  bool HasElectronCoincidence(); 
  bool HasDelayedElectron(double lowT = 20., double highT = 600.);
    
  double GetTime() { return time; }
  double GetElectronDelayTime() { return electron_delay_time; }
  int GetElectronDetectorID() { return electronDetectorID; }
  
  //Set Functions************************
  void SetLowNotHi(bool value){ lowNotHi = value; } 
  void SetMuSCHiPileUp(bool value) { muSCHiPileUp = value; } 
  void SetMuSCLoPileUp(bool value) { muSCLoPileUp = value; }
  void SetMuSCAPileUp(bool value) { muSCAPileUp = value; }
  void SetMuSCHiPileUpWide(bool value) { muSCHiPileUpWide = value; } 
  void SetMuSCLoPileUpWide(bool value) { muSCLoPileUpWide = value; }
  void SetMuSCAPileUpWide(bool value) { muSCAPileUpWide = value; }
  void SetBookEnd(bool value) { bookEnd = value; }
  void SetElectron(bool value) { electron = value; }
  void SetTime(double value){ time = value; }
  void SetElectronDelayTime(double value) ;
  void SetElectronDetectorID(int value) { electronDetectorID = value; }
  
  //PP/Quality info
  bool IsMuon() { return !lowNotHi; }
  bool IsPPMuon();
  bool IsPPMuonWide();
  bool IsPPMuonWithLo();
  
  struct TimeSortHits {
    bool operator()(TMuonHit h1, TMuonHit h2) {
      double t1 = h1.GetTime();
      double t2 = h2.GetTime();
      return t1 < t2;
    }
  };

  
 private:
 
  //flags
  bool lowNotHi;
  bool muSCHiPileUp;
  bool muSCLoPileUp;
  bool muSCAPileUp;
  bool muSCHiPileUpWide;
  bool muSCLoPileUpWide;
  bool muSCAPileUpWide;
  bool bookEnd;
  bool electron;
  int electronDetectorID;
  
  //times and time differences
  double time;
  int id;
  double electron_delay_time;

  public:

  /// Copying is made explicitly private since we do not need it yet.
  //TMuonHit(const TMuonHit& src);
  /// Assignment is made explicitly private since we do not need it yet.
  //TMuonHit operator=(const TMuonHit& rhs); 
  
  void Reset();


  ClassDef(TMuonHit, 2);
};

#endif

/** @}*/
