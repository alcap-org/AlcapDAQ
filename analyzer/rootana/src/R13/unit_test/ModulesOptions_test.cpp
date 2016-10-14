#include "tut.h"

// Normally theres should be no aditional I/O during tests, but it
// can be useful when writing the class
//#include <iostream>

#include "ModulesOptions.h"
#include <string>

namespace tut
{
  struct MODULESOPTIONS
  {
    // This struct will be initailiased at the start of each test.
    // Use this to set up common data inputs etc...
  };

  typedef test_group<MODULESOPTIONS> MODULESOPTIONS_group;
  MODULESOPTIONS_group MODULESOPTIONS_pet("modules::options tests");

  namespace {
    // We use the unnamed namespace here so that the typename "tester"
    // can be reused by every test.
    typedef MODULESOPTIONS_group::object tester;
  }

  // Test numbers can run from 1 to 50. They will be performed in 
  // order, but need not be sequential.  So if you have different
  // categories of tests they can be grouped by category. e.g. 
  // 01~10:  Basic tests
  // 11~20:  Advanced tests
  // 21~30:  [...]
  //--------------------------------------------------------------------
  template<> template<>
  void tester::test<1>()
  {
      //set_test_name("optional_parameters_not_set");

      modules::options opts("test1");
      opts.SetOption("int","1");
      opts.SetOption("double","1.6");
      opts.SetOption("string","o");

      ensure_equals("default int", opts.GetInt("none",2),2);
      ensure_equals("default double", opts.GetDouble("none",2.34),2.34);
      ensure_equals("default string", opts.GetString("none","something with spaces"),std::string("something with spaces"));
      ensure_equals("default bool number", opts.GetBool("none","1"),true);
      ensure_equals("default bool alpha", opts.GetBool("none","TRUE"),true);
  }
  
  //--------------------------------------------------------------------
  template<> template<>
  void tester::test<2>()
  {
      //set_test_name("optional_parameters_set");

      modules::options opts("test1");
      opts.SetOption("int","5");
      opts.SetOption("double","1.6");
      opts.SetOption("string","  testing  ");
      opts.SetOption("bool","false");

      ensure_equals( "set int"    , opts.GetInt   ( "int"    , 2                       ) , 5         );
      ensure_equals( "set double" , opts.GetDouble( "double" , 2.34                    ) , 1.6       );
      ensure_equals( "set string" , opts.GetString( "string"   , "something with spaces ") , std::string("testing") );
      ensure_equals( "set bool "  , opts.GetBool  ( "bool"   , "1"                     ) , false     );
  }
  
  //--------------------------------------------------------------------
  template<> template<>
  void tester::test<3>()
  {
      //set_test_name("compulsory_parameters_no_range_check");

      modules::options opts("test1");
      opts.SetOption( "int"    , "1"         ) ;
      opts.SetOption( "double" , "1.6"       ) ;
      opts.SetOption( "string" , "something" ) ;
      opts.SetOption( "bool"   , "true"      ) ;
      opts.SetOption( "flag"   , ""      ) ;

      ensure_equals("compulsory int"    , opts.GetInt   ("int"    ) , 1           ) ;
      ensure_equals("compulsory double" , opts.GetDouble("double" ) , 1.6         ) ;
      ensure_equals("compulsory string" , opts.GetString("string" ) , "something" ) ;
      ensure_equals("compulsory bool"   , opts.GetBool  ("bool"   ) , true        ) ;
      ensure_equals("compulsory flag"   , opts.GetFlag  ("flag"   ) , true        ) ;

      /// Check missing args throw
      try{
          opts.GetInt("_int");
          fail("GetInt should have thrown missing_option");
      }catch(modules::missing_option& e){}
      try{
          opts.GetDouble("_double");
          fail("GetDouble should have thrown missing_option");
      }catch(modules::missing_option& e){}
      try{
          opts.GetBool("_bool");
          fail("GetBool should have thrown missing_option");
      }catch(modules::missing_option& e){}
      try{
          opts.GetString("_string");
          fail("GetString should have thrown missing_option");
      }catch(modules::missing_option& e){}
  }

  template<> template<>
  void tester::test<4>()
  {
      //set_test_name("compulsory_parameters_range_check");

      modules::options opts("test1");
      opts.SetOption( "int"    , "1"   ) ;
      opts.SetOption( "double" , "1.6" ) ;

      // should pass this test
      ensure_equals("compulsory int"    , opts.GetInt   ("int"    , "int==1"                ) , 1   ) ;
      ensure_equals("compulsory double" , opts.GetDouble("double" , "double<3 && double >0" ) , 1.6 ) ;
      ensure_equals("compulsory double" , opts.GetDouble("double" , "x<3 && x>=0" ) , 1.6 ) ;

      /// Check bad args throw
      try{
          opts.GetInt("int","int<0");
          fail("GetInt should have thrown bad_value");
      }catch(modules::bad_value& e){}
      try{
          opts.GetDouble("double", "double!=1.6");
          fail("GetDouble should have thrown bad_value");
      }catch(modules::bad_value& e){}
  }

};  //namespace tut
