#include "BankSelection.h"
#include <iostream>
#include <algorithm>
#include <functional>

namespace {
  bool unary_iwcc(const BankSelection::SourceID& s) 
    {return s.isWildCardChannel();}

  struct functor_match {
    functor_match(const BankSelection::SourceID& sid)
      : fS(sid) {}
    bool operator() (const BankSelection::SourceID& s2) const
    { return fS.matches(s2); }
    const BankSelection::SourceID& fS;
  };
};



//----------------------------------------------------------------------
BankSelection::BankSelection(bool match_all)
  :  fWildCards(match_all ? 1 : 0)
{}



//----------------------------------------------------------------------
BankSelection::BankSelection(const SourceList_t& list)  
{
  this->MatchOnly(list);
}



//----------------------------------------------------------------------
BankSelection::~BankSelection()
{}



//----------------------------------------------------------------------
bool BankSelection::HasWildCardMatch(const SourceID& sid) const
{
  citer end = fWildCards.end();
  functor_match match(sid);
  if ( find_if(fWildCards.begin(), end, match) != end ) return true;
  return false;
}



//----------------------------------------------------------------------
bool BankSelection::HasExactMatch(const SourceID& sid) const
{
  citer end = fMatches.end();
  functor_match match(sid);
  if ( find_if(fMatches.begin(), end, match) != end ) return true;
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


//----------------------------------------------------------------------
BankSelection& BankSelection::MatchOnly(const SourceList_t& list)
{
  fMatches.clear();
  fWildCards.clear();
  int n_wc = std::count_if(list.begin(), list.end(), unary_iwcc);
  fWildCards.reserve(n_wc);
  fMatches.reserve(list.size() - n_wc);
  for (citer it = list.begin(); it != list.end(); ++it){
    if ( unary_iwcc(*it) ) {
      fWildCards.push_back(*it);
    } else {
      fMatches.push_back(*it); 
    }
  }
  return *this;
}
