#ifndef MakeMuonEvents_h__
#define MakeMuonEvents_h__

#include "FillHistBase.h"
#include "TGlobalData.h"
namespace modules{
class options;
}

class MakeMuonEvents : public FillHistBase{
 public:
  MakeMuonEvents(char *HistogramDirectoryName,modules::options* opts,TSetupData* setup) ;
  ~MakeMuonEvents();

 private:
  virtual int ProcessEntry(TGlobalData *gData);

  std::map<std::string,std::string > fDetectors;
};

#endif

