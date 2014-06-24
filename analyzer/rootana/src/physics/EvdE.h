#ifndef EvdE_h__
#define EvdE_h__

#include "BaseModule.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"

class EvdE : public BaseModule{
 public:
  EvdE(char *HistogramDirectoryName);
  EvdE(char *HistogramDirectoryName, double t0, double t1);
  EvdE(modules::options* opt);
  ~EvdE();

 private:
	double tCoincidence; // Time cut to look for a thin hit on either side of a thick
	double tPP;      // Pileup window time (no two muons allowed within this window of each other)
	double tStop;
	double tStart;
	double adcCutThick[2];
	double adcCutThin[2];
	double eCutThick[2];
	double eCutThin[2];
	double slopeThick[2];
	double offsetThick[2];
	double slopeThin[2][4];
	double offsetThin[2][4];
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
};

#endif

