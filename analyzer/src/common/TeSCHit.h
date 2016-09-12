#ifndef TeSCHit_h
#define TeSCHit_h

#include <vector>
#include <string>

#include <TObject.h>
#include <TH1.h>




class TeSCHit : public TObject {

 public:
  /// This defaultconstructor I believe exists so ROOT can load these from file and is not used explcitly.
  TeSCHit();
  TeSCHit(int ch);
  ~TeSCHit();
  


  //Get Functions*********************
  int GetChannel() {return channel;}
  double GetTime() ;
  
  bool LowAndHi() { return isLow && isHigh; };
  bool LowNotHi() { return isLow && !isHigh; } ;
  bool HiNotLow() { return !isLow && isHigh; } ;
  
   
  
  //Set Functions************************
  void SetChannel(int ch) { channel = ch; }
  void SetLowTime(double value) { lowTime = value; isLow = true; }
  void SetHighTime(double value) { highTime = value; isHigh = true; }

  
  struct TimeSortHits {
    bool operator()(TeSCHit h1, TeSCHit h2) {
      double t1 = h1.GetTime();
      double t2 = h2.GetTime();
      return t1 < t2;
    }
  };


 private:
 
  int channel;
  bool isLow;
  bool isHigh;
  double lowTime;
  double highTime;



  ClassDef(TeSCHit, 1);
};

#endif

/** @}*/
