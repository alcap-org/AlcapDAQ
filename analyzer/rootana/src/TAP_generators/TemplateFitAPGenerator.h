#ifndef TEMPLATEFITAPGENERATOR_H__
#define TEMPLATEFITAPGENERATOR_H__

#include "TSetupData.h"
#include "TVAnalysedPulseGenerator.h"
#include "definitions.h"
#include "TemplateArchive.h"
#include "TemplateFitter.h"
#include "TTemplate.h"
#include "TTemplateFitAnalysedPulse.h"

class TemplateFitAPGenerator:public TVAnalysedPulseGenerator {

 public:
  TemplateFitAPGenerator(TAPGeneratorOptions* opts);
  virtual ~TemplateFitAPGenerator(){};

 public:
   virtual int ProcessPulses( const PulseIslandList&,AnalysedPulseList&);

   // This function should return true if this generator could break up a TPI
   // into more than one TAP
   virtual bool MayDivideTPIs(){return true;};
   

   static const char* TapType() {
       return TTemplateFitAnalysedPulse::Class()->GetName();
   }

private:
   static TemplateArchive* fTemplateArchive;
   TTemplate* fTemplate;
   TemplateFitter* fFitter;

};

#endif //TEMPLATEFITAPGENERATOR_H__
