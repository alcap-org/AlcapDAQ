#ifndef %MODULE%_H__
#define %MODULE%_H__

#include "TSetupData.h"
#include "TVAnalysedPulseGenerator.h"
#include "definitions.h"

class %module%APGenerator:public TVAnalysedPulseGenerator {

 public:
  %module%APGenerator(TAPGeneratorOptions* opts);
  virtual ~%module%APGenerator(){};

 public:
   virtual int ProcessPulses( const PulseIslandList&,AnalysedPulseList&);

   // This function should return true if this generator could break up a TPI
   // into more than one TAP
   virtual bool MayDivideTPIs(){return true;};

};

#endif //%MODULE%_H__
