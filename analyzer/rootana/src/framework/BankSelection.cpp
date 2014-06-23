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
  for (citer it = cBegin(fWildCards); it != fWildCards.end() ; ++it) {
    //if (sid.Matches(*it)) return true;
    if (sid == (*it)) return true;
}

 return false;
}
//----------------------------------------------------------------------
bool BankSelection::ExactMatch(const SourceID& sid)
{
  for (citer it = cBegin(fMatches); it != fMatches.end() ; ++it) {
    //if (sid.Matches(*it)) return true;
    if (sid == (*it)) return true;
  }
  return false;
}


//----------------------------------------------------------------------
// emacs metadata
// Local Variables:
// mode: c++
// End:
