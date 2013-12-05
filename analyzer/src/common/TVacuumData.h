#ifndef TVacuumData_h
#define TVacuumData_h

#include <vector>
#include <string>

#include <TObject.h>
#include <TH1.h>

class TVacuumData : public TObject {
  private:
  std::vector<int> fStatus; // Status
  std::vector<float> fPressure; // Pressure
  std::vector<int> fTime; // Time
//  double fClockTickInNs; // clock tick in nanoseconds
//  int fTimeStamp; // time stamp in clock ticks
//  std::string fBankName; // FADC Bank name of this island's channel

  public:
  TVacuumData();
  TVacuumData(std::vector<int> time, std::vector<int> status, std::vector<float> pressure);

  void Reset(Option_t* o = "");

  void AddPoint(int time, int status, float pressure){
  	  fTime.push_back(time);
  	  fStatus.push_back(status);
  	  fPressure.push_back(pressure);
  }
  std::vector<int> GetStatus() const { return fStatus; }
  std::vector<int> GetTime() const { return fTime; }
  std::vector<float> GetPressure() const { return fPressure; }
 // double GetClockTickInNs() const { return fClockTickInNs; }
 // int GetTimeStamp() const { return fTimeStamp; }
 // std::string GetBankName() const { return fBankName; }
  
 // double GetPulseHeight() const;
 // double GetPulseTime() const;  
 // TH1I* GetPulseWaveform(std::string histname, std::string histtitle) const;
 // int GetPulseLength() const { return fSamples.size(); }

 // int GetPeakSample() const;
 // double GetPedestal(int nPedSamples) const;
  ClassDef(TVacuumData, 1);
};

#endif
