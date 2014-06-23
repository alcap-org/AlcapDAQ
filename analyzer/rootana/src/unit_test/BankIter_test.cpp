#include "tut.h"
#include "BankIter.h"
#include "BankIter.tpl"

template class ::BankIter<void*>;
typedef BankIter<void*> DummyBankIter;

namespace tut
{

  struct BANKITER
  {};

  typedef test_group<BANKITER> BANKITER_group;
  BANKITER_group BANKITER_pet("BankIter<> tests");

  typedef BANKITER_group::object tester;

  //01~10 : Checks template instantiations
  //11~20 : Check generic behavior
  //21~30 : for BankIters<TPulseIsland> specific-tests
  //31~40 : for BankIters<TAnalysedPulse> specific-tests
  //41~50 : for BankIters<TDetectorPulse> specific-tests

  //Check the iterator is in malleable 'undefined' state on creation
  // and  locked after acessing an element
  template<> template<>
  void tester::test<11>()
  {
    typedef BankIter<void*> DummyBankIter;

    DummyBankIter dummy;
    ensure("Starts Locked", !dummy.IsLocked());
    ensure_equals("Lock() returns self", &(dummy.Lock()), &dummy);
    ensure("Is locked after Lock()", dummy.IsLocked());

    DummyBankIter dummy2;
    dummy2.Next();
    ensure("Is locked after Next()", dummy2.IsLocked());
    
    DummyBankIter dummy3;
    ensure_equals("Rewind() returns self", &(dummy3.Rewind()),&dummy3);
    ensure("Is locked after Rewind()", dummy3.IsLocked());
  }

  //Check that a Lock()ed Iter cannot be modified 
  template<> template<>
  void tester::test<12>()
  {
    typedef BankIter<void*> DummyBankIter;

    DummyBankIter dummy;
    dummy.Lock();

  }
};
