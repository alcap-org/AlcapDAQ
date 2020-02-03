// A TMuonEvent contains:
// -- a central muon (defined as a hit in one of the SiT channels above a certain energy cut)
// -- all pulses in all detectors within a certain time window of the central muon
// This header file is supposed to help setup reading the branches of our TMETree

#ifndef TMETreeBranches_h_
#define TMETreeBranches_h_

#include "TTree.h"

#include "AlCapConstants.h"
#include "src/plotters/SimplePulse.h"

#include "TBranch.h"
#include "TTree.h"

#include <vector>

// #ifdef __CINT__
// #pragma link C++ class SimplePulse+;
// #pragma link C++ class vector<SimplePulse >+;
// #pragma link C++ class vector< vector<SimplePulse >* >+;
// #endif


// Still to add:
// -- ideniftier of who created the tree and with which software
Int_t runId; // run number
Int_t blockId; // MIDAS event number
Int_t TMEId; // muon event number
Double_t TMEWindowWidth; // half width ofTMuonEvent window
std::string* centralMuonChannel; // SiT channel that the central muon was seen in (e.g. SiT-1-S)
Int_t centralMuonTPIID; // the ID of the central muon TPulseIsland
Double_t centralMuonEnergy; // energy of the central muon
Double_t centralMuonTime; // block time of the central muon
Double_t timeToPrevTME; // time from the central muon in this TME to the central muon in the previous TME (-1 if first TME)
Double_t timeToNextTME; // time from the central muon in this TME to the central muon in the next TME (-1 if last TME)
Bool_t anyDoubleCountedPulses; // have any pulses in this TMuonEvent been included in more than one TMuonEvent?

std::vector<SimplePulse>* muSc = 0; // all the pulses in this channel for this TMuonEvent

std::vector<SimplePulse>* SiT_1 = 0; // all the pulses in this channel for this TMuonEvent
std::vector<SimplePulse>* SiT_2 = 0;
std::vector<SimplePulse>* SiT_3 = 0;
std::vector<SimplePulse>* SiT_4 = 0;
std::vector< std::vector<SimplePulse>* > all_SiT_channels(n_SiT_channels);

std::vector<SimplePulse>* SiL1_1 = 0; // all the pulses in this channel for this TMuonEvent
std::vector<SimplePulse>* SiL1_2 = 0;
std::vector<SimplePulse>* SiL1_3 = 0;
std::vector<SimplePulse>* SiL1_4 = 0;
std::vector<SimplePulse>* SiL1_5 = 0;
std::vector<SimplePulse>* SiL1_6 = 0;
std::vector<SimplePulse>* SiL1_7 = 0;
std::vector<SimplePulse>* SiL1_8 = 0;
std::vector<SimplePulse>* SiL1_9 = 0;
std::vector<SimplePulse>* SiL1_10 = 0;
std::vector<SimplePulse>* SiL1_11 = 0;
std::vector<SimplePulse>* SiL1_12 = 0;
std::vector<SimplePulse>* SiL1_13 = 0;
std::vector<SimplePulse>* SiL1_14 = 0;
std::vector<SimplePulse>* SiL1_15 = 0;
std::vector<SimplePulse>* SiL1_16 = 0;
std::vector< std::vector<SimplePulse>* > all_SiL1_channels(n_SiL1_channels);

std::vector<SimplePulse>* SiL3 = 0;

std::vector<SimplePulse>* SiR1_1 = 0; // all the pulses in this channel for this TMuonEvent
std::vector<SimplePulse>* SiR1_2 = 0;
std::vector<SimplePulse>* SiR1_3 = 0;
std::vector<SimplePulse>* SiR1_4 = 0;
std::vector< std::vector<SimplePulse>* > all_SiR1_channels(n_SiR1_channels);

std::vector<SimplePulse>* SiR2 = 0;

std::vector<SimplePulse>* SiR3 = 0;

std::vector<SimplePulse>* GeLoGain = 0;
std::vector<SimplePulse>* GeHiGain = 0;

std::vector< std::vector<SimplePulse>* > all_channels(n_all_channels);

std::vector<SimplePulse>* empty = 0;

void SetTMEBranchAddresses(TTree* tmetree) {
  tmetree->SetBranchStatus("*", true);

  tmetree->SetBranchAddress("runId",                  &runId);
  tmetree->SetBranchAddress("blockId",                &blockId);
  tmetree->SetBranchAddress("TMEId",                  &TMEId);
  tmetree->SetBranchAddress("TMEWindowWidth",         &TMEWindowWidth);
  tmetree->SetBranchAddress("centralMuonChannel",     &centralMuonChannel);
  tmetree->SetBranchAddress("centralMuonTPIID",       &centralMuonTPIID);
  tmetree->SetBranchAddress("centralMuonEnergy",      &centralMuonEnergy);
  tmetree->SetBranchAddress("centralMuonTime",        &centralMuonTime);
  tmetree->SetBranchAddress("timeToPrevTME",          &timeToPrevTME);
  tmetree->SetBranchAddress("timeToNextTME",          &timeToNextTME);
  tmetree->SetBranchAddress("anyDoubleCountedPulses", &anyDoubleCountedPulses);
  tmetree->SetBranchAddress("muSc",                   &muSc);
  tmetree->SetBranchAddress("SiT_1",                  &SiT_1);
  tmetree->SetBranchAddress("SiT_2",                  &SiT_2);
  tmetree->SetBranchAddress("SiT_3",                  &SiT_3);
  tmetree->SetBranchAddress("SiT_4",                  &SiT_4);
  tmetree->SetBranchAddress("SiL1_1",                 &SiL1_1);
  tmetree->SetBranchAddress("SiL1_2",                 &SiL1_2);
  tmetree->SetBranchAddress("SiL1_3",                 &SiL1_3);
  tmetree->SetBranchAddress("SiL1_4",                 &SiL1_4);
  tmetree->SetBranchAddress("SiL1_5",                 &SiL1_5);
  tmetree->SetBranchAddress("SiL1_6",                 &SiL1_6);
  tmetree->SetBranchAddress("SiL1_7",                 &SiL1_7);
  tmetree->SetBranchAddress("SiL1_8",                 &SiL1_8);
  tmetree->SetBranchAddress("SiL1_9",                 &SiL1_9);
  tmetree->SetBranchAddress("SiL1_10",                &SiL1_10);
  tmetree->SetBranchAddress("SiL1_11",                &SiL1_11);
  tmetree->SetBranchAddress("SiL1_12",                &SiL1_12);
  tmetree->SetBranchAddress("SiL1_13",                &SiL1_13);
  tmetree->SetBranchAddress("SiL1_14",                &SiL1_14);
  tmetree->SetBranchAddress("SiL1_15",                &SiL1_15);
  tmetree->SetBranchAddress("SiL1_16",                &SiL1_16);
  tmetree->SetBranchAddress("SiL3",                   &SiL3);
  tmetree->SetBranchAddress("SiR1_1",                 &SiR1_1);
  tmetree->SetBranchAddress("SiR1_2",                 &SiR1_2);
  tmetree->SetBranchAddress("SiR1_3",                 &SiR1_3);
  tmetree->SetBranchAddress("SiR1_4",                 &SiR1_4);
  tmetree->SetBranchAddress("SiR2",                   &SiR2);
  tmetree->SetBranchAddress("SiR3",                   &SiR3);
  tmetree->SetBranchAddress("GeLoGain",               &GeLoGain);
  tmetree->SetBranchAddress("GeHiGain",               &GeHiGain);

  tmetree->GetEntry(0); // This sets the addresses for CollectChannels
  empty = new std::vector<SimplePulse>;
}

void CollectChannels() {

  // collect all the names
  int i_counter = 0;
  for (int i_SiT_channel = 0; i_SiT_channel < n_SiT_channels; ++i_SiT_channel) {
    all_channel_names[i_counter++] = all_SiT_channel_names[i_SiT_channel];
  }
  for (int i_SiL1_channel = 0; i_SiL1_channel < n_SiL1_channels; ++i_SiL1_channel) {
    all_channel_names[i_counter++] = all_SiL1_channel_names[i_SiL1_channel];
  }
  for (int i_SiL3_channel = 0; i_SiL3_channel < n_SiL3_channels; ++i_SiL3_channel) {
    all_channel_names[i_counter++] = all_SiL3_channel_names[i_SiL3_channel];
  }
  for (int i_SiR1_channel = 0; i_SiR1_channel < n_SiR1_channels; ++i_SiR1_channel) {
    all_channel_names[i_counter++] = all_SiR1_channel_names[i_SiR1_channel];
  }
  for (int i_SiR2_channel = 0; i_SiR2_channel < n_SiR2_channels; ++i_SiR2_channel) {
    all_channel_names[i_counter++] = all_SiR2_channel_names[i_SiR2_channel];
  }
  for (int i_SiR3_channel = 0; i_SiR3_channel < n_SiR3_channels; ++i_SiR3_channel) {
    all_channel_names[i_counter++] = all_SiR3_channel_names[i_SiR3_channel];
  }
  for (int i_GeLoGain_channel = 0; i_GeLoGain_channel < n_GeLoGain_channels; ++i_GeLoGain_channel) {
    all_channel_names[i_counter++] = all_GeLoGain_channel_names[i_GeLoGain_channel];
  }
  for (int i_GeHiGain_channel = 0; i_GeHiGain_channel < n_GeHiGain_channels; ++i_GeHiGain_channel) {
    all_channel_names[i_counter++] = all_GeHiGain_channel_names[i_GeHiGain_channel];
  }

  // SiT
  i_counter = 0;
  all_SiT_channels[i_counter++] = SiT_1;
  all_SiT_channels[i_counter++] = SiT_2;
  all_SiT_channels[i_counter++] = SiT_3;
  all_SiT_channels[i_counter++] = SiT_4;

  // SiL1
  i_counter = 0;
  all_SiL1_channels[i_counter++] = SiL1_1;
  all_SiL1_channels[i_counter++] = SiL1_2;
  all_SiL1_channels[i_counter++] = SiL1_3;
  all_SiL1_channels[i_counter++] = SiL1_4;
  all_SiL1_channels[i_counter++] = SiL1_5;
  all_SiL1_channels[i_counter++] = SiL1_6;
  all_SiL1_channels[i_counter++] = SiL1_7;
  all_SiL1_channels[i_counter++] = SiL1_8;
  all_SiL1_channels[i_counter++] = SiL1_9;
  all_SiL1_channels[i_counter++] = SiL1_10;
  all_SiL1_channels[i_counter++] = SiL1_11;
  all_SiL1_channels[i_counter++] = SiL1_12;
  all_SiL1_channels[i_counter++] = SiL1_13;
  all_SiL1_channels[i_counter++] = SiL1_14;
  all_SiL1_channels[i_counter++] = SiL1_15;
  //  all_SiL1_channels[i_counter++] = SiL1_16;

  // SiR1
  i_counter = 0;
  all_SiR1_channels[i_counter++] = SiR1_1;
  all_SiR1_channels[i_counter++] = SiR1_2;
  all_SiR1_channels[i_counter++] = SiR1_3;
  all_SiR1_channels[i_counter++] = SiR1_4;

  // Full list
  i_counter = 0;
  all_channels[i_counter++] = muSc;
  all_channels[i_counter++] = SiT_1;
  all_channels[i_counter++] = SiT_2;
  all_channels[i_counter++] = SiT_3;
  all_channels[i_counter++] = SiT_4;

  all_channels[i_counter++] = SiL1_1;
  all_channels[i_counter++] = SiL1_2;
  all_channels[i_counter++] = SiL1_3;
  all_channels[i_counter++] = SiL1_4;
  all_channels[i_counter++] = SiL1_5;
  all_channels[i_counter++] = SiL1_6;
  all_channels[i_counter++] = SiL1_7;
  all_channels[i_counter++] = SiL1_8;
  all_channels[i_counter++] = SiL1_9;
  all_channels[i_counter++] = SiL1_10;
  all_channels[i_counter++] = SiL1_11;
  all_channels[i_counter++] = SiL1_12;
  all_channels[i_counter++] = SiL1_13;
  all_channels[i_counter++] = SiL1_14;
  all_channels[i_counter++] = SiL1_15;
  // all_channels[i_counter++] = SiL1_16;

  all_channels[i_counter++] = SiL3;

  all_channels[i_counter++] = SiR1_1;
  all_channels[i_counter++] = SiR1_2;
  all_channels[i_counter++] = SiR1_3;
  all_channels[i_counter++] = SiR1_4;

  all_channels[i_counter++] = SiR2;

  all_channels[i_counter++] = SiR3;

  all_channels[i_counter++] = GeLoGain;
  all_channels[i_counter++] = GeHiGain;
}


#endif
