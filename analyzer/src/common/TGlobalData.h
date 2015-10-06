#ifndef TGlobalData_h__
#define TGlobalData_h__

#include <vector>
#include <map>
#include <string>

#include <stdint.h>

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
  std::map< std::string, std::vector<int64_t> > fTDCHitsToChannelMap;

  /// Pulse alignment
  /// The index of the TDC synchronization pulse that aligns with the
  /// first WFD synchronization pulse in a crate.
  /// The vector is indexed by crate number.
  std::vector<int> fTDCSynchronizationPulseIndex;
  /// The timing difference, indexed by crate number, between the TDC
  /// matched synchronization pulse and the first WFD synchronization
  /// pulse (TDC-WFD, since the TDC seems to always start first).
  /// To use, add to all times in the crate.
  std::vector<double> fTDCSynchronizationPulseOffset;

  void SetNMuBlock(int n) {
    fNMuRun += (fNMuBlock = n);
  }
  int NMuBlock() {return fNMuBlock;}
  int NMuRun() {return fNMuRun;} 
  void SetNBlocks(int n) {
    fNBlocks = n;
  }
  int NBlocks() {return fNBlocks;}

  TGlobalData() : fNMuBlock(0), fNMuRun(0), fNBlocks(0) {}

 private:
  TGlobalData(const TGlobalData&);
  TGlobalData operator=(const TGlobalData&);

  int fNMuBlock; //! Don't save
  int fNMuRun;   //! Don't save
  int fNBlocks;  //! Don't save

  ClassDef(TGlobalData, 7)
};
#endif
