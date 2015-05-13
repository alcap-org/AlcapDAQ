#ifndef TGlobalData_h__
#define TGlobalData_h__

#include <vector>
#include <map>
#include <string>

#include <TObject.h>
#include "TPulseIsland.h"
#include "TMuPCCluster.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief
/// This class holds all of the data created by alcapana analysis modules.
/// \ingroup alcapana
/// \ingroup rootana
///
/// \details
/// During analysis in alcapana, a pointer to a structure of %TGlobalData (TGD)
/// type is instantiated to hold all of the TPIs of a given MIDAS event. During
/// rootana production, the data is loaded back into a TGD to be passed on to
/// the next stage of analysis.
////////////////////////////////////////////////////////////////////////////////
class TGlobalData : public TObject{
 public:
  static TGlobalData* Instance();

  /// Deletes all the held TPulseIsland*s and sets their pointers to
  /// 0x0. Then clears the vectors<TPulseIsland*> 
  /// WARNING: this method is for offline (rootana) use only!  It has
  /// not been checked for online (alcapana, or below) use and may
  /// result in double-frees if used there.
  void Clear(const Option_t* = 0x0);

  /// Map of bank names to vectors of TPIs.
  std::map<std::string, std::vector<TPulseIsland*> >
    fPulseIslandToChannelMap;

  /// TMuPCCluster
  std::vector<TMuPCCluster*> fMuPCClusterVector;

  /// Map of bank name to vector of TDC hits
  std::map< std::string, std::vector<long> > fTDCHitsToChannelMap;

  TGlobalData(){}
 private:
  TGlobalData(const TGlobalData&);
  TGlobalData operator=(const TGlobalData&);

  ClassDef(TGlobalData, 4)
};
#endif
