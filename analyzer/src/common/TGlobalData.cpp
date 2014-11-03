#include "TGlobalData.h"

ClassImp(TGlobalData)

//----------------------------------------------------------------------
  void TGlobalData::Clear(const Option_t* /*opt*/)
{
  // WARNING: this method is for offline (rootana) use only!  It has
  // not been checked for online (alcapana, or below) use and may
  // result in double-frees if used there.
  typedef std::map<std::string, std::vector<TPulseIsland*> > PulseMap;
  typedef PulseMap::iterator PulseMapIt;
  PulseMapIt mapIter;
  PulseMapIt mapEnd = this->fPulseIslandToChannelMap.end();
  for(mapIter = this->fPulseIslandToChannelMap.begin(); mapIter != mapEnd; mapIter++) {
    // The iterator is pointing to a pair<string, vector<TPulseIsland*> >
    std::vector<TPulseIsland*>& pulse_vector= mapIter->second;
    for(size_t i=0; i<pulse_vector.size(); i++){
      delete pulse_vector[i];
      pulse_vector[i] = NULL;
    }
    pulse_vector.clear();
  }

  for (unsigned int i = 0; i < fMuPCClusterVector.size(); ++i)
    delete fMuPCClusterVector[i];
  fMuPCClusterVector.clear();

}
//----------------------------------------------------------------------
#ifdef ROOT_VERSION
TGlobalData* TGlobalData::Instance()
{
  return NULL;
}
#endif
