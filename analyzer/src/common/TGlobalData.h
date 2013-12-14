#ifndef TGlobalData_h__
#define TGlobalData_h__

#include <vector>
#include <map>
#include <string>

#include <TObject.h>
#include "TPulseIsland.h"

/** This class holds all of the data created by analysis modules.
  */
class TGlobalData : public TObject{
 public:
  void Clear(Option_t* opt = "");
  
  // Map of bank names to vectors of islands.
  std::map<std::string, std::vector<TPulseIsland*> >
    fPulseIslandToChannelMap;

  ClassDef(TGlobalData, 2)
};
#endif
