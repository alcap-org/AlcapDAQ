/* Standard includes */
#include <cstring>

/* ROOT invludes */
#include <TBranch.h>
#include <TTree.h>

/* AlCap includes */
#include "TTrendTree.h"

TTrendTree::TTrendTree() : fActive(false), fpBlockVals(), fpRunVals(),
			   fpBlockDefaults(), fpRunDefaults(),
			   fBlockSizeOfPtrs(), fRunSizeOfPtrs(),
                           fRunTree("RunTrend", "Trends over run"),
                           fBlockTree("BlockTrend", "Trends over block") {
  fRunTree.SetDirectory(0);
  fBlockTree.SetDirectory(0);
}

void TTrendTree::Init() {
  fActive = true;
}

void TTrendTree::EOB() {
  fBlockTree.Fill();
  for (int i = 0; i < fpBlockVals.size(); ++i)
    memcpy(fpBlockVals[i], fpBlockDefaults[i], fBlockSizeOfPtrs[i]);
}

void TTrendTree::EOR() {
  fRunTree  .Fill();
  for (int i = 0; i < fpRunVals.size(); ++i)
    memcpy(fpRunVals[i], fpRunDefaults[i], fRunSizeOfPtrs[i]);  
  fRunTree  .Write();
  fBlockTree.Write();
  fRunTree  .Reset();
  fBlockTree.Reset();
}
