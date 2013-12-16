#include "EvdE.h"
#include "TAnalysedPulse.h"

#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <map>

#include "TH2I.h"

extern std::map< std::string, std::vector<TAnalysedPulse*> > gAnalysedPulseMap;
static double tCoincidence; // Two time cuts in nanosecond (minimum time before looking for decay, maximum time)
static double tPileUp;      // Pileup window time (no two muons allowed within this window of each other)
static double tDecay;       // Window to look for the decay in
static int nSec;            // Number of sections in the silicon thin detectors
static int eCutThick[2];    // Thick energy cuts (right, left)
static int eCutThin[2][4];  // Thin energy cuts (right, left)(1,2,3,4)
static TH2I* hEvdE[2];      // Histograms (right, left)

EvdE::EvdE(char *HistogramDirectoryName) :
  FillHistBase(HistogramDirectoryName) {

  // Thin silicon is in quadrants
  nSec = 4;
  // Time in ns
  tCoincidence = 100.;
  tPileUp = 10000.;
  tDecay = 5000.;
  // Energy cuts in ADC
  eCutThick[0] = 500;
  eCutThick[1] = 500;
  eCutThin[0][0] = 500;
  eCutThin[0][1] = 500;
  eCutThin[0][2] = 500;
  eCutThin[0][3] = 500;
  eCutThin[1][0] = 500;
  eCutThin[1][1] = 500;
  eCutThin[1][2] = 500;
  eCutThin[1][3] = 500;
  // Prepare histograms
  hEvdE[0] = new TH2I("hEvdE_Right", "dEdx vs E (Right)",
		      (int)std::pow(2,12), 0., (double)(std::pow(2,12)-1),
		      (int)std::pow(2,12), 0., (double)(std::pow(2,12) - 1));
  hEvdE[1] = new TH2I("hEvdE_Right", "dEdx vs E (Left)",
		      (int)std::pow(2,12), 0., (double)(std::pow(2,12)-1),
		      (int)std::pow(2,12), 0., (double)(std::pow(2,12)-1));
}

EvdE::~EvdE(){
}

int EvdE::ProcessEntry(TGlobalData *gData, TSetupData *gSetup) {

  // Look for muSc and at least one arm
  std::vector<TAnalysedPulse*> *musc;
  std::vector<TAnalysedPulse*> *sirThin[4], *silThin[4];
  std::vector<TAnalysedPulse*> *sirThick, *silThick;
  if (gAnalysedPulseMap.count("muSc")) musc = &gAnalysedPulseMap.at("muSc");
  else return 0;

  if (gAnalysedPulseMap.count("SiR1-1-S")) sirThin[0]= &gAnalysedPulseMap.at("SiR1-1-S");
  else sirThin[0] = NULL;
  if (gAnalysedPulseMap.count("SiR1-2-S")) sirThin[0]= &gAnalysedPulseMap.at("SiR1-2-S");
  else sirThin[1] = NULL;
  if (gAnalysedPulseMap.count("SiR1-3-S")) sirThin[0]= &gAnalysedPulseMap.at("SiR1-3-S");
  else sirThin[2] = NULL;
  if (gAnalysedPulseMap.count("SiR1-4-S")) sirThin[0]= &gAnalysedPulseMap.at("SiR1-4-S");
  else sirThin[3] = NULL;

  if (gAnalysedPulseMap.count("SiL1-1-S")) silThin[0]= &gAnalysedPulseMap.at("SiL1-1-S");
  else silThin[0] = NULL;
  if (gAnalysedPulseMap.count("SiL1-2-S")) silThin[0]= &gAnalysedPulseMap.at("SiL1-2-S");
  else silThin[1] = NULL;
  if (gAnalysedPulseMap.count("SiL1-3-S")) silThin[0]= &gAnalysedPulseMap.at("SiL1-3-S");
  else silThin[2] = NULL;
  if (gAnalysedPulseMap.count("SiL1-4-S")) silThin[0]= &gAnalysedPulseMap.at("SiL1-4-S");
  else silThin[3] = NULL;

  if (gAnalysedPulseMap.count("SiR2-S")) sirThick = &gAnalysedPulseMap.at("SiR2-S");
  else sirThick = NULL;

  if (gAnalysedPulseMap.count("SiL2-S")) silThick = &gAnalysedPulseMap.at("SiL2-S");
  else silThick = NULL;

  // If there's nothing logical to get data from, don't bother analysing
  if (!(sirThick && (sirThin[0] || sirThin[1] || sirThin[2] || sirThin[3])) &&
      !(silThick && (silThin[0] || silThin[1] || silThin[2] || silThin[3])))
    return 0;


  // Iterators through aforementioned vectors
  int iSec;
  std::vector<TAnalysedPulse*>::iterator cMusc;
  std::vector<TAnalysedPulse*>::iterator cSiRThick, cSiLThick;
  std::vector<TAnalysedPulse*>::iterator cSiRThin[4], cSiLThin[4];
  cMusc = musc->begin();
  if (sirThick)
    cSiRThick = sirThick->begin();
  if (silThick)
    cSiLThick = silThick->begin();
  for (iSec = 0; iSec < nSec; ++iSec) {
    if (sirThin[iSec])
      cSiRThin[iSec] = sirThin[iSec]->begin();
    if (silThin[iSec])
      cSiLThin[iSec] = silThin[iSec]->begin();
  }

  double tMusc;
  std::vector<double> tSiRThick, tSiLThick;
  std::vector<int> ampSiRThick, ampSiLThick;
  unsigned int iHit, nThickHits;
  for (; cMusc < musc->end(); ++cMusc) {
    tMusc = (*cMusc)->GetTime();
    tSiRThick.clear(); tSiLThick.clear();
    ampSiRThick.clear(); ampSiLThick.clear();
    // If there's pile-up, advance beyond it
    if ((cMusc+1) != musc->end() && (*(cMusc + 1))->GetTime() - tMusc < tPileUp) {
      ++cMusc;
      continue;
    }
    // Advance silicon pulses beyond time of muSc
    if (sirThick)
      while (cSiRThick != sirThick->end() && (*cSiRThick)->GetTime() < tMusc)
	++cSiRThick;
    if (silThick)
      while (cSiLThick != silThick->end() && (*cSiLThick)->GetTime() < tMusc)
	++cSiLThick;
    for (iSec = 0; iSec < nSec; ++iSec) {
      if (sirThin[iSec])
	while (cSiRThin[iSec] != sirThin[iSec]->end() && (*(cSiRThin[iSec]))->GetTime() < tMusc)
	  ++(cSiRThin[iSec]);
      if (silThin[iSec])
	while (cSiLThin[iSec] != silThin[iSec]->end() && (*(cSiLThin[iSec]))->GetTime() < tMusc)
	  ++(cSiLThin[iSec]);
    }
    // Check for hits
    // Look for hits in thick detectors first
    if (sirThick) {                                                                      // If there was at least one hit in the thick detector
      while (cSiRThick != sirThick->end() && (*cSiRThick)->GetTime() - tMusc < tDecay) { // Look for hits in the decay window after muSc hit
	if ((*cSiRThick)->GetAmplitude() > eCutThick[0]) {                               // If a hit passes the amplitude cut
	  tSiRThick.push_back((*cSiRThick)->GetTime());                                  // Save the time
	  ampSiRThick.push_back((*cSiRThick)->GetAmplitude());                           // And the amplitude
	}
      }
    }
    if (silThick) {
      while (cSiLThick != silThick->end() && (*cSiLThick)->GetTime() - tMusc < tDecay) {
	if ((*cSiLThick)->GetAmplitude() > eCutThick[1]) {
	  tSiLThick.push_back((*cSiLThick)->GetTime());
	  ampSiLThick.push_back((*cSiLThick)->GetAmplitude());
	}
      }
    }
    if (tSiRThick.size() > 0) {                                                         // If there was at least one hit in the thick window
      nThickHits = tSiRThick.size();
      for (iHit = 0; iHit < nThickHits; ++iHit) {                                       // Loop through the times of all of the hits
	for (iSec = 0; iSec < nSec; ++iSec) {                                           // Loop through all the sections of the thin silicon
	  if (sirThin[iSec]) {                                                          // And, if there are hits in this thin detector
	    while ((*(cSiRThin[iSec]))->GetTime() - tSiRThick[iHit] < -tCoincidence)    // Catch the iterator up to the beginning of the time window
	      ++(cSiRThin[iSec]);
	    while ((*(cSiRThin[iSec]))->GetTime() - tSiRThick[iHit] < tCoincidence)     // And look for hits in this window
	      if ((*(cSiRThin[iSec]))->GetAmplitude() > eCutThin[0][iSec]) {            // If a hit is found in the thin above threshold
		hEvdE[0]->Fill(ampSiRThick[iHit], (*(cSiRThin[iSec]))->GetAmplitude()); // Fill the histogram
		break;
	      }
	  }
	}
      }
    }
    if (tSiLThick.size() > 0) {
      nThickHits = tSiLThick.size();
      for (iHit = 0; iHit < nThickHits; ++iHit) {
	for (iSec = 0; iSec < nSec; ++iSec) {
	  if (silThin[iSec]) {
	    while ((*(cSiLThin[iSec]))->GetTime() - tSiLThick[iHit] < -tCoincidence)
	      ++(cSiLThin[iSec]);
	    while ((*(cSiLThin[iSec]))->GetTime() - tSiLThick[iHit] < tCoincidence)
	      if ((*(cSiLThin[iSec]))->GetAmplitude() > eCutThin[1][iSec]) {
		hEvdE[1]->Fill(ampSiLThick[iHit], (*(cSiLThin[iSec]))->GetAmplitude());
		break;
	      }
	  }
	}
      }
    }
  }
  return 0;
}
