#include "BankIter.h"
#include <iostream>


//----------------------------------------------------------------------
template<typename BANK> 
const BANK* BankIter<BANK>::Next()
{
  this->Lock();
  return new BANK;
}


//----------------------------------------------------------------------
template<typename BANK>
BankIter<BANK>& BankIter<BANK>::Rewind()
{
  this->Lock();
  return *this;
}


// emacs metadata
// Local Variables:
// mode: c++
// End:
