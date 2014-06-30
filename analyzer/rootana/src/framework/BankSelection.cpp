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
  
  struct functor_equals {
    functor_equals(const BankSelection::SourceID& sid)
      : fS(sid) {}
    bool operator() (const BankSelection::SourceID& s2) const
    {return fS == s2;}
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
BankSelection::BankSelection(const SourceID& sid)
{
  this->MatchOnly(sid);
}


//----------------------------------------------------------------------
BankSelection::~BankSelection()
{}


//----------------------------------------------------------------------
bool BankSelection::ListMatch(const SourceID& sid, 
                              const SourceList_t& list) const
{
  functor_match match(sid);
  if (find_if(list.begin(), list.end(), match) != list.end()) return true;
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
  int n_wc = std::count_if(list.begin(), list.end(), unary_iwcc);
  fWildCards.reserve(n_wc);
  fMatches.reserve(list.size() - n_wc);

  return this->MatchNone().Add(list);

}


//----------------------------------------------------------------------
BankSelection& BankSelection::MatchOnly(const SourceID& sid)
{
  return this->MatchNone().Add(sid);
}

//----------------------------------------------------------------------
BankSelection& BankSelection::Add(const SourceList_t& list)
{
  for (citer it = list.begin(); it != list.end(); ++it){
    this->Add(*it);
  }
  return *this;
}


//----------------------------------------------------------------------
BankSelection& BankSelection::Add(const SourceID& sid)
{
  if (unary_iwcc(sid)){
    fWildCards.push_back(sid);
  } else {
    fMatches.push_back(sid);
  }

  return *this;
}


//----------------------------------------------------------------------
void BankSelection::SortUnique(SourceList_t& list)
{
  std::sort(list.begin(), list.end());
  iter dupes = std::unique(list.begin(), list.end());  
  list.erase(dupes, list.end()); 
}


//----------------------------------------------------------------------
BankSelection& BankSelection::Compact()
{
  SortUnique(fWildCards);
  SortUnique(fMatches);
  return *this;
} 


//----------------------------------------------------------------------
BankSelection::iter_pair BankSelection::ImpRemove(const SourceList_t& list)
{
  // REM. This only removes the elements, it does not erase them!

  this->Compact();
  iter_pair back;
  back.wildcard = fWildCards.end();
  back.match = fMatches.end();
  for (citer it = list.begin(); it != list.end() ; ++it) {
    if ( unary_iwcc(*it) ){
      back.wildcard = std::remove(fWildCards.begin(), back.wildcard, (*it));
    } else {
      back.match = std::remove(fMatches.begin(), back.match, (*it));
    }
  }    
  return back;
}


//----------------------------------------------------------------------
BankSelection& BankSelection::Remove(const SourceList_t& list, 
                                     SourceList_t& removed)
{
  const iter_pair back = ImpRemove(list);

  //use swap/temporary in case some loon passes the same object as
  //both arguments
  SourceList_t tmp; 
  std::copy(back.wildcard, fWildCards.end(), tmp.end()); 
  std::copy(back.match, fMatches.end(), tmp.end());
  removed.swap(tmp);
  
  fWildCards.erase(back.wildcard, fWildCards.end());
  fMatches.erase(back.match, fMatches.end());
  return *this;
}


//----------------------------------------------------------------------
BankSelection& BankSelection::Remove(const SourceList_t& list)
{
  iter_pair back = ImpRemove(list);
  fWildCards.erase(back.wildcard, fWildCards.end());
  fMatches.erase(back.match, fMatches.end());
  return *this;
}

//----------------------------------------------------------------------
BankSelection& BankSelection::Remove(const SourceID& sid)
{
  return this->Remove(SourceList_t(1, sid));
}
