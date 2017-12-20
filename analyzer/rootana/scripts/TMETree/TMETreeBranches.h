// A TMuonEvent contains:
// -- a central muon (defined as a hit in one of the SiT channels above a certain energy cut)
// -- all pulses in all detectors within a certain time window of the central muon
// This header file is supposed to help setup reading the branches of our TMETree

#ifndef TMETreeBranches_h_
#define TMETreeBranches_h_

#include "AlCapConstants.h"
#include "src/plotters/SimplePulse.h"

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
std::vector<SimplePulse>* all_SiT_channels[n_SiT_channels];
  
//std::vector<SimplePulse>* SiL1_1 = 0; // all the pulses in this channel for this TMuonEvent
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
//std::vector<SimplePulse>* SiL1_16 = 0;
std::vector<SimplePulse>* all_SiL1_channels[n_SiL1_channels];

std::vector<SimplePulse>* SiL3 = 0;

std::vector<SimplePulse>* SiR1_1 = 0; // all the pulses in this channel for this TMuonEvent
std::vector<SimplePulse>* SiR1_2 = 0;
std::vector<SimplePulse>* SiR1_3 = 0;
std::vector<SimplePulse>* SiR1_4 = 0;
std::vector<SimplePulse>* all_SiR1_channels[n_SiR1_channels];
  
std::vector<SimplePulse>* SiR2 = 0;
  
std::vector<SimplePulse>* SiR3 = 0;

std::vector<SimplePulse>* GeLoGain = 0;
std::vector<SimplePulse>* GeHiGain = 0;
  
std::vector<SimplePulse>* all_channels[n_all_channels];


void SetTMEBranchAddresses(TTree* tmetree) {
    
  TBranch* br_runId = tmetree->GetBranch("runId");
  TBranch* br_blockId = tmetree->GetBranch("blockId");
    
  TBranch* br_TMEId = tmetree->GetBranch("TMEId");
  TBranch* br_TMEWindowWidth = tmetree->GetBranch("TMEWindowWidth");
  TBranch* br_centralMuonChannel = tmetree->GetBranch("centralMuonChannel");
  TBranch* br_centralMuonTPIID = tmetree->GetBranch("centralMuonTPIID");
  TBranch* br_centralMuonEnergy = tmetree->GetBranch("centralMuonEnergy");
  TBranch* br_centralMuonTime = tmetree->GetBranch("centralMuonTime");
  TBranch* br_timeToPrevTME = tmetree->GetBranch("timeToPrevTME");
  TBranch* br_timeToNextTME = tmetree->GetBranch("timeToNextTME");
  TBranch* br_anyDoubleCountedPulses = tmetree->GetBranch("anyDoubleCountedPulses");

  TBranch* br_muSc = tmetree->GetBranch("muSc");
  
  TBranch* br_SiT_1 = tmetree->GetBranch("SiT_1");
  TBranch* br_SiT_2 = tmetree->GetBranch("SiT_2");
  TBranch* br_SiT_3 = tmetree->GetBranch("SiT_3");
  TBranch* br_SiT_4 = tmetree->GetBranch("SiT_4");

  //  TBranch* br_SiL1_1 = tmetree->GetBranch("SiL1_1");
  TBranch* br_SiL1_2 = tmetree->GetBranch("SiL1_2");
  TBranch* br_SiL1_3 = tmetree->GetBranch("SiL1_3");
  TBranch* br_SiL1_4 = tmetree->GetBranch("SiL1_4");
  TBranch* br_SiL1_5 = tmetree->GetBranch("SiL1_5");
  TBranch* br_SiL1_6 = tmetree->GetBranch("SiL1_6");
  TBranch* br_SiL1_7 = tmetree->GetBranch("SiL1_7");
  TBranch* br_SiL1_8 = tmetree->GetBranch("SiL1_8");
  TBranch* br_SiL1_9 = tmetree->GetBranch("SiL1_9");
  TBranch* br_SiL1_10 = tmetree->GetBranch("SiL1_10");
  TBranch* br_SiL1_11 = tmetree->GetBranch("SiL1_11");
  TBranch* br_SiL1_12 = tmetree->GetBranch("SiL1_12");
  TBranch* br_SiL1_13 = tmetree->GetBranch("SiL1_13");
  TBranch* br_SiL1_14 = tmetree->GetBranch("SiL1_14");
  TBranch* br_SiL1_15 = tmetree->GetBranch("SiL1_15");
  //  TBranch* br_SiL1_16 = tmetree->GetBranch("SiL1_16");

  TBranch* br_SiL3 = tmetree->GetBranch("SiL3");

  TBranch* br_SiR1_1 = tmetree->GetBranch("SiR1_1");
  TBranch* br_SiR1_2 = tmetree->GetBranch("SiR1_2");
  TBranch* br_SiR1_3 = tmetree->GetBranch("SiR1_3");
  TBranch* br_SiR1_4 = tmetree->GetBranch("SiR1_4");

  TBranch* br_SiR2 = tmetree->GetBranch("SiR2");

  TBranch* br_SiR3 = tmetree->GetBranch("SiR3");

  TBranch* br_GeLoGain = tmetree->GetBranch("GeLoGain");
  TBranch* br_GeHiGain = tmetree->GetBranch("GeHiGain");

  br_runId->SetAddress(&runId);
  br_blockId->SetAddress(&blockId);
  // Check these branches exist because we could be being passed the NonTMETree
  if (br_TMEId) {
    br_TMEId->SetAddress(&TMEId);
  }
  if (br_TMEWindowWidth) {
    br_TMEWindowWidth->SetAddress(&TMEWindowWidth);
  }
  if (br_centralMuonChannel) {
    br_centralMuonChannel->SetAddress(&centralMuonChannel);
  }
  if (br_centralMuonTPIID) {
    br_centralMuonTPIID->SetAddress(&centralMuonTPIID);
  }
  if (br_centralMuonEnergy) {
    br_centralMuonEnergy->SetAddress(&centralMuonEnergy);
  }
  if (br_centralMuonTime) {
    br_centralMuonTime->SetAddress(&centralMuonTime);
  }
  if (br_timeToPrevTME) {
    br_timeToPrevTME->SetAddress(&timeToPrevTME);
  }
  if (br_timeToNextTME) {
    br_timeToNextTME->SetAddress(&timeToNextTME);
  }
  if (br_anyDoubleCountedPulses) {
    br_anyDoubleCountedPulses->SetAddress(&anyDoubleCountedPulses);
  }

  if (br_muSc) {
    br_muSc->SetAddress(&muSc);
  }

  if (br_SiT_1) {
    br_SiT_1->SetAddress(&SiT_1);
  }
  if (br_SiT_2) {
    br_SiT_2->SetAddress(&SiT_2);
  }
  if (br_SiT_3) {
    br_SiT_3->SetAddress(&SiT_3);
  }
  if (br_SiT_4) {
    br_SiT_4->SetAddress(&SiT_4);
  }

  //  if (br_SiL1_1) {
    //  br_SiL1_1->SetAddress(&SiL1_1);
  //  }
  if (br_SiL1_2) {
    br_SiL1_2->SetAddress(&SiL1_2);
  }
  if (br_SiL1_3) {
    br_SiL1_3->SetAddress(&SiL1_3);
  }
  if (br_SiL1_4) {
    br_SiL1_4->SetAddress(&SiL1_4);
  }
  if (br_SiL1_5) {
    br_SiL1_5->SetAddress(&SiL1_5);
  }
  if (br_SiL1_6) {
    br_SiL1_6->SetAddress(&SiL1_6);
  }
  if (br_SiL1_7) {
    br_SiL1_7->SetAddress(&SiL1_7);
  }
  if (br_SiL1_8) {
    br_SiL1_8->SetAddress(&SiL1_8);
  }
  if (br_SiL1_9) {
    br_SiL1_9->SetAddress(&SiL1_9);
  }
  if (br_SiL1_10) {
    br_SiL1_10->SetAddress(&SiL1_10);
  }
  if (br_SiL1_11) {
    br_SiL1_11->SetAddress(&SiL1_11);
  }
  if (br_SiL1_12) {
    br_SiL1_12->SetAddress(&SiL1_12);
  }
  if (br_SiL1_13) {
    br_SiL1_13->SetAddress(&SiL1_13);
  }
  if (br_SiL1_14) {
    br_SiL1_14->SetAddress(&SiL1_14);
  }
  if (br_SiL1_15) {
    br_SiL1_15->SetAddress(&SiL1_15);
  }
  //  if (br_SiL1_16) {
    //  br_SiL1_16->SetAddress(&SiL1_16);
  //  }

  if (br_SiL3) {
    br_SiL3->SetAddress(&SiL3);
  }

  if (br_SiR1_1) {
    br_SiR1_1->SetAddress(&SiR1_1);
  }
  if (br_SiR1_2) {
    br_SiR1_2->SetAddress(&SiR1_2);
  }
  if (br_SiR1_3) {
    br_SiR1_3->SetAddress(&SiR1_3);
  }
  if (br_SiR1_4) {
    br_SiR1_4->SetAddress(&SiR1_4);
  }

  if (br_SiR2) {
    br_SiR2->SetAddress(&SiR2);
  }

  if (br_SiR3) {
    br_SiR3->SetAddress(&SiR3);
  }

  if (br_GeLoGain) {
    br_GeLoGain->SetAddress(&GeLoGain);
  }
  if (br_GeHiGain) {
    br_GeHiGain->SetAddress(&GeHiGain);
  }
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
  //  all_SiL1_channels[i_counter++] = SiL1_1;
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

  //  all_channels[i_counter++] = SiL1_1;
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
  //  all_channels[i_counter++] = SiL1_16;

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
