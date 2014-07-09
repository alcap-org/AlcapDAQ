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

    std::string string_ch0("Ge-F");
    std::string string_ch1("ge-f");
    std::string string_ch2("ge_f");
    IDs::channel ch0(string_ch0);
    IDs::channel ch1(string_ch1);
    IDs::channel ch2(string_ch2);
    ensure_equals("Ge-F == ge-f", ch0,ch1);    
    ensure_equals("Ge-F == ge_f", ch0,ch2);    
    ensure_equals("ge_f == ge-f", ch2,ch1);    

    ensure_equals("IDs::channel(\"Ge-F\").str() should give Ge-F", ch0.str(),string_ch0);    
    ensure_equals("IDs::channel(\"ge-f\").str() should give Ge-F", ch1.str(),string_ch0);    
    ensure_equals("IDs::channel(\"ge_f\").str() should give Ge-F", ch2.str(),string_ch0);    

    IDs::channel muSc_string("muSc");
    IDs::channel SiR1_S_string("SiR1-S");
    ensure("Ge-F should be fast", ch0.isFast());    
    ensure_equals("muSc should be neither fast or slow", muSc_string.SlowFast(),IDs::kNotApplicable);    
    ensure("SiR1-S should be slow", SiR1_S_string.isSlow());    

    IDs::channel rubbish("ohsbfpsihbf-f");
    ensure("ohsbfpsihbf-f should be invalid", !rubbish.isValid());    
  }
}


