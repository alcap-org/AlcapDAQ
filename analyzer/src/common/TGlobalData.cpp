#include "TGlobalData.h"

ClassImp(TGlobalData)

//----------------------------------------------------------------------
void TGlobalData::Clear(Option_t* opt)
{
  //Deletes all the contained TPulseIslands that were constructed
  std::map< std::string , std::vector < TPulseIsland* > >::iterator map_it;
  for (map_it = fPulseIslandToChannelMap.begin(); 
       map_it != fPulseIslandToChannelMap.end(); ++map_it){
    std::vector<TPulseIsland*> vec = map_it->second;
    for (int i = 0; i < vec.size(); ++i){
      delete vec[i];
    } 
  }
}
