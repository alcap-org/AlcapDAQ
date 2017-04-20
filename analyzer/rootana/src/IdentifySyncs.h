#ifndef IDENTIFYSYNCS_H_
#define IDENTIFYSYNCS_H_

#include "BaseModule.h"
#include "IdBoard.h"
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
 public:
  IdentifySyncs(modules::options* opts);
  // Want to see if compiles without this
  // ~IdentifySyncs();

 private:
  int ProcessEntry(TGlobalData *gData, const TSetupData *gSetup);
  int BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup);
  int AfterLastEntry(TGlobalData* gData,const TSetupData *setup) { return 0; }

  std::map< IDs::board, std::vector<IDs::source> > fBoardMap;
};

#endif //IDENTIFYSYNCS_H_
