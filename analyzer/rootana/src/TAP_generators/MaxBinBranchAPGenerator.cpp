#include "TAPGeneratorFactory.h"
#include "MaxBinBranchAPGenerator.h"
#include "TPulseIsland.h"
#include "TAnalysedPulse.h"
#include "EventNavigator.h"
#include "SetupNavigator.h"
#include "TSetupData.h"
#include "TTree.h"
#include <algorithm>
#include <iostream>

// IsTimeOrdered()
// -- Returns true of the first pulse is before the second
// Static so that only this file sees it
// Useful for sorting the pulses
static bool IsTimeOrdered(TAnalysedPulse* a, TAnalysedPulse* b) {
  // Want a to be before b
  return ( a->GetTime() < b->GetTime() );
}

//======================================================================
MaxBinBranchAPGenerator::MaxBinBranchAPGenerator(TAPGeneratorOptions* opts)
  : TVAnalysedPulseGenerator("MaxBinBranchAPGenerator", opts),
    fMaxBinAmplitude(SetupNavigator::Instance()->GetPedestal(GetChannel()),
		     TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(GetChannel().str()))),
    fMaxBinTime(TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(GetChannel().str())),
                TSetupData::Instance()->GetClockTick(TSetupData::Instance()->GetBankName(GetChannel().str())),
                0., // Time shift
                TSetupData::Instance()->GetDownSampling(GetBank().c_str(), SetupNavigator::Instance()->GetRunNumber())),
    fTree(new TTree("MaxBin", "MaxBin")), fAmp(0.), fTime(0.), fBlock(0),
    fRenamed(false) {
  fTree->Branch("e", &fAmp);
  fTree->Branch("t", &fTime);
  fTree->Branch("block", &fBlock);
}


//----------------------------------------------------------------------
int MaxBinBranchAPGenerator::ProcessPulses(const PulseIslandList& pulseList,
					   AnalysedPulseList& analysedList)
{
  if ( !(fRenamed || pulseList.empty()) ) {
    fRenamed = true;
    fTree->SetName(("MaxBin"+pulseList[0]->GetBankName()).c_str());
  }
    ++fBlock;

  for (PulseIslandList::const_iterator pulseIter = pulseList.begin(); pulseIter != pulseList.end(); pulseIter++) {
    fAmp  = fMaxBinAmplitude(*pulseIter);
    fTime = fMaxBinTime(*pulseIter);
    fTree->Fill();

    // Make the TAnalysedPulse pulse
    TAnalysedPulse* outPulse = MakeNewTAP(pulseIter-pulseList.begin());
    outPulse->SetAmplitude(fAmp);
    outPulse->SetTime(fTime);
    //    outPulse->SetEnergy(energy); //AE (3rd Aug 2014): Commenting this out for the time being since we don't really know how we're doing calibration. The equation used before was:   energy = fECalibSlope * amplitude + fECalibOffset;
    // Add the pulse into the list
    analysedList.push_back(outPulse);

  }
  std::sort(analysedList.begin(), analysedList.end(), IsTimeOrdered);
  return 0;
}
ALCAP_TAP_GENERATOR(MaxBinBranch);
