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
    /*
        int foo[7] = {12, 54, 7, 38, 9, 432924803, 90201203};
    std::cout << "\n";
    std::cout << "mmr  " << hash_utils::void_to_int(foo, 7*sizeof(int)) <<std::endl;
    //std::cout << "32   " << hash_utils::hash32(foo, 7*sizeof(int)) <<std::endl;
    hash_utils::MurmurHash3 mmr;
    for (int i =0 ; i < 7; ++i ){
      //Byte_t* bar = (Byte_t*) foo;
      mmr.Add(i+((Byte_t*)foo),sizeof(char));
      //mmr.Add((Byte_t*)&foo[i], sizeof(char));
    }
    hash_utils::MurmurHash3 mmr2;
    mmr2.Add((Byte_t*)foo, 7 * sizeof(char));
    std::cout << "MMR  " << mmr.End() << std::endl;
    std::cout << "MMR2 " << mmr2.End() << std::endl;
    */
  }


};  //namespace tut
