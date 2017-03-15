#include "TH2.h"

class SiArm {
 public:
 SiArm(std::string armname): fArmName(armname) { }

  std::string GetArmName() { return fArmName; }

  TH2F* GetEvdEPlot() { return fEvdEPlot; }
  void SetEvdEPlot(TH2F* plot) { fEvdEPlot = plot; }

 private:
  std::string fArmName;
  TH2F* fEvdEPlot;
};
