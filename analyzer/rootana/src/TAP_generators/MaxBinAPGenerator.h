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
  MaxBinAPGenerator(TAPGeneratorOptions* opts):
	  TVAnalysedPulseGenerator(opts),fSetup(TSetupData::Instance()){};
  virtual ~MaxBinAPGenerator(){};
  void SetBankInfo(std::string bankname){
      fBankname=bankname;
      fPedestal = fSetup->GetPedestal(bankname);
      fTriggerPolarity = fSetup->GetTriggerPolarity(bankname);
      fECalibSlope = fSetup->GetADCSlopeCalib(bankname);
      fECalibOffset = fSetup->GetADCOffsetCalib(bankname);
      fClockTick = fSetup->GetClockTick(bankname);
      fTimeShift = fSetup->GetTimeShift(bankname);
      fDetName=fSetup->GetDetectorName(bankname);
   }

 public:
   virtual int ProcessPulses(const PulseIslandList&,AnalysedPulseList&);
   virtual bool MayDivideTPIs(){return false;};

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
   TSetupData* fSetup;

};

#endif // MAXBINAPGENERATOR_H__
