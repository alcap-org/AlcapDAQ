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
  //typedef BankSelection::SourceList_t SourceList_t;

  namespace ID = IDs;

  struct SELECTION
  {
    BankSelection::SourceList_t foobar_list;
    BankSelection::SourceList_t wild_list;
    BankSelection::SourceList_t mixed_list;
    const int Ndet;

    //Initialise anything everyting that is generally useful here.
    //Constructor is called in every test so if it is expensive make it static
    SELECTION()
      : Ndet(22)
    {
      foobar_list.reserve(Ndet*4*2);
      wild_list.reserve(Ndet*4*2);
      for (int d = 0; d < Ndet; ++d){
        for (int f = 0; f < 4; ++f){
          //list.push_back(SourceID(ChannelID(d,f),GeneratorID("foo","bar")));
          ID::Detector_t det = static_cast<ID::Detector_t>(d);
          ID::SlowFast_t sf = static_cast<ID::SlowFast_t>(f);
          foobar_list.push_back(SourceID(det,sf,"foo","bar"));
          wild_list.push_back(SourceID(det,sf,ID::kAnyGenerator, ID::kAnyConfig));
        }
      }
      mixed_list.reserve(10);
    //differ by slow/fast & generator
    mixed_list.push_back(SourceID(ID::kGe, ID::kFast,"ge_fast", "default"));
    mixed_list.push_back(SourceID(ID::kGe, ID::kSlow,"ge_slow", "default"));

    //differ by detector
    mixed_list.push_back(SourceID(ID::kScGe, ID::kNotApplicable, "general", "default"));
    mixed_list.push_back(SourceID(ID::kScL, ID::kNotApplicable,"general", "default"));
    
    //a channel wildcard 
    mixed_list.push_back(SourceID(ID::kAnyDetector, ID::kFast, "general", "default"));

    //differ by generator
    mixed_list.push_back(SourceID(ID::kMuSc, ID::kNotApplicable,"general", "default"));
    mixed_list.push_back(SourceID(ID::kMuSc, ID::kNotApplicable,"beam", "default"));

    //another channel wild card
    mixed_list.push_back(SourceID(ID::kSiR2, ID::kAnySlowFast, "general", "default"));

    //differ by config
    mixed_list.push_back(SourceID(ID::kMuSc, ID::kNotApplicable,"magic", "dougal"));
    mixed_list.push_back(SourceID(ID::kMuSc, ID::kNotApplicable,"magic", "zebedee"));

      
    }
 
    virtual ~SELECTION()
    {
      foobar_list.clear();
      wild_list.clear();
    }
   
  };

  typedef test_group<SELECTION> SELECTION_group;
  SELECTION_group SELECTION_pet("BankSelection tests");

  typedef SELECTION_group::object tester;

  //======================================================================
  // Preconditions / test data sanity checks
  template<> template<>
  void tester::test<1>()
  {
    ensure_greaterthan("List size sanity check 1", foobar_list.size(), 0);    
    ensure_greaterthan ("List size sanity check 2", wild_list.size(), 0);
    ensure_greaterthan ("List size sanity check 3", mixed_list.size(), 0);    
  }

  //----------------------------------------------------------------------
  //Test simple constructors
  template<> template<>
  void tester::test<11>()
  {
    BankSelection m_none(0);
    BankSelection m_all;
        
    std::string msg("Empty set does not Include() ");
    for (size_t i = 0 ; i < foobar_list.size(); ++i){
      ensure(msg + foobar_list[i].str(),!m_none.Includes(foobar_list[i]));
      ensure(msg + wild_list[i].str(),!m_none.Includes(wild_list[i]));
    }

    msg= "Wildcard set Include()s ";
    for (size_t i = 0 ; i < foobar_list.size(); ++i){
      ensure(msg + foobar_list[i].str(),m_all.Includes(foobar_list[i]));
    }
    
  }
  
  //Test explicit constructor
  template<> template<>
  void tester::test<12>()
  {
    BankSelection mixed(mixed_list);
    ensure_equals("Has 2 wildcards", mixed.fWildCards.size(), 2u);
    ensure_equals("Has 8 exact matches", mixed.fMatches.size(), 8u);
    
    
    std::string msg;
    msg = "Explicit set does Include()s ";
    for (size_t i =0; i < mixed_list.size(); ++i){
      ensure(msg + mixed_list[i].str(),mixed.Includes(mixed_list[i]));
    }
  }

  //----------------------------------------------------------------------
  //Check that MatchAll() and MatchNone() do as advertised 
  template<> template<>
  void tester::test<13>()
  {
    BankSelection m_none; m_none.MatchNone();
    BankSelection m_all(0); m_all.MatchAll();

    std::string msg;
    msg ="MatchNone() set does not Include() ";
    for (size_t i = 0 ; i < foobar_list.size(); ++i){
      ensure(msg + foobar_list[i].str(),!m_none.Includes(foobar_list[i]));
    }

    msg = "MatchAll() set does not Include() ";
    for (size_t i = 0 ; i < foobar_list.size(); ++i){
      ensure(msg + foobar_list[i].str(),m_all.Includes(foobar_list[i]));
    }
  }
  //----------------------------------------------------------------------
  //Check that MatchOnly() does as advertised
  template<> template<>
  void tester::test<14>()
  {
    BankSelection mixed;
    mixed.MatchOnly(mixed_list);
    ensure_equals("Has 2 wildcards", mixed.fWildCards.size(), 2u);
    ensure_equals("Has 8 exact matches", mixed.fMatches.size(), 8u);
    
    std::string msg;
    msg = "Should not match ";
    SourceID sid(ID::kScVe,ID::kNotApplicable,"general","default");
    ensure(msg + sid.str(), !mixed.Includes(sid));
    
    sid = SourceID(ID::kMuSc,ID::kNotApplicable,"wrong","default");
    ensure(msg + sid.str(), !mixed.Includes(sid));

    
    msg = "Should match ";
    sid = SourceID(ID::kMuSc,ID::kNotApplicable,"general","default");
    ensure(msg + sid.str(), mixed.Includes(sid));

    sid = SourceID(ID::kSiR1_4, ID::kFast, "general", "default");
    ensure(msg + sid.str(), mixed.Includes(sid));
  }

  //----------------------------------------------------------------------
  //Check Compact()
  template<> template<>
  void tester::test<16>()
  {
    BankSelection::SourceList_t repeat_list(10, SourceID(ID::kScVe,
                                                         ID::kNotApplicable,
                                                         "G","D"));
    BankSelection repeat1(repeat_list);
    ensure_equals("Has repeats before Compact()",
                  repeat1.fMatches.size(), repeat_list.size());   
    repeat1.Compact();
    ensure_equals("Fully Compact()s",  repeat1.fMatches.size(), 1);        


    for (int i = 0; i < 8; i+=4){
      repeat_list[i] = SourceID(ID::kMuSc,ID::kNotApplicable,"G","D");
      repeat_list[i+2] = SourceID(ID::kMuSc,ID::kNotApplicable,"G","F");
    }
    //01 23 45 67 89
    //BA CA BA CA AA
    
    BankSelection repeat2(repeat_list);
    ensure_equals("Has repeats before Compact() 2",
                  repeat2.fMatches.size(), repeat_list.size());   
    repeat2.Compact();
    ensure_equals("Fully Compacts() 2",  repeat2.fMatches.size(), 3);       


    repeat_list[3] = SourceID(ID::kAnyDetector,ID::kAnySlowFast,"G","D");
    repeat_list[7] = SourceID(ID::kMuSc,ID::kAnySlowFast,"G","D");
    repeat_list[9] = SourceID(ID::kMuSc,ID::kAnySlowFast,"G","D");
    //01 23 45 67 89
    //BA C1 BA C2 A2


    BankSelection repeat3(repeat_list);
    ensure_equals("Has repeats with w/c before Compact()",
                  repeat3.fMatches.size() + repeat3.fWildCards.size(),
                  repeat_list.size());   
    repeat3.Compact();
    
    ensure_equals("Fully Compacts() with w/c 1",  
                  repeat3.fMatches.size(), 3);

    ensure_equals("Fully Compacts() with w/c 2",  
                  repeat3.fWildCards.size(), 2);
    
    //repeated compaction test
    repeat1.Compact();
    ensure_equals("Repeat Compact()",  repeat1.fMatches.size(), 1);       
    
    repeat2.Compact().Compact();
    ensure_equals("Repeat Compact()",  repeat2.fMatches.size(), 3);       
    
    repeat3.Compact().Compact().Compact();
    ensure_equals("Repeat Compact()",  
                  repeat3.fMatches.size() + repeat3.fWildCards.size(), 5);       
    
  }
};


//TODO: Test Add, Remove
