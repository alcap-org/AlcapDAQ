/* Standard includes */
#include <cassert>

/* ROOT invludes */
#include <TBranch.h>
#include <TTree.h>

/* AlCap includes */
#include "TTrendTree.h"

TTrendTree::TTrendTree() : fRunTree(NULL), fBlockTree(NULL) {
}

void TTrendTree::Init(int run) {
  fRunTree   = new TTree("RunTrend", "Trends over run");
  fBlockTree = new TTree("BlockTrend", "Trends over block");
  FillRunTree("run", run);
}

void TTrendTree::Finish() {
  if (fRunTree) {
    fRunTree->SetEntries();
    fRunTree->Write();
    delete fRunTree;
    fRunTree = NULL;
  }
  if (fBlockTree) {
    fBlockTree->SetEntries();
    fBlockTree->Write();
    delete fBlockTree;
    fBlockTree = NULL;
  }
}
