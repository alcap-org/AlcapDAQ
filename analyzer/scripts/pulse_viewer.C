#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>

#include "TFile.h"
#include "TTree.h"
#include "TH1I.h"
#include "TString.h"
#include "TCanvas.h"
#include "TAxis.h"

#include "../src/common/TPulseIsland.h"
#include "../src/common/TGlobalData.h"
#include "../src/common/TSetupData.h"

char GetInput() {
  std::map<char, std::string> opts;
  opts['n'] = std::string("Next pulse");
  opts['p'] = std::string("Previous pulse");
  opts['N'] = std::string("Next block");
  opts['P'] = std::string("Previous block");
  opts['d'] = std::string("Choose a different detector");
  opts['b'] = std::string("Enter block number");
  opts['q'] = std::string("Quit");
  char choice = '\0';
  while (!opts.count(choice)) {
    for (std::map<char,std::string>::iterator i = opts.begin(); i != opts.end(); ++i)
      std::cout << i->first << ":\t" << i->second << std::endl;
    std::cin >> choice;
  }
  return choice;
}

int ChooseDetector(const std::vector<std::string>& dets) {
  std::cout << "Choose detector:" << std::endl;
  int choice = 0;
  while (choice < 1 || choice > (int)dets.size()) {
    for (int i = 1; i <= (int)dets.size(); ++i)
      std::cout << i << ":\t" << dets.at(i-1) << std::endl;
    std::cin >> choice;
  }
  return choice - 1;
}

void view_pulses(std::string fname) {
  TGlobalData* gData = NULL;
  TSetupData* gSetup = NULL;

  TCanvas can;
  
  TFile f(fname.c_str(), "READ");
  
  TTree *events = NULL, *setup = NULL;
  events = (TTree*) f.Get("EventTree");
  setup = (TTree*) f.Get("SetupTree");
  
  events->SetBranchAddress("Event", &gData);
  setup->SetBranchAddress("Setup", &gSetup);
  setup->GetEntry(0);
  
  int nBlocks = events->GetEntries();

  // Build list of detectors based on the banks present in the events
  // and the setup data.
  std::cout << "Building list of detectors present for this run...";
  std::vector< std::string > dets;
  for (int iBlock = 0; iBlock < nBlocks; ++iBlock) {
    events->GetEntry(iBlock);
    std::map< std::string, std::vector<TPulseIsland*> >::iterator iBank;
    for (iBank = gData->fPulseIslandToChannelMap.begin(); iBank != gData->fPulseIslandToChannelMap.end(); ++iBank)
      if (!std::count(dets.begin(), dets.end(), gSetup->GetDetectorName(iBank->first)))
	dets.push_back(gSetup->GetDetectorName(iBank->first));
  }
  std::cout << "done." << std::endl;

  std::cout << "Sorting detector list...";
  std::sort(dets.begin(), dets.end());
  std::cout << "done." << std::endl;
  
  int det = ChooseDetector(dets);
  std::string bank;
  
  for (int iBlock = 0; iBlock < nBlocks; ++iBlock) {
    bank = gSetup->GetBankName(dets.at(det));
    events->GetEntry(iBlock);

    // Go to next block if there's no data for this detector
    if (!gData->fPulseIslandToChannelMap.count(bank)) {
      std::cout << "No hits for block " << iBlock + 1 << "." << std::endl;
      continue;
    }

    std::vector<TPulseIsland*> &pulses = gData->fPulseIslandToChannelMap.at(bank);
    int nPulses = (int)pulses.size();

    for (int iPulse = 0; iPulse < nPulses; ++iPulse) {
      std::vector<int> samps = pulses.at(iPulse)->GetSamples();
      TH1I pulse("pulse", (dets.at(det) + ";Time (ticks); ADC Value").c_str(), samps.size(), -0.5, samps.size() - 0.5);
      
      std::cout << "Time per Tick: " << gSetup->GetClockTick(bank) << std::endl;
      std::cout << "Block (Pulse): " << iBlock + 1 << "/" << nBlocks << " (" << iPulse + 1 << "/" << nPulses << ")" << std::endl;
      for (int iSample = 0; iSample < (int)samps.size(); iSample++)
	pulse.SetBinContent(iSample + 1, samps[iSample]);
      pulse.Draw();
      can.Update();
      char in = GetInput();
      switch (in) {
      case 'n' : // Next pulse
	break;
      case 'p' : // Previous pulse
	iPulse -= 2;
	if (iPulse < -1)
	  iPulse = -1;
	break;
      case 'N' : // Next block
	iPulse = nPulses;
	break;
      case 'P' : // Previous block
	iBlock -= 2;
	if (iBlock < -1)
	  iBlock = -1;
	iPulse = nPulses;
	break;
      case 'd' : // Choose a different detector
	det = ChooseDetector(dets);
	iBlock = -1;
	iPulse = nPulses;
	break;
      case 'b' : // Enter block number
	std::cout << "Block: ";
	std::cin >> iBlock;
	if (iBlock < 0 || iBlock >= nBlocks) {
	  std::cout << "Invalid block number. Going back to first block." << std::endl;
	  iBlock = -1;
	}
	iBlock -= 2;
	if (iBlock < -1)
	  iBlock = -1;
	iPulse = nPulses;
	break;
      case 'q' : // Quit
	iPulse = nPulses;
	iBlock = nBlocks;
	break;
      default : // Unknown choice
	std::cout << "Unknown option. Continuing to next pulse." << std::endl;
	break;
      }
    }
  }
}
