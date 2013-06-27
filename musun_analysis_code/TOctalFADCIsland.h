#ifndef TOctalFADCIsland_h
#define TOctalFADCIsland_h

#include <vector>

/** Timestamp and vector of ADC samples. This class is not specific
  * to neutron detectors or Ge det. It should be useable for any of
  * Fred's FADCs
  */



class TOctalFADCIsland {
 public:

  TOctalFADCIsland(int timestamp, const std::vector<int>& samples, int channel)
    : fTime(timestamp), fData(samples), fChannel(channel){fmaxGauss = -1; fmaxbinGauss = -1; fchiSqrRed = -1;fPulseQuality = -1;}

  int GetTime() const { return fTime; }
  int GetNSamples() const { return fData.size(); }
  int GetSample(int i) const { return fData[i]; }
  int GetFADCChannel() const { return fChannel; }



  std::vector<int>& GetSampleVector() { return fData; }

  int GetMax(int first = 0) const { return GetMax(first,fData.size()-1); } //Make these two steps so you only need one implementation. Giving no argument just finds the maximum. Still, you can ask for the max between "first" and "last" sample
  int GetMax(int first, int last) const;
  int GetMaxBin(int first = 0) const { return GetMaxBin(first, fData.size()-1);}
  int GetMaxBin(int first, int last) const;
  int GetMaxBinBlockTime(int first = 0) const {return GetMaxBinBlockTime(first, fData.size()-1);}
  int GetMaxBinBlockTime(int first, int last) const;


  double GetTimeNs() const; //Returns fTime in ns  This time is used in the event display
  



//Added by Frederik, oct/nov 2011. Written in function of the Ge and NaI analysis, still can be used more general

  double GetAverageMax(int nAv, int first = 0) const { return GetAverageMax(nAv,first,fData.size()-1); } //First argument is number of point around the maximum you want to average "0" should give the same result as the GetMax method, "2" averages the maximum with the sample left and right of it, i.e. an average of 3 points 
  double GetAverageMax(int nAv, int first, int last) const;
  bool GoodPulse(int shapTime, double clockPeriod, int level) ; // check if it is a "good" FADC pulse. The level indicates how many of the conditions will be checked
  bool GoodPulse(int shapTime, double clockPeriod, int pedestal, int runnr, int level, int channel) ; // custom for weird overshoot samples of the Ge detector
  int GetCFBlockTime(int first = 0) const {return GetCFBlockTime(first, fData.size()-1);}
  int GetCFBlockTime(int first, int last) const;
  int GetCFBin(int first = 0) const {return GetCFBin(first, fData.size()-1);}
  int GetCFBin(int first, int last) const;

  double GetAverage(int first = 0) const {return GetAverage(first, fData.size()-1);}
  double GetAverage(int first,int last) const; //Give the average value between two boundaries. It is the user responsibility to make sure that "first" and "last" are ok. Use GetNSamples for that. The function will return -1 for arguments out of range
  double GetIntegral(int first = 0) const {return GetIntegral(first, fData.size()-1);}
  double GetIntegral(int first,int last) const;

  int FitGauss(int left, int right, int shapTime, double clockPeriod, int pedestal);//Call this function first to do the fits before asking for the max etc ...
  double GetMaxGauss() const {return fmaxGauss;}
  double GetMaxBinGauss() const {return fmaxbinGauss;}
  double GetChiSqrRed() const {return fchiSqrRed;}

  void SetMaxGauss(double value) {fmaxGauss = value;}
  void SetMaxBinGauss(double value) {fmaxbinGauss = value;}
  void SetChiSqrRed(double value) {fchiSqrRed = value;}

  void SetPulseQuality(int value) {fPulseQuality = value;}
  int GetPulseQuality() const {return fPulseQuality;}


  struct TimeSortFADCIslands {
    bool operator()(const TOctalFADCIsland& i1, const TOctalFADCIsland& i2) {
      int t1 = i1.GetTime();
      int t2 = i2.GetTime();
      return t1 < t2;
    }
  };

  struct TOctalFADCIslandTimeComparator {
    bool operator() (const TOctalFADCIsland& a, double time) {
      return a.GetTimeNs() < time;
    }
    bool operator() (double time,const TOctalFADCIsland& b) {
      return time < b.GetTimeNs();
    }
  };

  private:
  int fTime, fChannel;
  int fPulseQuality; // -1 is an unchecked pulse, 0 is an accepted pulse, 1,2,.... represent the level at  which the pulse/island is rejected  
  std::vector<int> fData;
  double fmaxbinGauss, fmaxGauss, fchiSqrRed;
  //TH1I* island; 
};

#endif
