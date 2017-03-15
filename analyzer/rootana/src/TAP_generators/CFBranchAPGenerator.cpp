#include "TAPGeneratorFactory.h"
#include "CFBranchAPGenerator.h"
#include "TPulseIsland.h"
#include "TAnalysedPulse.h"
#include "EventNavigator.h"
#include "SetupNavigator.h"
#include "TSetupData.h"
#include "TTree.h"
#include "ExportPulse.h"

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
CFBranchAPGenerator::CFBranchAPGenerator(TAPGeneratorOptions* opts)
  : TVAnalysedPulseGenerator("CFBranchAPGenerator", opts),
    fMaxBinAmplitude(SetupNavigator::Instance()->GetPedestal(GetChannel()),
		     TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(GetChannel().str()))),
    fMaxBinTime(TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(GetChannel().str())),
                TSetupData::Instance()->GetClockTick(TSetupData::Instance()->GetBankName(GetChannel().str())),
                0., // Time shift
                TSetupData::Instance()->GetDownSampling(GetBank().c_str(), SetupNavigator::Instance()->GetRunNumber())),
    fCFTime(SetupNavigator::Instance()->GetPedestal(GetChannel()),
      TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(GetChannel().str())),
      TSetupData::Instance()->GetClockTick(TSetupData::Instance()->GetBankName(GetChannel().str())),
      opts->GetBool("no_time_shift", false) ? 0. : SetupNavigator::Instance()->GetCoarseTimeOffset(GetSource()),
      TSetupData::GetDownSampling(GetBank().c_str(), SetupNavigator::Instance()->GetRunNumber()),
      opts->GetDouble("constant_fraction")),
    fTree(new TTree("MaxBinCF", "MaxBinCF")), fAmp(0.), fTime(0.), fBlock(0),
    fInitialized(false) {
  fTree->Branch("e", &fAmp);
  fTree->Branch("t", &fTime);
  fTree->Branch("block", &fBlock);
}


//----------------------------------------------------------------------
int CFBranchAPGenerator::ProcessPulses(const PulseIslandList& pulseList,
					   AnalysedPulseList& analysedList)
{
  if ( !fInitialized) {
    fInitialized = true;
    fTree->SetName(("MaxBinCF"+pulseList[0]->GetBankName()).c_str());
  }

  fBlock = EventNavigator::Instance().EntryNo();
  for (PulseIslandList::const_iterator pulseIter = pulseList.begin();
       pulseIter != pulseList.end(); ++pulseIter) {
    fAmp  = fMaxBinAmplitude(*pulseIter);
    fTime = fCFTime(*pulseIter);
    //if (fTime < 10e3 || fTime > 96e6) { // roughly cut out sync pulses
    //  continue;
    //}
    fTree->Fill();
    // if (ExportPulse::Instance())
    //   ExportPulse::Instance()->AddToExportList(TSetupData::Instance()->GetDetectorName((*pulseIter)->GetBankName()),
    //                                            pulseIter-pulseList.begin());
    //   //ExportPulse::Instance()->AddToExportList((*pulseIter)->GetBankName(), pulseIter-pulseList.begin());
  }
  return 0;
}
ALCAP_TAP_GENERATOR(CFBranch,constant_fraction,no_time_shift);
