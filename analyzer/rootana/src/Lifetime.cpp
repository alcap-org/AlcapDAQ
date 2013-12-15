#include "Lifetime.h"
#include "TAnalysedPulse.h"

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "TH1I.h"

// The stuff we'll be using
extern std::map< std::string, std::vector<TAnalysedPulse*> > gAnalysedPulseMap;
int nECut;
double tCut[2];
double tPileUp;
std::vector<int> eCut;
std::vector<std::string> sNames;
std::vector<std::string> sTitles;
std::vector<TH1I*> hLifetime;

Lifetime::Lifetime(char *HistogramDirectoryName) :
  FillHistBase(HistogramDirectoryName) {

  // Time in ns
  tCut[0] = 800.;
  tCut[1] = 2000.;
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
  if (nECut != sNames.size() || nECut != sTitles.size())
    std::cout << "Error! Coder forgot a line or two in initialization of Lifetime module in rootana!" << std::endl;

  // Prepare histograms
  for (int iECut = 0; iECut < nECut; ++iECut)
    hLifetime.push_back(new TH1I(sNames[iECut].c_str(), sTitles[iECut].c_str(), 1000, 0., 2000.));
}

Lifetime::~Lifetime(){
}

int Lifetime::ProcessEntry(TGlobalData *gData, TSetupData *gSetup) {

  int nSec = 8;
  int iSec;
  // All fast pulses
  std::vector<TAnalysedPulse*>* musc;
  std::vector<TAnalysedPulse*>* targ[8];
  if (gAnalysedPulseMap.count("muSc")) musc = &gAnalysedPulseMap.at("muSc");
  else return 0;

  if (gAnalysedPulseMap.count("Si16-1")) targ[0] = &gAnalysedPulseMap.at("Si16-1");
  else if (gAnalysedPulseMap.count("SiL16-1")) targ[0] = &gAnalysedPulseMap.at("SiL16-1");
  else targ[0] = NULL;

  if (gAnalysedPulseMap.count("Si16-2")) targ[1] = &gAnalysedPulseMap.at("Si16-2");
  else if (gAnalysedPulseMap.count("SiL16-2")) targ[1] = &gAnalysedPulseMap.at("SiL16-2");
  else targ[1] = NULL;

  if (gAnalysedPulseMap.count("Si16-3")) targ[2] = &gAnalysedPulseMap.at("Si16-3");
  else if (gAnalysedPulseMap.count("SiL16-3")) targ[2] = &gAnalysedPulseMap.at("SiL16-3");
  else targ[2] = NULL;

  if (gAnalysedPulseMap.count("Si16-4")) targ[3] = &gAnalysedPulseMap.at("Si16-4");
  else if (gAnalysedPulseMap.count("SiL16-4")) targ[3] = &gAnalysedPulseMap.at("SiL16-4");
  else targ[3] = NULL;

  if (gAnalysedPulseMap.count("Si16-5")) targ[4] = &gAnalysedPulseMap.at("Si16-5");
  else if (gAnalysedPulseMap.count("SiL16-5")) targ[4] = &gAnalysedPulseMap.at("SiL16-5");
  else targ[4] = NULL;

  if (gAnalysedPulseMap.count("Si16-6")) targ[5] = &gAnalysedPulseMap.at("Si16-6");
  else if (gAnalysedPulseMap.count("SiL16-6")) targ[5] = &gAnalysedPulseMap.at("SiL16-6");
  else targ[5] = NULL;

  if (gAnalysedPulseMap.count("Si16-7")) targ[6] = &gAnalysedPulseMap.at("Si16-7");
  if (gAnalysedPulseMap.count("SiL16-7")) targ[6] = &gAnalysedPulseMap.at("SiL16-7");
  else targ[6] = NULL;

  if (gAnalysedPulseMap.count("Si16-8")) targ[7] = &gAnalysedPulseMap.at("Si16-8");
  if (gAnalysedPulseMap.count("SiL16-8")) targ[7] = &gAnalysedPulseMap.at("SiL16-8");
  else targ[7] = NULL;

  // Iterators through aforementioned vectors
  std::vector<TAnalysedPulse*>::iterator cMusc;
  std::vector<TAnalysedPulse*>::iterator cTarg[8];
  cMusc = musc->begin();
  for (iSec = 0; iSec < nSec; ++iSec)
    if (targ[iSec])
      cTarg[iSec] = targ[iSec]->begin();

  int iECut;
  double tMusc;
  double tTarg[8];
  bool bFound[15];
  for (; cMusc < musc->end(); ++cMusc) {
    tMusc = (*cMusc)->GetTime();
    // If there's pile-up, advance beyond it
    if ((cMusc+1) != musc->end() && (*(cMusc + 1))->GetTime() - tMusc < tPileUp) {
      ++cMusc;
      continue;
    }
    // Set all thresholds to not-found-yet
    for (iECut = 0; iECut < nECut; ++iECut)
      bFound[iECut] = false;
    // Advance silicon pulses beyond time of muSc + time cut
    for (iSec = 0; iSec < nSec; ++iSec)
      if (targ[iSec])
	while (cTarg[iSec] != targ[iSec]->end() && (*(cTarg[iSec]))->GetTime() - tMusc < tCut[0])
	  ++(cTarg[iSec]);
    // Check for hits
    for (iSec = 0; iSec < nSec; ++iSec) {
      if (targ[iSec]) {
	while (cTarg[iSec] < targ[iSec]->end() && (*(cTarg[iSec]))->GetTime() - tMusc < tCut[1]) {
	  for (iECut = 0; iECut < nECut; ++iECut) {
	    if (!(bFound[iECut])) {
	      if ((*(cTarg[iSec]))->GetAmplitude() > eCut[iECut]) {
		hLifetime[iECut]->Fill((*(cTarg[iSec]))->GetTime() - tMusc);
		bFound[iECut] = true;
	      }
	    }
	  }
	  ++(cTarg[iSec]);
	}
      }
    }
  }

  return 0;
}
