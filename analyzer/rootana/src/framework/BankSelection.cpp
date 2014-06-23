#include "BankSelection.h"
#include <iostream>

//----------------------------------------------------------------------
BankSelection::BankSelection(bool match_all)
  :  fWildCards(match_all ? 1 : 0)
{}


//----------------------------------------------------------------------
BankSelection::BankSelection(const SourceList_t& list)
{
  fWildCards.reserve(list.size()/4);
  fMatches.reserve(list.size()/4);
  
  for (citer it = list.begin(); it != list.end(); ++it) {
    //TODO! need to rebase first
    //if (*it).
  }

}
//----------------------------------------------------------------------
BankSelection::~BankSelection()
{}

//----------------------------------------------------------------------
bool BankSelection::WildCardMatch(const SourceID& sid)
{
  for (citer it = cBegin(fWildCards); it != fWildCards.end() ; ++it) {
    if (sid.matches(*it)) return true;
}

 return false;
}

//----------------------------------------------------------------------
bool BankSelection::ExactMatch(const SourceID& sid)
{
  for (citer it = cBegin(fMatches); it != fMatches.end() ; ++it) {
    if (sid.matches(*it)) return true;
  }
  return false;
}


//----------------------------------------------------------------------
BankSelection& BankSelection::MatchAll()
{
  fMatches.clear();
  SourceList_t tmp(1);
  fWildCards.swap(tmp);
  return *this;
}


//----------------------------------------------------------------------
BankSelection& BankSelection::MatchNone()
{
  fMatches.clear();
  fWildCards.clear();
  return *this;
}
