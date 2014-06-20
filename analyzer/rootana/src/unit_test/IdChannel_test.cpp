#include "tut.h"
#include "IdChannel.h"


namespace tut 
{
  struct IDCHAN
  {};

  typedef test_group<IDCHAN> IDCHAN_group;
  IDCHAN_group IDCHAN_pet("IDs::channel tests");

  namespace {
    typedef IDCHAN_group::object tester;
  }


  //Test isWildCard() methods
  template<> template<>
  void tester::test<1>()
  {
    IDs::channel tame(IDs::kMuSc,IDs::kSlow);
    ensure("All fixed is not wild", !tame.isWildCard());
    ensure("All fixed is not wild det", !tame.isWildCardDetector());
    ensure("All fixed is not wild S/F", !tame.isWildCardSlowFast());
    
    IDs::channel sf_wild(IDs::kMuSc, IDs::kAnySlowFast);
    ensure("Slow/Fast wild is wild", sf_wild.isWildCard());
    ensure("Slow/Fast wild is not wild det", !sf_wild.isWildCardDetector());
    ensure("Slow/Fast wild is wild S/F", sf_wild.isWildCardSlowFast());

    IDs::channel det_wild(IDs::kAnyDetector, IDs::kSlow);
    ensure("Detector wild is wild", det_wild.isWildCard());
    ensure("Detector wild is wild det", det_wild.isWildCardDetector());
    ensure("Detector wild is not wild S/F", !det_wild.isWildCardSlowFast());

    IDs::channel all_wild(IDs::kAnyDetector, IDs::kAnySlowFast);
    ensure("Doublely wild is wild", all_wild.isWildCard());    
    ensure("Doublely wild is wild det", all_wild.isWildCardDetector());    
    ensure("Doublely wild is wild S/F", all_wild.isWildCardSlowFast());    
  }
}


