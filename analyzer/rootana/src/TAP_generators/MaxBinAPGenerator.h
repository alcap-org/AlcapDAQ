#ifndef MAXBINAPGENERATOR_H__
#define MAXBINAPGENERATOR_H__

#include "TSetupData.h"
#include "TVAnalysedPulseGenerator.h"
#include <vector>
#include <string>

class TPulseIsland;
class TAnalysedPulse;

class MaxBinAPGenerator:public TVAnalysedPulseGenerator {

 public:
  MaxBinAPGenerator():TVAnalysedPulseGenerator(){};
  virtual ~MaxBinAPGenerator(){};
  void SetBankInfo(const TSetupData* gSetup,std::string bankname){
      fBankname=bankname;
      fPedestal = gSetup->GetPedestal(bankname);
      fTriggerPolarity = gSetup->GetTriggerPolarity(bankname);
      fECalibSlope = gSetup->GetADCSlopeCalib(bankname);
      fECalibOffset = gSetup->GetADCOffsetCalib(bankname);
      fClockTick = gSetup->GetClockTick(bankname);
      fTimeShift = gSetup->GetTimeShift(bankname);
      fDetName=gSetup->GetDetectorName(bankname);
   }

 public:
   virtual void ProcessPulses(const TSetupData*, const PulseIslandList&,AnalysedPulseList&);
   void GetAllParameters_MaxBin(const TPulseIsland* pulse, double& amplitude,
         double& time, double& integral, double& energy) ;

 private:
   // Ideally we would have the TSetupData storing all this as a single struct so
   // we wouldn't need the following fields
   std::string fBankname;
   std::string fDetName;
   double fPedestal;
   int fTriggerPolarity;
   double fECalibSlope;
   double fECalibOffset;
   double fClockTick;
   double fTimeShift;

};

#endif // MAXBINAPGENERATOR_H__
