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

  public:
  TVacuumData();
  TVacuumData(std::vector<int> time, std::vector<int> status, std::vector<float> pressure);

  void Reset(Option_t* o = "");

  void AddPoint(int time, int status, float pressure){
  	  fTime.push_back(time);
  	  fStatus.push_back(status);
  	  fPressure.push_back(pressure);
  }

  ClassDef(TVacuumData, 1);
};

#endif
