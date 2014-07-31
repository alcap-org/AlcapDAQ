#ifndef TEMPLATE_H__
#define TEMPLATE_H__

#include "TSetupData.h"
#include "TVAnalysedPulseGenerator.h"
#include "definitions.h"

class TemplateAPGenerator:public TVAnalysedPulseGenerator {

 public:
  TemplateAPGenerator(TAPGeneratorOptions* opts);
  virtual ~TemplateAPGenerator(){};

 public:
   virtual int ProcessPulses( const PulseIslandList&,AnalysedPulseList&);

   // This function should return true if this generator could break up a TPI
   // into more than one TAP
   virtual bool MayDivideTPIs(){return true;};

};

#endif //TEMPLATE_H__
