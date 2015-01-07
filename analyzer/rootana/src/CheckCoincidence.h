#ifndef CheckCoincidence_h__
#define CheckCoincidence_h__

#include "FillHistBase.h"
#include "TGlobalData.h"

class CheckCoincidence : public FillHistBase{
        typedef std::map<std::string, bool> CoincidenceList;
 public:
  CheckCoincidence(char *HistogramDirectoryName,TSetupData* setup);
  ~CheckCoincidence();

 private:
  virtual int ProcessEntry(TGlobalData *gData);

  CoincidenceList fDetectors;
  CoincidenceList fBanks;
};

#endif

