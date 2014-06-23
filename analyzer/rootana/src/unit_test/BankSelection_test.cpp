#include "tut.h"

// Make everything in the test class public so we can test at lesure.
// Evil has its uses.
#define private public
#define protected public
#include "BankSelection.h"
#undef protected
#undef private

#include <typeinfo>


namespace tut
{
    typedef IDs::source SourceID;
    typedef IDs::channel ChannelID;
    typedef IDs::generator GeneratorID;

  struct SELECTION
  {};

  typedef test_group<SELECTION> SELECTION_group;
  SELECTION_group SELECTION_pet("BankSelection tests");

  typedef SELECTION_group::object tester;

  //======================================================================
  //Test constructors
  template<> template<>
  void tester::test<11>()
  {
    BankSelection::SourceList_t list;

    const int Ndet =22;
    list.reserve(Ndet*4*2);
    for (int d = 0; d < Ndet; ++d){
      for (int f = 0; f < 4; ++f){
        //list.push_back(SourceID(ChannelID(d,f),GeneratorID("foo","bar")));
        IDs::Detector_t det = static_cast<IDs::Detector_t>(d);
        IDs::SlowFast_t sf = static_cast<IDs::SlowFast_t>(f);
        list.push_back(BankSelection::SourceID(det,sf,"foo","bar"));
      }
    }

    BankSelection m_none(0);
    BankSelection m_all;
        
    std::string msg("Empty set does not Include() ");
    for (size_t i = 0 ; i < list.size(); ++i){
      //ensure(msg + list[i].str(),m_all.Includes(list[i]));
      ensure(msg + list[i].str(),!m_none.Includes(list[i]));
    }
    msg= "Wildcard set does not Include() ";
    for (size_t i = 0 ; i < list.size(); ++i){
      ensure(msg + list[i].str(),m_all.Includes(list[i]));
      //ensure(msg + list[i].str(),!m_none.Includes(list[i]));
    }
      //DummyBankSelection m_none(0);
    /*
    ensure("Starts Locked", !dummy.IsLocked());
    ensure_equals("Lock() returns self", &(dummy.Lock()), &dummy);
    ensure("Is locked after Lock()", dummy.IsLocked());
    DummyBankSelection dummy2;
    dummy2.Next();
    ensure("Is locked after Next()", dummy2.IsLocked());
    
    DummyBankSelection dummy3;
    ensure_equals("Rewind() returns self", &(dummy3.Rewind()),&dummy3);
    ensure("Is locked after Rewind()", dummy3.IsLocked());
    */  
  }

  //Check that a Lock()ed Iter cannot be modified 
  template<> template<>
  void tester::test<12>()
  {
    //typedef BankSelection<void*> DummyBankSelection;

    //DummyBankSelection dummy;
    //ummy.Lock();

  }
};
