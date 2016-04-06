#ifndef TTrendTree_h__
#define TTrendTree_h__

/* Standard includes */
#include <cassert>
#include <cstring>
#include <limits>
#include <vector>
#include <string>

/* ROOT includes */
#include <TTree.h>

class TTrendTree {
 public:
  TTrendTree();
  void Init();
  template<class T> void InitBlockBranch(const std::string& branchname,
					 T default_value);
  template<class T> void InitRunBranch(const std::string& branchname,
				       T default_value);
  template<class T> void FillBlockTree(const std::string& branchname, T value);
  template<class T> void FillRunTree(const std::string& branchname, T value);
  void EOB();
  void EOR();

 private:
  bool fActive;
  std::vector<void*>  fpBlockVals, fpRunVals;
  std::vector<void*>  fpBlockDefaults, fpRunDefaults;
  std::vector<size_t> fBlockSizeOfPtrs, fRunSizeOfPtrs;
  TTree fRunTree;
  TTree fBlockTree;

};

template<class T> void TTrendTree::InitBlockBranch(const std::string& brname,
						   T defval) {
  assert(!fBlockTree.FindBranch(brname.c_str()));
  fpBlockVals     .push_back(new T(defval));
  fpBlockDefaults .push_back(new T(defval));
  fBlockSizeOfPtrs.push_back(sizeof(T));
  fBlockTree.Branch(brname.c_str(), (T*)fpBlockVals.back());
}

template<class T> void TTrendTree::InitRunBranch(const std::string& brname,
						 T defval) {
  assert(!fRunTree.FindBranch(brname.c_str()));
  fpRunVals     .push_back(new T(defval));
  fpRunDefaults .push_back(new T(defval));
  fRunSizeOfPtrs.push_back(sizeof(T));
  fRunTree.Branch(brname.c_str(), (T*)fpRunVals.back());
}

template<class T> void TTrendTree::FillBlockTree(const std::string& brname,
						 T value) {
  if (!fActive) return;
  *(T*)fBlockTree.FindBranch(brname.c_str())->GetAddress() = value;
}

template<class T> void TTrendTree::FillRunTree(const std::string& brname,
					       T value) {
  if (!fActive) return;
  *(T*)fRunTree.FindBranch(brname.c_str())->GetAddress() = value;
}

#endif
