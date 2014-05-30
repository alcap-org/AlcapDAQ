#include "Normalization.h"
#include "TGlobalData.h"
#include "TSetupData.h"

#include <iostream>
#include <string>
#include <vector>
#include <map>

static TH1F* hNumberOfMuons;

Normalization::Normalization(char *HistogramDirectoryName, int threshold = 350) :
  FillHistBase(HistogramDirectoryName), fNMuons(0), fThreshold(threshold) {

  N_MU = 0;

  hNumberOfMuons = new TH1F("hNumberOfMuons", "hNumberOfMuons", 1,0,1);

  gDirectory->cd("/");
}

Normalization::~Normalization(){
}

int Normalization::ProcessEntry(TGlobalData *gData, TSetupData *gSetup) {

  // Make sure no other module changed N_MU
  if (N_MU != fNMuons) {
    std::cout << "ERROR: Normalization::N_MU changed unexpectadly!" << std::endl;
    return -1;
  }
  fNMuons = 0;

  // Find the bankname associated with muSc
  std::string bankname = "";
  std::map<std::string, std::string>& B2D = gSetup->fBankToDetectorMap;
  static std::map<std::string, std::string>::iterator iBank;
  for (iBank = B2D.begin(); iBank != B2D.end(); ++iBank) {
    if (iBank->second == "muSc") {
      bankname = iBank->first;
      break;
    }
  }

  // Make certain we found muSc
  if (bankname == "") {
    std::cout << "Warning: Could not find muSc pulses! (TPulseIsland* vector not found, N_MU set to zero)" << std::endl;
    Normalization::N_MU = 0;
    return 0;
  }

  // Count the muons
  std::vector<TPulseIsland*>& pulses = gData->fPulseIslandToChannelMap.at(bankname);
  static std::vector<TPulseIsland*>::iterator iPulse;
  for (iPulse = pulses.begin(); iPulse != pulses.end(); ++iPulse)
    if (IsOverThreshold(iPulse))
      ++fNMuons;

  N_MU = fNMuons;
  hNumberOfMuons->Fill("muons", fNMuons);

  return 0;
}

bool Normalization::IsOverThreshold(std::vector<TPulseIsland*>::iterator& pulse) {

  std::vector<int> samples = (*pulse)->GetSamples();
  static std::vector<int>::iterator iSample;
  for (iSample = samples.begin(); iSample != samples.end(); ++iSample)
    if (*iSample > fThreshold)
      return true;
  
  return false;
}

unsigned int Normalization::GetMuonCount() {
  return N_MU;
}

unsigned int Normalization::N_MU = 0;
