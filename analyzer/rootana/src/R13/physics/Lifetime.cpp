#include "Lifetime.h"
#include "TAnalysedPulse.h"
#include "definitions.h"

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "TH1I.h"

extern StringAnalPulseMap gAnalysedPulseMap;
static unsigned int nECut;                // Number of histograms to make (each with different energy cut for final "capture" event)
static double tCut[2];                    // Two time cuts in nanosecond (minimum time before looking for decay, maximum time)
static double tPileUp;                    // Pileup window time (no two muons allowed within this window of each other)
static std::vector<int> eCut;             // Energy cuts to make for histograms
static std::vector<std::string> sNames;   // Histogram names for ROOT application
static std::vector<std::string> sTitles;  // Histogram titles
static std::vector<TH1I*> hLifetime;      // Histograms

Lifetime::Lifetime(char *HistogramDirectoryName) :
  BaseModule(HistogramDirectoryName) {

  // Time in ns
  tCut[0] = 800.;
  tCut[1] = 5000.;
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

  gDirectory->cd("/");
}

Lifetime::~Lifetime(){
}

int Lifetime::ProcessEntry(TGlobalData *gData, TSetupData *gSetup) {

  // Look for muSc and at least one arm
  AnalysedPulseList *musc;
  AnalysedPulseList *sir, *sil;
  if (gAnalysedPulseMap.count("muSc")) musc = &gAnalysedPulseMap.at("muSc");
  else return 0;

  if (gAnalysedPulseMap.count("SiR2-F")) sir = &gAnalysedPulseMap.at("SiR2-F");
  else sir = NULL;

  if (gAnalysedPulseMap.count("SiL2-F")) sil = &gAnalysedPulseMap.at("SiL2-F");
  else sil = NULL;

  // If neither arm had hits, there's nothing to loop through
  if (sil == NULL && sir == NULL) return 0;

  // Iterators through aforementioned vectors
  AnalysedPulseList::iterator cMusc;
  AnalysedPulseList::iterator cSiR, cSiL;
  cMusc = musc->begin();
  if (sir)
    cSiR = sir->begin();
  if (sil)
    cSiL = sil->begin();

  unsigned int iECut;
  double tMusc;
  bool bFound[15];
  for (; cMusc < musc->end(); ++cMusc) {
    tMusc = (*cMusc)->GetTime();
    // If there's pile-up, advance beyond it
    if ((cMusc+1) != musc->end() && (*(cMusc + 1))->GetTime() - tMusc < tPileUp) {
      ++cMusc;
      continue;
    }
    // Set all flags to 'not-found-yet' status
    for (iECut = 0; iECut < nECut; ++iECut)
      bFound[iECut] = false;
    // Advance silicon pulses beyond time of muSc
    if (sir)
      while (cSiR != sir->end() && (*cSiR)->GetTime() -tMusc < tCut[0])
	++cSiR;
    if (sil)
      while (cSiL != sil->end() && (*cSiL)->GetTime() < tMusc)
	++cSiL;
    // Check for hits
    if (sir)
      while (cSiR != sir->end() && (*cSiR)->GetTime() - tMusc < tCut[1]) {
	for (iECut = 0; iECut < nECut; ++iECut)
	  if (!(bFound[iECut]))
	    if ((*cSiR)->GetAmplitude() > eCut[iECut]) {
	      hLifetime[iECut]->Fill((*cSiR)->GetTime() - tMusc);
	      bFound[iECut] = true;
	    }
	++cSiR;
      }
    if (sil)
      while (cSiL != sil->end() && (*cSiL)->GetTime() - tMusc < tCut[1]) {
	for (iECut = 0; iECut < nECut; ++iECut)
	  if (!(bFound[iECut]))
	    if ((*cSiL)->GetAmplitude() > eCut[iECut]) {
	      hLifetime[iECut]->Fill((*cSiL)->GetTime() - tMusc);
	      bFound[iECut] = true;
	    }
	++cSiL;
      }
  }

  return 0;
}
