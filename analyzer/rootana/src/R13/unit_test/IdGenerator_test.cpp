#include "tut.h"
#include "IdGenerator.h"


namespace tut 
{
  struct IDGEN
  {};

  typedef test_group<IDGEN> IDGEN_group;
  IDGEN_group IDGEN_pet("IDs::generator tests");

  namespace {
    typedef IDGEN_group::object tester;
  }


  //Test isWildCard() methods
  template<> template<>
  void tester::test<1>()
  {
    IDs::generator tame("dummy","dummy");
    ensure("All fixed is not wild", !tame.isWildCard());
    ensure("All fixed is not wild type", !tame.isWildCardType());
    ensure("All fixed is not wild config", !tame.isWildCardConfig());
    
    IDs::generator conf_wild("dummy", IDs::kAnyConfig);
    ensure("Config wild is wild", conf_wild.isWildCard());
    ensure("Config wild is not wild type", !conf_wild.isWildCardType());
    ensure("Config wild is wild config", conf_wild.isWildCardConfig());

    IDs::generator type_wild(IDs::kAnyGenerator, "dummy");
    ensure("Type wild is wild", type_wild.isWildCard());
    ensure("Type wild is wild type", type_wild.isWildCardType());
    ensure("Type wild is not wild config", !type_wild.isWildCardConfig());

    IDs::generator all_wild(IDs::kAnyGenerator, IDs::kAnyConfig);
    ensure("Doublely wild is wild", all_wild.isWildCard());    
    ensure("Doublely wild is wild type", all_wild.isWildCardType());    
    ensure("Doublely wild is wild config", all_wild.isWildCardConfig());    
  }
}


