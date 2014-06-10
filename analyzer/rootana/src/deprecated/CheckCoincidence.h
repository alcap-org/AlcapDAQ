#ifndef CheckCoincidence_h__
#define CheckCoincidence_h__

#include "BaseModule.h"
#include "TGlobalData.h"
namespace modules{
	class options;
}

class CheckCoincidence : public BaseModule{
        typedef std::map<std::string, bool> CoincidenceList;
 public:
  CheckCoincidence(char *HistogramDirectoryName,modules::options *opts,TSetupData* setup);
  ~CheckCoincidence();

 private:
  virtual int ProcessEntry(TGlobalData *gData);

  CoincidenceList fDetectors;
  CoincidenceList fBanks;
};

#endif

