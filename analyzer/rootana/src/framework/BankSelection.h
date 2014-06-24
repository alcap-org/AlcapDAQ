#ifndef BANKSELECTION_H
#define BANKSELECTION_H

#include "definitions.h"

/// This class represents a (possibly wildcard-like) selection of
/// Source IDs.  It can be modified by appending SourceIDs.
/// 
/// @detail Assume that the most common reason for using wild cards
/// (or not) is to match multiple detector channels.  In contrast
/// people asking for specific generators will likely have specific
/// intentions in mind, and the output of particular generators will
/// often be used for specific channels.  It will be faster to check
/// against wildcards than long lists of sources, so we store source
/// wildcards separately.
///
/// @detail As this class has no pointers, the default copy semantics
/// are reasonable.
///
/// @remark We don't currently allow Intersection or Inverted matching
/// of elements. The current design does not know the set of elements
/// matching a wildcard, so a logical "anything except xyz" would
/// require additional lists of negated channels This may be
/// revisited if there is a clear need.
class BankSelection 
{
public:
  typedef IDs::source SourceID;
  typedef std::vector<SourceID> SourceList_t;

  /// The default-constructed selection matches all banks of the correct pulse type
  /// \param match_all [in] if optionally passed \c false or \c 0 it will match none.
  BankSelection(bool match_all = true);

  /// Constuct matching the the list given
  BankSelection(const SourceList_t& list);

  /// Destructior. We may inherit from this class
  virtual ~BankSelection();

  // Returns true if the Iterator match criteria have been fixed. 
  //bool IsLocked() const {return fLocked;}
  
  // Fix the match criteria of banks this Iterator will return.
  //BankSelection& Lock() {fLocked =1; return *this;}

  // /// Return the number of matching banks according to the current
  // /// criteria. This is not necessarily cheap.
  //int Size() const;

  /// Check if the provided SourceID is in the selection, either due
  /// to wildcards or exact matches
  inline bool Includes(const SourceID& sid) const
  {return HasWildCardMatch(sid) || HasExactMatch(sid);}

  /// Check if the provided SourceID would be selected by the (channel) wildcards
  inline bool HasWildCardMatch(const SourceID& sid) const
  {return ListMatch(sid, fWildCards);}

  /// Check if the provided SourceID is on the list of exact (channel) matches
  inline bool HasExactMatch(const SourceID& sid) const
  {return ListMatch(sid, fMatches);}

  /// Reset match criteria - accept everything. 
  /// Note that once the Selection is Lock()ed the criteria cannot be
  /// changed.
  /// @return *this
  BankSelection& MatchAll();

  /// Reset match criteria - reject everything.
  /// Note that once the Selection is Lock()ed the criteria cannot be
  /// changed.
  /// @return *this
  BankSelection& MatchNone();

  /// Set the match criteria to the provided list, overwriting the
  /// previous criteria
  /// @return *this
  BankSelection& MatchOnly(const SourceList_t& list);

  // /// Only match Sources that are also on the provided list of Source IDs
  // /// Logical: SELF = SELF & LIST
  // /// @return *this
  // BankSelection& Intersect(SourceList_t list);

  /// Add Sources from the provided list of Source IDs 
  /// Logicaly this looks like SELF = SELF | LIST 
  /// Note however, that this will add a Source ID even if
  /// an exact equal exists.  Thus (for example)
  /// @c myBankSelection.Add(listA).Add(listA); will append @c listA twice
  /// To eliminate the duplicates, use BankSelection::Compact()
  /// @return *this
  BankSelection& Add(const SourceList_t& list);

  /// Remove any exact duplicates from the selection.  This should not
  /// change the logic, but may speed up searches if many duplicates
  /// have been added.
  BankSelection& Compact();

  /// Remove Sources if they are on the provided list of Source IDs
  /// \warning This removes IDs based on exact equality of IDs, not
  /// logical negation. Thus it cannot be used to exclude a group of
  /// channels from a wildcard match.  
  /// Note: It also causes Compact() to be called, in order to eliminate
  /// duplicates
  ///@return *this
  BankSelection& Remove(const SourceList_t& list);

  /// \overload BankSelection& Remove(const SourceList_t& list) 

  /// \param removed [out] is an optional parameter that lists all the
  /// IDs Removed() from the selection, If all parameters in list are
  /// unique, and all were removed then @p removed.size() = @p list.size()
  BankSelection& Remove(const SourceList_t& list, SourceList_t& removed);
  

private:
  typedef SourceList_t::iterator iter; 
  typedef SourceList_t::const_iterator citer; 

  struct iter_pair { iter wildcard; iter match; };

  ///Convenience function that checks if a given Source ID matches any
  ///from a list
  bool ListMatch(const SourceID& sid, const SourceList_t& list) const;

  ///Convenience function that removes duplicate SourceIDs from a list
  void SortUnique(SourceList_t& list);
  
  ///Convenience function to remove a list and return the new ends
  /// first is wildcards second is exacts
  ///Must call erase after!
  //const std::pair<iter, iter> 
  iter_pair ImpRemove(const SourceList_t& list);

  //citer cBegin(SourceList_t& list) const
  //{ return const_cast<const SourceList_t&>(list).begin(); }

  /// The list of Source IDs that match several Channels
  SourceList_t fWildCards;

  /// The list of SourceIDs tha match a single Channel
  SourceList_t fMatches;

};

#endif //BANKSELECTION_H


// emacs metadata
// Local Variables:
// mode: c++
// End:
