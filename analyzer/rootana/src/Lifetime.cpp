#include "Lifetime.h"
#include "TAnalysedPulse.h"

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "TH1I.h"

extern std::map< std::string, std::vector<TAnalysedPulse*> > gAnalysedPulseMap;
static const int nSec = 8;
static unsigned int nECut;
static double tCut[3];
static double tPileUp;
static std::vector<int> eCut;
static std::vector<std::string> sNames;
static std::vector<std::string> sTitles;
static std::vector<TH1I*> hLifetime;

Lifetime::Lifetime(char *HistogramDirectoryName) :
  FillHistBase(HistogramDirectoryName) {

  // Time in ns
  tCut[0] = 50.;
  tCut[1] = 400.;
  tCut[2] = 5000.;
  tPileUp = 10000.;
  // Energy cuts in ADC
  eCut.push_back(100);
  eCut.push_back(200);
  eCut.push_back(300);
  eCut.push_back(400);
  eCut.push_back(500);
  eCut.push_back(600);
  eCut.push_back(700);
  eCut.push_back(800);
  eCut.push_back(900);
  eCut.push_back(1000);
  eCut.push_back(1100);
  eCut.push_back(1200);
  eCut.push_back(1300);
  eCut.push_back(1400);
  eCut.push_back(1500);
  sNames.push_back("hLifetime100");
  sNames.push_back("hLifetime200");
  sNames.push_back("hLifetime300");
  sNames.push_back("hLifetime400");
  sNames.push_back("hLifetime500");
  sNames.push_back("hLifetime600");
  sNames.push_back("hLifetime700");
  sNames.push_back("hLifetime800");
  sNames.push_back("hLifetime900");
  sNames.push_back("hLifetime1000");
  sNames.push_back("hLifetime1100");
  sNames.push_back("hLifetime1200");
  sNames.push_back("hLifetime1300");
  sNames.push_back("hLifetime1400");
  sNames.push_back("hLifetime1500");
  sTitles.push_back("Lifetime (100 ADC cut)");
  sTitles.push_back("Lifetime (200 ADC cut)");
  sTitles.push_back("Lifetime (300 ADC cut)");
  sTitles.push_back("Lifetime (400 ADC cut)");
  sTitles.push_back("Lifetime (500 ADC cut)");
  sTitles.push_back("Lifetime (600 ADC cut)");
  sTitles.push_back("Lifetime (700 ADC cut)");
  sTitles.push_back("Lifetime (800 ADC cut)");
  sTitles.push_back("Lifetime (900 ADC cut)");
  sTitles.push_back("Lifetime (1000 ADC cut)");
  sTitles.push_back("Lifetime (1100 ADC cut)");
  sTitles.push_back("Lifetime (1200 ADC cut)");
  sTitles.push_back("Lifetime (1300 ADC cut)");
  sTitles.push_back("Lifetime (1400 ADC cut)");
  sTitles.push_back("Lifetime (1500 ADC cut)");
  // Number to index later
  nECut = eCut.size();
  // Sanity check: All vectors are the same size above
  if (nECut != sNames.size() || nECut != sTitles.size())
    std::cout << "Error! Coder forgot a line or two in initialization of Lifetime module in rootana!" << std::endl;
  // Prepare histograms
  for (unsigned int iECut = 0; iECut < nECut; ++iECut)
    hLifetime.push_back(new TH1I(sNames[iECut].c_str(), sTitles[iECut].c_str(), 1000, 0., 2000.));
}

Lifetime::~Lifetime(){
}

int Lifetime::ProcessEntry(TGlobalData *gData, TSetupData *gSetup) {

  int iSec;
  // All fast pulses
  std::vector<TAnalysedPulse*>* musc;
  std::vector<TAnalysedPulse*>* targ[8];
  if (gAnalysedPulseMap.count("muSc")) musc = &gAnalysedPulseMap.at("muSc");
  else return 0;

  if (gAnalysedPulseMap.count("Si16-1-S")) targ[0] = &gAnalysedPulseMap.at("Si16-1-S");
  else targ[0] = NULL;

  if (gAnalysedPulseMap.count("Si16-2-S")) targ[1] = &gAnalysedPulseMap.at("Si16-2-S");
  else targ[1] = NULL;

  if (gAnalysedPulseMap.count("Si16-3-S")) targ[2] = &gAnalysedPulseMap.at("Si16-3-S");
  else targ[2] = NULL;

  if (gAnalysedPulseMap.count("Si16-4-S")) targ[3] = &gAnalysedPulseMap.at("Si16-4-S");
  else targ[3] = NULL;

  if (gAnalysedPulseMap.count("Si16-5-S")) targ[4] = &gAnalysedPulseMap.at("Si16-5-S");
  else targ[4] = NULL;

  if (gAnalysedPulseMap.count("Si16-6-S")) targ[5] = &gAnalysedPulseMap.at("Si16-6-S");
  else targ[5] = NULL;

  if (gAnalysedPulseMap.count("Si16-7-S")) targ[6] = &gAnalysedPulseMap.at("Si16-7-S");
  else targ[6] = NULL;

  if (gAnalysedPulseMap.count("Si16-8-S")) targ[7] = &gAnalysedPulseMap.at("Si16-8-S");
  else targ[7] = NULL;

  // Iterators through aforementioned vectors
  std::vector<TAnalysedPulse*>::iterator cMusc;
  std::vector<TAnalysedPulse*>::iterator cTarg[8];
  cMusc = musc->begin();
  for (iSec = 0; iSec < nSec; ++iSec)
    if (targ[iSec])
      cTarg[iSec] = targ[iSec]->begin();

  unsigned int iECut;
  double tMusc;
  bool bFound[15];
  bool isStop;
  for (; cMusc < musc->end(); ++cMusc) {
    tMusc = (*cMusc)->GetTime();
    isStop = false;
    // If there's pile-up, advance beyond it
    if ((cMusc+1) != musc->end() && (*(cMusc + 1))->GetTime() - tMusc < tPileUp) {
      ++cMusc;
      continue;
    }
    // Set all thresholds to not-found-yet
    for (iECut = 0; iECut < nECut; ++iECut)
      bFound[iECut] = false;
    // Advance silicon pulses beyond time of muSc
    for (iSec = 0; iSec < nSec; ++iSec)
      if (targ[iSec])
	while (cTarg[iSec] != targ[iSec]->end() && (*(cTarg[iSec]))->GetTime() < tMusc)
	  ++(cTarg[iSec]);
    // Check for stop in target
    for (iSec = 0; iSec < nSec; ++iSec) {
      if (targ[iSec]) {
	while (cTarg[iSec] != targ[iSec]->end() && (*(cTarg[iSec]))->GetTime() - tMusc < tCut[0]) {
	  ++(cTarg[iSec]);
	  isStop = true;
	}
      }
    }
    // If no stop, continue
    if (!isStop)
      continue;
    // Now progress until in meaasurement window
    for (iSec = 0; iSec < nSec; ++iSec)
      if (targ[iSec])
	while (cTarg[iSec] != targ[iSec]->end() && (*(cTarg[iSec]))->GetTime() - tMusc < tCut[1])
	  ++(cTarg[iSec]);
    // Check for hits
    for (iSec = 0; iSec < nSec; ++iSec) {
      if (targ[iSec]) {
	while (cTarg[iSec] < targ[iSec]->end() && (*(cTarg[iSec]))->GetTime() - tMusc < tCut[2]) {
	  for (iECut = 0; iECut < nECut; ++iECut) {
	    if (!(bFound[iECut])) {
	      if ((*(cTarg[iSec]))->GetAmplitude() > eCut[iECut]) {
		hLifetime[iECut]->Fill((*(cTarg[iSec]))->GetTime() - tMusc);
		bFound[iECut] = true;
	      } // If energy is greater than cut value
	    } // If no hit has been found in this energy region yet
	  } // For all energy cuts
	  ++(cTarg[iSec]);
	} // While there are still silicon target events to loop through
      } // If this section has a hits collection
    } // For all silicon target sections
  } // For all muSc hits

  return 0;
}
