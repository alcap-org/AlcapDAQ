#ifndef TOctalFADCIsland_h
#define TOctalFADCIsland_h

#include <vector>

/** Timestamp and vector of ADC samples. This class is not specific
  * to neutron detectors or Ge det. It should be useable for any of
  * the Octal FADCs.
  */
class TOctalFADCIsland {
 public:

  TOctalFADCIsland();
  TOctalFADCIsland(int timestamp, const std::vector<int>& samples)
    : fTime(timestamp), fData(samples) {}

  int GetTime() const { return fTime; }
  int GetNSamples() const { return fData.size(); }
  std::vector<int>& GetSampleVector() { return fData; }

  int GetMax(int first = 0) const { return GetMax(first,fData.size()-1); }
  int GetMax(int first, int last) const;
  int GetMaxBin(int first = 0) const { return GetMaxBin(first, fData.size()-1);}
  int GetMaxBin(int first, int last) const;
  int GetMaxBinTime(int first = 0) const { return GetMaxBinTime(first, fData.size()-1);}
  int GetMaxBinTime(int first, int last) const;

  double GetAverageMax(int nAv, int first = 0) const {
    return GetAverageMax(nAv,first,fData.size()-1); }
  double GetAverageMax(int nAv, int first, int last) const;
  int GetCFBlockTime(int first = 0) const {return GetCFBlockTime(first, fData.size()-1);}
  int GetCFBlockTime(int first, int last) const;
  int GetCFBin(int first = 0) const {return GetCFBin(first, fData.size()-1);}
  int GetCFBin(int first, int last) const;

  double GetAverage(int first = 0) const {return GetAverage(first, fData.size()-1);}
  double GetAverage(int first,int last) const;
  double GetIntegral(int first = 0) const {return GetIntegral(first, fData.size()-1);}
  double GetIntegral(int first,int last) const;

  struct TimeSortFADCIslands {
    bool operator()(const TOctalFADCIsland* i1, const TOctalFADCIsland* i2) {
      int t1 = i1->GetTime();
      int t2 = i2->GetTime();
      return t1 < t2;
    }
  };

  protected:
  int fTime;
  std::vector<int> fData;
};

#endif
