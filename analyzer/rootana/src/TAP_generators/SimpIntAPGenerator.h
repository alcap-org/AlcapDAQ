#ifndef SIMPINT_H__
#define SIMPINT_H__

#include "TSetupData.h"
#include "TVAnalysedPulseGenerator.h"
#include "definitions.h"

class SimpIntAPGenerator:public TVAnalysedPulseGenerator {

 public:
  SimpIntAPGenerator(TAPGeneratorOptions* opts):
  TVAnalysedPulseGenerator("SimpInt", opts),fSetup(TSetupData::Instance()){};
  virtual ~SimpIntAPGenerator(){};

  void SetBankInfo(std::string bankname)
  {
    fBankname= bankname;
    fDetname = fSetup->GetDetectorName(bankname);
    fPedestal = fSetup->GetPedestal(bankname);
    fTriggerPolarity = fSetup->GetTriggerPolarity(bankname);
    fECalibSlope = fSetup->GetADCSlopeCalib(bankname);
    fECalibOffset = fSetup->GetADCOffsetCalib(bankname);
  }


 public:
   virtual int ProcessPulses( const PulseIslandList&,AnalysedPulseList&);
   virtual bool MayDivideTPIs(){return false;};


 private:
   // This will likely be unnecessary

   std::string fBankname;
   std::string fDetname;
   double fPedestal, fECalibSlope, fECalibOffset;
   int fTriggerPolarity;
   TSetupData* fSetup;
};

#endif //SIMPLEINTEGRATOR_H__
