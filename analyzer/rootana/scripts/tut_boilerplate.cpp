#include "tut.h"

//C Normally theres should be no aditional I/O during tests, but it
//C can be useful when writing the class
//#include <iostream>

namespace tut
{
  struct GROUP_NAME
  {
    //C This struct will be initailiased at the start of each test.
    //C Use this to set up common data inputs etc...
  };

  typedef test_group<GROUP_NAME> GROUP_NAME_group;
  GROUP_NAME_group pet("TEST_DESCRIPTION");

  namespace {
    //C We use the unnamed namespace here to that the typename "tester"
    //C can be reused by every test.
    typedef GROUP_NAME_group::object tester;
  }

  //C Test numbers can run from 1 to 50. They will be performed in 
  //C order, but need not be sequential.  So if you have different
  //C categories of tests they can be grouped by category. e.g. 
  //C 01~10:  Basic tests
  //C 11~20:  Advanced tests
  //C 21~30:  [...]
  //--------------------------------------------------------------------
  template<> template<>
  void tester::test<1>()
  {
    //C See src/unit_test/example_test.cpp for examples of test assertions 
    //C uncaught throws are also detected as failures.     
    //C If you want to set the test name use: "set_test_name("some_name");"
  }
  

  //--------------------------------------------------------------------
  template<> template<>
  void tester::test<2>()
  {

  }


};  //namespace tut
