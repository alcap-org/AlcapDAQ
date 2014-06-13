#include "tut.h"
#include "BankIter.h"
namespace tut
{
  struct BANKITER
  {};

  typedef test_group<BANKITER> BANKITER_group;
  BANKITER_group pet("BankIter<> tests");

  typedef BANKITER_group::object tester;

  //01~10 : Checks template instantiations
  //11~20 : Check generic behavior
  //21~30 : for BankIters<TPulseIsland> specific-tests
  //31~40 : for BankIters<TAnalysedPulse> specific-tests
  //41~50 : for BankIters<TDetectorPulse> specific-tests

  //Check the iterator is in malleable 'undefined' state on creation
  template<> template<>
  void tester::test<11>()
  {
    typedef BankIter<void*> DummyBankIter;

    //PulseIslandBankIter pibi;
    DummyBankIter dummy;
    ensure("Starts undefined", !dummy.IsDefined());
    ensure_equals("Define() returns self", &(dummy.Define()), &dummy);
    ensure("Is locked after Define()", dummy.IsDefined());

    DummyBankIter dummy2;
    //dummy2.Next();
    //ensure("Is locked after Next()", dummy2.IsDefined());
    
    PulseIslandBankIter dummy3;
    //ensure_equals("Rewind() returns self", &(dummy3.Rewind()),&dummy3());
    //ensure("Is locked after Rewind()", dummy3.IsDefined());
  }

  template<> template<>
  void tester::test<12>()
  {
    PulseIslandBankIter pibi;
    //ensure_equals("Starts undefined", pibi.Rewind().IsDefined(), 0);
  }
};
