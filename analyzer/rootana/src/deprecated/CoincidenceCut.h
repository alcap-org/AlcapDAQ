#ifndef CoincidenceCut_h__
#define CoincidenceCut_h__

#include "BaseModule.h"
#include "TGlobalData.h"
#include "TSetupData.h"

class CoincidenceCut : public BaseModule{
 public:
  CoincidenceCut(char *HistogramDirectoryName, std::string det_name_a, std::string det_name_b, double start_window, double stop_window);
  ~CoincidenceCut();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);


  // Checking DetB against DetA and so that means the DetB pulses are the ones that will be cut
  std::string fDetNameA;
  std::string fDetNameB;
  double fStartWindow;
  double fStopWindow;
};

#endif

