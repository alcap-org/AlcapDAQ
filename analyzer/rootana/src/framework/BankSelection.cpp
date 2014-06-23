#include "BankSelection.h"
#include <iostream>

//----------------------------------------------------------------------
BankSelection::BankSelection(bool match_all)
  :  fWildCards(match_all ? 1 : 0)
{}

//----------------------------------------------------------------------
BankSelection::~BankSelection()
{}

//----------------------------------------------------------------------
bool BankSelection::WildCardMatch(const SourceID& sid)
{
  return true;
}
//----------------------------------------------------------------------
bool BankSelection::ExactMatch(const SourceID& sid)
{
  return true;
}


//----------------------------------------------------------------------
// emacs metadata
// Local Variables:
// mode: c++
// End:
