#ifndef TNaIHit_h
#define TNaIHit_h

#include "TObject.h"

class TOctalFADCIsland;

/** This class is a wrapper for the two NaI channels (timing, egy)
  */
class TNaIHit  : public TObject{
  public:
  TNaIHit();
  TNaIHit(const TOctalFADCIsland& timePulse, const TOctalFADCIsland& energyPulse, int count, int nAv);
  TNaIHit(const TOctalFADCIsland& timePulse, const TOctalFADCIsland& energyPulse, int count);
  
  double GetTime() const { return GetTimingTime(); }
  double GetEnergy() const { return GetEnergyAmp(); }
  int GetDetector() const { return detector;}

  void SetFromTimingPulse(const TOctalFADCIsland& timePulse);
  void SetFromEnergyPulse(const TOctalFADCIsland& energyPulse);

  const TOctalFADCIsland* GetTimingIsland() const { return fTimePulse; }
  const TOctalFADCIsland* GetEnergyIsland() const { return fEnergyPulse; }

  void SetTimingIsland(const TOctalFADCIsland* island) { fTimePulse = island;}
  void SetEnergyIsland(const TOctalFADCIsland* island) { fEnergyPulse = island;}

  double GetTimingTime() const { return fTimingTime; }
  double GetTimingAmp() const { return  fTimingAmp; }
  double GetEnergyTime() const { return fEnergyTime; }
  double GetEnergyAmp() const { return  fEnergyAmp; }

  void SetTimingTime(double time) { fTimingTime = time;}
  void SetEnergyTime(double time) { fEnergyTime = time; }
  void SetTimingAmp(double amp) { fTimingAmp = amp;}
  void SetEnergyAmp(double amp) { fEnergyAmp = amp; }
  void SetDetector(int det) { detector = det;}

  double GetCount() const { return fCount; }
  double GetnAverage() const { return nAverage; }
  bool GetPileUp() const { return fPileUp; }
  bool GetNoisy() const { return fNoisy; }



  void SetCount(int counts){ fCount = counts; }
  void SetnAverage(int Av){ nAverage = Av;}
  void SetPileUp(bool PU){ fPileUp = PU; }
  void SetNoisy(bool fN){ fNoisy = fN; }



  struct TNaIHitTimeComparator {
    bool operator() (const TNaIHit& a, double time) {
      return a.GetTime() < time;
    }
    bool operator() (double time, const TNaIHit& b) {
      return time < b.GetTime();
    }
  };

  struct TNaIHitComparator {
    bool operator() (const TNaIHit& a, const TNaIHit& b){
      return a.GetTime() < b.GetTime();
    }
  };

struct TimeSortNaIHits {
    bool operator()(const TNaIHit& h1, const TNaIHit& h2) {
      int t1 = h1.GetTime();
      int t2 = h2.GetTime();
      return t1 < t2;
    }
  };

  private:

  double fTimingTime;
  double fTimingAmp;// save as an integer, 100 times the real values
  double fEnergyTime;
  double fEnergyAmp;

  int detector;//"1" is NaI, "2" is CsI
  int fCount;
  int nAverage;
  bool fPileUp;
  bool fNoisy; //There was switching power supply noisy on the timing channel.  This does not mean the hit is useless, but the E info from the PMT pulse was degraded. flag this hit

  const TOctalFADCIsland* fTimePulse; //! This does not own the FADC Islands
  const TOctalFADCIsland* fEnergyPulse; //! This does not own the FADC Islands


ClassDef(TNaIHit,1);

};

#endif
