#ifndef BANKSELECTION_H
#define BANKSELECTION_H

#include "definitions.h"

/// @brief This class represents a (possibly wildcard-like) selection of
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

  /// @brief The default-constructed selection matches all banks of
  /// the correct pulse type \param match_all [in] if optionally
  /// passed \c false or \c 0 it will match none.
  BankSelection(bool match_all = true);

  /// Construct matching a single Source ID (can be wild card)
  BankSelection(const SourceID& sid);

  /// Construct matching the the list given
  BankSelection(const SourceList_t& list);

  /// Destructior. We may inherit from this class
  virtual ~BankSelection();

  /// @brief Check if the provided SourceID is in the selection, either due
  /// to wildcards or exact matches
  inline bool Includes(const SourceID& sid) const
  {return HasWildCardMatch(sid) || HasExactMatch(sid);}

  /// @brief Check if the provided SourceID would be selected by the
  /// (channel) wildcards
  inline bool HasWildCardMatch(const SourceID& sid) const
  {return ListMatch(sid, fWildCards);}

  /// @brief Check if the provided SourceID is on the list of exact
  /// (channel) matches
  inline bool HasExactMatch(const SourceID& sid) const
  {return ListMatch(sid, fMatches);}

  /// @brief Reset match criteria - accept everything. 
  /// @return (*this)
  BankSelection& MatchAll();

  /// @brief Reset match criteria - reject everything.
  /// @return (*this)
  BankSelection& MatchNone();

  /// @brief Set the match criteria to the provided list, overwriting the
  /// previous criteria
  /// @return (*this)
  BankSelection& MatchOnly(const SourceList_t& list);

  /// @brief Set the match criteria to the provided ID, overwriting the
  /// previous criteria
  /// @return (*this)
  BankSelection& MatchOnly(const SourceID& sid);

  // /// Only match Sources that are also on the provided list of Source IDs
  // /// Logical: SELF = SELF & LIST
  // /// @return *this
  // BankSelection& Intersect(SourceList_t list);

  /// @brief Add Sources from the provided list of Source IDs 
  /// @detail Logicaly this looks like SELF = SELF | LIST 
  /// Note however, that this will add a Source ID even if
  /// an exact equal exists.  Thus (for example)
  /// @c myBankSelection.Add(listA).Add(listA); will append @c listA twice
  /// To eliminate the duplicates, use BankSelection::Compact()
  /// @return *this
  BankSelection& Add(const SourceList_t& list);

  /// @brief Add the provided Source ID 
  /// @detail Logicaly this looks like SELF = SELF | LIST 
  /// Note however, that this will add a Source ID even if
  /// an exact equal exists.  Thus (for example)
  /// @c myBankSelection.Add(A).Add(A); will append @c A twice
  /// To eliminate the duplicates, use BankSelection::Compact()
  /// @return *this
  BankSelection& Add(const SourceID& sid);

  /// Remove any exact duplicates from the selection.  This should not
  /// change the logic, but may speed up searches if many duplicates
  /// have been added.
  /// @return *this
  BankSelection& Compact();

  /// Remove Sources if they are on the provided list of Source IDs
  /// \warning This removes IDs based on exact equality of IDs, not
  /// logical negation. Thus it cannot be used to exclude a group of
  /// channels from a wildcard match.  
  /// Note: It also causes Compact() to be called, in order to eliminate
  /// duplicates
  /// @return *this
  BankSelection& Remove(const SourceList_t& list);

  /// Remove Sources if they are on the provided list of Source IDs
  /// \warning This removes IDs based on exact equality of IDs, not
  /// logical negation. Thus it cannot be used to exclude a group of
  /// channels from a wildcard match.  
  /// Note: It also causes Compact() to be called, in order to eliminate
  /// duplicates
  /// \param removed [out] is an optional parameter that lists all the
  /// IDs Removed() from the selection, If all parameters in list are
  /// unique, and all were removed then @p removed.size() = @p list.size()
  BankSelection& Remove(const SourceList_t& list, SourceList_t& removed);
  

  /// Remove the given Source IDs. This overload of @overload
  /// BankSelection& Remove(const SourceList_t& list) has all the same
  /// caveats.
  BankSelection& Remove(const SourceID& sid);

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
