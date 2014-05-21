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
  static TGlobalData* Instance();

  /// Deletes all the held TPulseIsland*s and sets their pointers to
  /// 0x0. Then clears the vectors<TPulseIsland*> 
  /// WARNING: this method is for offline (rootana) use only!  It has
  /// not been checked for online (alcapana, or below) use and may
  /// result in double-frees if used there.
  void Clear(const Option_t* = 0x0);

  // Map of bank names to vectors of islands.
  std::map<std::string, std::vector<TPulseIsland*> >
    fPulseIslandToChannelMap;

  TGlobalData(){}
 private:
  TGlobalData(const TGlobalData&);
  TGlobalData operator=(const TGlobalData&);

  ClassDef(TGlobalData, 2)
};
#endif
