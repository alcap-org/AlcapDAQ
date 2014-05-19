#ifndef TemplateAPGenerator_H__
#define TemplateAPGenerator_H__

#include "TSetupData.h"
#include "TVAnalysedPulseGenerator.h"
#include <vector>
#include <string>

class TPulseIsland;
class TAnalysedPulse;

class TemplateAPGenerator:public TVAnalysedPulseGenerator {

 public:
  TemplateAPGenerator():TVAnalysedPulseGenerator(){};
  virtual ~TemplateAPGenerator(){};
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
   virtual void ProcessPulses(const TSetupData*, const PulseIslandList_t&,AnalysedPulseList_t&);

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

#endif // TemplateAPGenerator_H__
