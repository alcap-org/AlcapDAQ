#ifndef BANKITER_H
#define BANKITER_H


#include "definitions.h"
template <typename BANK> class BankIter {
  /// This class provides sequential access to various banks stored by
  /// the EventNavigator.  As far as we are concerned here a bank is
  /// something like a PulseIslandList == vector<TPulseIsland*>
public:

  /// C'tors /D'tors
  BankIter(){fDefined =0;};
  virtual ~BankIter(){fDefined =0;};

  /// Returns true if the Iterator match criteria have been fixed. 
  bool IsDefined() const;

  /// Return the number of matching banks according to the current
  /// criteria. This is not necessarily cheap.
  int Count() const;

  /// Fix the match criteria of banks this Iterator will return.
  BankIter& Define();

  /// Get the next Bank that fufills out filter criteria
  /// Side Effects: Define()s this Iterator.
  const BANK* Next();
  

  /// Go back to start, and return self. So to get the first element do 
  /// someBankIter.Rewind().Next(). 
  /// Side Effects: Define()s this Iterator.
  BankIter& Rewind();

  /// Reset match criteria - accept everything. 
  /// Note that once the Iter is Define()ed the criteria cannot be
  /// changed.
  BankIter& MatchAll();

  /// Reset match criteria - reject everything.
  /// Note that once the Iter is Define()ed the criteria cannot be
  /// changed.
  BankIter& MatchNone();

  /// Modify match criteria - accept a list of banks to match,
  BankIter& MatchSources(SourceID* id_array, int size);

  /// Modify match criteria - accept a list of banks to match,
  BankIter& ExcludeSources(std::vector <SourceID>);

  /// TODO: probably it is better ho have a criteria object.  What
  /// shoild the logic of this be?

protected:
  BankIter(const BankIter&);
  BankIter& operator= (const BankIter &);

private:
  /// Whether the match criteria are defined.
  bool fDefined; 
};

typedef BankIter<PulseIslandList> PulseIslandBankIter;
typedef BankIter<AnalysedPulseList> AnalysedPulseBankIter;
typedef BankIter<DetectorPulseList> DetectorPulseBankIter;

#endif  //BANKITER_H

// emacs metadata
// Local Variables:
// mode: c++
// End:
