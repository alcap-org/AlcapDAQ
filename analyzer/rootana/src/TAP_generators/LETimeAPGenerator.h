#ifndef LETIME_H__
#define LETIME_H__

#include "TSetupData.h"
#include "TVAnalysedPulseGenerator.h"
#include "definitions.h"

class LETimeAPGenerator:public TVAnalysedPulseGenerator {

 private:
  double fLeadingEdge;

 public:
  LETimeAPGenerator(TAPGeneratorOptions* opts);
  virtual ~LETimeAPGenerator(){};

 public:
   virtual int ProcessPulses( const PulseIslandList&,AnalysedPulseList&);

   // This function should return true if this generator could break up a TPI
   // into more than one TAP
   virtual bool MayDivideTPIs(){return true;};

};

#endif //LETIME_H__
