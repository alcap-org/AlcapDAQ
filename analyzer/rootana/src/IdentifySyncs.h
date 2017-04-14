#ifndef IDENTIFYSYNCS_H_
#define IDENTIFYSYNCS_H_

#include "BaseModule.h"
#include "IdSource.h"
namespace modules {class options;}

////////////////////////////////////////////////////////////////////////////////
/// \ingroup rootana_modules
/// \author John R Quirk
///
/// \brief
/// Identify synchronization pulses. Either create database or read in database.
////////////////////////////////////////////////////////////////////////////////
class IdentifySyncs : public BaseModule {
  enum Board {
    D4,         D5,         D7,         T4,
    SIS3350_B1, SIS3350_B2, SIS3300_B1, SIS3300_B2,
    SIS3300_B3, SIS3300_B4, SIS3300_B5, SIS3301_B6,
    NBRD
  };
  static const std::string BOARDSTR[NBRD];
 public:
  IdentifySyncs(modules::options* opts);
  // Want to see if compiles without this
  // ~IdentifySyncs();

 private:
  int ProcessEntry(TGlobalData *gData, const TSetupData *gSetup);
  int BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup);
  int AfterLastEntry(TGlobalData* gData,const TSetupData *setup) { return 0; }

  std::map< Board, std::vector<IDs::source> > fBoardMap;
};

#endif //IDENTIFYSYNCS_H_
