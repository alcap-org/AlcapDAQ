#include "tut.h"
#include "IdSource.h"


namespace tut 
{
  struct IDSOURCE
  {};

  typedef test_group<IDSOURCE> IDSOURCE_group;
  IDSOURCE_group IDSOURCE_pet("IDs::source tests");

  namespace {
    typedef IDSOURCE_group::object tester;
  }


  //Test isWildCard() methods
  template<> template<>
  void tester::test<1>()
  {
    IDs::generator g_set("dummy", "dummy");
    IDs::generator g_wild;
    IDs::channel c_set(IDs::kMuSc, IDs::kSlow);
    IDs::channel c_wild;


    IDs::source tame(c_set, g_set);
    ensure("All fixed is not wild", !tame.isWildCard());
    ensure("All fixed is not wild chan", !tame.isWildCardChannel());
    ensure("All fixed is not wild gen", !tame.isWildCardGenerator());
    
    IDs::source gen_wild(c_set, g_wild);
    ensure("Generator wild is wild", gen_wild.isWildCard());
    ensure("Generator wild is not wild chan", !gen_wild.isWildCardChannel());
    ensure("Generator wild is wild gen", gen_wild.isWildCardGenerator());

    IDs::source chan_wild(c_wild, g_set);
    ensure("Channel wild is wild", chan_wild.isWildCard());
    ensure("Channel wild is wild chan", chan_wild.isWildCardChannel());
    ensure("Channel wild is not wild gen", !chan_wild.isWildCardGenerator());

    IDs::source all_wild(c_wild, g_wild);
    ensure("Doublely wild is wild", all_wild.isWildCard());    
    ensure("Doublely wild is wild chan", all_wild.isWildCardChannel());    
    ensure("Doublely wild is wild gen", all_wild.isWildCardGenerator());    
  }
}


