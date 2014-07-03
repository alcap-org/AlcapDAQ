#include "tut.h"

//ROOT
#include "TRandom3.h"

// Local
#include "MurmurHash3.h"
//#include <iostream>

namespace tut
{
  struct HASH
  {};

  typedef test_group<HASH> HASH_group;
  HASH_group HASH_pet("Hash implementation tests");

  namespace {
    typedef HASH_group::object tester;
  };


  //====================================================================
  using hash::MurmurHash3;

  //--------------------------------------------------------------------
  //Check that the hash is repeatable
  template<> template<>
  void tester::test<1>()
  {
    enum {nReps = 4};
    TRandom3 random(0);
    Double_t data[nReps];
    MurmurHash3 hasher1;   int h1;
    MurmurHash3 hasher2;   int h2;
    for (int i = 0; i < nReps; ++i){
      hash::MsgPtr_t msg= hash::MsgPtr(data[i]);
      h1 = hasher1.Add(data[i], sizeof(double)).Value();
      h2 = hasher2.Add(data[i], sizeof(double)).Value();
      ensure_equals("Add() is reproducable",h1, h2);
      h1= hasher1.Value();
      ensure_equals("Value() doesen't change state", h1, h2);
    }
    h2= hasher2.End();
    ensure_equals("First End() returns same as Value()", h1, h2);
 
 }
  

  //--------------------------------------------------------------------
  template<> template<>
  void tester::test<2>()
  {
    
  }


};  //namespace tut
