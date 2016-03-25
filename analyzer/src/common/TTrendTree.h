#ifndef TTrendTree_h__
#define TTrendTree_h__

/* Standard includes */
#include <cassert>

/* ROOT includes */
class TTree;

class TTrendTree {
 public:
  TTrendTree();
  void Init(int run);
  template<class T> void FillRunTree(const char* branchname, T value);
  template<class T> void FillBlockTree(const char* branchname, T value);
  void Finish();

 private:
  TTree* fRunTree;
  TTree* fBlockTree;
  double fValue;

};

template<class T> void TTrendTree::FillRunTree(const char* brname, T value) {
  if (!fRunTree) return;
  assert(!fRunTree->FindBranch(brname));
  TBranch* br = fRunTree->Branch(brname, &value);
  br->Fill();
}

template<class T> void TTrendTree::FillBlockTree(const char* brname, T value) {
  if (!fBlockTree) return;
  TBranch* br = fBlockTree->FindBranch(brname);
  if (!br)
    br = fBlockTree->Branch(brname, &value);
  br->Fill();
}

#endif
