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

 public:
  IdentifySyncs(modules::options* opts);
  ~IdentifySyncs();

 private:
  virtual int ProcessEntry(TGlobalData *gData, const TSetupData *gSetup);
  virtual int BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup);
  virtual int AfterLastEntry(TGlobalData* gData,const TSetupData *setup);

  std::map< std::string, std::vector<IDs::source> > fBoardMap;
};

#endif //IDENTIFYSYNCS_H_
