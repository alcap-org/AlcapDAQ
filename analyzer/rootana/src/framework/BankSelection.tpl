#include "BankSelection.h"
#include <iostream>

//----------------------------------------------------------------------
template <typename BANK>
BankSelection<BANK>::BankSelection(bool match_all)
  :  fWildCards(match_all ? 1 : 0)
{}

//----------------------------------------------------------------------
template <typename BANK>
BankSelection<BANK>::~BankSelection()
{}

//----------------------------------------------------------------------
//template <typename BANK>


//----------------------------------------------------------------------
// emacs metadata
// Local Variables:
// mode: c++
// End:
