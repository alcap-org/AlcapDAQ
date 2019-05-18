#ifndef MAFLABR3APGENERATOR_H_T6YH9NCO
#define MAFLABR3APGENERATOR_H_T6YH9NCO
#include "TSetupData.h"
#include "TVAnalysedPulseGenerator.h"
#include "definitions.h"

#include "TAPAlgorithms.h"
#include "PulseCandidateFinder.h"

////////////////////////////////////////////////////////////////////////////////
/// \ingroup rootana_modules
/// \author Nam Tran
///
/// \brief
/// Digital signal processing on a pulse island
///
/// \details
/// Pulse height is calculated using gated integration method (aka FIR boxcar
/// filter), the width of the gated window is specified via the gate_width
/// parameter. The default width is 40 based on a test on LaBr3 pulses. The
/// integration is set to be the same as the pulse height.
/// The pulse height calculated by this generator will likely overflowed the
/// normal pulse height spectrum, please use the integral plot instead.
/// 
/// Pulse timing is obtained from a simple digital CFD, the default constant
/// fraction is 0.3.
/// There is an option to not using pedestal from the DB but recalculating
/// using a number of first samples.
////////////////////////////////////////////////////////////////////////////////

class GatedIntegralCFDAPGenerator:public TVAnalysedPulseGenerator {

 public:
  GatedIntegralCFDAPGenerator(TAPGeneratorOptions* opts);
  virtual ~GatedIntegralCFDAPGenerator();

 public:
   virtual int ProcessPulses( const PulseIslandList&,AnalysedPulseList&);
   virtual bool MayDivideTPIs(){return false;};

 private:
   void DrawPulse(int original, int pulse_timestamp, int n_pulse_samples);
   void Analyse(int tpi_ID, const TPulseIsland* tpi,
       AnalysedPulseList& analysedList);

   int fGateWidth;
   double fConstantFraction;
   bool fPedCalculation;
   unsigned int fNped;
   double fPedestal;
   bool fNoTimeShift;
};
#endif /* end of include guard: MAFLABR3APGENERATOR_H_T6YH9NCO */
