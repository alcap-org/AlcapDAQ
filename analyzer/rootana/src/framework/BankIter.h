#ifndef BANKITER_H
#define BANKITER_H

//C++/STL

//ROOT
#include <TTree.h>

//Local
#include "definitions.h"
#include "BankSelection.h"


template <typename BANK> class BankIter {
  /// This class provides sequential access to various banks stored by
  /// the EventNavigator.  As far as we are concerned here a bank is
  /// something like a PulseIslandList == vector<TPulseIsland*>
private: 
  typedef IDs::source SourceID;

public:

  /// C'tors /D'tors
  BankIter() {};
  virtual ~BankIter(){};

  /// Returns true if the Iterator match criteria have been fixed. 
  // bool IsLocked() const {return fLocked;}

  /// Return the number of matching banks according to the current
  /// criteria. This is not necessarily cheap.
  int Count() const;

  /// Fix the match criteria of banks this Iterator will return.
  //BankIter& Lock() {fLocked =1; return *this;}

  /// Get the next Bank that fufills out filter criteria
  /// Side Effects: Lock()s this Iterator.
  const BANK* Next();
  
  /// Go back to start, and return self. So to get the first element do 
  /// someBankIter.Rewind().Next(). 
  /// Side Effects: Lock()s this Iterator.
  BankIter& Rewind();

  /// TODO: probably it is better ho have a criteria object.  What
  /// shoild the logic of this be?

protected:
  BankIter(const BankIter&);
  BankIter& operator= (const BankIter &);

  ///Get the appropriate tree for the 
  TTree* GetTree();

private:
  /// The match criteria the BankIterator adheres to. 
  const BankSelection fCriteria;

  ///The TTree this iterator iterates over
  TTree* fTree;

};

typedef BankIter<PulseIslandList> PulseIslandBankIter;
typedef BankIter<AnalysedPulseList> AnalysedPulseBankIter;
typedef BankIter<DetectorPulseList> DetectorPulseBankIter;

#endif  //BANKITER_H

// emacs metadata
// Local Variables:
// mode: c++
// End:
