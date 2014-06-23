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

  struct SELECTION
  {
    BankSelection::SourceList_t foobar_list;
    BankSelection::SourceList_t wild_list;
    const int Ndet;

    //Initialise anything everyting that is generally useful here.
    //Constructor is called in every test so if it is expensive make it static
    SELECTION()
      : Ndet(22)
    {
      foobar_list.reserve(Ndet*4*2);
      for (int d = 0; d < Ndet; ++d){
        for (int f = 0; f < 4; ++f){
          //list.push_back(SourceID(ChannelID(d,f),GeneratorID("foo","bar")));
          IDs::Detector_t det = static_cast<IDs::Detector_t>(d);
          IDs::SlowFast_t sf = static_cast<IDs::SlowFast_t>(f);
          foobar_list.push_back(SourceID(det,sf,"foo","bar"));
          wild_list.push_back(SourceID(det,sf,IDs::kAnyGenerator, IDs::kAnyConfig));
        }
      }
    }
 
    virtual ~SELECTION()
    {
      foobar_list.clear();
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
    ensure ("List size sanity check1", foobar_list.size() > 0);    
    ensure ("List size sanity check2", wild_list.size() > 0);    
  }

  //----------------------------------------------------------------------
  //Test constructors
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

    msg= "Wildcard set does not Include() ";
    for (size_t i = 0 ; i < foobar_list.size(); ++i){
      ensure(msg + foobar_list[i].str(),m_all.Includes(foobar_list[i]));
    }
    
  }
  
  template<> template<>
  void tester::test<12>()
  {
    BankSelection::SourceList_t sources;
    sources.reserve(10);
    //differ by slow/fast & generator
    sources.push_back(SourceID(IDs::kGe, IDs::kFast,"ge_fast", "default"));
    sources.push_back(SourceID(IDs::kGe, IDs::kSlow,"ge_slow", "default"));

    //differ by detector
    sources.push_back(SourceID(IDs::kScGe, IDs::kNotApplicable, "general", "default"));
    sources.push_back(SourceID(IDs::kScL, IDs::kNotApplicable,"general", "default"));
    
    //a channel wildcard 
    sources.push_back(SourceID(IDs::kAnyDetector, IDs::kFast, "general", "default"));

    //differ by generator
    sources.push_back(SourceID(IDs::kMuSc, IDs::kNotApplicable,"general", "default"));
    sources.push_back(SourceID(IDs::kMuSc, IDs::kNotApplicable,"beam", "default"));

    //another channel wild card
    sources.push_back(SourceID(IDs::kSiR2, IDs::kAnySlowFast, "general", "default"));

    //differ by config
    sources.push_back(SourceID(IDs::kMuSc, IDs::kNotApplicable,"magic", "dougal"));
    sources.push_back(SourceID(IDs::kMuSc, IDs::kNotApplicable,"magic", "zebedee"));

    BankSelection mixed(sources);
    ensure_equals("Has 2 wildcards", mixed.fWildCards.size(), 2u);
    ensure_equals("Has 8 exact matches", mixed.fMatches.size(), 8u);
  }

  //----------------------------------------------------------------------
  //Check that MatchAll() and MatchNone() do as advertised 
  template<> template<>
  void tester::test<13>()
  {
    BankSelection m_none; m_none.MatchNone();
    BankSelection m_all(0); m_all.MatchAll();

    std::string msg("MatchNone() set does not Include() ");
    for (size_t i = 0 ; i < foobar_list.size(); ++i){
      ensure(msg + foobar_list[i].str(),!m_none.Includes(foobar_list[i]));
    }

    msg= "MatchAll() set does not Include() ";
    for (size_t i = 0 ; i < foobar_list.size(); ++i){
      ensure(msg + foobar_list[i].str(),m_all.Includes(foobar_list[i]));
    }
    

  }
};
